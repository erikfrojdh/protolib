


set(ExampleSrc
    e1_read_file.cpp
    e2_read_jungfrau_file.cpp
    e3_just_the_data.cpp
    e4_iterate_file.cpp
    e5_eiger.cpp
    e6_benchmark.cpp
    e7_save_numpy.cpp
    e8_benchmark_pixelsum.cpp
)

foreach(FILE ${ExampleSrc})
    get_filename_component(EXENAME ${FILE} NAME_WE)
    add_executable(${EXENAME} ${FILE})
    target_link_libraries(${EXENAME} fmt::fmt protolib project_options)
    set_target_properties(${EXENAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()

