#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include "random.h"
#include "pelecom.h"
#define PROJ_ID 17
#define THOUSAND 1000


key_t key,keyQuit,key_new,key_repair,key_upgrade,key_clerk;
int msgid,msgid_quit,msgid_new,msgid_repair,msgid_upgrade,msgid_clerk;
Customer c;
Clerk repairClerk,upgradeClerk,newClerk;

int main(int argc ,char* argv[]){
	initrand();
    int pid,pid1,pid2,pid3,pid4;
	int status;
	c.c_id = 1;
	c.c_data.type = 1;
	newClerk.clerk_id = 1;
	newClerk.data.type = TYPE_NEW;
	upgradeClerk.clerk_id = 1;
	upgradeClerk.data.type = TYPE_UPGRADE;
	repairClerk.clerk_id = 1;
	repairClerk.data.type = TYPE_REPAIR;

	stopwatch sw; // creating the stopwatch for the whole sim
	swstart(&sw); // starting the watch

	///creating the unique keys for the queues
	key = make_key("sort");
	keyQuit = make_key("quit");
	key_upgrade = make_key("upgrade");
	key_new = make_key("newCustomer");
	key_repair = make_key("repair");
	key_clerk = make_key("clerk");
	
	///creating the queues using he keys
	msgid_quit = create_message_queue(keyQuit);
	msgid = create_message_queue(key);
	msgid_new = create_message_queue(key_new);
	msgid_upgrade = create_message_queue(key_upgrade);
	msgid_repair = create_message_queue(key_repair);
    msgid_clerk = create_message_queue(key_clerk);

    ///starting the arrival and creation of costumers process
    pid = fork();
    if (pid == 0){
        arrivel(&sw);
        return 0;
    } else{
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
    waitpid(pid2,&status,0);
    waitpid(pid3,&status,0);
    waitpid(pid4,&status,0);

    printf("\n");
    printf("Sorter quitting\n\n");
    int i;
    Clerk clerk;
    for ( i = 0; i < 3 ; ++i) {
        if (msgrcv(msgid_clerk, &clerk, sizeof(Clerk), 1, IPC_NOWAIT) == -1) {
            if (errno == ENOMSG) {
                return 1;
            }
        }
        print_clerk_data(&clerk);

    }


	///make delete func for the queues to make code more clean
	queue_remove(msgid);
    queue_remove(msgid_quit);
    queue_remove(msgid_new);
    queue_remove(msgid_upgrade);
    queue_remove(msgid_repair);
    queue_remove(msgid_clerk);

	return 0;
}

void arrivel(stopwatch* sw){
    initrand();
    int i = 10000;
    int rand_res;
    int min =0,max = 100; ///used for randomization of the customers creating
    int flag = 1; /// for while loop
    long wait_time;

    while (flag){
        if(quit_action(msgid_quit,msgid) == 2){
            flag = 0;
            continue;
        } else {
            rand_res = urand(min, max);
            ///activate swlap() for the entry ttime
            c.c_data.enter_time = swlap(sw);
            c.c_data.id = i;
            wait_time =(int)pnrand(AVRG_ARRIVE,SPRD_ARRIVE,MIN_ARRIVE);
            usleep(wait_time);
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
}

void sorter(stopwatch* sw){
    initrand();
    printf("Sorter running\n");
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
		wait_time = pnrand(AVRG_SORT, SPRD_SORT, MIN_SORT) / THOUSAND;
		usleep(wait_time * THOUSAND);

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
    initrand();
	printf("Clerk for repair customers is starting\n");
	long elapsed_time_start = swlap(sw);
	long elapsed_time_end;
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
		c.c_data.process_time = pnrand(AVRG_UPGRADE, SPRD_UPGRADE, MIN_UPGRADE) / THOUSAND;
		usleep(c.c_data.process_time * THOUSAND);
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
			elapsed_time_end = swlap(sw);
			flag = 0;
			continue;
		}
		printf("%d: repair ",c.c_data.id);
		print_customer_data(&c);

	}
	repairClerk.data.num_of_customers = customer_cnt;
	repairClerk.data.avrg_service = total_work/customer_cnt;
	repairClerk.data.avrg_wait = total_wait/customer_cnt;
	repairClerk.data.total_service_time = total_work;
	repairClerk.data.total_wait_time = total_wait;
	repairClerk.data.elapsed_time = elapsed_time_end - elapsed_time_start;
    if (msgsnd(msgid_clerk, &repairClerk, sizeof(Clerk), 0) == -1) {
        printf(" customer type %d\n",c.c_data.type);
        perror("bla bla line 126");
        printf("%d\n",errno);
        exit(EXIT_FAILURE);
    }
}

void new(stopwatch* sw){
    initrand();
	printf("Clerk for new customers is starting\n");
	long elapsed_time_start = swlap(sw);
	long elapsed_time_end;
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
			elapsed_time_end = swlap(sw);
			flag = 0;
			continue;
		}
		///start time its swlap()
		c.c_data.start_time = swlap(sw);
		///sleep procces time using prand
		c.c_data.process_time = pnrand(AVRG_UPGRADE, SPRD_UPGRADE, MIN_UPGRADE) / THOUSAND;
		usleep(c.c_data.process_time * THOUSAND);
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

        printf("%d: new ",c.c_data.id);
        print_customer_data(&c);
	}
    newClerk.data.num_of_customers = customer_cnt;
    newClerk.data.avrg_service = total_work/customer_cnt;
    newClerk.data.avrg_wait = total_wait/customer_cnt;
    newClerk.data.total_service_time = total_work;
    newClerk.data.total_wait_time = total_wait;
    newClerk.data.elapsed_time = elapsed_time_end - elapsed_time_start;
    if (msgsnd(msgid_clerk, &newClerk, sizeof(Clerk), 0) == -1) {
        printf(" customer type %d\n",c.c_data.type);
        perror("bla bla line 126");
        printf("%d\n",errno);
        exit(EXIT_FAILURE);
    }
}

void upgrade(stopwatch* sw){
    initrand();
	printf("Clerk for upgrade customers is starting\n");
	long elapsed_time_start = swlap(sw);
	long elapsed_time_end;
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
			elapsed_time_end = swlap(sw);
			flag = 0;
			continue;
		}
		///start time its swlap()
		c.c_data.start_time = swlap(sw);
		///sleep procces time using prand
		c.c_data.process_time = pnrand(AVRG_UPGRADE, SPRD_UPGRADE, MIN_UPGRADE) / THOUSAND;
		usleep(c.c_data.process_time * THOUSAND);
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

        printf("%d: upgrade ",c.c_data.id);
        print_customer_data(&c);
	}
    upgradeClerk.data.num_of_customers = customer_cnt;
    upgradeClerk.data.avrg_service = total_work/customer_cnt;
    upgradeClerk.data.avrg_wait = total_wait/customer_cnt;
    upgradeClerk.data.total_service_time = total_work;
    upgradeClerk.data.total_wait_time = total_wait;
    upgradeClerk.data.elapsed_time = elapsed_time_end - elapsed_time_start;
    if (msgsnd(msgid_clerk, &upgradeClerk, sizeof(Clerk), 0) == -1) {
        printf(" customer type %d\n",c.c_data.type);
        perror("bla bla line 126");
        printf("%d\n",errno);
        exit(EXIT_FAILURE);
    }

}

int create_message_queue(key_t key){
    int id;
    id = msgget(key, 0666 | IPC_CREAT);
    if(id == -1){
        perror("msg get failed");
        exit(EXIT_FAILURE);
    }
    return id;
}

void queue_remove(int id){
    if (msgctl(id,IPC_RMID,NULL) == -1){
        perror("clear failed2");
        exit(EXIT_FAILURE);
    }
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

key_t make_key(char* fileName){
    key_t newKey;
    newKey = ftok(fileName, PROJ_ID);
    if(newKey == -1){
        perror("key failed");
        exit(EXIT_FAILURE);
    }
    return newKey;
}

void print_clerk_data(Clerk* c){
    if (c->data.type == TYPE_NEW){
        printf("Clerk for new customers is quitting\n");
        printf("Clerk for new customers: processed %d customers\n elapsed: %ld work: %d wait: %d\n per customer: work: %d wait: %d\n\n",c->data.num_of_customers
                ,c->data.elapsed_time
                ,c->data.total_service_time
                ,c->data.total_wait_time
                ,c->data.avrg_service
                ,c->data.avrg_wait);
    }
    if (c->data.type == TYPE_UPGRADE){
        printf("Clerk for upgrade customers is quitting\n");
        printf("Clerk for upgrade customers: processed %d customers\n elapsed: %ld work: %d wait: %d\n per customer: work: %d wait: %d\n\n",c->data.num_of_customers
                ,c->data.elapsed_time
                ,c->data.total_service_time
                ,c->data.total_wait_time
                ,c->data.avrg_service
                ,c->data.avrg_wait);
    }
    if (c->data.type == TYPE_REPAIR){
        printf("Clerk for repair customers is quitting\n");
        printf("Clerk for repair customers: processed %d customers\n elapsed: %ld work: %d wait: %d\n per customer: work: %d wait: %d\n\n",c->data.num_of_customers
                ,c->data.elapsed_time
                ,c->data.total_service_time
                ,c->data.total_wait_time
                ,c->data.avrg_service
                ,c->data.avrg_wait);
    }
}

void print_customer_data(Customer* c){
    printf("arrived: %ld started: %ld processed: %d exited: %ld elapse: %ld\n",
           c->c_data.enter_time,
           c->c_data.start_time,
           c->c_data.process_time,
           c->c_data.exit_time,
           c->c_data.elapse_time);
}
