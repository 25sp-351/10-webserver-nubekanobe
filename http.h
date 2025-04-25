#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

#define METHOD_LENGTH 8
#define PATH_LENGTH 256
#define PROTOCOL_LENGTH 32

typedef struct {
    int client_fd; 
    char method[METHOD_LENGTH]; 
    char path[PATH_LENGTH]; 
    char protocol[PROTOCOL_LENGTH]; 
} HttpRequest;

typedef struct {
    const char* protocol;
    int status_code;
    const char* status_text;  
    const char* content_type; 
    const char* body;
    size_t body_length;
} HttpResponse;

void initiate_http_request(HttpRequest* request, int client_fd);
int parse_http_request(HttpRequest* request, char* buffer); 
int route_http_request(HttpRequest* request); 
int handle_static_file(HttpRequest* request); 
char* get_content_type(const char* ext); 
int handle_calculation(HttpRequest* request);
void send_response(int client_fd, HttpResponse* res);

#endif
