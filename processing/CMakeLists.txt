

target_include_directories(protolib PUBLIC include)

target_sources(protolib PRIVATE      
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pedestal.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/src/apply_calibration.cpp

)

if(USE_TESTS)
    target_sources(tests PRIVATE 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/ClusterFinder.test.cpp

    )
endif()