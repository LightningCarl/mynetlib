# 设置包含头文件
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/usr/include/libxml2)

# common
set(COMM_SRC 
    common/agent.cpp
    common/TcpWriter.cpp
    common/epoll.cpp
    common/TcpReader.cpp
    common/tcpagent.cpp
    common/BufferManager.cpp
    common/epollEvent.cpp
    common/ReadCallBack.cpp
    common/socketAddr.cpp
    common/tcpSocket.cpp
    common/agentManager.cpp
    common/log.cpp
    common/logAgentBuffer.cpp
)

add_library(base_relay ${COMM_SRC})