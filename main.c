#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <resolv.h>
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
typedef struct msg
{
	long mtype;
	char word[MAX_SIZE];
} message;

void send_message(message m,int msgid){
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		if (msgsnd(msgid, &m, sizeof(message), 0) == -1) {
			perror("send msg\n");
			exit(EXIT_FAILURE);
		}
	}
}

void rcv_message(message m,int msgid){
	pid_t pid;
	pid = fork();
	if (pid == 0) {
		if (msgrcv(msgid, &m, sizeof(message),1, 0) == -1) {
			perror("send msg\n");
			exit(EXIT_FAILURE);
		}
		printf("the message received is :\n");
		printf(" %s\n",m.word);
	}
}

int main() {

	/**
	 * proccess name keys will be
	 * upgrade
	 * fixing
	 * entry
	 * openNewAccount
	 * exitProccess
	 */
	message m;
	m.mtype = 1;
	
	key_t myKey = ftok("msgtest",17);
	if (myKey == -1){
		perror("key\n");
		exit(EXIT_FAILURE);
	}
	
	int msgid = msgget(myKey,0644 | IPC_CREAT);
	if (msgid == -1){
		perror("msgid\n");
		exit(EXIT_FAILURE);
	}
	char* line = NULL;
	ssize_t bufsize = 0; // have getline allocate a buffer for us
	 while (fgets(m.word,MAX_SIZE,stdin)){
	 	printf("\n");
		send_message(m,msgid);
		rcv_message(m,msgid);
	}
	
	if (msgctl(msgid,IPC_RMID,NULL) == -1){
		perror("msgcntrl\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}
