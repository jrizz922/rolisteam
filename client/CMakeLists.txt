cmake_minimum_required(VERSION 3.10)


project(rolisteam-gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES main.cpp mainwindow.cpp)
set(HEADERS mainwindow.h)

set(QT_REQUIRED_VERSION "5.12.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED COMPONENTS Core Widgets Gui)
find_package(libcore)

add_executable(rolisteam ${SOURCES} ${HEADERS})

#target_include_directories(libcore PUBLIC .)
target_link_libraries(rolisteam PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets libcore)