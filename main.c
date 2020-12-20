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
#define PROJ_ID 17

/*void sig_handler(int signum);
void quit_action(int msgid_quit,int msgid);*/

int main(int argc ,char* argv[]){
	initrand();
	//signal(SIGINT,sig_handler);
	key_t key,keyQuit;
	int msgid,msgid_quit;
	int i = 0;
	int rand_res;
	int min =0;
	int max = 100;
	int flag = 0;
	int wpid;
	int status;
	Customer c;
	c.c_id = 1;
	c.c_data.type = 1;
	
	key = ftok("sort", PROJ_ID);
	if(key == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
/*	keyQuit = ftok("quit", PROJ_ID);
	if(keyQuit == -1){
		perror("key failed\n");
		exit(EXIT_FAILURE);
	}
	
	msgid_quit = msgget(key, 0666 | IPC_CREAT);
	if(msgid_quit == -1){
		perror("msg send failed\n");
		exit(EXIT_FAILURE);
	}*/
	
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


	while (i < 10){
		//quit_action(msgid_quit,msgid);
		rand_res = urand(min,max);
		if (rand_res <= POP_NEW){
			c.c_data.type = TYPE_NEW;
		}
		if (rand_res > POP_NEW && rand_res <= POP_REPAIR){
			c.c_data.type = TYPE_UPGRADE;
		}
		if (rand_res > POP_REPAIR){
			c.c_data.type = TYPE_REPAIR;
		}
		if (msgsnd(msgid, &c, sizeof(c), IPC_NOWAIT) == -1){
			perror("bla bla\n");
			exit(EXIT_FAILURE);
		}

		if(flag) { // dont really sure about wait
			
			//check if returned -1
			wpid = waitpid(pid, &status, WUNTRACED);
		}
		//c.c_data.type = i;
		i++;
		usleep(1000000);
	}
	
	if (msgctl(msgid,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

/*void sig_handler(int signum){
	//Return type of the handler function should be void
	printf("\nInside handler function\n");
	pid_t pid;
	pid = fork();
	if (pid == 0){
		char *args[]={"stop",NULL};
		execv(args[0],args);
	}
}*/

/*
void quit_action(int msgid_quit,int msgid){
	Customer c;
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		if (msgrcv(msgid_quit, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
			if (errno == ENOMSG) {
				printf("No New Messages\n");
			}
			if (c.c_data.type == TYPE_QUIT) {
				if (msgsnd(msgid, &c, sizeof(c), 0) == -1) {
					perror("fuck you\n");
					exit(EXIT_FAILURE);
				}
				printf("quit has arrived\n");
				c.c_data.type = TYPE_QUIT;
				exit(EXIT_FAILURE);
			}
		}
	}
}*/
