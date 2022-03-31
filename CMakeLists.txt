cmake_minimum_required(VERSION 3.12)
project(protolib)
set(PROJECT_VERSION 0.0.1)

include(GNUInstallDirs)




set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3") 
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DPLDEBUG")

#Should we force this on the top level?
find_package(fmt REQUIRED)
find_package(TBB REQUIRED)

option(USE_SANITIZER "Sanitizers for debugging" OFF)
option(USE_TESTS "Unit tests" ON)
option(USE_PYTHON "Unit tests" ON)
option(TUNE_LOCAL "local arch" ON)
option(DISABLE_LTO "dont use lto" OFF)

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

#Enable LTO if available
include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_AVAILABLE)
if((CMAKE_BUILD_TYPE STREQUAL "Release") AND  LTO_AVAILABLE)
    message(STATUS "Building with link time optimization")
else()
    message(STATUS "Building without link time optimization")
endif()

if(USE_SANITIZER)
    target_compile_options(project_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_libraries(project_options INTERFACE -fsanitize=address,undefined)    
    # target_compile_options(project_options INTERFACE -fsanitize=thread)
    # target_link_libraries(project_options INTERFACE -fsanitize=thread)
endif()

if(TUNE_LOCAL)
    if(UNIX AND NOT APPLE)
        message(STATUS "unix")
        target_compile_options(project_options INTERFACE -mtune=native -march=native )
    elseif(APPLE)
        message(STATUS "compiling for apple")
        target_compile_options(project_options INTERFACE -mtune=apple-m1 -mcpu=apple-m1 )
    endif()
    # 
endif()


set(PublicHeaders "")


add_library(protolib STATIC)


if(USE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()




add_subdirectory(array)
add_subdirectory(core)
add_subdirectory(file_io)
add_subdirectory(processing)
add_subdirectory(examples)

if(USE_PYTHON)
    find_package (Python 3.6 COMPONENTS Interpreter Development)
    find_package(pybind11)
    add_subdirectory(python)
endif()

if(LTO_AVAILABLE AND NOT DISABLE_LTO)
    set_property(TARGET protolib PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif()
set_property(TARGET protolib PROPERTY POSITION_INDEPENDENT_CODE ON)


target_link_libraries(protolib 
PRIVATE 
    project_options 
PUBLIC
    fmt::fmt 
    TBB::tbb
)
message(STATUS "${PublicHeaders}")
set_target_properties(protolib PROPERTIES PUBLIC_HEADER 
    "${PublicHeaders}"
)

install(TARGETS protolib EXPORT protolib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/protolib
)

install(TARGETS project_options EXPORT protolib)

install(EXPORT protolib
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/protolib
    NAMESPACE protolib::
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/protolibConfig.cmake.in                      # the template file
    ${CMAKE_CURRENT_BINARY_DIR}/protolibConfig.cmake   # destination
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/protolib
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/protolibConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)


install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/protolibConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/protolibConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/protolib
)