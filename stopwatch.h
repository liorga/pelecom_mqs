//
// Created by lior on 13/12/2020.
//

#ifndef PELECOM_MQS_STOPWATCH_H
#define PELECOM_MQS_STOPWATCH_H

/**
 *  stopwatch.h - header file for millisecond stopwatch
 */

#include <sys/time.h>

typedef struct stopwatch
{
	struct timeval tv;
} stopwatch;

void swstart( stopwatch *sw );
long swlap( stopwatch *sw );

#endif //PELECOM_MQS_STOPWATCH_H
