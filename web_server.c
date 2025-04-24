#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "web_server.h"
#include "http.h"
#include "error.h"

#define LISTEN_BACKLOG 10
#define BUFFER_SIZE 1024

// Define the struct passed to threads
// This struct holds the client file descriptor and server configuration
typedef struct {
    int client_fd;
    ServerConfig* server_config;
} ThreadStruct; 


// =================== CREATE_LISTENING_SOCKET =================== //
// This function creates a listening socket and binds it to the    // 
// specified port number. It also sets the socket to listen for    // 
// incoming connections                                            // 
// =============================================================== //

int createListeningSocket(int port_number){

    // Socket file descriptor, allows me to bind to a port, listen for connections, and accept connections
    // AF_INET - address family, specifies the protocol family to be used - (IPv4)
    // SOCK_STREAM - socket type, specifies the type of socket to be created (0 is default - TCP)
    
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Struct to hold the server's address information
    struct sockaddr_in socket_address; 
    memset(&socket_address, '\0', sizeof(socket_address)); 
    socket_address.sin_family = AF_INET; // Set the address family to IPv4
    socket_address. sin_addr.s_addr = htonl(INADDR_ANY); // Set the IP address to any available interface   
    socket_address.sin_port = htons(port_number); // Set the port number to the specified value

   
    // Bind the socket to the address 
    printf("Binding to port %d\n", port_number);   
    int returnval = bind(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address)); // Bind the socket to the address and 
    if (returnval < 0) { 
        perror("Error binding to port");
        return -1;
    }

    // Listen for incoming connections, with a backlog of 5
    returnval = listen(socket_fd, LISTEN_BACKLOG); 
    if (returnval < 0) {
        perror("Error listening for incoming connections");
        return -1;
    }

    printf("Listening for incoming connections on port %d\n", port_number);
    return socket_fd; // Return the socket file descriptor
}

// ======================== ACCEPT_CONNECTIONS ======================= // 
// This function accepts incoming connections and creates a new thread // 
// to handle each connection                                           //
// =================================================================== //

void acceptConnections(int socket_fd, ServerConfig* server_config){

    while (1){

        pthread_t thread; 

        struct sockaddr_in client_address; // Struct to hold the client's address information
        socklen_t client_address_len = sizeof(client_address); // Length of the client's address structure

        int client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_len); // Accept a connection from a client
        if (client_fd < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Allocate memory for the thread arguments
        ThreadStruct* thread_struct = malloc(sizeof(ThreadStruct));
        if (!thread_struct) {
            perror("Error allocating memory for thread args");
            close(client_fd);
            continue;
        }

        thread_struct->client_fd = client_fd;
        thread_struct->server_config = server_config;

        printf("Accepted connection from client\n");
        pthread_create(&thread, NULL, handleConnection, (void*)thread_struct); // Create a new thread to handle the connection
    }
}

// =================== HANDLE_CONNECTION =================== //
// This function handles the connection with the client      //
// It reads data from the client and echoes it back          //
// It runs in a separate thread for each client connection   //
// ========================================================= //

void* handleConnection(void* arg){

    ThreadStruct* thread_struct = (ThreadStruct*)arg; 
    int client_fd = thread_struct->client_fd; 
    free(thread_struct); 

    char buffer[BUFFER_SIZE]; // Buffer to hold incoming data
    ssize_t number_of_bytes_read; // Variable to hold the number of bytes received

    printf("Handling connection on %d\n", client_fd); 

    while(1){
        // Read data from client
        number_of_bytes_read = read(client_fd, buffer, sizeof(buffer) - 1); // Read data from the client socket
            
        if (number_of_bytes_read < 0) { // Check for read error
                perror("Error reading from client");
                break; 

        } else if (number_of_bytes_read == 0) { // Check for end of stream
                printf("Client disconnected without sending anything\n");
                break; 

        } else {
                buffer[number_of_bytes_read] = '\0'; // Null-terminate the buffer

                char method[METHOD_LENGTH]; // Buffer to hold the HTTP method
                char path[PATH_LENGTH]; // Buffer to hold the URL
                char protocol[PROTOCOL_LENGTH]; 

                if (parse_http_request(buffer, method, path, protocol)) { 
                    if(!route_http_request(client_fd, method, path, protocol))
                        break;
                } else {
                    send_error_response_code(client_fd, HTTP_ERR_BAD_REQUEST);
                    break; 
                }                 
        }
    }

    close(client_fd); // Close the client socket
    printf("Closed connection on %d\n", client_fd);

    return NULL;
}



