/*
 * rand.h
 *
 * Part of the ecl library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#ifndef RAND_H
#define RAND_H

#include "ecl/config.h"

#ifdef __cplusplus
extern "C" {
#endif

int my_rand(unsigned char * buff, int bsize, void * state);

void rand_digit(ecl_digit *d);

#ifdef __cplusplus
}
#endif

#endif /*RAND_H*/
