#ifndef WEB_SERVER_H
#define WEB_SERVER_H

typedef struct{
    int port_number;  
} ServerConfig;

int createListeningSocket(int port_number);
void acceptConnections(int socket_fd, ServerConfig* server_config);
void* handleConnection(void* arg); 

#endif