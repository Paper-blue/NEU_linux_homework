#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFERSIZE 4096
#define COPYMODE 0644

void oops(char*, char*);

// argc for argument count, argv for argument vector
int main (int argc, char** argv) {
	
	int fdSource, fdDestination, n_chars;
	char buf[BUFFERSIZE];

	if (argc != 3) {
		fprintf(stderr, "usage:%s source destination\n", *argv);
		exit(1);
	}

	if ( (fdSource = open(argv[1], O_RDONLY)) == -1) oops("Cannot open", argv[1]);

	if ( (fdDestination = creat(argv[2], COPYMODE)) == -1) oops("Cannot create", argv[2]);
	
	while ( (n_chars = read(fdSource, buf, BUFFERSIZE)) > 0) {
		if (write(fdDestination, buf, n_chars) != n_chars) oops("Write error to", argv[2]);
	}

	if (n_chars == -1) oops("Read error from", argv[1]);
	if (close(fdSource) == -1 || close(fdDestination) == -1) oops("Error closing files", "");
	return 0;
}

void oops (char* s1, char* s2) {
	fprintf(stderr, "Error:%s ", s1);
    perror(s2);
	
	exit(1);
}
