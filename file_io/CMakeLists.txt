

add_library(file_io INTERFACE)
target_include_directories(file_io INTERFACE include)
target_link_libraries(file_io INTERFACE project_options)