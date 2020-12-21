#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <resolv.h>
#include <errno.h>
#include <wait.h>
#include "random.h"
#include "pelecom.h"
#include "stopwatch.h"
#include <sys/shm.h>
#define PROJ_ID 17



int quit_action(int msgid_quit,int msgid);

int main(int argc ,char* argv[]){
	
	initrand();
	
	///start watch
	key_t key,keyQuit,sharedKey;
	int msgid,msgid_quit,shmID;
	int i = 10000;
	int rand_res;
	int min =0,max = 100;
	int flag = 1;
	int status;
	int thousand = 1000;
	Customer c;
	c.c_id = 1;
	c.c_data.type = 1;
	
	struct stopwatch* sw;
	
	sharedKey = ftok("main.c", PROJ_ID);
	if(sharedKey == -1){
		perror("shared key failed\n");
		exit(EXIT_FAILURE);
	}
	
	shmID = shmget(sharedKey,2048,0644| IPC_CREAT);
	if(shmID == -1){
		perror("shared memory failed1\n");
		exit(EXIT_FAILURE);
	}
	
	sw = (struct stopwatch*)shmat( shmID, NULL, 0 );
	swstart(sw);
	//printf("the time is now from main %ld\n",sw->tv.tv_sec);
	
	key = ftok("sort", PROJ_ID);
	if(key == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
	keyQuit = ftok("quit", PROJ_ID);
	if(keyQuit == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
	
	
	msgid_quit = msgget(keyQuit, 0666 | IPC_CREAT);
	if(msgid_quit == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}
	
	
	pid_t pid;
	pid = fork();
	if (pid == 0){
		char *args[]={"sorter",NULL};
		execv(args[0],args);
	}
	
	long wait_time;
	while (flag){
		if(quit_action(msgid_quit,msgid) == 2){
			flag = 0;
			c.c_id = 2;
			c.c_data.type = TYPE_QUIT;
		} else {
			rand_res = urand(min, max);
			///activate swlap() for the entry ttime
			c.c_data.enter_time = swlap(sw);
			c.c_data.id = i;
			wait_time = pnrand(AVRG_ARRIVE,SPRD_ARRIVE,MIN_ARRIVE)/thousand;
			usleep(wait_time*thousand);
			///pnrand() with arrive data in pelecom.h wait avrg time
			if (rand_res <= POP_NEW) {
				///pnrand() for each type use pelecom header for type procces time
				c.c_data.type = TYPE_NEW;
				c.c_data.process_time = pnrand(AVRG_NEW,SPRD_NEW,MIN_NEW);
			}
			if (rand_res > POP_NEW && rand_res <= POP_REPAIR) {
				c.c_data.type = TYPE_UPGRADE;
				c.c_data.process_time = pnrand(AVRG_UPGRADE,SPRD_UPGRADE,MIN_UPGRADE);
			}
			if (rand_res > POP_REPAIR) {
				c.c_data.type = TYPE_REPAIR;
				c.c_data.process_time = pnrand(AVRG_REPAIR,SPRD_REPAIR,MIN_REPAIR);
			}
		}
		//printf("iam herer\n");
		if (msgsnd(msgid, &c, sizeof(c), 0) == -1) {
			perror("bla bla\n");
			exit(EXIT_FAILURE);
		}
		///use arrive entry time for sleep
		///divide before printing data
		
		i++;
		usleep(10000);
	}
	wait(&status);
	
	
	int address;
	address = shmdt(sw);
	if (address == -1){
		perror("shmdt failed\n");
		exit(EXIT_FAILURE);
	}
/*	if (msgctl(msgid,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}*/
	if (msgctl(msgid_quit,IPC_RMID,NULL) == -1){
		perror("clear failed2\n");
		exit(EXIT_FAILURE);
	}
	if(shmctl(shmID,IPC_RMID,NULL) == -1 ){
		perror("shmctl has failed\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}


int quit_action(int msgid_quit,int msgid){
	Customer c;
	
	if (msgrcv(msgid_quit, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
		if (errno == ENOMSG) {
			//printf("No New Messages\n");
			return 1;
		}
	}
	//printf("data num is %d\n",c.c_data.type);
	if(c.c_data.type == TYPE_QUIT) {
		//printf("i got quit to send to sorter\n");
		if (msgsnd(msgid, &c, sizeof(c), 0) == -1) {
			perror("msgsnd failed\n");
			exit(EXIT_FAILURE);
		}
		printf("quit has arrived\n");
		c.c_data.type = TYPE_QUIT;
		return 2;
	}
	
	return 1;
}