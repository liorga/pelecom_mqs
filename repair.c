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
	int thousand = 1000;
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
	int customer_cnt = 0,total_work =0,total_wait = 0;
	printf("time is now from repair: %ld\n",swlap(sw));
	while(flag){
		if (res = msgrcv(msgid_repair,&c,sizeof(c),1,0) == -1){
			perror("mgs sent2 failed\n");
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
			printf("quit arrived to repair\n");
			flag = 0;
			continue;
		}
		printf("%d: repair arrived: %ld started: %ld processed: %d exited: %ld elapse: %ld\n", c.c_data.id,
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
	
	if (msgctl(msgid_repair,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}