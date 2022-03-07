cmake_minimum_required(VERSION 3.12)
project(protolib)
set(PROJECT_VERSION 0.0.1)

#Should we force this on the top level?
find_package(fmt)


add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_subdirectory(array)
add_subdirectory(file_io)
add_subdirectory(examples)