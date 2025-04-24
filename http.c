#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>  

#include "http.h"
#include "error.h"

#define MAX_HEADER_LENGTH 512
#define MAX_PATH_LENGTH 512
#define STATIC_DIR_LENGTH strlen("/static/")
#define CALC_DIR_LENGTH strlen("/calc/")
#define OPERATION_LENGTH 4
#define CALC_PARAMETERS 3
#define CALC_RESPONSE_MAX_CHARACTERS 100
#define ERROR 0
#define SUCCESS 1

// ========== PARSE HTTP REQUEST ============ //
// This function parses the HTTP request      // 
// ========================================== // 

int parse_http_request(char* buffer, char* method, char* path, char* protocol) {
    
    int num_items = sscanf(buffer, "%7s %255s %31s", method, path, protocol);

    if (num_items == 3) {
        if (strncmp(method, "GET", num_items) != 0) {
            return ERROR; // Invalid method
        }
        return SUCCESS; // Valid request
    }
    return ERROR; // Invalid request
}

// ========== ROUTE HTTP REQUEST ============  //
// This function routes the HTTP request       //
// to the appropriate handler based on the URL //
// ==========================================  //

int route_http_request(int client_fd, const char* method, const char* path, char* protocol) {

    if (strlen(path) > PATH_LENGTH) {
        send_error_response_code(client_fd, HTTP_ERR_PATH_TOO_LONG);
        return ERROR; 
    }

    // DEBUG // 
    /*
    printf("Method: %s\n", method); // Print the HTTP method
    printf("Path: %s\n", path); // Print the URL    
    printf("Protocol: %s\n", protocol); // Print the HTTP protocol
    */

    if (strncmp(path, "/static/", STATIC_DIR_LENGTH) == 0) {
        if (!handle_static_file(client_fd, path + STATIC_DIR_LENGTH)) { // skip the leading "/static/"
            return ERROR; 
        }
    } else if (strncmp(path, "/calc/", CALC_DIR_LENGTH) == 0) {
        if (!handle_calculation(client_fd, path + CALC_DIR_LENGTH)) { // skip the leading "/calc/"
            return ERROR;
        }
    } else {
        send_error_response_code(client_fd, HTTP_ERR_NOT_FOUND); 
        return ERROR;
    }

    return SUCCESS; 
}

// ========== HANDLE STATIC FILE ============ //
// This function handles static file requests //        
// ========================================== //

int handle_static_file(int client_fd, const char* filepath) {
    char full_path[MAX_PATH_LENGTH];
    snprintf(full_path, sizeof(full_path), "./static/%s", filepath); 

    FILE* file = fopen(full_path, "rb");
    if (!file) {
        send_error_response_code(client_fd, HTTP_ERR_NOT_FOUND);
        return ERROR; 
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    rewind(file);

    // Allocate buffer for file contents
    char* file_buffer = malloc(filesize);
    if (!file_buffer) {
        fclose(file);
        send_error_response_code(client_fd, HTTP_ERR_UNKOWN);
        return ERROR; 
    }

    fread(file_buffer, 1, filesize, file);
    fclose(file);

    // Determine content type
    const char* ext = strrchr(filepath, '.');
    const char* content_type = get_content_type(ext);

    HttpResponse res = {
        .status_code = 200,
        .status_text = "OK",
        .content_type = content_type,
        .body = file_buffer,
        .body_length = filesize
    };

    send_response(client_fd, &res);
    free(file_buffer);

    return SUCCESS;
}

// ======= GET_CONTENT_TYPE ============  //
// This function returns the content type //
// of the file based on its extension     //  
//  ====================================  //

char* get_content_type(const char* ext) {
    if (!ext) return "application/octet-stream";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".ico") == 0) return "image/x-icon";
    if (strcmp(ext, ".txt") == 0) return "text/plain";
    return "application/octet-stream";
}

// ============== HANDLE CALC =============== //
// This function handles calculations         //        
// ========================================== //

int handle_calculation(int client_fd, const char* calc_path) {
    char operation[OPERATION_LENGTH];
    int num1, num2, result; 
    
    if (sscanf(calc_path, "%3[^/]/%d/%d", operation, &num1, &num2) != CALC_PARAMETERS) {
        send_error_response_code(client_fd, HTTP_ERR_INVALID_CALC_PATH_FORMAT); 
        return ERROR; 
    }

    if (strcmp(operation, "add") == 0) {
        result = num1 + num2;
    } else if (strcmp(operation, "mul") == 0) {
        result = num1 * num2;
    } else if (strcmp(operation, "div") == 0) {
        if (num2 == 0) {
            send_error_response_code(client_fd, HTTP_ERR_DIV_ZERO);
            return ERROR; 
        }
        result = num1 / num2;
    } else {
        send_error_response_code(client_fd, HTTP_ERR_UNSUPPORTED_OPERATION);
        return ERROR; 
    }

    char calc_body[CALC_RESPONSE_MAX_CHARACTERS]; 
    size_t calc_body_length = snprintf(calc_body, sizeof(calc_body), "%d\n", result);

    HttpResponse calc_response = {
        .status_code = 200,
        .status_text = "OK",
        .content_type = "text/plain",
        .body = calc_body,
        .body_length = calc_body_length
    };

    send_response(client_fd, &calc_response); 

    return SUCCESS; 
}

// ================== SEND_RESPONSE ================ // 
// This function sends a response to the client      //
// ================================================= //

void send_response(int client_fd, HttpResponse* response) {

    char header[MAX_HEADER_LENGTH];
    int header_length = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "\r\n",
        response->status_code, response->status_text,
        response->content_type, response->body_length);

    write(client_fd, header, header_length);
    write(client_fd, response->body, response->body_length);
}
