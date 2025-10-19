cmake_minimum_required(VERSION 3.16)
project(SoundSpace)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

# ДОБАВЬ Qt5::Network для работы с API
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)

include_directories(src)
include_directories(src/database)
include_directories(src/core)
include_directories(src/gui)
include_directories(src/gui/AuthWindow)
include_directories(src/gui/MainWindow)
include_directories(src/api)  

set(SOURCE_FILES
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/gui/AuthWindow/AuthWindow.cpp
    src/gui/MainWindow/MainWindow.cpp
    src/database/api/MusicAPIManager.cpp
)

add_executable(SoundSpace ${SOURCE_FILES})

target_link_libraries(SoundSpace 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Sql 
    Qt5::Network
)