qt_add_library(scene STATIC
        griditem.hpp
        beaconitem.hpp
        espitem.hpp
        scene.cpp
        scene.hpp
)

target_link_libraries(scene PUBLIC
        Qt6::Core
        Qt6::Widgets
)

target_include_directories(scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})