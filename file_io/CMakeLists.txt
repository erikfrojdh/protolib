

add_library(file_io STATIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/File.cpp
)

target_include_directories(file_io PUBLIC include)
target_link_libraries(file_io PUBLIC project_options core array fmt::fmt)
set_target_properties(file_io PROPERTIES POSITION_INDEPENDENT_CODE ON)


if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.test.cpp
    )
endif()