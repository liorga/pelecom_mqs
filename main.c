#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <resolv.h>
#include <errno.h>
#include "random.h"
#include "pelecom.h"
#include "stopwatch.h"
#define PROJ_ID 17

void sig_handler(int signum){
	//Return type of the handler function should be void
	printf("\nInside handler function\n");
	pid_t pid;
	pid = fork();
	if (pid == 0){
		char *args[]={"stop",NULL};
		execv(args[0],args);
	}
}

int main(int argc ,char* argv[]){
	signal(SIGINT,sig_handler);
	key_t key,keyQuit;
	int msgid,msgid_quit;
	Customer c;
	c.c_id = 1;
	c.c_data.type = 1;
	key = ftok("sort", PROJ_ID);
	if(key == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	keyQuit = ftok("quit", PROJ_ID);
	if(keyQuit == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	msgid_quit = msgget(key, 0666 | IPC_CREAT);
	if(msgid_quit == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	int i = 1;
	while (c.c_data.type != TYPE_QUIT){
		if (msgrcv(msgid_quit, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
			if (errno == ENOMSG) {
				printf("No New Messages\n");
			}
			if(c.c_data.type == TYPE_QUIT){
				if (msgsnd(msgid, &c, sizeof(c), 0) == -1){
					perror("fuck you\n");
					exit(EXIT_FAILURE);
				}
				c.c_data.type = TYPE_QUIT;
				continue;
			}
		}
		if (msgsnd(msgid, &c, sizeof(c), 0) == -1){
			perror("fuck you\n");
			exit(EXIT_FAILURE);
		}
		pid_t pid;
		pid = fork();
		if (pid == 0){
			char *args[]={"sorter",NULL};
			execv(args[0],args);
		}
		c.c_data.type = i;
		i++;
		usleep(1000000);
	}

	
	
	return 0;
}