#include "ApplicationUpdater.h"
#include "Application.h"

ApplicationUpdater::~ApplicationUpdater() {
    newVersionTempDir.remove();
    newVersionTempFile.remove();

    delete downloader;
    delete unzipper;
}

void ApplicationUpdater::checkForUpdates() {
    QNetworkRequest request;
    request.setUrl(QUrl("https://version.emojigun.com"));
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    delete checkForUpdatesReply;
    checkForUpdatesReply = emojigunNetworkManager->get(request);

    connect(
        checkForUpdatesReply, &QNetworkReply::finished,
        this, &ApplicationUpdater::checkForUpdatesFinished
    );
}

void ApplicationUpdater::checkForUpdatesFinished() {
    QString contents = checkForUpdatesReply->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8());
    QJsonObject obj = doc.object();
    
    auto jsonMap = obj.toVariantMap();

    auto version = jsonMap.take("version").toString();
    auto downloadURL = jsonMap.take("downloadURL").toString();

    auto responseVersion = QVersionNumber::fromString(version);
    auto thisAppVersion = QVersionNumber::fromString(PROJECT_VER);

    if (responseVersion > thisAppVersion) {
        ApplicationVersionDetails versionDetails;
        versionDetails.version = version;
        versionDetails.downloadUrl = downloadURL;

        // TODO REMOVEME
        // versionDetails.downloadUrl = "https://s3-eu-west-1.amazonaws.com/version.emojigun.com/emojiguntest.zip";
        
        emit updateAvailable(versionDetails);
    }
}

void ApplicationUpdater::updateToVersion(ApplicationVersionDetails details) {
    delete downloader;

    // this should create the temp file
    newVersionTempFile.open();
    newVersionTempFile.close();

    downloader = new FileDownloader(
        emojigunNetworkManager, details.downloadUrl, newVersionTempFile.fileName()
    );

    QObject::connect(
        downloader, &FileDownloader::finished,
        this, &ApplicationUpdater::updateDownloaded
    );

    QObject::connect(
        downloader, &FileDownloader::error,
        this, &ApplicationUpdater::updateDownloadFailed
    );

    QObject::connect(
        downloader, &FileDownloader::IOError,
        this, &ApplicationUpdater::updateDownloadFailed
    );

   // QObject::connect(
   //     downloader, &FileDownloader::progress,
   //     this, [](qint64 bytesDownloaded, qint64 bytesTotal) {
   //     }
   // );

    downloader->start();
}

QString ApplicationUpdater::getPathToInstalledExe() {
    QDir appDataDir(emojigunApp->installDirectory());

    return appDataDir.filePath("emojigun.exe");
}

bool ApplicationUpdater::exeIsInstalled() {
    auto exePath = getPathToInstalledExe();

    return QFile::exists(exePath);
}

void ApplicationUpdater::checkSavedExeVersion() {
    QDir installDir(emojigunApp->installDirectory());
    auto oldFiles = installDir.entryList(QStringList("*.old"));

    if (oldFiles.count() > 0) {
        foreach (auto oldFileName, oldFiles) {
            QFile oldFile(installDir.filePath(oldFileName));
            oldFile.remove();
        }
    }

    if (!installedExeOlderThanLaunchedExe()) {
        return;
    }

    auto exePath = getPathToInstalledExe();

    QFile::remove(exePath);
    QFile::copy(pathToExecutable, exePath);
    emojigunSettings.setValue("installed_exe_version", PROJECT_VER);
}

bool ApplicationUpdater::installedExeOlderThanLaunchedExe() {
    if (!exeIsInstalled()) {
        return true;
    }

    auto installedVersionString = emojigunSettings.value("installed_exe_version", "1.0.0").toString();

    auto installedVersion = QVersionNumber::fromString(installedVersionString);
    auto thisAppVersion = QVersionNumber::fromString(PROJECT_VER);

    return thisAppVersion > installedVersion;
}

void ApplicationUpdater::setPathToExecutable(QString pathToExecutable) {
    this->pathToExecutable = pathToExecutable;
}

void ApplicationUpdater::updateDownloaded() {
    Unzipper unzipper(newVersionTempFile.fileName());
    unzipper.unzipAllFilesToPath(newVersionTempDir.path());

    QDir installDir(emojigunApp->installDirectory());
    QStringList oldFileList = installDir.entryList(QStringList("*"), QDir::Files);

    foreach (auto oldFileName, oldFileList) {
        qDebug() << "old file" << oldFileName;
        QFile oldFile(installDir.filePath(oldFileName));
        oldFile.rename(oldFile.fileName() + ".old");
    }

    QDir newVersionDir(newVersionTempDir.path());
    QStringList newFileList = newVersionDir.entryList(QStringList("*"), QDir::Files);

    foreach (auto newFileName, newFileList) {
        QFile newFile(newVersionDir.filePath(newFileName));

        qDebug() << "new file" << newFileName;

        QFileInfo fileInfo(newFile.fileName());

        newFile.rename(installDir.filePath(fileInfo.fileName()));
    }

    emit updateInstalled(getPathToInstalledExe());
}

void ApplicationUpdater::updateDownloadFailed() {

}