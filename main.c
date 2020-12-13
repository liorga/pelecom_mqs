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

#include <stdio.h>
#include "random.h"
#include "pelecom.h"
#include "stopwatch.h"

int main() {


	srand(time(NULL));
	double ra = 0;
	printf("hello world\n");
	ra = urand01();
	printf("%lf\n",ra);
	return 0;
}
