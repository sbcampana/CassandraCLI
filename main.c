#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "cassandra.h"

char used_keyspace[35];
char used_table[35];

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

CassCluster* create_cluster(){

	CassCluster* cluster = cass_cluster_new();
	cass_cluster_set_contact_points(cluster, "127.0.0.1");
	return cluster;
}

void
print_error(CassFuture* future){
	const char* message;
	size_t message_length;
	cass_future_error_message(future, &message, &message_length);
	fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

CassError connect_session(CassSession* session, const CassCluster* cluster) {
	CassError rc = CASS_OK;
	CassFuture* future = cass_session_connect(session, cluster);

	cass_future_wait(future);

	rc = cass_future_error_code(future);
	if(rc != CASS_OK) {
		printf("connect session failed\n");
		print_error(future);
	}
	cass_future_free(future);
	printf("connect session done\n");

	return rc;
}


static void
cli_show(CassSession* session){

	printf("You ran show\n");

}

static void
cli_list(CassSession* session){
	printf("you ran list\n");
}

static void
cli_use(CassSession* session){
	printf("you ran use\n");
}

static void
cli_get(CassSession* session){
	printf("you ran get\n");
}

static void
cli_insert(CassSession* session){
	printf("you ran insert\n");
}

void
cli(CassSession* session)
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
			cli_show(session);
			continue;

		}
		if (strcmp(cmd, "list") == 0 || strcmp(cmd, "l") == 0) {
			cli_list(session);
			continue;
		}

		if (strcmp(cmd, "use") == 0 || strcmp(cmd, "u") == 0) {

			nextarg(cmdline, &pos, ".", cmd);
            printf("%s\n", cmd);
			strcpy(used_keyspace, cmd);

			nextarg(cmdline, &pos, " ", cmd);
            char *new_cmd = cmd;
			*new_cmd++;
			printf("%s\n", new_cmd);

			strcpy(used_table, new_cmd);
			// used_table is the table
			// user_key space is the key space

			cli_use(session);
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
	CassCluster* cluster = create_cluster();
	CassSession* session = cass_session_new();
	CassFuture* close_future = NULL;

	if(connect_session(session, cluster) != CASS_OK){
		cass_cluster_free(cluster);
		cass_session_free(session);
		return -1;
	}


	cli(session);

	close_future = cass_session_close(session);

	cass_future_wait(close_future);
	cass_future_free(close_future);

	cass_cluster_free(cluster);
	cass_session_free(session);

	exit(0);
}
