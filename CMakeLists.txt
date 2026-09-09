cmake_minimum_required(VERSION 3.16)

project(MediaPlayerForUOS VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    MultimediaWidgets
    WebSockets
)

add_subdirectory(src)

include(GNUInstallDirs)

install(
    FILES packaging/linux/media-player-for-uos.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(
    FILES resources/icons/icon.jpeg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
    RENAME media-player-for-uos.jpeg
)
