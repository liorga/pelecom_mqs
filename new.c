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
#include "random.h"
#include "pelecom.h"
#include "stopwatch.h"


#define PROJ_ID 17

int main(){
	Customer c;
	key_t key_new;
	int msgid_new;
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
	ssize_t res = 0;
	while(i < 10){
		if (res = msgrcv(msgid_new,&c,sizeof(c),1,0) == -1){
			perror("mgs sent3 failed\n");
			exit(EXIT_FAILURE);
		}
		printf("new customer received is: %d\n", c.c_data.type);
		usleep(1000000);
		i++;
	}
	if (msgctl(msgid_new,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}