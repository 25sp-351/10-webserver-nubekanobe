#ifndef WEB_SERVER_H
#define WEB_SERVER_H

typedef struct{
    int port_number;  
} ServerConfig;

int create_listening_socket(int port_number);
void accept_connections(int socket_fd, ServerConfig* server_config);
void* handle_connection(void* arg); 

#endif