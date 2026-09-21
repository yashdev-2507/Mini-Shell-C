#include "shell.h"

char *read_input(){
    int c;
    int size = 1024;
    char *input_array = malloc(sizeof(char)*size);
    int index = 0;
    if(!input_array){
        fprintf(stderr,"lsh: allocation errror");
        exit(EXIT_FAILURE);
    }
    while(1){
        c = getchar();
        if(c == EOF || c == '\n'){
            input_array[index]='\0';
            return input_array;
        }
        else{
            input_array[index]=c;
        }
        index++;

        if(index>=size){
            size += 1024;
            input_array = realloc(input_array,sizeof(char)*size);
            if(!input_array){
                fprintf(stderr,"lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }

        }

    }


}
