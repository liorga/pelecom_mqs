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
	Customer c;
	key_t key_upgrade;
	int msgid_upgrade;
	key_upgrade = ftok("upgrade", PROJ_ID);
	if(key_upgrade == -1){
		perror("key_upgrade1 failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid_upgrade = msgget(key_upgrade, 0666 | IPC_CREAT);
	if(msgid_upgrade == -1){
		perror("msg_upgrade send1 failed\n");
		exit(EXIT_FAILURE);
	}
	int i = 0;
	ssize_t res = 0;
	while(i < 10){
		if (res = msgrcv(msgid_upgrade,&c,sizeof(c),1,0) == -1){
			perror("mgs sent1 failed\n");
			exit(EXIT_FAILURE);
		}
		printf("upgrade customer received is: %d\n", c.c_data.type);
		usleep(1000000);
		i++;
	}
	if (msgctl(msgid_upgrade,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}