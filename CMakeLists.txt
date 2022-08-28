cmake_minimum_required(VERSION 3.20)
project(socket-programming C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

add_executable(time_server time_server.c)
add_executable(tcp_serve_chat tcp_serve_chat.c)
add_executable(tcp_toupper_serve tcp_toupper_serve.c)
add_executable(tcp_client tcp_client.c)

add_executable(udp_recvfrom udp_recvfrom.c)
add_executable(udp_sendto udp_sendto.c)
add_executable(udp_toupper_serve udp_toupper_serve.c)
add_executable(udp_client udp_client.c)

if (WIN32)
    target_link_libraries(tcp_client ws2_32.lib)
endif()

