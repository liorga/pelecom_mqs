//
// Created by lior on 13/12/2020.
//

#ifndef PELECOM_MQS_RANDOM_H
#define PELECOM_MQS_RANDOM_H

/**
 *  random.h - header file for random number generating routines
 */

void   initrand();
double nrand();
double pnrand( double avg, double std, double min );
double urand( double min, double max );

#endif //PELECOM_MQS_RANDOM_H
