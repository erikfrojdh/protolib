

target_include_directories(protolib PUBLIC 
"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(protolib PRIVATE      
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataType.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/File.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NumpyFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NumpyFileHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/RawMasterFile.cpp
)

set(PublicHeaders
    ${PublicHeaders}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/DataType.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/File.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/FileInfo.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/FilePtr.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/FileWrapper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/NumpyFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/NumpyFileHeader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/RawFile.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/RawFileWrappers.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/RawMasterFile.hpp
    PARENT_SCOPE
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