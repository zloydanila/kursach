cmake_minimum_required(VERSION 3.16)
project(SoundSpace)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql)

# 🔽 ДОБАВЬ ЭТИ СТРОКИ:
include_directories(src)
include_directories(src/database)
include_directories(src/database/models)

# Файлы проекта
set(SOURCE_FILES
    src/main.cpp
    src/database/DatabaseManager.cpp
    src/gui/AuthWindow/AuthWindow.cpp
)

add_executable(SoundSpace ${SOURCE_FILES})
target_link_libraries(SoundSpace Qt5::Core Qt5::Widgets Qt5::Sql)