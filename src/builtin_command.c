#include <string.h>
#include <stdio.h>
#include "builtin_command.h"
#include "environment_variable.h"
#include "chdir.h"




int is_builtin_command(char** arglist) {
	if (strcmp(arglist[0], "set") == 0 ||
		strcmp(arglist[0], "unset") == 0 ||
		strcmp(arglist[0], "export") == 0 ||
		( (strchr(arglist[0], '=') != NULL) && (assign(arglist[0]) != 1) ) ||
		strcmp(arglist[0], "read") == 0 ||
		strcmp(arglist[0], "cd") == 0 )
		return 1;
	return 0;
}
		



int execute_builtin_command(char** arglist) {
	int ret_of_process = 0;   //0 means success, 1 means fail

	if (process_environment_variable(arglist, &ret_of_process)) {
		return ret_of_process;
	}

	if (process_read(arglist, &ret_of_process)) {
		return ret_of_process;
	}
	else if (process_cd(arglist, &ret_of_process)) {
		return ret_of_process;
	}

	//handle other builtin command
	return 1;
}

