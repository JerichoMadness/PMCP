#ifndef __ERROR_H__
#define __ERROR_H__

enum _error;

typedef enum _error error_t;

struct _errordesc {
    int  code;
    char *message;
};

const char *errorString(error_t errnum);

#endif
