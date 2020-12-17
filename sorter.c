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

int main(){
	if (msgrcv(LINEMAN_msgid, &c, sizeof(c.c_data),c.c_id,0 ) == -1) {
		perror("rcv msg\n");
		exit(EXIT_FAILURE);
	}
	printf("the message received is :\n");
	printf(" %ld\n",c.c_id);
	return 0;
}
