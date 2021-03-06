/*
 * Copyright 2020 Han Young <hanyoung@protonmail.com>
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.2
import QtQuick.Shapes 1.12
import org.kde.kirigami 2.11 as Kirigami
import kweather 1.0

Rectangle {
    implicitWidth: Kirigami.Units.gridUnit * 5
    implicitHeight: hourElement.height
    color: "transparent"

    property WeatherHour weather
    property color textColor

    // actual hour display
    ColumnLayout {
        id: hourElement
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Kirigami.Units.largeSpacing
        anchors.rightMargin: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Icon {
            source: weather.weatherIcon
            Layout.preferredHeight: Kirigami.Units.iconSizes.medium
            Layout.preferredWidth: Kirigami.Units.iconSizes.medium
        }
        Label {
            text: weather.temperature
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.3
            color: textColor
        }
        Label {
            text: weather.weatherDescription
            color: textColor
        }

        // precipitation
        RowLayout {
            visible: weather != null
            Kirigami.Icon {
                source: "raindrop"
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                color: settingsModel.forecastStyle === "Dynamic" ? weatherLocation.iconColor : ""
            }
            Label {
                color: settingsModel.forecastStyle === "Dynamic" ? KWeatherStyle.disabledTextColor : Kirigami.Theme.disabledTextColor
                text: i18n("%1mm", weather.precipitation.toFixed(1))
            }
        }

        // wind
        RowLayout {
            Kirigami.Icon {
                source: "arrow-right"
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                color: settingsModel.forecastStyle === "Dynamic" ? weatherLocation.iconColor : ""
            }
            Label {
                color: settingsModel.forecastStyle === "Dynamic" ? KWeatherStyle.disabledTextColor : Kirigami.Theme.disabledTextColor
                text: weather.windSpeed
            }
        }

        Label {
            font.weight: Font.Bold
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1
            text: weather.date.toLocaleString(Qt.locale(), "h ap").replace(".", "").replace(".", "")
            color: textColor
        }
    }
}
