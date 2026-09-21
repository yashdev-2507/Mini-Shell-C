#include "shell.h"

char *builtin_string[]={
    "cd",
    "help",
    "exit"
};

int (*builtin_function_pointer[])(char **)={
    &cd,
    &help,
    &exit_lsh
};

int builtin_count(){
    return sizeof(builtin_string)/sizeof(char *);
}

int cd(char **parsed_input){
    if(parsed_input[1]==NULL){
        fprintf(stderr,"lsh: expected argument to \"cd\"\n");

    }
    else{
        if(chdir(parsed_input[1])!=0){
            perror("lsh");
        }
    }
    return 1;
}

int help(char **parsed_input){
    printf("builtin command list\n");
    for(int i = 0; i<builtin_count(); i++){
        printf(" %s\n",builtin_string[i]);
    }
    printf("Enter the command and arguments after it to run\n");
    return 1;
}

int exit_lsh(char **args){
    return 0;
}
