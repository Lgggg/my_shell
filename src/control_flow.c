#include "control_flow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum state {NORMAL, WANT_THEN, THEN_BLOCK, ELSE_BLOCK};
enum result {SUCCESS, FAIL};

static int current_state = NORMAL;
static int if_result = SUCCESS;
static int process_stat = 0;



int is_control_command(char* cmd) {
	return (strcmp(cmd, "if") == 0 || 
			strcmp(cmd, "then") == 0 || 
			strcmp(cmd, "else") == 0 ||
			strcmp(cmd, "fi") == 0);
}


int execute_control_command(char** arglist) {
	int ret = -1;	//-1 not ok, 0 ok
	char* cmd = arglist[0];
	
	if (strcmp(cmd, "if") == 0) {
		if (current_state != NORMAL) {
			ret = syn_err("if unexpected");
		}
		else {
			process_stat = process(arglist + 1);					//这里要延迟输出的话可以在添加全局变量，标记这命令是if的，在execute步骤中添加判断逻辑，重定向输出到文件,然后输入fi后若等到接受到SIGCHILD信号后输出临时文件内容
			if_result = (process_stat == 0 ? SUCCESS : FAIL);
			current_state = WANT_THEN;
		}
	}
	else if (strcmp(cmd, "then") == 0) {
		if (current_state != WANT_THEN) {
			ret = syn_err("then unexpected");
		}
		else {
			current_state = THEN_BLOCK;
			ret = 0;
		}
	}
	else if (strcmp(cmd, "else") == 0) {
		if (current_state != THEN_BLOCK) {
			ret = syn_err("else unexpected");
		}
		else {
			current_state = ELSE_BLOCK;
			ret = 0;
		}
			
	}
	else if (strcmp(cmd, "fi") == 0) {
		if (current_state != THEN_BLOCK && current_state != ELSE_BLOCK) {
			ret = syn_err("fi unexpected");
		}
		else {
			current_state = NORMAL;
			ret = 0;
		}
	}
	else {
		perror("error in processing this command");
		exit(1);
	}
	return ret;
			
}




int ok_to_execute() {
	int ret = 1;	//1 ok, 0 not ok
	if (current_state == WANT_THEN) {
		syn_err("then expected");
		ret = 0;
	}
	else if (current_state == THEN_BLOCK) {
		if (if_result == SUCCESS) {
			ret = 1;
		}
		else {
			ret = 0;
		}
	}
	else if (current_state == ELSE_BLOCK) {
		if (if_result == FAIL) {
			ret = 1;
		}
		else {
			ret = 0;
		}
	}
	return ret;
}

int syn_err(char* msg) {
	current_state = NORMAL;
	fprintf(stderr, "syntax error:%s\n", msg);
	return -1;
}
