#ifndef __EXECUTE_H
#define __EXECUTE_H
#include "builtin_command.h"

int execute(char** );
int is_background_command(char** );
int exec_in_background(char** );

void run(char** );
void run_with_pipe(char**, int);
void run_with_redirect(char**, int);

#endif
