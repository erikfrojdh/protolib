pybind11_add_module(_protolib
    src/main.cpp 
)

target_link_libraries(_protolib PRIVATE fmt::fmt protolib project_options)

set_target_properties(_protolib PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
                

configure_file( scripts/play.py  
    ${CMAKE_BINARY_DIR}/play.py 
)
configure_file( scripts/numpy_inspect.py  
    ${CMAKE_BINARY_DIR}/numpy_inspect.py 
)


set(PythonFiles
    __init__.py
    folder_helper.py
    root_helper.py
    PythonClusterFinder.py
    function.py
)

foreach(FILE ${PythonFiles})
    configure_file( protolib/${FILE}  
    ${CMAKE_BINARY_DIR}/protolib/${FILE})
endforeach()

set(PythonExamples
    pyex1_read_file.py 
    pyex2_jungfrau_file.py
    pyex3_imshow.py 
    pyex4_iterate_file.py 
    pyex5_iterate_benchmark.py
)

foreach(FILE ${PythonExamples})
    configure_file(examples/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE})
endforeach()

