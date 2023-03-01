find_package(Qt6 6.2 COMPONENTS Quick REQUIRED)

qt_add_library(fileslist_module STATIC)

qt_add_qml_module(fileslist_module
    URI "FilesList"
    VERSION 1.0
    RESOURCE_PREFIX "/FilesListModule"
    QML_FILES
        FilesListView.qml FilesListViewDelegate.qml Button.qml
    SOURCES
        FilesList.h FilesList.cpp FilesExplorer.h FilesExplorer.cpp FileDataStruct.h
)

target_include_directories(fileslist_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(fileslist_module
    PRIVATE Qt6::Quick compressor_lib)

