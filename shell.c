#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define delimiter " \t\r\n\a"



void lsh_loop();
char *read_input();
char **parsing(char *);
int execute(char **);
int lsh_launch(char **);
int cd(char **);
int help(char **);
int exit_lsh(char **);


int main(int arg_count, char **arg_arr){
    lsh_loop();
    return(EXIT_SUCCESS);
}

void lsh_loop(){    //loop
    int status;
    char *input_array;
    char **parsed_arr;
    do{
        printf("> ");

        input_array = read_input();

        parsed_arr = parsing(input_array);

        status = execute(parsed_arr);
        free(input_array);
        free(parsed_arr);

    }while(status);

}

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
    for(int i = 0; i<builtin_count; i++){
        if(strcmp(parsed_arr[0],builtin_string[i])==0){
            return builtin_function_pointer[i](parsed_arr);
        }
    }

    
    if(pipe_index == -1){
        return lsh_launch(parsed_arr);
    }

    return pipe_parsing(parsed_arr, pipe_index, position);
}


