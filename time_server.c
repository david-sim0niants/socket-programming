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
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
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

    int option = 0;
    if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY,
                   (void*)&option, sizeof(option)))
    {
        fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO()); 
        return 1;
    }

    option = 1;
    if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR,
                   (void*)&option, sizeof(option)))
    {
        fprintf(stderr, "setsockopt() failed. (%d)\n", GETSOCKETERRNO()); 
        return 1;
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0)
    {
        fprintf(stderr, "listen() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
                                  (struct sockaddr*) &client_address,
                                  &client_len);

    if (!ISVALIDSOCKET(socket_client))
    {
        fprintf(stderr, "accept() failed. (%d)\n", GETSOCKETERRNO());
        return 1;
    }

    printf("Client is connected...");
    char address_buffer[100];
    getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer, 
                sizeof(address_buffer), NULL, 0, NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);
    printf("%.*s", bytes_received, request);

    printf("Sending response...\n");
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";

    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Send %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    printf("Closing connection...\n");
    CLOSESOCKET(socket_client);

    printf("Closing listening socket...\n");
    CLOSESOCKET(socket_listen);

#ifdef _WIN32
    WSACleanup();
#endif

    printf("Finished.\n");
    return 0;
}
