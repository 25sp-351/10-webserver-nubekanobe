#ifndef ERROR_H
#define ERROR_H

#include "http.h"

typedef enum {
    HTTP_ERR_BAD_REQUEST,
    HTTP_ERR_NOT_FOUND,
    HTTP_ERR_DIV_ZERO,
    HTTP_ERR_UNSUPPORTED_OPERATION,
    HTTP_ERR_PATH_TOO_LONG, 
    HTTP_ERR_INVALID_CALC_PATH_FORMAT, 
    HTTP_ERR_UNKOWN
} HttpError;

void send_error_response_code(int client_fd, HttpError error);

#endif