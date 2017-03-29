#include <stdlib.h>
#include <signal.h>
#include "handle_command.h"
#include "builtin_command.h"


int main() {
	char* command = NULL;
	char** arglist = NULL;
	extern char** environ;
	environ2table(environ);

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	while (fflush(stdin), (command = get_command(stdin)) != NULL) {
		if ((arglist = split_command(command)) != NULL) {
			process(arglist);
			free_list(arglist);
		}
		free(command);
	}
	return 0;
}

