

add_executable(e1 e1_read_file.cpp)
target_link_libraries(e1 fmt::fmt file_io)
set_target_properties(e1 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)


add_executable(e2 e2_read_jungfrau_file.cpp)
target_link_libraries(e2 fmt::fmt file_io)
set_target_properties(e2 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(e3 e3_just_the_data.cpp)
target_link_libraries(e3 fmt::fmt file_io project_options)
set_target_properties(e3 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)