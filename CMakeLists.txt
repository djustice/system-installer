cmake_minimum_required(VERSION 3.29)

project(system-installer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)

cmake_policy(SET CMP0160 OLD)

find_package(ECM 6.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(ECMDeprecationSettings)
include(ECMInstallIcons)
include(ECMPoQmTools)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)
include(GNUInstallDirs)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

ecm_set_disabled_deprecation_versions(
    QT 6.4.0
    KF 5.240
)

find_package(Qt6DBus 6.5.0 CONFIG REQUIRED)

find_package(QT
    NAMES Qt6
    REQUIRED COMPONENTS
        Core
        Widgets
        Network
)

find_package(KF6
    REQUIRED COMPONENTS
        Auth
        CoreAddons
        KIO
)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    colorizedpixmap.h
    colorizedpixmap.cpp
    colorchooser.h
    colorchooser.cpp
    colorchooser.ui
    installationhandler.h
    installationhandler.cpp
    resources.qrc
)

add_executable(system-installer_authmount kauth/authmount.cpp)
target_link_libraries(system-installer_authmount KF6::AuthCore)

add_executable(system-installer ${PROJECT_SOURCES})
target_compile_options(system-installer PRIVATE -Wno-dev)

target_link_libraries(system-installer
    PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    KF6::AuthCore
    KF6::KIOCore
)

install(TARGETS system-installer_authmount DESTINATION ${KAUTH_HELPER_INSTALL_DIR})

kauth_install_helper_files(system-installer_authmount org.kde.systeminstaller root)
kauth_install_actions(org.kde.systeminstaller kauth/org.kde.systeminstaller.actions)

install(TARGETS system-installer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

