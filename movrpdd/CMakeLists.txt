file(GLOB source_files src/*.cpp include/*.h)
add_library(MOVRPDD ${source_files})
target_include_directories(MOVRPDD PUBLIC ${PROJECT_SOURCE_DIR}/include)