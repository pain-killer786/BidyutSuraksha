/* helpers.c */

#include "helpers.h"
#include "application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * tokenize_string(char * source, char const * delimiter, char ** context)
{
    if (source == NULL)
    {
        return NULL;
    }
    char * rest_string = strpbrk(source, delimiter);
    char copy_rest_string[STR_MAX_LENGTH];
    char * token = NULL;
    if (rest_string != NULL)
    {
        strcpy(copy_rest_string, rest_string);
        *rest_string = 0;
        token = strdup(source);
        free(*context);
        *context = strdup(copy_rest_string + 1);
    }
    else
    {
        source[strlen(source) - 1] = '\0';
        token = strdup(source);
        free(*context);
        *context = NULL;
    }
    return token;
}

void flush_buffer()
{
    while(getchar() != '\n');
}