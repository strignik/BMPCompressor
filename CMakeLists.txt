cmake_minimum_required(VERSION 3.16)

project(BMPCompressor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_STANDARD 17)

find_package(Qt6 6.2 COMPONENTS Quick Core REQUIRED)

qt_add_executable(appBMPCompressor
    main.cpp
)

qt_add_qml_module(appBMPCompressor
    VERSION 1.0
    URI BMPCompressor
    QML_FILES qml/main.qml 
)

add_subdirectory(FilesListModule/)
add_subdirectory(CompressorLib/)

set_target_properties(appBMPCompressor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appBMPCompressor
    PRIVATE Qt6::Quick fileslist_moduleplugin)

install(TARGETS appBMPCompressor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
