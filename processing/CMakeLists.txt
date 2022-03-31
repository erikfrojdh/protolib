

target_include_directories(protolib PUBLIC 
"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_sources(protolib PRIVATE      
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pedestal.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/apply_calibration.cpp

)

set(PublicHeaders
    ${PublicHeaders}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/calibration.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/ClusterFinder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/conversion_helper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/protolib/pedestal.hpp
    PARENT_SCOPE
)




if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/ClusterFinder.test.cpp

    )
endif()