# TODO: adapt "org.kde.plasma" here & elsewhere if needed (see README)
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.kweather_1x4\")

set(kweather_1x4_SRCS
    kweather_1x4.cpp
)
add_library(plasma_applet_kweather_1x4 MODULE ${kweather_1x4_SRCS})

kcoreaddons_desktop_to_json(plasma_applet_kweather_1x4 package/metadata.desktop SERVICE_TYPES plasma-applet.desktop)

target_link_libraries(plasma_applet_kweather_1x4
                      Qt5::Gui
                      Qt5::DBus
                      KF5::Plasma
                      KF5::I18n)


install(TARGETS plasma_applet_kweather_1x4 DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/applets)

plasma_install_package(package org.kde.plasma.kweather_1x4)
