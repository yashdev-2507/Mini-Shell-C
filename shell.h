#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define delimiter " \t\r\n\a"

/* main loop */
void lsh_loop();

/* input.c */
char *read_input();

/* parser.c */
char **parsing(char *input_array);
int arr_size(char **parsed_arr);
int pipe_find(char **parsed_arr);

/* builtins.c */
extern char *builtin_string[];
extern int (*builtin_function_pointer[])(char **);
int builtin_count();
int cd(char **parsed_input);
int help(char **parsed_input);
int exit_lsh(char **args);

/* executor.c */
int execute(char **parsed_arr);
int lsh_launch(char **parsed_arr);
int pipe_parsing(char **parsed_arr, int index, int size);
int pipe_execute(char **left, char **right);

#endif
