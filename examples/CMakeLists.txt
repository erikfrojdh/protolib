

add_executable(e1 e1_read_file.cpp)
target_link_libraries(e1 fmt::fmt file_io)

set_target_properties(e1 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)