cmake_minimum_required(VERSION 4.0)
project(app)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/pyro/Qt/6.8.3/gcc_64/lib/cmake")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

qt6_add_resources(RESOURCES resources.qrc)

add_subdirectory(lib/model)
add_subdirectory(lib/view)
add_subdirectory(lib/connector)

set_target_properties(connector PROPERTIES AUTOMOC ON)

add_executable(app main.cpp)
target_link_libraries(app
        Qt::Core
        Qt::Gui
        Qt::Widgets
        mainwindow
        model
)

