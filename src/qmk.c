#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

void printHelp(char *name) {
	fprintf(stderr, "Usage of %s:\n", name);
	fprintf(stderr, "\t%s [-n] QUEUE\n\n", name);
	fprintf(stderr, "'n' flag makes the utility return an error if QUEUE exists.\n\n");
	fprintf(stderr, "Make POSIX message queue 'QUEUE'\n");
}

int main(int argc, char* argv[]) {
	int numMsgs = 10;
	int mqFlags = O_RDONLY|O_CREAT;
	int c;
	while((c = getopt(argc, argv, "hnm:")) != -1){
		switch(c){
		case 'h':
			printHelp(argv[0]);
			exit(0);
		case 'n':
			mqFlags |= O_EXCL;
			break;
		case 'm':
			numMsgs = atoi(optarg);
			break;
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
	struct mq_attr attr;
	attr.mq_maxmsg = numMsgs;
	attr.mq_msgsize = 8192;
	if(mq_open(queueName, mqFlags, 0660, &attr) == -1){
		fprintf(stderr, "error: %d\n", errno);
		switch(errno){
		case EACCES:
			fprintf(stderr, "permissions in correct or name invalid.\n");
			break;
		case EEXIST:
			fprintf(stderr, "queue '%s' already exists.\n", queueName);
			exit(3);
		case ENAMETOOLONG:
			fprintf(stderr, "queue name too long.\n");
			break;
		case ENOENT:
			fprintf(stderr, "create no specified. somehow.\n");
			break;
		case ENOMEM:
		case EINVAL:
		case ENOSPC:
			fprintf(stderr, "reseource limits hit.\n");
		}
		exit(2);
	}
	return 0;
}
