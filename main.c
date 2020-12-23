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
void upgrade(stopwatch* sw);
void new(stopwatch* sw);
void repair(stopwatch* sw);
void sorter(stopwatch* sw);

key_t key,keyQuit,key_new,key_repair,key_upgrade;
int msgid,msgid_quit,msgid_new,msgid_repair,msgid_upgrade;
Customer c;

int main(int argc ,char* argv[]){
	
	initrand();
	
	///start watch
	int i = 10000;
	int rand_res;
	int min =0,max = 100;
	int flag = 1;
	int status;
	int thousand = 1000;
	//Customer c;
	c.c_id = 1;
	c.c_data.type = 1;
	
	stopwatch sw;
	swstart(&sw);
	
	///todo: function for keys and qeueus to make code more clean and readble
	key = ftok("sort", PROJ_ID);
	if(key == -1){
		perror("key failed");
		exit(EXIT_FAILURE);
	}
	
	keyQuit = ftok("quit", PROJ_ID);
	if(keyQuit == -1){
		perror("key failed");
		exit(EXIT_FAILURE);
	}
	key_upgrade = ftok("upgrade", PROJ_ID);
	if (key_upgrade == -1) {
		
		perror("key_upgrade failed");
		exit(EXIT_FAILURE);
	}
	
	key_new = ftok("newCustomer", PROJ_ID);
	if (key_new == -1) {
		perror("key_new failed");
		exit(EXIT_FAILURE);
	}
	
	key_repair = ftok("repair", PROJ_ID);
	if (key_repair == -1) {
		perror("key_repair failed");
		exit(EXIT_FAILURE);
	}
	
	
	msgid_quit = msgget(keyQuit, 0666 | IPC_CREAT);
	if(msgid_quit == -1){
		perror("msg send failed");
		exit(EXIT_FAILURE);
	}
	
	msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid == -1){
		perror("msg send failed");
		exit(EXIT_FAILURE);
	}
	msgid_new = msgget(key_new, 0666 | IPC_CREAT);
	if (msgid_new == -1) {
		perror("msg_new send failed");
		exit(EXIT_FAILURE);
	}
	msgid_upgrade = msgget(key_upgrade, 0666 | IPC_CREAT);
	if (msgid_upgrade == -1) {
		perror("msg_upgrade send failed");
		exit(EXIT_FAILURE);
	}
	msgid_repair = msgget(key_repair, 0666 | IPC_CREAT);
	if (msgid_repair == -1) {
		perror("msg_repair send failed");
		exit(EXIT_FAILURE);
	}
	
	//
	//
	//
	//
	int pid,pid1,pid2,pid3,pid4;
	pid = fork();
	if (pid == 0){
		long wait_time;
		while (flag){
			if(quit_action(msgid_quit,msgid) == 2){
				flag = 0;
				continue;
			} else {
				rand_res = urand(min, max);
				///activate swlap() for the entry ttime
				c.c_data.enter_time = swlap(&sw);
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
				if (msgsnd(msgid, &c, sizeof(c), 0) == -1) {
					printf(" customer type %d\n",c.c_data.type);
					perror("bla bla line 126");
					printf("%d\n",errno);
					exit(EXIT_FAILURE);
				}
			}
			i++;
		}
		return 0;
	}
	else{
		pid1 = fork();
		if (pid1 == 0){
			sorter(&sw);
			return 0;
		} else{
			pid2 = fork();
			if (pid2 == 0){
				new(&sw);
				return 0;
			} else{
				pid3 = fork();
				if (pid3 == 0){
					upgrade(&sw);
					return 0;
				} else{
					pid4 = fork();
					if (pid4 == 0){
						repair(&sw);
						return 0;
					}
				}
			}
		}
	}
	
		waitpid(pid1,&status,0);
		//printf("pid1 status is : %d\n",WEXITSTATUS(status));
		waitpid(pid2,&status,0);
		//printf("pid2 status is : %d\n",WEXITSTATUS(status));
		waitpid(pid3,&status,0);
		//printf("pid3 status is : %d\n",WEXITSTATUS(status));
		waitpid(pid4,&status,0);
		//printf("pid4 status is : %d\n",WEXITSTATUS(status));
	
//	while (wait(&status) > 0);
	
	
	///make delete func for the queues to make code more clean
	if (msgctl(msgid_quit,IPC_RMID,NULL) == -1){
		perror("clear failed2");
		exit(EXIT_FAILURE);
	}
	if (msgctl(msgid_new, IPC_RMID, NULL) == -1) {
		perror("clear failed34");
		exit(EXIT_FAILURE);
	}
	if (msgctl(msgid_repair, IPC_RMID, NULL) == -1) {
		perror("clear failed23");
		exit(EXIT_FAILURE);
	}
	if (msgctl(msgid_upgrade, IPC_RMID, NULL) == -1) {
		perror("clear failed54");
		exit(EXIT_FAILURE);
	}
	if (msgctl(msgid,IPC_RMID,NULL) == -1){
		perror("clear failed\n");
		exit(EXIT_FAILURE);
	}
	
	return 0;
}


int quit_action(int msgid_quit,int msgid){
	//Customer c;
	if (msgrcv(msgid_quit, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
		if (errno == ENOMSG) {
			//printf("No New Messages\n");
			return 1;
		}
	}
	if(c.c_data.type == TYPE_QUIT) {
		if (msgsnd(msgid, &c,sizeof(c), 0) == -1) {
			perror("msgsnd failed line 169");
			printf("%d\n",errno);
			exit(EXIT_FAILURE);
		}
		//rintf("quit has arrived\n");
		c.c_data.type = TYPE_QUIT;
		return 2;
	}
	
	return 1;
}

void sorter(stopwatch* sw){
		//Customer c;
		int status;
		int thousand = 1000;
		int i = 0;
		int flag = 1;
		ssize_t res = 0;
		long wait_time;
		while (flag) {
			if (msgrcv(msgid, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
				if (errno == ENOMSG){
					continue;
				}
				perror("mgs rcv failed");
				exit(EXIT_FAILURE);
			}
			///sort wait use avg sort from pnrand()
			///put to sleep in avg sort time
			wait_time = pnrand(AVRG_SORT, SPRD_SORT, MIN_SORT) / thousand;
			usleep(wait_time * thousand);
			
			if (c.c_data.type == TYPE_QUIT) {
				//printf("im here with quit\n");
				if (msgsnd(msgid_new, &c, sizeof(c), 0) == -1) {
					perror("new snd");
					exit(EXIT_FAILURE);
				}
				if (msgsnd(msgid_upgrade, &c, sizeof(c), 0) == -1) {
					perror("upgrade snd");
					exit(EXIT_FAILURE);
				}
				if (msgsnd(msgid_repair, &c, sizeof(c), 0) == -1) {
					perror("repair snd");
					exit(EXIT_FAILURE);
				}
				flag = 0;
				continue;
			}
			if (c.c_data.type == TYPE_NEW) {
				//printf("new customer\n");
				if (msgsnd(msgid_new, &c, sizeof(c), 0) == -1) {
					perror("new snd");
					exit(EXIT_FAILURE);
				}
			}
			if (c.c_data.type == TYPE_UPGRADE) {
				//printf("customer upgrade\n");
				if (msgsnd(msgid_upgrade, &c, sizeof(c), 0) == -1) {
					perror("upgrade snd");
					exit(EXIT_FAILURE);
				}
			}
			if (c.c_data.type == TYPE_REPAIR) {
				//printf("customer repair\n");
				if (msgsnd(msgid_repair, &c, sizeof(c), 0) == -1) {
					perror("repair snd");
					exit(EXIT_FAILURE);
				}
			}
			i++;
		}
	
}

void repair(stopwatch* sw){

		//Customer c;
		int thousand = 1000;
		int i = 0;
		int flag = 1;
		ssize_t res = 0;
		int customer_cnt = 0, total_work = 0, total_wait = 0;
		
		while (flag) {
			//printf("customer id: %ld\n");
			if (msgrcv(msgid_repair, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
				if (errno == ENOMSG){
					continue;
				}
				perror("mgs rcv2 failed line 345");
				printf("%d\n",errno);
				exit(EXIT_FAILURE);
			}
			
			if (c.c_data.type == TYPE_QUIT) {
				//printf("quit arrived to repair\n");
				flag = 0;
				continue;
			}
			///start time its swlap()
			c.c_data.start_time = swlap(sw);
			///sleep procces time using prand
			c.c_data.process_time = pnrand(AVRG_UPGRADE, SPRD_UPGRADE, MIN_UPGRADE) / thousand;
			usleep(c.c_data.process_time * thousand);
			c.c_data.exit_time = swlap(sw);
			///as wake up using swlap() for exit time
			
			
			///exit time - entry time = elapsed time
			c.c_data.elapse_time = c.c_data.exit_time - c.c_data.enter_time;
			///countimg customer ++
			customer_cnt++;
			///work time total += exit time - start time
			total_work += c.c_data.exit_time - c.c_data.start_time;
			///wait total += start - entry
			total_wait += c.c_data.start_time - c.c_data.enter_time;
			if (c.c_data.type == TYPE_QUIT) {
				//printf("quit arrived to repair\n");

				flag = 0;
				continue;
			}
			printf("%d: repair arrived: %ld started: %ld processed: %d exited: %ld elapse: %ld\n", c.c_data.id,
			       c.c_data.enter_time,
			       c.c_data.start_time,
			       c.c_data.process_time,
			       c.c_data.exit_time,
			       c.c_data.elapse_time);
			i++;
		}

}

void new(stopwatch* sw){
		//Customer c;
		int thousand = 1000;
		int i = 0;
		int flag = 1;
		int customer_cnt = 0, total_work = 0, total_wait = 0;
		while (flag) {
			if (msgrcv(msgid_new, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
				if (errno == ENOMSG){
					continue;
				}
				perror("mgs rcv3 failed line 425");
				printf("%d\n",errno);
				exit(EXIT_FAILURE);
			}
			
			if (c.c_data.type == TYPE_QUIT) {
				//printf("quit arrived to new\n");
				flag = 0;
				continue;
			}
			///start time its swlap()
			c.c_data.start_time = swlap(sw);
			///sleep procces time using prand
			c.c_data.process_time = pnrand(AVRG_UPGRADE, SPRD_UPGRADE, MIN_UPGRADE) / thousand;
			usleep(c.c_data.process_time * thousand);
			c.c_data.exit_time = swlap(sw);
			///as wake up using swlap() for exit time
			
			///exit time - entry time = elapsed time
			c.c_data.elapse_time = c.c_data.exit_time - c.c_data.enter_time;
			///countimg customer ++
			customer_cnt++;
			///work time total += exit time - start time
			total_work += c.c_data.exit_time - c.c_data.start_time;
			///wait total += start - entry
			total_wait += c.c_data.start_time - c.c_data.enter_time;

			printf("%d: new arrived: %ld started: %ld processed: %d exited: %ld elapse: %ld\n", c.c_data.id,
			       c.c_data.enter_time,
			       c.c_data.start_time,
			       c.c_data.process_time,
			       c.c_data.exit_time,
			       c.c_data.elapse_time);
			i++;
		}

}
void upgrade(stopwatch* sw){
	//activete watch
		//Customer c;
		int thousand = 1000;
		int i = 0;
		int flag = 1;
		ssize_t res = 0;
		int customer_cnt = 0, total_work = 0, total_wait = 0;
		while (flag) {
			if (msgrcv(msgid_upgrade, &c, sizeof(c), 1, IPC_NOWAIT) == -1) {
				if (errno == ENOMSG){
					continue;
				}
				perror("mgs rcv1 failed line 504");
				printf("%d\n",errno);
				exit(EXIT_FAILURE);
			}
			if (c.c_data.type == TYPE_QUIT) {
				//printf("quit arrived to upgrade\n");
				flag = 0;
				continue;
			}
			///start time its swlap()
			c.c_data.start_time = swlap(sw);
			///sleep procces time using prand
			c.c_data.process_time = pnrand(AVRG_UPGRADE, SPRD_UPGRADE, MIN_UPGRADE) / thousand;
			usleep(c.c_data.process_time * thousand);
			c.c_data.exit_time = swlap(sw);
			///as wake up using swlap() for exit time
			
			
			///exit time - entry time = elapsed time
			c.c_data.elapse_time = c.c_data.exit_time - c.c_data.enter_time;
			///countimg customer ++
			customer_cnt++;
			///work time total += exit time - start time
			total_work += c.c_data.exit_time - c.c_data.start_time;
			///wait total += start - entry
			total_wait += c.c_data.start_time - c.c_data.enter_time;
			///then printing
			///when printing divide by 1000
			///printing here customer data

			printf("%d: upgrade arrived: %ld started: %ld processed: %d exited: %ld elapse: %ld\n", c.c_data.id,
			       c.c_data.enter_time,
			       c.c_data.start_time,
			       c.c_data.process_time,
			       c.c_data.exit_time,
			       c.c_data.elapse_time);
			i++;
		}

}