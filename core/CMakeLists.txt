target_include_directories(protolib PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)




target_sources(protolib PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/defs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
)

set(PublicHeaders
    ${PublicHeaders}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/defs.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/protolib.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/SimpleQueue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/utils.hpp
    PARENT_SCOPE
)



if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/defs.test.cpp
    )
endif()