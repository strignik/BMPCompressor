add_library(compressor_lib STATIC Compressor.cpp Compressor.h)
target_include_directories(compressor_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(compressor_lib
    PRIVATE Qt6::Quick Qt6::Core)

install(TARGETS compressor_lib DESTINATION lib)
