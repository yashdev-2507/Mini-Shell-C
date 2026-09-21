#include "shell.h"

int lsh_launch(char **parsed_arr){
    pid_t pid,wait_pid;
    int status;
    pid = fork();
    if(pid == 0){
        if(execvp(parsed_arr[0],parsed_arr)==-1){
            perror("lsh");
            exit(EXIT_FAILURE);
        }

    }
    else if(pid<0){
        perror("lsh");

    }
    else{
        do{
            wait_pid = waitpid(pid,&status,WUNTRACED);
        }while(!WIFEXITED(status)&& !WIFSIGNALED(status));



    }
    return 1;
}

int pipe_parsing(char **parsed_arr, int index, int size){
    char **left_arr = malloc(sizeof(char *)*(index+1));
    char **right_arr = malloc(sizeof(char *)*(size-index+1));
    for(int i = 0; i < index; i++){
        left_arr[i] = parsed_arr[i];
    }
    int r = 0;
    for(int i = index+1; parsed_arr[i] != NULL; i++){
        right_arr[r] = parsed_arr[i];
        r++;
    }
    left_arr[index] = NULL;
    right_arr[r] = NULL;

    return pipe_execute(left_arr,right_arr);
}

int pipe_execute(char **left,char **right){
    pid_t pid1,pid2;
    int status;

    int fd[2];
    if(pipe(fd)<0){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid1 = fork();
    if(pid1==0){

        dup2(fd[1],STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        if(execvp(left[0],left)<0){
            perror("lsh");
            exit(EXIT_FAILURE);
        }

    }
    else if(pid1<0){
        perror("fork");
    }


    pid2 = fork();
    if(pid2 == 0){

        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        if(execvp(right[0],right)<0){
            perror("lsh");
            exit(EXIT_FAILURE);
        }
    }
    else if(pid2<0){
        perror("fork");
    }

    close(fd[0]);
    close(fd[1]);

    do{
        waitpid(pid1, &status,WUNTRACED);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    do{
        waitpid(pid2,&status,WUNTRACED);
    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    return 1;


}

int execute(char **parsed_arr){
    int pipe_index = pipe_find(parsed_arr);
    int position = arr_size(parsed_arr);
    for(int i = 0; i<builtin_count(); i++){
        if(strcmp(parsed_arr[0],builtin_string[i])==0){
            return builtin_function_pointer[i](parsed_arr);
        }
    }


    if(pipe_index == -1){
        return lsh_launch(parsed_arr);
    }

    return pipe_parsing(parsed_arr, pipe_index, position);
}
