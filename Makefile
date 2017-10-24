all:
	gcc -o cassandra main.c -lcassandra

clean:
	rm -f cassandra

