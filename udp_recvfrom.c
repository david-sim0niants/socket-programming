#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x600
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif


#ifdef _WIN32
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#ifdef _WIN32
#ifdef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif
#endif


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


int main()
{
#ifdef _WIN32
    WSADATA d;

    if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return 1;
    }
#endif

    printf("Configuring local address...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    printf("Creating socket...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                           bind_address->ai_socktype,
                           bind_address->ai_protocol);

    if (!ISVALIDSOCKET(socket_listen))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    // // setup dual socket aka accept both IPv4 and IPv6 addresses
    // int option = 0;
    // if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY,
    //                (void*)&option, sizeof(option)))
    // {
    //     fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO()); 
    //     return 1;
    // }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    freeaddrinfo(bind_address);

    while (1)
    {
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        char read[1024];
        int bytes_received = recvfrom(socket_listen, read, 1024, 0,
                                      (struct sockaddr *) &client_address,
                                      &client_len);
        if (bytes_received < 0)
            continue;

        printf("Received (%d bytes): %.*s\n",
                bytes_received, bytes_received, read);

        printf("Remote address is: ");
        char address_buffer[100];
        char service_buffer[100];
        getnameinfo((struct sockaddr *) &client_address, client_len,
                    address_buffer, sizeof(address_buffer),
                    service_buffer, sizeof(service_buffer),
                    NI_NUMERICHOST | NI_NUMERICSERV);
        printf("%s %s\n", address_buffer, service_buffer);
    }

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#ifdef _WIN32
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
