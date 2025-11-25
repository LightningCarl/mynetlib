include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/usr/include/libxml2)

set(TESTSERVER_SRC
    main.cpp
    serveragent.cpp
)

add_executable(test_server ${TESTSERVER_SRC})
target_link_libraries(test_server base_relay)