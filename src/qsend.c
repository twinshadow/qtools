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
	fprintf(stderr, "  -n:\tdon't block if message cannot be sent\n");
	fprintf(stderr, "  -e:\texit if message cannot be sent (only useful with -n)\n");
	fprintf(stderr, "  -p N:\tset message priority to N (default: 0)\n");
	fprintf(stderr, "\nSend each line of stdin to POSIX message queue 'QUEUE'\n");
}

int main(int argc, char* argv[]) {
	int exitNoSend = 0;
	int prio = 0;
	int mqFlags = O_WRONLY;
	int c;
	while((c = getopt(argc, argv, "hnep:")) != -1){
		switch(c){
		case 'h':
			printHelp(argv[0]);
			exit(0);
		case 'p':
			prio = atoi(optarg);
			break;
		case 'n':
			mqFlags |= O_NONBLOCK;
			break;
		case 'e':
			exitNoSend = 1;
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
		case ENAMETOOLONG:
			fprintf(stderr, "queue name too long.\n");
			break;
		case ENOENT:
			fprintf(stderr, "queue '%s' does not exist.\n", queueName);
			break;
		case ENOMEM:
		case EINVAL:
		case ENOSPC:
			fprintf(stderr, "reseource limits hit.\n");
		}
		exit(2);
	}
	size_t len = 1024;
	char *line = (char *)calloc(1024, sizeof(char));
	int sz = 0;
	for(sz = getline(&line, &len, stdin); sz > 0; sz = getline(&line, &len, stdin)){
		if(mq_send(q, line, sz-1, prio) != 0){
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
				/* if we're non-blocking and can't send, don't bother. */
				if(exitNoSend != 0){
					free(line);
					mq_close(q);
					exit(3);
				}
				fprintf(stderr, "message dropped\n");
				continue;
			}
			exit(2);
		}
		memset(line, 0, sz);
	}
	free(line);
	mq_close(q);
	return 0;
}
