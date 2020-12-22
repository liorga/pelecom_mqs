//
// Created by lior on 18/12/2020.
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
	int thousand = 1000;
	key_t key_new,sharedKey;
	int msgid_new,shmID;
	
	
/*	sharedKey = ftok("shm", PROJ_ID);
	if(sharedKey == -1){
		perror("shared key failed4\n");
		exit(EXIT_FAILURE);
	}
	
	shmID = shmget(sharedKey,sizeof(struct stopwatch*),0644 | IPC_CREAT);
	if(shmID == -1){
		perror("shared memory failed5\n");
		exit(EXIT_FAILURE);
	}
	
	sw = (struct stopwatch*)shmat( shmID, NULL, 0 );*/
	printf("time is now from new : %ld\n",swlap(sw));
	
	key_new = ftok("newCustomer", PROJ_ID);
	if(key_new == -1){
		perror("key_new1 failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid_new = msgget(key_new, 0666 | IPC_CREAT);
	if(msgid_new == -1){
		perror("msg_new send1 failed\n");
		exit(EXIT_FAILURE);
	}
	int i = 0;
	int flag =1;
	ssize_t res = 0;
	int customer_cnt = 0,total_work =0,total_wait = 0;
	while(flag){
		if (msgrcv(msgid_new,&c,sizeof(c),1,0) == -1){
			perror("mgs sent3 failed\n");
			exit(EXIT_FAILURE);
		}
		///start time its swlap()
		c.c_data.start_time = swlap(sw);
		///sleep procces time using prand
		c.c_data.process_time = pnrand(AVRG_UPGRADE,SPRD_UPGRADE,MIN_UPGRADE)/thousand;
		usleep(c.c_data.process_time*thousand);
		c.c_data.exit_time = swlap(sw);
		///as wake up using swlap() for exit time
		
		
		///exit time - entry time = elapsed time
		c.c_data.elapse_time = c.c_data.exit_time - c.c_data.enter_time;
		///countimg customer ++
		customer_cnt++;
		///work time total += exit time - start time
		total_work += c.c_data.exit_time - c.c_data.start_time;
		///wait total += start - entry
		total_wait += c.c_data.start_time - c.c_data.enter_time;
		if (c.c_data.type == TYPE_QUIT){
			printf("quit arrived to new\n");
			if (msgrcv(msgid_new,&c,sizeof(c),1,IPC_NOWAIT) == -1){
				if(errno == ENOMSG){
					flag = 0;
					continue;
				}
			}
			//flag = 0;
			//continue;
		}
		printf("%d: new arrived: %ld started: %ld processed: %d exited: %ld elapse: %ld\n", c.c_data.id,
		       c.c_data.enter_time,
		       c.c_data.start_time,
		       c.c_data.process_time,
		       c.c_data.exit_time,
		       c.c_data.elapse_time);
		i++;
	}
	
	int address;
	address = shmdt(sw);
	if (address == -1){
		perror("shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	if (msgctl(msgid_new,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}