cmake_minimum_required(VERSION 3.16)
project(SoundSpace VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Sql Multimedia)

qt_standard_project_setup()

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")
file(GLOB HEADERS "src/*.h" "src/*/*.h")

add_executable(SoundSpace ${SOURCES} ${HEADERS})

target_link_libraries(SoundSpace 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Network 
    Qt6::Sql 
    Qt6::Multimedia
)

install(TARGETS SoundSpace RUNTIME DESTINATION bin)
