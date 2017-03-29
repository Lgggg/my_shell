#include "chdir.h"
#include <stdio.h>
#include <unistd.h>


int process_cd(char** arglist, int* ret_of_process) {
	if (strcmp(arglist[0], "cd") != 0) {
		return 0;
	}


	if (arglist[1] != NULL) {
			if (chdir(arglist[1]) != 0) {
				*ret_of_process = 1;
				perror("cd error");
			}
		}
		else {
			*ret_of_process = 1;
			printf("Error format! Usage: cd dirname\n");
		}

	return 1;
}

