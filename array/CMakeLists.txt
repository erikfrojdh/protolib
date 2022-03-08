add_library(array STATIC
${CMAKE_CURRENT_SOURCE_DIR}/src/Frame.cpp)
target_include_directories(array PUBLIC include)
set_target_properties(array PROPERTIES POSITION_INDEPENDENT_CODE ON)