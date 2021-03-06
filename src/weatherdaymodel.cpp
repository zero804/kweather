/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "weatherdaymodel.h"
#include "weatherlocation.h"
/* ~~~ WeatherDay ~~~ */

WeatherDay::WeatherDay()
{
}

WeatherDay::WeatherDay(AbstractDailyWeatherForecast &dailyForecast, AbstractSunrise &sunrise)
{
    this->maxTemp_ = dailyForecast.maxTemp();
    this->minTemp_ = dailyForecast.minTemp();
    this->weatherIcon_ = dailyForecast.weatherIcon();
    this->weatherDescription_ = dailyForecast.weatherDescription();
    this->date_ = dailyForecast.date();
    this->precipitation_ = dailyForecast.precipitation();
    this->uvIndex_ = dailyForecast.uvIndex();
    this->humidity_ = dailyForecast.humidity();
    this->pressure_ = dailyForecast.pressure();

    this->sunrise_ = sunrise.sunRise().toString("hh:mm ap");
    this->sunset_ = sunrise.sunSet().toString("hh:mm ap");
    if (sunrise.moonPhase() <= 5) {
        this->moonPhase_ = "New Moon";
    } else if (sunrise.moonPhase() <= 25) {
        this->moonPhase_ = "Waxing Crescent";
    } else if (sunrise.moonPhase() <= 45) {
        this->moonPhase_ = "Waxing Gibbous";
    } else if (sunrise.moonPhase() <= 55) {
        this->moonPhase_ = "Full Moon";
    } else if (sunrise.moonPhase() <= 75) {
        this->moonPhase_ = "Waning Gibbous";
    } else if (sunrise.moonPhase() <= 95) {
        this->moonPhase_ = "Waning Crescent";
    } else {
        this->moonPhase_ = "New Moon";
    }
}

/* ~~~ WeatherDayListModel ~~~ */

WeatherDayListModel::WeatherDayListModel(WeatherLocation *location)
{
    connect(location, &WeatherLocation::weatherRefresh, this, &WeatherDayListModel::refreshDaysFromForecasts);
}

int WeatherDayListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return daysList.size();
}

QVariant WeatherDayListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= daysList.count() || index.row() < 0) {
        return {};
    }
    if (role == Roles::DayItemRole) {
        return QVariant::fromValue(daysList.at(index.row()));
    }
    return {};
}

QHash<int, QByteArray> WeatherDayListModel::roleNames() const
{
    return {{Roles::DayItemRole, "dayItem"}};
}

WeatherDay *WeatherDayListModel::get(int index)
{
    if (index < 0 || index >= daysList.count())
        return {};
    return daysList.at(index);
}

void WeatherDayListModel::refreshDaysFromForecasts(AbstractWeatherForecast &forecasts)
{
    emit layoutAboutToBeChanged();
    emit beginRemoveRows(QModelIndex(), 0, daysList.count() - 1);
    auto oldList = daysList;
    daysList.clear();
    emit endRemoveRows();

    emit beginInsertRows(QModelIndex(), 0, forecasts.dailyForecasts().count() - 1);

    // add weatherdays with forecast day lists
    for (auto forecast : forecasts.dailyForecasts()) {
        AbstractSunrise daySunrise;

        // find sunrise data, if it exists
        for (auto sunrise : forecasts.sunrise()) {
            if (sunrise.sunRise().date().daysTo(forecast.date()) == 0) {
                daySunrise = sunrise;
                break;
            }
        }

        WeatherDay *weatherDay = new WeatherDay(forecast, daySunrise);
        QQmlEngine::setObjectOwnership(weatherDay, QQmlEngine::CppOwnership); // prevent segfaults from js garbage collecting
        daysList.append(weatherDay);
    }

    emit endInsertRows();
    emit layoutChanged();
    for (auto ptr : oldList)
        delete ptr;
}

void WeatherDayListModel::updateUi()
{
    for (auto h : daysList) {
        emit h->propertyChanged();
    }
    emit dataChanged(createIndex(0, 0), createIndex(daysList.count() - 1, 0));
}
