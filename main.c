#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"



char *readline(char *prompt);

static int tty = 0;

static void
cli_about()
{
	printf("You executed a command!\n");
}

static void
cli_help()
{
	return;
}

void
cli()
{
	char *cmdline = NULL;
	char cmd[BUFSIZE], prompt[BUFSIZE];
	int pos;

	tty = isatty(STDIN_FILENO);
	if (tty)
		cli_about();

	/* Main command line loop */
	for (;;) {
		if (cmdline != NULL) {
			free(cmdline);
			cmdline = NULL;
		}
		memset(prompt, 0, BUFSIZE);
		sprintf(prompt, "cassandra> ");

		if (tty)
			cmdline = readline(prompt);
		else
			cmdline = readline("");

		if (cmdline == NULL)
			continue;

		if (strlen(cmdline) == 0)
			continue;

		if (!tty)
			printf("%s\n", cmdline);

		if (strcmp(cmdline, "?") == 0) {
			cli_help();
			continue;
		}
		if (strcmp(cmdline, "quit") == 0 ||
		    strcmp(cmdline, "q") == 0)
			break;

		memset(cmd, 0, BUFSIZE);
		pos = 0;
		nextarg(cmdline, &pos, " ", cmd);

		if (strcmp(cmd, "about") == 0 || strcmp(cmd, "a") == 0) {
			cli_about();
			continue;

		}

        if (strcmp(cmd, "show") == 0 || strcmp(cmd, "s") == 0) {
            cli_show();
            continue;
        }

        if (strcmp(cmd, "list") == 0 || strcmp(cmd, "l") == 0) {
            cli_list();
            continue;
        }

        // if use but it takes in arguments

        if (strcmp(cmd, "get") == 0 || strcmp(cmd, "g") == 0) {
            cli_get();
            continue;
        }

        // if statement for insert that looks for a key and a value



	}
}

int
main(int argc, char**argv)
{
	cli();
	exit(0);
}
