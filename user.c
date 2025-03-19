/* User.c */

#include "user.h"
#include "helpers.h"
#include "application.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * user_file_name = "users.txt";
User ** users = NULL;
unsigned long last_user_id = 0;
FILE * user_fp = NULL;
unsigned long number_of_users = 0;

User * get_user (unsigned long user_id) {
    for (int i = 0; i < number_of_users; i++) {
        if (users[i]->user_id == user_id) {
            return users[i];
        }
    }
    return NULL;
}

char * get_user_string (User * user_info)
{
    char user_id_string[ID_LENGTH];
    char * user_info_string = (char *) malloc(sizeof(char) * STR_MAX_LENGTH);

    sprintf(user_id_string, "%lu", user_info->user_id);
    
    strcpy(user_info_string, strcat(user_id_string, DELIMITER));
    user_info_string=strcat(user_info_string, user_info->user_name);
    user_info_string=strcat(user_info_string, DELIMITER);
    user_info_string=strcat(user_info_string, user_info->user_contact);

    return user_info_string;
}

User * get_user_info (char * user_info)
{
    char * token = NULL;
    int COLUMN_TAG=0;
    char * user_info_copy = strdup(user_info);
    User * read_user = (User *) malloc(sizeof(User));
    while((token = tokenize_string(user_info_copy,DELIMITER, &user_info_copy)) != NULL)
    {
        switch(COLUMN_TAG)
        {
            case 0:
                read_user->user_id = atoi(token);
                COLUMN_TAG++;
                free(token);
                break;
            case 1:
                read_user->user_name = strdup(token);
                COLUMN_TAG++;
                free(token);
                break;
            case 2:
                read_user->user_contact = strdup(token);
                COLUMN_TAG++;
                free(token);
                break;
            default:
                close_application(EXIT_FAILURE, "Error while parsing user database.....");
                break;
        }
    }

    free(user_info_copy);

    if (COLUMN_TAG != 3)
    {
        free(read_user);
    }
    return read_user;
}

User ** read_all_users_from_file(FILE * fp)
{
    User ** all_users = NULL;
    User * read_user = NULL;
    unsigned long user_count = 0;
    char * user_info = (char *)malloc(sizeof(char) * STR_MAX_LENGTH);
    rewind(fp);
    while(fgets(user_info, STR_MAX_LENGTH, fp) != NULL)
    {
        if(strcmp(user_info, "\n") == 0)
        {
            continue;
        }
        read_user = get_user_info(user_info);
        if (read_user == NULL)
        {
            continue;
        }
        all_users = (User **) realloc(all_users, sizeof(User *) * (1+ user_count));
        all_users[user_count++] = read_user;
    }
    free(user_info);
    last_user_id = number_of_users = user_count;
    return all_users;
}

void replace_user_in_file(FILE * fp, User * updated_user)
{
    User * read_user = NULL;
    char * updated_user_string = get_user_string(updated_user);
    char line[STR_MAX_LENGTH];
    FILE * write_fp = fopen("temp.txt", "w");
    rewind(fp);
    while((fgets(line, STR_MAX_LENGTH, fp)) != NULL)
    {
        if ((read_user = get_user_info(line)) == NULL)
        {
            continue;
        }
        if (read_user->user_id == updated_user->user_id)
        {
            fputs(updated_user_string, write_fp);
            fputs("\n", write_fp);
        }
        else
        {
            fputs(line, write_fp);
        }
        free(read_user);
    }
    fclose(write_fp);
    fclose(fp);
    remove(user_file_name);
    rename("temp.txt", user_file_name);
    user_fp = fopen(user_file_name, "a+");
    free(updated_user_string);
}

void release_users()
{
    if(!users)
    {
        return;
    }
    for(int i = 0; i < number_of_users; i++)
    {
        free(users[i]->user_name);
        free(users[i]->user_contact);
    }
    free(users);
    users = NULL;
}