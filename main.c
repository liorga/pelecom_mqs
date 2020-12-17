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

/**
 *          workplan
 *  1.i need to create 5 keys
 *  2.then i need to create 5 of message_queues for the services
 *  3.then probably in a while(true) loop(stop.c will send a signal to stop the loop)
 *  4.then create new customer according to the given type by random and send it to the doorman queue
 *  5.then call each service of message_queues
 *  6.then the doorman queue sends the customers to the right service for them
 *  7.then services are doing their job
 *  8.and return to the loop
 */
#define MAX_SIZE 200
#define PROJ_ID 17



void send_message(Customer m,int msgid);
void rcv_message(Customer m,int msgid);
int rand_type();
void clear_msg_queue(int newc_msgid,int repair_msgid,int upgrade_msgid,int lineman_msgid, int quit_msgid);


int main() {

	//srand(time(NULL));
	initrand();//put in all func that usr rand for better randomization
	//messages ids
	int NEWC_msgid;
	int UPGRADE_msgid;
	int REPAIR_msgid;
	int QUIT_msgid;
	int LINEMAN_msgid;
	
	int rand_res;
	int min = 0,max=100;

	Customer c;
	
	
	//creating the keys
	key_t  lineManagerKey = ftok("lineManager",PROJ_ID); //key for new customer queue
	if (lineManagerKey == -1){
		perror("lineManagerKey\n");
		exit(EXIT_FAILURE);
	}
	key_t  newCustomerKey = ftok("newCustomer",PROJ_ID); //key for new customer queue
	if (newCustomerKey == -1){
		perror("newCustomerKey\n");
		exit(EXIT_FAILURE);
	}
	key_t  upgradeKey = ftok("upgrade",PROJ_ID); // key for upgrade customer queue
	if (upgradeKey == -1){
		perror("upgradeKey\n");
		exit(EXIT_FAILURE);
	}
	key_t  repairKey = ftok("repair",PROJ_ID); // key for repair customer queue
	if (repairKey == -1){
		perror("repairKey\n");
		exit(EXIT_FAILURE);
	}
	key_t  quitKey = ftok("quit",PROJ_ID); // key for quit from program queue
	if (quitKey == -1){
		perror("quitKey\n");
		exit(EXIT_FAILURE);
	}
	
	NEWC_msgid = msgget(newCustomerKey,0644 | IPC_CREAT);
	printf("message id is %d\n",NEWC_msgid);
	if (NEWC_msgid == -1){
		perror("msgid\n");
		exit(EXIT_FAILURE);
	}
	
	
	/*char* line = NULL;
	ssize_t bufsize = 0; // have getline allocate a buffer for us
	fgets(m.word,MAX_SIZE,stdin);*/
	while (1){
		initrand();
/*		switch (rand_type) {
			case TYPE_NEW:{
				break;
			}
			case TYPE_UPGRADE:{
				break;
			}
			case TYPE_REPAIR:{
				break;
			}
		}*/
	 	//printf("\n");
	 	rand_res = urand(min,max);
	 	printf("%d\n",rand_res);
	 	usleep(1000000);
		//send_message(c,NEWC_msgid);
		//rcv_message(c,NEWC_msgid);
		if (c.c_id == TYPE_QUIT){
			break;
		}
	}
	
	//clear_msg_queue(NEWC_msgid,REPAIR_msgid,UPGRADE_msgid,LINEMAN_msgid,QUIT_msgid);
	printf("reached here bitch\n");
	return 0;
}



void send_message(Customer c,int msgid){
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		if (msgsnd(msgid, &c, sizeof(c.c_data), 0) == -1) {
			perror("send msg\n");
			exit(EXIT_FAILURE);
		}
	}
	
}

void rcv_message(Customer c,int msgid){
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		if (msgrcv(msgid, &c, sizeof(c.c_data),c.c_id,0 ) == -1) {
			perror("rcv msg\n");
			exit(EXIT_FAILURE);
		}
		printf("the message received is :\n");
		printf(" %ld\n",c.c_id);
		//usleep(1000);
	}
}

void clear_msg_queue(int newc_msgid,int repair_msgid,int upgrade_msgid,int lineman_msgid, int quit_msgid){
	//close all the message queues that has created
	if (msgctl(newc_msgid,IPC_RMID,NULL) == -1){
		perror("newc_msgid\n");
		exit(EXIT_FAILURE);
	}
	if (msgctl(repair_msgid,IPC_RMID,NULL) == -1){
		perror("repair_msgid\n");
		exit(EXIT_FAILURE);
	}
	if (msgctl(upgrade_msgid,IPC_RMID,NULL) == -1){
		perror("upgrade_msgid\n");
		exit(EXIT_FAILURE);
	}
	if (msgctl(lineman_msgid,IPC_RMID,NULL) == -1){
		perror("lineman_msgid\n");
		exit(EXIT_FAILURE);
	}
	if (msgctl(quit_msgid,IPC_RMID,NULL) == -1){
		perror("quit_msgid\n");
		exit(EXIT_FAILURE);
	}
}

/*
int rand_type(){
	initrand();
	return (rand()%100) + 1;
}*/
