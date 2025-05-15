#include "udp_remote.h"
#include "nums.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>


/* Platform detection */
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET socket_t;
    #define SOCKET_ERROR_VAL INVALID_SOCKET
    #define CLOSE_SOCKET(s) closesocket(s)
    #define INIT_NETWORK() { WSADATA wsa_data; WSAStartup(MAKEWORD(2, 2), &wsa_data); }
    #define CLEANUP_NETWORK() WSACleanup()
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    typedef int socket_t;
    #define SOCKET_ERROR_VAL (-1)
    #define CLOSE_SOCKET(s) close(s)
    #define INIT_NETWORK()
    #define CLEANUP_NETWORK()
#endif

#define UDP_PORT 8080
#define BUFFER_SIZE 1024

static thrd_t udp_thread;
static bool running = false;

/* 
 * UDP Server Functions
 */

static socket_t udp_server_init(int port) {
    socket_t server_socket;
    struct sockaddr_in server_addr;
    int opt = 1;
    
    /* Initialize network if needed (Windows only) */
    INIT_NETWORK();
    
    /* Create UDP socket */
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == SOCKET_ERROR_VAL) {
        perror("Socket creation failed");
        return SOCKET_ERROR_VAL;
    }
    
    /* Set address reuse option */
    #ifdef _WIN32
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    #else
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif

    // struct timeval timeout;
    // 
    // /* Set socket timeout to 10 seconds */
    // timeout.tv_sec = 10;    /* 10 seconds */
    // timeout.tv_usec = 0;    /* 0 microseconds */
    //
    // if (setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    //     perror("setsockopt failed");
    //     return -1;
    // }
    
    /* Set up server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    /* Bind socket to address */
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        CLOSE_SOCKET(server_socket);
        return SOCKET_ERROR_VAL;
    }
    
    printf("UDP Server initialized on port %d\n", port);
    return server_socket;
}

static int udp_server_receive(socket_t server_socket, char* buffer, int buffer_size, 
                        struct sockaddr_in* client_addr, socklen_t* addr_size) {
    int recv_len;
    printf("receiving...\n");
    
    /* Receive message */
    recv_len = recvfrom(server_socket, buffer, buffer_size, 0,
                        (struct sockaddr*)client_addr, addr_size);
    
    if (recv_len < 0) {
        perror("recvfrom failed");
        return -1;
    }
    
    /* Null terminate the received data for string operations */
    buffer[recv_len] = '\0';
    
    return recv_len;
}

static int udp_server_send(socket_t server_socket, const char* buffer, int buffer_size,
                    struct sockaddr_in* client_addr, socklen_t addr_size) {
    int sent_len;
    
    /* Send message back to client */
    sent_len = sendto(server_socket, buffer, buffer_size, 0,
                     (struct sockaddr*)client_addr, addr_size);
    
    if (sent_len < 0) {
        perror("sendto failed");
        return -1;
    }
    
    return sent_len;
}

/*
 * UDP Client Functions
 */

static socket_t udp_client_init(void) {
    socket_t client_socket;
    
    /* Initialize network if needed (Windows only) */
    INIT_NETWORK();
    
    /* Create UDP socket */
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == SOCKET_ERROR_VAL) {
        perror("Socket creation failed");
        return SOCKET_ERROR_VAL;
    }
    
    return client_socket;
}

static int udp_client_send(socket_t client_socket, const char* buffer, int buffer_size,
                   const char* server_ip, int port) {
    struct sockaddr_in server_addr;
    int sent_len;
    
    /* Set up server address */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    /* Convert IP address from text to binary form */
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }
    
    /* Send message to server */
    sent_len = sendto(client_socket, buffer, buffer_size, 0,
                     (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    if (sent_len < 0) {
        perror("sendto failed");
        return -1;
    }
    
    return sent_len;
}

static int udp_client_receive(socket_t client_socket, char* buffer, int buffer_size) {
    int recv_len;
    
    /* Receive response */
    recv_len = recvfrom(client_socket, buffer, buffer_size, 0, NULL, NULL);
    
    if (recv_len < 0) {
        perror("recvfrom failed");
        return -1;
    }
    
    /* Null terminate the received data for string operations */
    buffer[recv_len] = '\0';
    
    return recv_len;
}

static int host_loop(void *arg) { 
    socket_t server_socket;
    struct sockaddr_in client_addr;
    socklen_t addr_size = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int recv_len;
    
    /* Initialize server */
    server_socket = udp_server_init(UDP_PORT);
    if (server_socket == SOCKET_ERROR_VAL) {
        printf("Failed to initialize server\n");
        exit(1);
    }

    running = true;
    
    printf("UDP Echo Server running on port %d\n", UDP_PORT);
    printf("Press Ctrl+C to exit\n");

    while (running) {
        /* Receive data from client */
        recv_len = udp_server_receive(server_socket, buffer, BUFFER_SIZE, &client_addr, &addr_size);
        
        if (recv_len > 0) {
            /* Print received message */
            printf("Received from client: %s\n", buffer);
            char answer[recv_len + 4];
            sprintf(answer, "hey%s", buffer);
            /* Echo back to client */
            udp_server_send(server_socket, answer, recv_len + 4, &client_addr, addr_size);
        }
    }
    /* Cleanup */
    CLOSE_SOCKET(server_socket);
    CLEANUP_NETWORK();
    return 0;
}

static int joiner_loop(void *arg) { 
    socket_t client_socket;
    char buffer[BUFFER_SIZE];
    char server_ip[16] = "127.0.0.1";  /* Default to localhost */
    int recv_len;
    
    /* Initialize client */
    client_socket = udp_client_init();
    if (client_socket == SOCKET_ERROR_VAL) {
        printf("Failed to initialize client\n");
        exit(1);
    }
    
    printf("UDP Client initialized. Connecting to %s:%d\n", server_ip, UDP_PORT);
    printf("Type a message and press Enter to send. Type 'exit' to quit.\n");
    
    /* Client main loop */
    for (;;) {
        /* Get message from user */
        printf("> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        /* Remove newline character */
        buffer[strcspn(buffer, "\n")] = '\0';
        
        /* Check for exit command */
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        
        /* Send message to server */
        if (udp_client_send(client_socket, buffer, strlen(buffer), server_ip, UDP_PORT) < 0) {
            printf("Failed to send message\n");
            continue;
        }
        
        /* Receive response */
        recv_len = udp_client_receive(client_socket, buffer, BUFFER_SIZE);
        if (recv_len > 0) {
            printf("Server response: %s\n", buffer);
        }
    }
    
    /* Cleanup */
    CLOSE_SOCKET(client_socket);
    CLEANUP_NETWORK();
    return 0;
}

void host_udp_server(void) {
    thrd_create(&udp_thread, host_loop, (void *) NULL);
}

void join_udp_server(void) {
    thrd_create(&udp_thread, joiner_loop, (void *) NULL);
}

void close_udp_server(void) {
}


