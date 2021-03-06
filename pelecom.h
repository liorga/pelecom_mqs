//
// Created by lior on 13/12/2020.
//

#ifndef PELECOM_MQS_PELECOM_H
#define PELECOM_MQS_PELECOM_H

/**
 *  pelecom.h - header file for simulation of service center for Pelecom LTD.
 */

/*
 * Customer type: what does the customer wish to accomplish?
 */
#include "stopwatch.h"

#define TYPE_NEW      0		// new customer
#define TYPE_UPGRADE  1		// customer wishes to upgrade hardware or program
#define TYPE_REPAIR   2		// customer wisher to repair hardware
#define TYPE_QUIT     3		// special customer type, used to terminate the simulation

/*
 * Ratio between the populations:
 */
#define POP_NEW     10		// 10% of incoming customers are new
#define POP_UPGRADE 35		// 40% of incoming customers wish to upgrade
#define POP_REPAIR  55		// half of all customers come in to fix broken phones


/*
 * Average service times and spreads for each customer type
 */
#define AVRG_NEW       150000	// Average processing time of new customer
#define SPRD_NEW        25000	// Standard deviation of new customer processing time
#define MIN_NEW         30000	// Minimum time for processing new customer

#define AVRG_UPGRADE    90000	// Same for upgrading customer
#define SPRD_UPGRADE    25000
#define MIN_UPGRADE     30000

#define AVRG_REPAIR    120000 	// Same for repair work
#define SPRD_REPAIR     20000
#define MIN_REPAIR      60000

#define AVRG_ARRIVE     40000	// Same for customer arrival rate
#define SPRD_ARRIVE    100000
#define MIN_ARRIVE          0

#define AVRG_SORT       10000	// Same for the time it takes to decide what the
#define SPRD_SORT       10000	// customer came to the center to do
#define MIN_SORT        5000

/*
 * Customer - structure of the messages on the queues
 */
typedef struct data
{
	int   type;
	int   process_time;
	long  enter_time;
	long  start_time;
	long  exit_time;
	long  elapse_time;
	int id;
} data;


typedef struct customer
{
	long  c_id;
	data  c_data;
} Customer;

typedef struct clerk_data{
    int num_of_customers;
    long elapsed_time;
    int total_wait_time;
    int total_service_time;
    int avrg_service;
    int avrg_wait;
    int type;
}CLdata;

typedef struct clerk{
    long clerk_id;
    CLdata data;
}Clerk;

int quit_action(int msgid_quit,int msgid);
void upgrade(stopwatch* sw);
void new(stopwatch* sw);
void repair(stopwatch* sw);
void sorter(stopwatch* sw);
void arrivel(stopwatch* sw);
int create_message_queue(key_t key);
void queue_remove(int id);
key_t make_key(char* fileName);
void print_clerk_data(Clerk* c);
void print_customer_data(Customer* c);

#endif //PELECOM_MQS_PELECOM_H
