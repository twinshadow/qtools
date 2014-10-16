#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

void printHelp(char *name) {
	fprintf(stderr, "Usage of %s:\n\n", name);
	fprintf(stderr, "    %s QUEUE\n\n", name);
	fprintf(stderr, "  -h:\tprint this help\n");
	fprintf(stderr, "\nRemove POSIX message queue 'QUEUE'\n");
}

int main(int argc, char* argv[]) {
	int c;
	while((c = getopt(argc, argv, "h")) != -1){
		switch(c){
		case 'h':
			printHelp(argv[0]);
			exit(0);
		}
	}
	if(optind != (argc-1)){
		fprintf(stderr, "no queue name specified\n");
		exit(1);
	}
	char *queueName = argv[optind];
	if(strlen(queueName) <= 1){
		fprintf(stderr, "invalid queue name.\n");
		exit(1);
	}
	if(queueName[0] != '/'){
		fprintf(stderr, "queue name does not start with '/'\n");
		exit(1);
	}
	if(mq_unlink(queueName) == -1){
		fprintf(stderr, "error: %d\n", errno);
		switch(errno){
		case EACCES:
			fprintf(stderr, "you do not have permission to remove this queue.\n");
			break;
		case ENAMETOOLONG:
			fprintf(stderr, "queue name too long.\n");
			break;
		case ENOENT:
			fprintf(stderr, "specified queue does not exist.\n");
		}
		exit(2);
	}
	return 0;
}
