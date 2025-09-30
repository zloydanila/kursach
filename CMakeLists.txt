cmake_minimum_required(VERSION 3.16)
project(SoundSpace)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql)

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB HEADERS "src/*.h" "src/*/*.h")

add_executable(SoundSpace ${SOURCES} ${HEADERS})
target_link_libraries(SoundSpace Qt5::Core Qt5::Widgets Qt5::Sql)