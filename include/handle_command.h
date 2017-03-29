#ifndef __HANDLE_COMMAND_H
#define __HANDLE_COMMAND_H
#include <stdio.h>

char* get_command(FILE* );
char** split_command(char* );
int is_delim(char );
char* newarg(char* , int );
int process(char** );
void free_list(char** );


#endif
