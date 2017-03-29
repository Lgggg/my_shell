#include "execute.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "environment_variable.h"

int execute(char** arglist) {
	if (arglist[0] == NULL) {
		return 0;
	}

	int pid;	
	int stat;
	if ((pid = fork()) < 0) {			//error
		perror("fork error in execute");
		exit(1);
	}
	else if (pid == 0) {				//child
		environ = table2environ();
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (is_background_command(arglist)) {
			exec_in_background(arglist);
		}
		else {
			run(arglist);
		}
		perror("execvp error");
		exit(1);
	}
	else {								//parent
		if (wait(&stat) == -1) {
			perror("wait error");
			//exit(1);
		}
		return stat; //0 ok , else fail , background cmd always success
	}
}

int is_background_command(char** arglist) {
	int ret = 0;
	int cnt = 0;
	int len;
	while (arglist[cnt] != NULL) {
		cnt++;
	}

	if (cnt == 1) {
		len = strlen(arglist[0]);
		if (len != 1) {
			if (arglist[0][len - 1] == '&') {
				arglist[0][len - 1] = '\0';
				ret = 1;
			}
		}
	}
	else {
		if (strcmp(arglist[cnt - 1], "&") == 0) {
			len = strlen(arglist[cnt - 2]);
			if (arglist[cnt - 2][len - 1] != '&') {
				free(arglist[cnt - 1]);
				arglist[cnt - 1] = NULL;
				ret = 1;
			}
		}
		else {
			len = strlen(arglist[cnt - 1]);
			if (len != 1) {
				if (arglist[cnt - 1][len - 1] == '&') {
					arglist[cnt - 1][len - 1] = '\0';
					ret = 1;
				}
			}
		}
	}
	return ret;
}
			

int exec_in_background(char** arglist){
	int pid;
	printf("run in back ground\n");

	if ((pid = fork()) < 0) {
		perror("fork error in execute");
		exit(1);
	}
	else if (pid > 0) {
		exit(0);   //这里儿子进程直接exit，不保证肯定比孙进程先退出。解决办法：在儿子进程返回孙子进程的pid给父进程，父进程捕捉SIGCHLD，并在信号处理函数中发送信号给孙进程,唤醒孙进程的wait（又或者用利用管道阻塞的原理，孙进程读管道足赛直到父进程在信号处理函数中往管道写)
	}

	run(arglist);
		
	return 0;

}


void run(char** arglist) {
	int num = 0;
	int has_pipe = 0;
	int has_redirect = 0;
	while (arglist[num] != NULL) {
		if (strcmp(arglist[num], "|") ==0) {
			if (num == 0 || arglist[num + 1] == NULL) {
				printf("error command format\n");
				exit(1);
			}
			has_pipe = 1;
			break;
		}

		if (strcmp(arglist[num], ">") ==0 || 
			strcmp(arglist[num], "<") ==0) {
			if (num == 0 || arglist[num + 1] == NULL) {
				printf("error command format\n");
				exit(1);
			}
			has_redirect = 1;
			break;
		}
		num++;
	}
	
	if (has_pipe) {
		run_with_pipe(arglist, num);
	}
	else if (has_redirect) {
		run_with_redirect(arglist, num);
	}
	else {
		execvp(arglist[0], arglist);
	}

}

void run_with_redirect(char** arglist, int num) {
	int fd;
	
	if (strcmp(arglist[num], "<") == 0) {
		fd = open(arglist[num + 1], O_RDONLY);
		if (fd == -1) {
			perror("open error");
			return;
		}
		if (fd != STDIN_FILENO) {
			if (dup2(fd, STDIN_FILENO) != STDIN_FILENO) {
				perror("dup2 error");
				exit(1);
			}
			close(fd);
		}

		while (arglist[num] != NULL) {	
			free(arglist[num]);
			arglist[num++] = NULL;
		}
			
		execvp(arglist[0], arglist);
			
	
	}

	if (strcmp(arglist[num], ">") == 0) {
		fd = creat(arglist[num + 1], 0644);
		if (fd == -1) {
			perror("creat error");
			return;
		}

		if (fd != STDOUT_FILENO) {
			if (dup2(fd, STDOUT_FILENO) != STDOUT_FILENO) {
				perror("dup2 error");
				exit(1);
			}
			close(fd);
		}

		while (arglist[num] != NULL) {	
			free(arglist[num]);
			arglist[num++] = NULL;
		}

		execvp(arglist[0], arglist);
	}
		
}

void run_with_pipe(char** arglist, int num) {
	int pipefd[2];
	int pfd;
	
	if (pipe(pipefd) == -1) {
		perror("pipe error");
		return;
	}

	if ((pfd = fork()) == -1) {
		perror("fork error");
		return;
	}
	else if (pfd == 0) {			//child
		close(pipefd[0]);
		if (pipefd[1] != STDOUT_FILENO) {
			if (dup2(pipefd[1], STDOUT_FILENO) != STDOUT_FILENO) {
				perror("dup2 error2");
				exit(1);
			}
			close(pipefd[1]);
		}
		free(arglist[num]);
		arglist[num] = NULL;
		execvp(arglist[0], arglist);
	}
	else {							//father
		close(pipefd[1]);
		if (pipefd[0] != STDIN_FILENO) {
			if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) {
				perror("dup2 error 1");
				exit(1);
			}
			close(pipefd[0]);
		}
		free(arglist[num]);
		arglist[num] = NULL;
		execvp(arglist[0], arglist);
	}
}
	
