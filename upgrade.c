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
	//activete watch
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
	int flag =1;
	ssize_t res = 0;
	while(flag){
		if (msgrcv(msgid_upgrade,&c,sizeof(c),1,0) == -1){
			perror("mgs sent1 failed\n");
			exit(EXIT_FAILURE);
		}
		///start time its swlap()
		///sleep procces time using prand
		///as wake up using swlap() for exit time
		
		
		///exit time - entry time = elapsed time
		///countimg customer ++
		///work time total += exit time - start time
		///wait total += start - entry
		///then printing
		///when printing divide by 1000
		///printing here customer data
		if (c.c_data.type == TYPE_QUIT){
			printf("quit arrived to upgrade\n");
			flag = 0;
			continue;
		}
		printf("upgrade customer received is: %d\n", c.c_data.type);
		usleep(1000000);
		i++;
	}
	
	
	///print here the quit total of clerky
	
	
	if (msgctl(msgid_upgrade,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}