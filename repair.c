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
#include <sys/shm.h>
#include "random.h"
#include "pelecom.h"
#include "stopwatch.h"

#define PROJ_ID 17

int main(){
	Customer c;
	struct stopwatch* sw;
	key_t key_repair,sharedKey;
	int msgid_repair,shmID;
	
	sharedKey = ftok("main.c", PROJ_ID);
	if(sharedKey == -1){
		perror("shared key failed\n");
		exit(EXIT_FAILURE);
	}
	
	shmID = shmget(sharedKey,1024,0644| IPC_CREAT);
	if(shmID == -1){
		perror("shared memory failed3\n");
		exit(EXIT_FAILURE);
	}
	
	sw = (struct stopwatch*)shmat( shmID, NULL, 0 );

	
	key_repair = ftok("repair", PROJ_ID);
	if(key_repair == -1){
		perror("key_repair1 failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid_repair = msgget(key_repair, 0666 | IPC_CREAT);
	if(msgid_repair == -1){
		perror("msg_repair send1 failed\n");
		exit(EXIT_FAILURE);
	}
	int i = 0;
	int flag =1;
	ssize_t res = 0;
	printf("time is now from repair: %ld\n",swlap(sw));
	while(flag){
		if (res = msgrcv(msgid_repair,&c,sizeof(c),1,0) == -1){
			perror("mgs sent2 failed\n");
			exit(EXIT_FAILURE);
		}
		if (c.c_data.type == TYPE_QUIT){
			printf("quit arrived to repair\n");
			flag = 0;
			continue;
		}
		printf("repair customer received is: %d\n", c.c_data.type);
		usleep(1000000);
		i++;
	}
	
	int address;
	address = shmdt(sw);
	if (address == -1){
		perror("shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	if (msgctl(msgid_repair,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}