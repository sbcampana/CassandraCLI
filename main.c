#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "cassandra.h"



char *readline(char *prompt);

static int tty = 0;



static int
issep(char *sep, char ch)
{
	int i, len;

	if (sep == NULL)
		return 0;

	for (len = strlen(sep), i = 0; i < len; i++)
		if (ch == sep[i])
			return 1;
	return 0;
}




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


// My Additions



static void
cli_show(){

	printf("You ran show\n");

}

static void
cli_list(){
	printf("you ran list\n");
}

static void
cli_use(){
	printf("you ran use\n");
}

static void
cli_get(){
	printf("you ran get\n");
}

static void
cli_insert(){
	printf("you ran insert\n");
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

		if (strcmp(cmd, "use") == 0 || strcmp(cmd, "u") == 0) {
			cli_use();
			continue;
		}

		if (strcmp(cmd, "get") == 0 || strcmp(cmd, "g") == 0) {
			cli_get();
			continue;
		}

		if (strcmp(cmd, "insert") == 0 || strcmp(cmd, "i") == 0) {
			cli_insert();
			continue;
		}
	}
}

int
main(int argc, char**argv)
{
	cli();
	exit(0);
}
