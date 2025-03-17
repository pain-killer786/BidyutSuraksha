/* request.h */

#ifndef request_h
#define request_h   

#include "user.h"
#include <stdio.h>

typedef enum request_status 
{
    PENDING,
    IN_PROCESS,
    REJECTED
} request_status;

char * get_request_status_string(request_status);

typedef struct Request
{
    unsigned long request_id;
    User * requested_by;
    request_status status;
    char * comment;
    char * address;
} Request;

extern const char * request_file_name;
extern Request ** requests;
extern unsigned long number_of_requests;
extern FILE * request_fp;
extern unsigned long last_request_id;
extern Request * get_request (unsigned long request_id);
char * get_request_string(Request * request);
Request * get_request_info(char * request_info_string);
Request ** read_all_requests_from_file(FILE * fp);
void replace_request_in_file(FILE * fp, Request * request);
Request ** get_request_of_user(unsigned long user_id, int * request_count);
void release_requests(void);
#endif /* request_h */