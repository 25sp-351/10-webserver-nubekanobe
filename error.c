#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "error.h"

// ====== SEND ERROR RESPONSE CODE ====== //
// This function prepares and sends an    //
// error response to the client based on  //
// the error type                         //
// ====================================== //

void send_error_response_code(HttpRequest* request, HttpError error) {
    const char* status_title;
    const char* error_body;
    int error_code;
    int client_fd = request->client_fd;

    switch (error) {
        case HTTP_ERR_BAD_REQUEST:
            error_code = BAD_REQUEST;
            status_title = "Bad Request";
            error_body = "400: Invalid HTTP request format.";
            break;
        case HTTP_ERR_NOT_FOUND:
            error_code = NOT_FOUND;
            status_title = "Not Found";
            error_body = "404: The requested resource was not found.";
            break;
        case HTTP_ERR_DIV_ZERO:
            error_code = BAD_REQUEST;
            status_title = "Bad Request";
            error_body = "400: Division by zero.";
            break;
        case HTTP_ERR_UNSUPPORTED_OPERATION:
            error_code = BAD_REQUEST;
            status_title = "Bad Request";
            error_body = "400: Unsupported operation.";
            break;
        case HTTP_ERR_PATH_TOO_LONG:
            error_code = REQUEST_URI_TOO_LONG;
            status_title = "Request-URI Too Long";
            error_body = "414: The request path is too long.";
            break;
        case HTTP_ERR_INVALID_CALC_PATH_FORMAT:
            error_code = BAD_REQUEST; 
            status_title = "Bad Request"; 
            error_body = "400: Invalid calculation path format."; 
            break;  
        case HTTP_ERR_FORBIDDEN:
            error_code = BAD_REQUEST;
            status_title = "Bad Request";
            error_body = "400: Forbidden path.";
            break;
        case HTTP_ERR_UNKOWN:
        default:
            error_code = INTERNAL_SERVER_ERROR;
            status_title = "500: Internal Server Error";
            error_body = "An unknown error occurred.";
            break;
    }

    HttpResponse error_response = {
        .protocol = request->protocol,
        .status_code = error_code,
        .status_text = status_title,
        .content_type = "text/plain",
        .body = error_body,
        .body_length = strlen(error_body)
    };

    send_response(client_fd, &error_response);
}
