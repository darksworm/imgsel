#define DEBUG false

#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include <QCursor>
#include "ConfigManager.h"
#include "ConfigBuilder.h"

void ConfigManager::loadConfig() {
    std::vector<std::string> imageExtensions = {
            "jpg",
            "jpeg",
            "png",
            "gif"
    };

    std::vector<Image> images;
    for (const auto &img:cliParams.imageFiles) {
        for (const auto &ext:imageExtensions) {
            if (img.length() >= ext.length() && 0 == img.compare(img.length() - ext.length(), ext.length(), ext)) {
                images.emplace_back(img);
                break;
            }
        }
    }

    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());

    auto builder = ConfigBuilder();

    QRect geo;

    if (cliParams.width.has_value()) {
        builder = builder.setWidth(cliParams.width.value())
                .setHeight(cliParams.height.value());

        geo = QRect(0, 0, cliParams.width.value(), cliParams.height.value());
    } else {
        geo = screen->geometry();
    }

    builder.setIsDebug(DEBUG)
            .setDefaultInputMode(cliParams.startInVimMode ? InputMode::VIM : InputMode::DEFAULT)

            .setCols(cliParams.cols.has_value() ? cliParams.cols.value() : 0)
            .setRows(cliParams.rows.has_value() ? cliParams.rows.value() : 0)

            .setMaxImageHeight(cliParams.maxImageHeight.has_value() ? cliParams.maxImageHeight.value() : 0)
            .setMaxImageWidth(cliParams.maxImageWidth.has_value() ? cliParams.maxImageWidth.value() : 0)

            .setPrintFilePath(cliParams.printFilePath)

            .setYPadding(cliParams.imageYPadding.has_value() ? cliParams.imageYPadding.value() : 30)
            .setXPadding(cliParams.imageXPadding.has_value() ? cliParams.imageXPadding.value() : 30)

            .setYMargin(cliParams.imageYMargin.has_value() ? cliParams.imageYMargin.value() : 30)
            .setXMargin(cliParams.imageXMargin.has_value() ? cliParams.imageXMargin.value() : 30)

            .setScreenGeometry(geo)

            .setImages(images);


    ConfigManager::config = builder.build();
    ConfigManager::configLoaded = true;
}

Config ConfigManager::getOrLoadConfig() {
    if (!ConfigManager::configLoaded) {
        loadConfig();
    }

    return *ConfigManager::config;
}

ConfigManager::ConfigManager() {
    ConfigManager::configLoaded = false;
}

void ConfigManager::setCLIParams(CLIParams params) {
    ConfigManager::cliParams = std::move(params);
}