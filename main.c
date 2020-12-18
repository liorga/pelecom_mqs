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

int main() {
    key_t key;
    int msgid;
    Customer c;
    c.c_id = 1;
    c.c_data.type = 0;

    key = ftok("lineManager", PROJ_ID);
    if (key == -1){
        perror("key failed\n");
        exit(EXIT_FAILURE);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid == -1){
        perror("msg get failed\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < 5; ++i) {
        if (msgsnd(msgid, &c, sizeof(Customer), 0) == -1) {
            perror("msg send\n");
            printf("%d\n",i);
            exit(EXIT_FAILURE);
        }
        c.c_data.type = i+1;
    }
    pid_t pid;
    pid = fork();
    if (pid == 0){
        char *args[]={"sorter",NULL};
        execv(args[0],args);
    }



	printf("reached here bitch\n");
	return 0;
}




