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
    key = ftok("lineManager", PROJ_ID);
    if (key == -1){
        perror("key soter failed\n");
        exit(EXIT_FAILURE);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if(msgid == -1){
        perror("msg get failed\n");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < 5; ++i){
        if (msgrcv(msgid, &c, sizeof(c.c_data), 0, IPC_NOWAIT) == -1) {
            if (errno != ENOMSG) {
                fprintf(stderr, "Message could not be received.\n");
                exit(EXIT_FAILURE);
            }
            usleep(50000);
            if (msgrcv(msgid, &c, sizeof(c.c_data), 0, 0) == -1) {
                fprintf(stderr, "Message could not be received.\n");
                exit(EXIT_FAILURE);
            }
        }
        printf("the customer type is %d\n",c.c_data.type);

    }
	return 0;
}
