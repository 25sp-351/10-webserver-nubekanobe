#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "error.h"

// ====== SEND ERROR RESPONSE CODE ====== //
// This function prepares and sends an    //
// error response to the client based on  //
// the error type                         //
// ====================================== //


void send_error_response_code(int client_fd, HttpError error) {
    const char* status_title;
    const char* error_body;
    int error_code;

    switch (error) {
        case HTTP_ERR_BAD_REQUEST:
            error_code = 400;
            status_title = "Bad Request";
            error_body = "Invalid HTTP request format.";
            break;
        case HTTP_ERR_NOT_FOUND:
            error_code = 404;
            status_title = "Not Found";
            error_body = "The requested resource was not found.";
            break;
        case HTTP_ERR_DIV_ZERO:
            error_code = 400;
            status_title = "Bad Request";
            error_body = "Division by zero.";
            break;
        case HTTP_ERR_UNSUPPORTED_OPERATION:
            error_code = 400;
            status_title = "Bad Request";
            error_body = "Unsupported operation.";
            break;
        case HTTP_ERR_PATH_TOO_LONG:
            error_code = 414;
            status_title = "Request-URI Too Long";
            error_body = "The request path is too long.";
            break;
        case HTTP_ERR_INVALID_CALC_PATH_FORMAT:
            error_code = 400; 
            status_title = "Bad Request"; 
            error_body = "Invalid calculation path format."; 
            break;  
        case HTTP_ERR_UNKOWN:
        default:
            error_code = 500;
            status_title = "Internal Server Error";
            error_body = "An unknown error occurred.";
            break;
    }

    HttpResponse error_response = {
        .status_code = error_code,
        .status_text = status_title,
        .content_type = "text/plain",
        .body = error_body,
        .body_length = strlen(error_body)
    };

    send_response(client_fd, &error_response);
}
