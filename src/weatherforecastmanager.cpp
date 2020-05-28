/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "weatherforecastmanager.h"
#include "weatherlocationmodel.h"
#include <KConfigCore/KConfigGroup>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QTimeZone>
#include <QTimer>

WeatherForecastManager::WeatherForecastManager(WeatherLocationListModel &model, int defaultAPI)
    : model_(model)
    , api_(defaultAPI)
{
    if (defaultAPI != NORWEGIAN && defaultAPI != OPENWEATHER) {
        qDebug() << "wrong api";
        exit(1);
    }

    // create cache location if it does not exist, and load cache
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/cache");
    if (!dir.exists())
        dir.mkpath(".");
    readFromCache();
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, &QTimer::timeout, this, &WeatherForecastManager::update);
    if (api_ == NORWEGIAN)
        updateTimer->start(0); // update when open
    else
        updateTimer->start(1000 * 3 * 3600 + random.bounded(0, 1800) * 1000);
}

WeatherForecastManager &WeatherForecastManager::instance(WeatherLocationListModel &model)
{
    static WeatherForecastManager singleton(model);
    return singleton;
}
void WeatherForecastManager::update()
{
    qDebug() << "update start";
    auto locations = model_.getList();
    for (auto wLocation : locations) {
        wLocation->weatherBackendProvider()->setLocation(wLocation->latitude(), wLocation->longitude());
        wLocation->update();
    }
    updateTimer->start(1000 * 3600 + random.bounded(0, 1800) * 1000); // reset timer
}

void WeatherForecastManager::readFromCache()
{
    QHash<WeatherLocation *, AbstractWeatherForecast *> map;
    for (auto wl : model_.getList())
        map[wl] = nullptr;

    QFile reader;
    QDirIterator It(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/cache", QDirIterator::Subdirectories); // list directory entries
    while (It.hasNext()) {
        reader.setFileName(It.next());
        QFileInfo fileName(reader); // strip absolute path
        bool isFound = false;       // indicate should we load this cache
        // loop over existing locations and add cached weather forecast data if location found
        for (auto wl : model_.getList()) {
            if (fileName.fileName() == wl->locationId()) {
                isFound = true;
                reader.open(QIODevice::ReadOnly);
                AbstractWeatherForecast *fc = convertFromJson(reader.readAll());
                // add forecast if it does not exist, or is newer than existing data
                if (map[wl] == nullptr || map[wl]->timeCreated() < fc->timeCreated()) {
                    map[wl] = fc;
                }
                break;
            }
        }
        if (!isFound) { // delete no longer needed cache
            reader.remove();
        }
        reader.close();
    }

    // add loaded locations from cache
    for (auto wl : model_.getList()) {
        if (map[wl] != nullptr) {
            wl->initData(map[wl]);
        }
    }
}

AbstractWeatherForecast *WeatherForecastManager::convertFromJson(QByteArray data)
{
    QJsonObject doc = QJsonDocument::fromJson(data).object();
    return AbstractWeatherForecast::fromJson(doc);
}
