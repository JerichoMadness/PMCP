#include <stdlib.h>
#include <stdio.h>

/* Enum for error handling
 *
 * Further errors can be added!
 *
 */

enum _error {
    
    E_SUCCESS = 0,
    E_NULL_POINTER = 1,
    E_WRONG_ARGUMENT_NUMBER = 2,
    E_INVALID_INPUT = 3,
    E_FILE_NOT_FOUND = 4,
    E_UNKNOWN_ERROR = 5    

};

typedef enum _error error_t;

/* Datatype needed to provide readable error messages
 *
 * Consists of a code number and an error message (As a kind of perror alternative)
 *
 */

struct _errordesc {
    int  code;
    char *message;
} errordesc[] = {
    { E_SUCCESS, "No error" },
    { E_NULL_POINTER, "Working with a null pointer" },
    { E_WRONG_ARGUMENT_NUMBER, "Wrong number of arguments provided" },
    { E_INVALID_INPUT, "Invalid input" },
    { E_FILE_NOT_FOUND, "File not found" },
    { E_UNKNOWN_ERROR, "Unknown error!"}
};

/* Function to print out the error message to the user
 *
 * Arguments:
 *
 * errnum = Error number
 *
 * Return argument:
 *
 * Error Message according to the error number
 *
 */


const char *errorString(error_t errnum) {

    int i;
    int size = sizeof(errordesc) / sizeof(struct _errordesc);

    //Fetch error message
    for(i=0;i<size;i++) 
        if(errordesc[i].code == errnum)
            return errordesc[i].message;

    //If error not provided return unknown error
    return errordesc[size-i].message;

}


