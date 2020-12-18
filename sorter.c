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
	int i,res;
	while(res != -1){
		if (msgrcv(msgid,&c,sizeof(c),1,0) == -1 ){
			perror("mgs sent failed\n");
			exit(EXIT_FAILURE);
		}
		if (c.c_data.type == TYPE_QUIT){
			printf("quit has received\n");
			exit(EXIT_FAILURE);
		} else {
			printf("the customer number received is: %d\n", c.c_data.type);
		}
	}
}
