cmake_minimum_required(VERSION 3.16)
project(SoundSpace)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Network Multimedia)

# Включаем директории
include_directories(${CMAKE_SOURCE_DIR})

# Список исходных файлов
set(SOURCE_FILES
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/database/api/MusicAPIManager.cpp
    src/gui/AuthWindow/AuthWindow.cpp
    src/gui/MainWindow/MainWindow.cpp
    src/gui/Frameless/FramelessWindow.cpp
    src/audio/AudioPlayer.cpp
    src/audio/TrackWidget.cpp
)

# Список заголовочных файлов
set(HEADER_FILES
    src/database/DatabaseManager.h
    src/database/api/MusicAPIManager.h
    src/core/Track.h
    src/core/Playlist.h
    src/gui/AuthWindow/AuthWindow.h
    src/gui/MainWindow/MainWindow.h
    src/gui/Frameless/FramelessWindow.h
    src/audio/AudioPlayer.h
    src/audio/TrackWidget.h
)

# Файлы ресурсов
set(RESOURCE_FILES
    style.qrc
)

# Создаем исполняемый файл
add_executable(SoundSpace 
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${RESOURCE_FILES}
)

# Подключаем библиотеки Qt
target_link_libraries(SoundSpace 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Sql 
    Qt5::Network
    Qt5::Multimedia
)

# Копируем файлы ресурсов в билд директорию
file(COPY ${CMAKE_SOURCE_DIR}/styles DESTINATION ${CMAKE_BINARY_DIR})