cmake_minimum_required(VERSION 3.16)
project(SoundSpace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Network Multimedia)

include_directories(
    src
    src/database
    src/database/api
    src/database/socialNetwork
    src/database/models
    src/gui
    src/gui/AuthWindow
    src/gui/MainWindow
)

set(SOURCE_FILES
    src/main.cpp
    src/gui/AuthWindow/AuthWindow.cpp
    src/gui/MainWindow/MainWindow.cpp
    src/database/DatabaseManager.cpp
    src/database/api/MusicAPIManager.cpp
    src/database/socialNetwork/FriendsManager.cpp
    src/database/socialNetwork/MessagesManager.cpp
    src/database/api/AudioPlayer/AudioPlayer.cpp
    src/database/api/AudioPlayer/TrackWidget.cpp
)

add_executable(SoundSpace ${SOURCE_FILES})

target_link_libraries(SoundSpace 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Sql 
    Qt5::Network
    Qt5::Multimedia
)