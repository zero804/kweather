#
# Copyright 2020 Han Young <hanyoung@protonmail.com>
# Copyright 2020 Devin Lin <espidev@gmail.com>
#
# SPDX-License-Identifier: GPL-2.0-or-later
#

set(kweather_SRCS
    main.cpp
    geoiplookup.cpp
    geolocation.cpp
    abstractweatherforecast.cpp
    weatherforecastmanager.cpp
    weatherlocationmodel.cpp
    weatherlocation.cpp
    weatherhourmodel.cpp
    weatherdaymodel.cpp
    owmweatherapi.cpp
    abstractweatherapi.cpp
    geotimezone.cpp
    locationquerymodel.cpp
    abstractdailyweatherforecast.cpp
    abstracthourlyweatherforecast.cpp
    nmiweatherapi2.cpp
    nmisunriseapi.cpp
    abstractsunrise.cpp
    resources.qrc
)

kconfig_add_kcfg_files(kweather_SRCS kweathersettings.kcfgc GENERATE_MOC)

add_executable(kweather ${kweather_SRCS})
target_link_libraries(kweather
    Qt5::Core
    Qt5::Qml
    Qt5::QuickControls2
    Qt5::Widgets
    Qt5::Network
    KF5::I18n
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::Notifications
    KF5::QuickCharts
)

if (ANDROID)
    target_link_libraries(kweather
        OpenSSL::SSL
        Qt5::Svg
    )

    kirigami_package_breeze_icons(ICONS
        raindrop
        globe
        settings-configure
        network-disconnect
        search
        list-add
        delete
        arrow-right
        speedometer
        go-up
        go-down
        compass

        weather-clear
        weather-clear-night
        weather-clear-wind
        weather-clear-wind-night
        weather-clouds
        weather-clouds-night
        weather-clouds-wind
        weather-clouds-wind-night
        weather-few-clouds
        weather-few-clouds-night
        weather-few-clouds-wind
        weather-few-clouds-wind-night
        weather-fog
        weather-freezing-rain
        weather-hail
        weather-many-clouds
        weather-many-clouds-wind
        weather-mist
        weather-none-available
        weather-overcast
        weather-overcast-wind
        weather-showers
        weather-showers-day
        weather-showers-night
        weather-showers-scattered
        weather-showers-scattered-day
        weather-showers-scattered-night
        weather-snow
        weather-snow-rain
        weather-snow-scattered
        weather-snow-scattered-day
        weather-snow-scattered-night
        weather-storm
        weather-storm-day
        weather-storm-night
    )
endif()

#if (UNIX AND (NOT ANDROID))
#add_subdirectory(plasmoid)
#endif()
install(TARGETS kweather ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
