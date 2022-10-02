cmake_minimum_required(VERSION 3.20)
project(socket-programming C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)

set(TARGET_LIST "")
function(add_executable_tolist EXEC_NAME)
    add_executable(${ARGV})
    list(APPEND TARGET_LIST ${EXEC_NAME})
    set(TARGET_LIST ${TARGET_LIST} PARENT_SCOPE)
endfunction(add_executable_tolist)

add_executable_tolist(time_server time_server.c)
add_executable_tolist(tcp_serve_chat tcp_serve_chat.c)
add_executable_tolist(tcp_toupper_serve tcp_toupper_serve.c)
add_executable_tolist(tcp_client tcp_client.c)

add_executable_tolist(udp_recvfrom udp_recvfrom.c)
add_executable_tolist(udp_sendto udp_sendto.c)
add_executable_tolist(udp_toupper_serve udp_toupper_serve.c)
add_executable_tolist(udp_client udp_client.c)

add_executable_tolist(lookup lookup.c)
add_executable_tolist(dns_query dns_query.c)
add_executable_tolist(web_get web_get.c)

if (WIN32)
    foreach(TARGET_NAME ${TARGET_LIST})
        target_link_libraries(${TARGET_NAME} ws2_32.lib)
    endforeach(TARGET_NAME)
endif()

