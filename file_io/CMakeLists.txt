

target_include_directories(protolib PUBLIC include)

target_sources(protolib PRIVATE      
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NumpyFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NumpyFileHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.cpp
)

if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataType.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/NumpyFile.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/NumpyFileHeader.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.test.cpp
    )
endif()