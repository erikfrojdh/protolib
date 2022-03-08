pybind11_add_module(_protolib
    src/main.cpp 
)

target_link_libraries(_protolib PRIVATE fmt::fmt file_io array)

set_target_properties(_protolib PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
                

configure_file( scripts/play.py  
    ${CMAKE_BINARY_DIR}/play.py 
)

configure_file( examples/e1_read_file.py  
    ${CMAKE_BINARY_DIR}/e1_read_file.py 
)

