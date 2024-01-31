cmake_minimum_required(VERSION 3.16)

project(system-installer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(GNUInstallDirs)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT
    NAMES Qt5
    REQUIRED COMPONENTS
        Widgets
        Network
)

find_package(KF5
    REQUIRED COMPONENTS
        KIO
)

find_package(KF5Auth
    NO_MODULE
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
target_link_libraries(system-installer_authmount KF5::AuthCore)

add_executable(system-installer
    ${PROJECT_SOURCES}
)

target_link_libraries(system-installer
    PRIVATE
    Qt5::Widgets
    Qt5::Network
    KF5::KIOCore
    KF5::AuthCore
)

install(TARGETS system-installer_authmount DESTINATION ${KAUTH_HELPER_INSTALL_DIR})

kauth_install_helper_files(system-installer_authmount org.kde.systeminstaller root)
#kauth_install_helper_files(system-installer_authunsquash org.kde.systeminstaller root)

kauth_install_actions(org.kde.systeminstaller kauth/org.kde.systeminstaller.actions)

install(TARGETS system-installer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
