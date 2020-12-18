//
// Created by lior on 17/12/2020.
//

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

int main(){
	
	key_t key;
	int msgid;
	Customer c;
	c.c_id = 1;
	c.c_data.type = TYPE_QUIT;
	key = ftok("quit", PROJ_ID);
	if(key == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	if (msgsnd(msgid, &c, sizeof(c), 0) == -1){
		perror("fuck you\n");
		exit(EXIT_FAILURE);
	}
	
	
	return 0;
}