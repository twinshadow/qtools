#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>

void printHelp(char *name) {
	fprintf(stderr, "Usage of %s:\n\n", name);
	fprintf(stderr, "    %s [OPTIONS] QUEUE\n\n", name);
	fprintf(stderr, "  -h:\tprint this help\n");
	fprintf(stderr, "  -e:\texit when the queue empties\n");
	fprintf(stderr, "\nReceive and print messages from POSIX message queue 'QUEUE'\n");
}

int main(int argc, char* argv[]) {
	int exitEmpty = 0;
	int mqFlags = O_RDONLY|O_NONBLOCK;
	int c;
	while((c = getopt(argc, argv, "he")) != -1){
		switch(c){
		case 'h':
			printHelp(argv[0]);
			exit(0);
		case 'e':
			exitEmpty = 1;
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
	mqd_t q = mq_open(queueName, mqFlags);
	if(q == -1){
		switch(errno){
		case EACCES:
			fprintf(stderr, "permissions in correct or name invalid.\n");
			break;
		case EEXIST:
			fprintf(stderr, "queue '%s' already exists.\n", queueName);
			break;
		case ENAMETOOLONG:
			fprintf(stderr, "queue name too long.\n");
			break;
		case ENOENT:
			fprintf(stderr, "create no specified. somehow.\n");
			break;
		case ENOMEM:
		case EINVAL:
		case ENOSPC:
			fprintf(stderr, "resource limits hit.\n");
		}
		exit(2);
	}

	unsigned int prio;
	struct mq_attr attr;
	mq_getattr(q, &attr);
	char *msg = (char *)calloc(attr.mq_msgsize, sizeof(char));

	for(;;){
		if(mq_receive(q, msg, attr.mq_msgsize, &prio) == -1){
			switch(errno){
			case EBADF:
				fprintf(stderr, "fd was bad.\n");
				break;
			case EINTR:
				fprintf(stderr, "send was interrupted.\n");
				break;
			case ETIMEDOUT:
				fprintf(stderr, "send timed out.\n");
				break;
			case EMSGSIZE:
				fprintf(stderr, "message too large.\n");
				break;
			case EAGAIN:
				if(exitEmpty != 0){
					goto leave;
				}
				continue;
			}
			exit(2);
		}
		printf("%d\t%s\n", prio, msg);
	}
leave:
	free(msg);
	mq_close(q);
	return 0;
}
