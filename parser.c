#include "shell.h"

char **parsing(char *input_array){
    int size = 1024;
    int position = 0;

    char **parsed_arr = malloc(sizeof(char *)*size);
    char *token;
    if(!parsed_arr){
        fprintf(stderr, "lsh : allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(input_array,delimiter);
    while(token!= NULL){
        parsed_arr[position]= token;
        if(position>=size){
            size *= 2;

            parsed_arr = realloc(parsed_arr, size*sizeof(char *));
            if(!parsed_arr){
                fprintf(stderr, "lsh : allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        position++;
        token = strtok(NULL, delimiter);

    }

    parsed_arr[position] = NULL;

    return parsed_arr;

}

int arr_size(char **parsed_arr){
    int size = 0;
    for(int i = 0; parsed_arr[i] != NULL; i++){
        size++;
    }
    return size;
}

int pipe_find(char **parsed_arr)
{
    for(int i = 0; parsed_arr[i] != NULL; i++)
    {
        if(strcmp(parsed_arr[i], "|") == 0)
        {
            return i;
        }
    }

    return -1;
}
