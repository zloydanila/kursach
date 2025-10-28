cmake_minimum_required(VERSION 3.16)
project(SoundSpace)


set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Network Multimedia)

include_directories(src)

set(SOURCE_FILES
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/database/DatabaseManager.h
    src/database/api/MusicAPIManager.cpp
    src/database/api/MusicAPIManager.h
    src/core/Track.cpp
    src/core/Track.h
    src/core/Playlist.cpp
    src/core/Playlist.h
    src/gui/AuthWindow/AuthWindow.cpp
    src/gui/AuthWindow/AuthWindow.h
    src/gui/MainWindow/MainWindow.cpp
    src/gui/MainWindow/MainWindow.h
    src/audio/AudioPlayer.cpp
    src/audio/AudioPlayer.h
    src/audio/TrackWidget.cpp
    src/audio/TrackWidget.h
)

add_executable(SoundSpace ${SOURCE_FILES})

target_link_libraries(SoundSpace 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Sql 
    Qt5::Network
    Qt5::Multimedia
)