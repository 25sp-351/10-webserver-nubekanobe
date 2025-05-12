#ifndef ERROR_H
#define ERROR_H

#include "http.h"

#define BAD_REQUEST 400
#define NOT_FOUND 404
#define REQUEST_URI_TOO_LONG 414
#define INTERNAL_SERVER_ERROR 500


typedef enum {
    HTTP_ERR_BAD_REQUEST,
    HTTP_ERR_NOT_FOUND,
    HTTP_ERR_DIV_ZERO,
    HTTP_ERR_UNSUPPORTED_OPERATION,
    HTTP_ERR_PATH_TOO_LONG, 
    HTTP_ERR_INVALID_CALC_PATH_FORMAT,
    HTTP_ERR_FORBIDDEN, 
    HTTP_ERR_UNKOWN
} HttpError;

void send_error_response_code(HttpRequest* request, HttpError error);

#endif