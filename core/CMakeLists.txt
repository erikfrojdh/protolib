
# add_library(core STATIC
#     src/defs.cpp
# )
# target_include_directories(core PUBLIC include)
# target_link_libraries(core PRIVATE fmt::fmt project_options)

target_include_directories(protolib PUBLIC include)
target_sources(protolib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/defs.cpp)

