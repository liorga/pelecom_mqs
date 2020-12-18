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

int main(int argc ,char* argv[]){
	key_t key;
	int msgid;
	Customer c;
	c.c_id = 1;
	c.c_data.type = 1;
	key = ftok("sort", PROJ_ID);
	if(key == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	int i = 1;
	while (i < 10){
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
	}

	
	
	return 0;
}