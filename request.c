/* request .c */

#include "request.h"
#include "helpers.h"
#include "application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Request **requests = NULL;
const char *request_file_name = "requests.txt";
unsigned long number_of_requests = 0;
FILE *request_fp = NULL;
unsigned long last_request_id = 0;

Request *get_request(unsigned long request_id)
{
    for (int i = 0; i < number_of_requests; i++)
    {
        if (requests[i]->request_id == request_id)
        {
            return requests[i];
        }
    }
    return NULL;
}

char *get_request_string(Request *request_info)
{
    char request_id_string[ID_LENGTH], user_id_string[ID_LENGTH], request_status[2];
    char *request_info_string = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);
    if (request_info->requested_by == NULL)
    {
        close_application(EXIT_FAILURE, "Some request does not have user. ");
    }

    sprintf(request_id_string, "%lu", request_info->requested_by);
    sprintf(user_id_string, "%lu", request_info->requested_by->user_id);
    sprintf(request_status, "%d", request_info->status);

    strcpy(request_info_string, strcat(request_id_string, DELIMITER));
    request_info_string = strcat(request_info_string, user_id_string);
    request_info_string = strcat(request_info_string, DELIMITER);
    request_info_string = strcat(request_info_string, request_status);
    request_info_string = strcat(request_info_string, DELIMITER);
    request_info_string = strcat(request_info_string, request_info->comment);
    request_info_string = strcat(request_info_string, DELIMITER);
    request_info_string = strcat(request_info_string, request_info->address);

    return request_info_string;
}

Request *get_request_info(char *request_info)
{
    char *token = NULL;
    int COLUMN_TAG = 0;
    char *request_info_copy = strdup(request_info);
    Request *read_request = (Request *)malloc(sizeof(Request));
    User *user = NULL;
    while ((token = tokenize_string(request_info_copy, DELIMITER, &request_info_copy)) != NULL)
    {
        switch (COLUMN_TAG)
        {
        case 0:
            read_request->request_id = atoi(token);
            COLUMN_TAG++;
            free(token);
            break;
        case 1:
            user = get_user(atoi(token));
            free(token);
            if (!user)
            {
                free(read_request);
                free(request_info_copy);
                return NULL;
            }
            read_request->requested_by = user;
            COLUMN_TAG++;
            break;
        case 2:
            read_request->status = atoi(token);
            COLUMN_TAG++;
            free(token);
            break;
        case 3:
            read_request->comment = strdup(token);
            COLUMN_TAG++;
            free(token);
            break;
        case 4:
            read_request->address = strdup(token);
            COLUMN_TAG++;
            free(token);
            break;
        default:
            close_application(EXIT_FAILURE, "Error while parsing user database....");
            break;
        }
    }
    free(request_info_copy);
    if (COLUMN_TAG != 5)
    {
        free(read_request);
    }
    return read_request;
}

Request **read_all_requests_from_file(FILE *fp)
{
    Request **all_requests = NULL;
    Request *read_request = NULL;
    unsigned long request_count = 0;
    char *request_info = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);
    rewind(fp);
    while (fgets(request_info, STR_MAX_LENGTH, fp) != NULL)
    {
        if (strcmp(request_info, "\n") == 0)
            continue;

        read_request = get_request_info(request_info);
        if (read_request == NULL)
        {
            continue;
        }
        all_requests = (Request **)realloc(all_requests, sizeof(Request *) * (1 + request_count));
        all_requests[request_count++] = read_request;
    }
    free(request_info);
    last_request_id = number_of_requests = request_count;
    return all_requests;
}

Request **get_request_of_user(unsigned long user_id, int *request_count)
{
    Request **user_requests = NULL;
    int count = 0;

    for (int i = 0; i < number_of_requests; i++)
    {
        if (requests[i]->requested_by->user_id == user_id)
        {
            user_requests = (Request **)realloc(user_requests, sizeof(Request *) * (1 + count));
            user_requests[count++] = requests[i];
        }
    }
    *request_count = count;
    return user_requests;
}

void replace_request_in_file(FILE *fp, Request *updated_request)
{
    Request *read_request = NULL;
    char *updated_request_string = get_request_string(updated_request);
    char line[STR_MAX_LENGTH];
    FILE *write_fp = fopen("temp.txt", "w");
    rewind(fp);
    while ((fgets(line, STR_MAX_LENGTH, fp)) != NULL)
    {
        if ((read_request = get_request_info(line)) == NULL)
            continue;
        if (read_request->request_id == updated_request->request_id)
        {
            fputs(updated_request_string, write_fp);
            fputs("\n", write_fp);
        }
        else
        {
            fputs(line, write_fp);
        }
        free(read_request);
    }
    fclose(write_fp);
    fclose(fp);
    remove(request_file_name);
    rename("temp.txt", request_file_name);
    request_fp = fopen(request_file_name, "a+");
    free(updated_request_string);
}

char *get_request_status_string(request_status status)
{
    switch (status)
    {
    case PENDING:
        return "Pending";
    case IN_PROCESS:
        return "In Progress";
    case REJECTED:
        return "Rejected";
    default:
        break;
    }
    return "Pending";
}

void release_requests()
{
    if (!requests)
    {
        return;
    }
    for (int i = 0; i < number_of_requests; i++)
    {
        free(requests[i]->comment);
        free(requests[i]->address);
        requests[i]->requested_by = NULL;
    }
    free(requests);
    requests = NULL;
}