# add_library(array STATIC
# ${CMAKE_CURRENT_SOURCE_DIR}/src/Frame.cpp)
# target_include_directories(array PUBLIC include)
# target_link_libraries(array PRIVATE fmt::fmt project_options)
# set_target_properties(array PROPERTIES POSITION_INDEPENDENT_CODE ON)


target_sources(protolib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/Frame.cpp )
target_include_directories(protolib PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set(PublicHeaders 
    ${PublicHeaders}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/DataSpan.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/Frame.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/ImageData.hpp
    PARENT_SCOPE
)




if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/Frame.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageData.test.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataSpan.test.cpp
    )
endif()