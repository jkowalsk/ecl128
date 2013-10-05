/*
 * rand.cc
 *
 * Part of the ecl library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#include <stdlib.h>

#include "ecl/config.h"
#include "rand.h"

int my_rand(unsigned char * buff, int bsize, void * state) {
  int i;
  for (i = 0; i < bsize; i++) {
    buff[i] = rand() & 0xff;
  }
  return bsize;
}

void rand_digit(ecl_digit *d) {
  int i;
  for (i = 0; i < DIGIT_BYTES; i++) {
    *d <<= 8;
    *d |= rand() & 0xFF;
  }
}
