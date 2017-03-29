#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "handle_command.h"
#include "execute.h"
#include "control_flow.h"
#include "builtin_command.h"

#define BUFSIZE 4096
#define ARGNUM 5


char* get_command(FILE* fp) {
	char* buf = NULL;
	
	printf(">");
	if ((buf = (char*)malloc(BUFSIZE)) == NULL) {
		perror("malloc error for command buf");
		exit(1);
	}
	if((fgets(buf, BUFSIZE, stdin)) == NULL)
		return NULL;
	buf[strlen(buf) - 1] = '\0';
	return buf;
}


char** split_command(char* command) {										//将输入的命令分割开来
	if (command == NULL)
		return NULL;
	
	char** args = NULL;
	if ((args = (char**)malloc(ARGNUM * sizeof(char*))) == NULL) {
		perror("malloc error for args");
		exit(1);
	}
	
	char* cmd = command;
	int capacity = ARGNUM;
	int argnum = 0;

	while (*cmd != '\0') {														
		while (is_delim(*cmd)) cmd++;
		if (*cmd == '\0') break;
		
		if (argnum + 1 >= capacity) {
			if ((args = (char**)realloc(args, (capacity + ARGNUM) * sizeof(char*))) == NULL) {
				perror("realloc error for args");
				exit(1);
			}
			capacity += ARGNUM;
		}

		if ((*cmd == '|' && *(cmd + 1) != '|') ||               //管道、重定向
			*cmd == '<' || 
			*cmd == '>') {
			args[argnum++] = newarg(cmd, 1);
			cmd++;
			continue;
		}

		char* begin = cmd;
		int len = 0;

		while (*cmd != '\0' && !(is_delim(*cmd))) {
			cmd++;
			len++;
		}

		args[argnum++] = newarg(begin, len);
	}


	args[argnum] = NULL;
	//int i;
	//for (i = 0; i < argnum; ++i)
		//printf("%s\n", args[i]);
	return args;
}


char* newarg(char* begin, int len) {
	char* ret = NULL;
	if ((ret = malloc(len + 1)) == NULL) {
		perror("malloc error for newarg");
		exit(1);
	}
	strncpy(ret, begin, len);
	ret[len] = '\0';
	return ret;
}
	

int is_delim(char c) {
	return (c == ' ' || c == '\t');
}


int process(char** arglist) {
	int ret = 0; //0 means success, ohter means fail
	
	if (arglist[0] == NULL) {
		return 0;
	}
	else if (strcmp(arglist[0], "exit") == 0) {
		exit(0);
	}
	else if (is_control_command(arglist[0])) {
		ret = execute_control_command(arglist);
	}
	else if (ok_to_execute()) {
		if (is_builtin_command(arglist)) {
			ret = execute_builtin_command(arglist);
		}
		else {
			ret = execute(arglist);
		}

	}
	
	return ret;
}


void free_list(char** arglist) {
	char** list = arglist;
	while (*list) {
		free(*list++);
	}
	free(arglist);
}
		
		
