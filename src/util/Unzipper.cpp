#include "Unzipper.h"
#include <QDir>

bool Unzipper::unzipAllFilesToPath(QString outputPath) {
    QDir outputDir(outputPath);

    if (!archive->open(QuaZip::Mode::mdUnzip)) {
        return false;
    }

    if (!outputDir.exists()) {
        return false;
    }

    for(bool f = archive->goToFirstFile(); f; f = archive->goToNextFile()) {
        QString filePath = archive->getCurrentFileName();
        QuaZipFile zFile(archive->getZipName(), filePath);

        zFile.open(QIODevice::ReadOnly);
        QByteArray ba = zFile.readAll();
        zFile.close();
        auto destFilePath = outputDir.filePath(filePath);

        QFile dstFile(destFilePath);
        dstFile.open(QIODevice::WriteOnly);
        dstFile.write(ba.data(), ba.size());
        dstFile.close();
    }

    return true;
}

Unzipper::Unzipper(QString archivePath) {
    archive = new QuaZip(archivePath);
    this->archivePath = archivePath;
}

Unzipper::~Unzipper(){
    delete archive;
}
