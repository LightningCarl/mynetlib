include_directories(${CMAKE_SOURCE_DIR}/include)

set(TESTCLIENT_SRC
     main.cpp
     center.cpp
     clientagent.cpp
)



add_executable(test_client ${TESTCLIENT_SRC})
target_link_libraries(test_client base_relay)