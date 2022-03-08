cmake_minimum_required(VERSION 3.12)
project(protolib)
set(PROJECT_VERSION 0.0.1)

#Should we force this on the top level?
find_package(fmt)

option(USE_SANITIZER "Sanitizers for debugging" OFF)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)
target_compile_options(project_options INTERFACE 
                                            -Wall
                                            -Wextra
                                            -Wno-unused-parameter
                                            # -Wold-style-cast
                                            -Wnon-virtual-dtor
                                            -Woverloaded-virtual
                                            -Wdouble-promotion
                                            -Wformat=2
                                            -Wredundant-decls
                                            # -Wconversion
                                            -Wvla
                                            -Wdouble-promotion
                                            -Werror=return-type
                                 )

if(USE_SANITIZER)
    target_compile_options(project_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_libraries(project_options INTERFACE -fsanitize=address,undefined)
    # target_compile_options(project_options INTERFACE -fsanitize=thread)
    # target_link_libraries(project_options INTERFACE -fsanitize=thread)
endif()

add_subdirectory(core)
add_subdirectory(array)
add_subdirectory(file_io)
add_subdirectory(examples)


find_package (Python 3.6 COMPONENTS Interpreter Development)
find_package(pybind11)
add_subdirectory(python)