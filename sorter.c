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
#include <sys/wait.h>
#include "random.h"
#include "pelecom.h"
#include "stopwatch.h"
#define TYPE_NEW      0		// new customer
#define TYPE_UPGRADE  1		// customer wishes to upgrade hardware or program
#define TYPE_REPAIR   2
#define PROJ_ID 17
int main(){
	key_t key,key_upgrade,key_repair,key_new;
	int msgid;
	int msgid_upgrade;
	int msgid_repair;
	int msgid_new;
	Customer c;
	int status;
	//c.c_id = 1;
	
	key = ftok("sort", PROJ_ID);
	if(key == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
	key_upgrade = ftok("upgrade", PROJ_ID);
	if(key_upgrade == -1){
		perror("key_upgrade failed\n");
		exit(EXIT_FAILURE);
	}
	
	key_new = ftok("newCustomer", PROJ_ID);
	if(key_new == -1){
		perror("key_new failed\n");
		exit(EXIT_FAILURE);
	}
	
	key_repair = ftok("repair", PROJ_ID);
	if(key_repair == -1){
		perror("key_repair failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	msgid_new = msgget(key_new, 0666 | IPC_CREAT);
	if(msgid_new == -1){
		perror("msg_new send failed\n");
		exit(EXIT_FAILURE);
	}
	msgid_upgrade = msgget(key_upgrade, 0666 | IPC_CREAT);
	if(msgid_upgrade == -1){
		perror("msg_upgrade send failed\n");
		exit(EXIT_FAILURE);
	}
	msgid_repair = msgget(key_repair, 0666 | IPC_CREAT);
	if(msgid_repair == -1){
		perror("msg_repair send failed\n");
		exit(EXIT_FAILURE);
	}
	pid_t pid,pid1,pid2;
	pid = fork();
	if (pid == 0){
		char *args[]={"cnew",NULL};
		execv(args[0],args);
	}
	pid1 = fork();
	if (pid1 == 0){
		char *args[]={"cupgrade",NULL};
		execv(args[0],args);
	}
	
	pid2 = fork();
	if (pid2 == 0){
		char *args[]={"crepair",NULL};
		execv(args[0],args);
	}
	int i = 0;
	int flag = 1;
	ssize_t res = 0;
	while(flag){
		
		if (msgrcv(msgid,&c,sizeof(c),1,0) == -1){
			perror("mgs sent failed\n");
			exit(EXIT_FAILURE);
		}
		///sort wait use avg sort from pnrand()
		///put to sleep in avg sort time
		
		//printf("the customer number received is: %d\n", c.c_data.type);
		usleep(1000000);
		if(c.c_data.type == TYPE_QUIT){
			printf("im here with quit\n");
			if (msgsnd(msgid_new, &c, sizeof(c), 0) == -1){
				perror("new snd\n");
				exit(EXIT_FAILURE);
			}
			if (msgsnd(msgid_upgrade, &c, sizeof(c), 0) == -1){
				perror("upgrade snd\n");
				exit(EXIT_FAILURE);
			}
			if (msgsnd(msgid_repair, &c, sizeof(c), 0) == -1){
				perror("repair snd\n");
				exit(EXIT_FAILURE);
			}
			flag = 0;
		}
		if (c.c_data.type == TYPE_NEW){
			//printf("new customer\n");
			if (msgsnd(msgid_new, &c, sizeof(c), 0) == -1){
				perror("new snd\n");
				exit(EXIT_FAILURE);
			}
		}
		if (c.c_data.type == TYPE_UPGRADE){
			//printf("customer upgrade\n");
			if (msgsnd(msgid_upgrade, &c, sizeof(c), 0) == -1){
				perror("upgrade snd\n");
				exit(EXIT_FAILURE);
			}
		}
		if (c.c_data.type == TYPE_REPAIR){
			//printf("customer repair\n");
			if (msgsnd(msgid_repair, &c, sizeof(c), 0) == -1){
				perror("repair snd\n");
				exit(EXIT_FAILURE);
			}
		}
		i++;
	}
	
	waitpid(pid,&status,0);
	waitpid(pid1,&status,0);
	waitpid(pid2,&status,0);
	
	
	///
	
	if (msgctl(msgid,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
/*	if (msgctl(msgid_new,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	if (msgctl(msgid_repair,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	if (msgctl(msgid_upgrade,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}*/
}
