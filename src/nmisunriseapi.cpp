/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "nmisunriseapi.h"
#include "abstractsunrise.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <stdlib.h>
NMISunriseAPI::NMISunriseAPI(float latitude, float longitude, int offset_secs)
    : latitude_(latitude)
    , longitude_(longitude)
    , offset_(offset_secs)
{
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &NMISunriseAPI::process);
}

void NMISunriseAPI::update()
{
    if (sunrise_.count() == 11) // don't update if we have enough data
        return;
    QUrl url;
    url.setScheme(QStringLiteral("https"));
    url.setHost(QStringLiteral("api.met.no"));
    url.setPath(QStringLiteral("/weatherapi/sunrise/2.0/.json"));
    QUrlQuery query;
    query.addQueryItem(QLatin1String("lat"), QString::number(latitude_));
    query.addQueryItem(QLatin1String("lon"), QString::number(longitude_));
    // if we already have data, request data beyond the last day
    query.addQueryItem(QLatin1String("date"), sunrise_.isEmpty() ? QDate::currentDate().toString(QLatin1String("yyyy-MM-dd")) : QDate::currentDate().addDays(sunrise_.count()).toString(QLatin1String("yyyy-MM-dd")));
    query.addQueryItem(QLatin1String("days"), sunrise_.isEmpty() ? QString::number(10) : QString::number(11 - sunrise_.count()));

    // calculate offset (form example: -04:00)
    QString offset = offset_ < 0 ? "-" : "+";
    int hour = std::abs(offset_) / 3600;
    if (hour >= 10)
        offset.append(QString::number(hour) + ":");
    else {
        offset.append("0" + QString::number(hour) + ":");
    }
    int min = (std::abs(offset_) - hour * 3600) / 60;
    if (min >= 10) {
        offset.append(QString::number(min));
    } else {
        offset.append("0" + QString::number(min));
    }
    query.addQueryItem(QLatin1String("offset"), offset);

    url.setQuery(query);
    qDebug() << url;
    QNetworkRequest req(url);
    manager->get(req);
}

void NMISunriseAPI::process(QNetworkReply *reply)
{
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonArray array = doc["location"].toObject()["time"].toArray();
    for (auto ob : array) {
        auto *sr = new AbstractSunrise();
        sr->setSunSet(QDateTime::fromString(ob.toObject()["sunset"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"));
        sr->setSunRise(QDateTime::fromString(ob.toObject()["sunrise"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"));
        sr->setMoonSet(QDateTime::fromString(ob.toObject()["moonset"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"));
        sr->setMoonRise(QDateTime::fromString(ob.toObject()["moonrise"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"));
        sr->setSolarMidnight(
            QPair<QDateTime, double>(QDateTime::fromString(ob.toObject()["solarmidnight"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"), ob.toObject()["solarmidnight"].toObject()["elevation"].toString().toDouble()));
        sr->setSolarNoon(QPair<QDateTime, double>(QDateTime::fromString(ob.toObject()["solarnoon"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"), ob.toObject()["solarnoon"].toObject()["elevation"].toString().toDouble()));
        sr->setHighMoon(QPair<QDateTime, double>(QDateTime::fromString(ob.toObject()["high_moon"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"), ob.toObject()["high_moon"].toObject()["elevation"].toString().toDouble()));
        sr->setLowMoon(QPair<QDateTime, double>(QDateTime::fromString(ob.toObject()["low_moon"].toObject()["time"].toString().left(19), "yyyy-MM-ddThh:mm:ss"), ob.toObject()["low_moon"].toObject()["elevation"].toString().toDouble()));
        sr->setMoonPhase(ob.toObject()["moonposition"].toObject()["phase"].toString().toDouble());
        sunrise_.push_back(sr);
    }

    noData = false;
    emit finished();
    reply->deleteLater();
}

void NMISunriseAPI::popDay()
{
    QDateTime today = QDateTime::currentDateTime();
    for (auto day : sunrise_) {
        if (day->sunRise().daysTo(today) > 0) {
            sunrise_.pop_front();
            delete day;
        }
    }
};

bool NMISunriseAPI::isDayTime(QDateTime date)
{
    for (auto sr : sunrise_) {
        // if on the same day
        if (sr->sunRise().date().daysTo(date.date()) == 0) {
            // 30 min threshold
            return !(sr->sunRise().secsTo(date) <= -1800 || sr->sunSet().secsTo(date) >= -1800);
        }
    }
    return true;
}

NMISunriseAPI::~NMISunriseAPI()
{
    delete manager;
    for (auto sr : sunrise_)
        delete sr;
}