#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "cassandra.h"

char used_keyspace[35];
char used_table[35];
char key[100];
char val[100];

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

CassIterator* execute_query(CassSession* session, const char *query) {

	char return_arr[100];

	CassError rc = CASS_OK;
	CassFuture* future = NULL;
	CassStatement* statement = cass_statement_new(query, 0);

	// execute
	future = cass_session_execute(session, statement);

	// free statement
	cass_statement_free(statement);

	// check future error
	rc = cass_future_error_code(future);
	if(rc != CASS_OK){
		printf("future error\n");
		print_error(future);
	}

	// get result
	const CassResult* result = cass_future_get_result(future);

	// check result error
	if(result == NULL){
		// Handle Error
		printf("result = NULL");
		cass_future_free(future);
		return NULL;
	}

	// Free Future
	cass_future_free(future);

	// Iterator
	CassIterator* row_iterator = cass_iterator_from_result(result);
	return row_iterator;
}


static void
cli_show(CassSession* session){

	printf("You ran show\n");

	const char *query = "SELECT * FROM system_schema.keyspaces;";
	// write execute_query()
	CassIterator* row_iterator = execute_query(session,query);
	while(cass_iterator_next(row_iterator)) {
		const char *svalue;
		size_t size = sizeof(svalue);
		const CassRow* row = cass_iterator_get_row(row_iterator);
		const CassValue *value = cass_row_get_column_by_name(row, "keyspace_name");
		cass_value_get_string(value, &svalue, &size);
		printf("%s\n", svalue);
	}
	cass_iterator_free(row_iterator);
}

static void
cli_list(CassSession* session){
	printf("you ran list\n");
	// list the table names for the current used_keyspace
}

static void
cli_use(CassSession* session){

	printf("you ran use\n");

	// check if used_keyspace exists and if it doesn't create it
	// else set global variable used_keyspace and used_table
}

static void
cli_get(CassSession* session, char* key){
	printf("you ran get\n");

	char query[500];

	strcpy(query, "SELECT * FROM ");
	strcat(query, used_keyspace);
	strcat(query, ".");
	strcat(query, used_table);
	CassResult* result = execute_query(session, query);
	if(result != NULL){
		CassIterator* row_iterator = cass_iterator_from_result(result);
		while(cass_iterator_next(row_iterator)){
			size_t i = 0;
			const char *svalue = "";
			size_t size;
			const CassRow* row = cass_iterator_get_row(row_iterator);
			const CassValue *value = cass_row_get_column_by_name(row,key);

			CassValueType type = cass_value_type(value);
			if(type == CASS_VALUE_TYPE_BOOLEAN){
				cass_bool_t t;
				cass_value_get_bool(value, &t);
				if(t){
					printf("TRUE\n");
				} else {
					printf("FALSE\n");
				}

			}
			else if(type == CASS_VALUE_TYPE_INT){
				cass_int32_t t;
				cass_value_get_int32(value, &t);
				printf("%d\n", t);

			}
			else if(type == CASS_VALUE_TYPE_DOUBLE){
				cass_double_t t;
				cass_value_get_double(value, &t);
				printf("%f\n", t);
			}
			else if(type == CASS_VALUE_TYPE_TEXT || type == CASS_VALUE_TYPE_ASCII || type == CASS_VALUE_TYPE_VARCHAR){
				const char *s;
				size_t size;
				cass_value_get_string(value, &s, &size);
				printf("%s\n", s);
			}
			else if(type == CASS_VALUE_TYPE_UUID){


			}/*
			else if(type == CASS_VALUE_TYPE_LIST){

			}
			else if(type == CASS_VALUE_TYPE_MAP){

			}
			else if(type == CASS_VALUE_TYPE_BLOB){

			}*/
			else {
				if(cass_value_is_null(value)){
					printf("null");
				}
				else{
					printf("YOUR TYPE IS NOT HANDALABLE");
				}
			}
			printf("%s\n", svalue);
		}
	} else {
		printf("PLEASE CHOOSE A KEY SPACE.");
	}


	// Get the value associated to the key from used_keyspace.used_table

}

static void
cli_insert(CassSession* session, char key[25], char value[25]){
	printf("you ran insert\n");
	char query[500];
	// insert a key and value into the used_keyspace.used_table
	//strcpy(query, )
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
			nextarg(cmdline, &pos, " ", cmd);
			cli_get(session, cmd);
			continue;
		}

		if (strcmp(cmd, "insert") == 0 || strcmp(cmd, "i") == 0) {

			nextarg(cmdline, &pos, " ", cmd);
			strcpy(key,cmd);

			nextarg(cmdline, &pos, " ", cmd);
			strcpy(val, cmd);

			printf("%s\n", key);
			printf("%s\n", val);
			cli_insert(session, key, val);
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
