#include "shell.h"

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
