

target_include_directories(protolib PUBLIC include)

target_sources(protolib PRIVATE      
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/File.cpp
)

if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.test.cpp
    )
endif()