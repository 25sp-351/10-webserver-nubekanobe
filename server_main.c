#include <stdio.h> 
#include <string.h> 
#include <unistd.h>

#include "web_server.h"

#define MAAXIMUM_PORT_NUMBER 65535
#define DEFAULT_PORT_NUMBER 80
#define MAX_COMMAND_LINE_ARGUMENTS 3

// ========================= MAIN FUNCTION ========================= //
// This function parses the command line arguments to assign a port  //
// number and enable verbose mode. It creates a listening socket and //
// accepts incoming connections. It also handles the shutdown of the //
// server.                                                           //
// ================================================================  //

int main(int argc, char* argv[]){

    ServerConfig server_config; // Server configuration structure which will be passed to threads
    server_config.port_number = DEFAULT_PORT_NUMBER; 

    int p_flag = 0;
    int p_arg_index;

    // Check for flags in command line arguments
    // -p <port> to specify the port number
    
    for (int ix = 1; ix < argc; ix++) {
        if (strcmp(argv[ix], "-p") == 0) {
            p_flag = 1;
            p_arg_index = ix + 1; 
        }
    }

    server_config.port_number = DEFAULT_PORT_NUMBER;

    // Check command line arguments for -p and port number
    if (argc > 1){

        if((p_flag == 0) || argc > MAX_COMMAND_LINE_ARGUMENTS || strcmp(argv[1], "-p") != 0){
            printf("Invalid command line arguments\n");
            return 1;
        }

        if(p_flag){

            if(p_arg_index >= argc) {
                printf("Port number not provided\n");
                return 1;
            }

            int parsed_character_count;
            if (sscanf(argv[p_arg_index], "%d%n", &server_config.port_number, &parsed_character_count) != 1 || argv[p_arg_index][parsed_character_count] != '\0' 
                || server_config.port_number > MAAXIMUM_PORT_NUMBER || p_arg_index >= argc ) {
                printf("Failed to provide a valid port\n");
                return 1;
            }
        } 
    }

    int socket_fd = createListeningSocket(server_config.port_number); // Create a listening socket on the specified port

    if (socket_fd < 0) {
        fprintf(stderr, "Failed to create listening socket. Exiting.\n");
        return 1;
    }

    acceptConnections(socket_fd, &server_config); // Accept incoming connections and handle them in separate threads

    // Not reaching this points means the server shut down 
    close(socket_fd); // Close the listening socket
    printf("Closed listening socket\n");
    return 0; 
}