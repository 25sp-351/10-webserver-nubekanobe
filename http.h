#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>

#define METHOD_LENGTH 8
#define PATH_LENGTH 256
#define PROTOCOL_LENGTH 32

typedef struct {
    int status_code;
    const char* status_text;  
    const char* content_type; 
    const char* body;
    size_t body_length;
} HttpResponse;

int parse_http_request(char* buffer, char* method, char* path, char* protocol); 
int route_http_request(int client_fd, const char* method, const char* path, char* protocol); 
int handle_static_file(int client_fd, const char* filepath); 
char* get_content_type(const char* ext); 
int handle_calculation(int client_fd, const char* calc_path);
void send_response(int client_fd, HttpResponse* res);

#endif
