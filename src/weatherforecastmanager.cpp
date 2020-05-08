#include "weatherforecastmanager.h"
#include "weatherlocationmodel.h"
#include <KConfigCore/KConfigGroup>
#include <QDirIterator>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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

    distribution = new std::uniform_int_distribution<int>(0, 30 * 60); // uniform random update interval, 60 min to 90 min
    auto rand = std::bind(*distribution, generator);

    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, &QTimer::timeout, this, &WeatherForecastManager::update);
    if (api_ == NORWEGIAN)
        updateTimer->start(0); // update when open
    else
        updateTimer->start(1000 * 3 * 3600 + rand() * 1000);
}

WeatherForecastManager &WeatherForecastManager::instance(WeatherLocationListModel &model)
{
    static WeatherForecastManager singleton(model);
    return singleton;
}
void WeatherForecastManager::update()
{
    auto locations = model_.getList();
    for (auto wLocation : locations) {
        wLocation->weatherBackendProvider()->setLocation(wLocation->latitude(), wLocation->longitude());
        wLocation->weatherBackendProvider()->update();
    }
    updateTimer->start(1000 * 3600 + rand() * 1000); // reset timer
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
        if (reader.fileName().right(10) != QLatin1String("cache.json")) // if not cache file ignore
            continue;
        reader.open(QIODevice::ReadOnly | QIODevice::Text);
        // obtain weather forecast
        AbstractWeatherForecast *fc = convertFromJson(reader.readAll());
        // loop over existing locations and add cached weather forecast data if location found
        for (auto wl : model_.getList()) {
            if (fc->locationId() == wl->locationId()) {
                // add forecast if it does not exist, or is newer than existing data
                if (map[wl] == nullptr || map[wl]->timeCreated() < fc->timeCreated()) {
                    map[wl] = fc;
                }
                break;
            }
        }
        reader.close();
    }

    // add loaded locations from cache
    for (auto wl : model_.getList()) {
        if (map[wl] != nullptr) {
            wl->updateData(map[wl]);
        }
    }
}

AbstractWeatherForecast *WeatherForecastManager::convertFromJson(QByteArray data)
{
    QJsonObject doc = QJsonDocument::fromJson(data).object();
    return AbstractWeatherForecast::fromJson(doc);
}
