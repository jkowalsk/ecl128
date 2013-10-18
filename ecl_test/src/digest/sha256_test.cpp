/*
 * @file sha256_test.cpp
 * @author Julien Kowalski
 */


#include <gtest/gtest.h>

#include "config.h"
#include "rand.h"
#include "clock.h"

#include "ecl/config.h"
#include "ecl/types.h"

#include "ecl/digest/sha256.h"


namespace ecl {

using ecl::digest::Sha256;

static sstring sha2_test_buf[3] = {
    "abc",
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
    ""
};

static  unsigned char sha256_test_vector[3][32] = {
    {
        0xBA, 0x78, 0x16, 0xBF, 0x8F, 0x01, 0xCF, 0xEA,
        0x41, 0x41, 0x40, 0xDE, 0x5D, 0xAE, 0x22, 0x23,
        0xB0, 0x03, 0x61, 0xA3, 0x96, 0x17, 0x7A, 0x9C,
        0xB4, 0x10, 0xFF, 0x61, 0xF2, 0x00, 0x15, 0xAD
    },
    {
        0x24, 0x8D, 0x6A, 0x61, 0xD2, 0x06, 0x38, 0xB8,
        0xE5, 0xC0, 0x26, 0x93, 0x0C, 0x3E, 0x60, 0x39,
        0xA3, 0x3C, 0xE4, 0x59, 0x64, 0xFF, 0x21, 0x67,
        0xF6, 0xEC, 0xED, 0xD4, 0x19, 0xDB, 0x06, 0xC1
    },
    {
        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
        0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
        0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55
    }
};

TEST(Digest, Sha256) {
  Sha256 md;
  FixedSizedBuffer<32> test_vector;
  FixedSizedBuffer<32> result;

  for( int i=0; i<3; i++) {
    md.init();
    md.update( sha2_test_buf[i] ) ;
    md.final(&result);
    test_vector = sha256_test_vector[i] ;
    bool eq = (test_vector == result) ;
    ASSERT_TRUE( eq ) ;
  }
}

static unsigned char a1000000_vector[32] = {
    0xcd, 0xc7, 0x6e, 0x5c, 0x99, 0x14, 0xfb, 0x92,
    0x81, 0xa1, 0xc7, 0xe2, 0x84, 0xd7, 0x3e, 0x67,
    0xf1, 0x80, 0x9a, 0x48, 0xa4, 0x97, 0x20, 0x0e,
    0x04, 0x6d, 0x39, 0xcc, 0xc7, 0x11, 0x2c, 0xd0
};

TEST(Digest, Sha256_2) {
  Sha256 md;
  FixedSizedBuffer<32> test_vector;
  FixedSizedBuffer<32> result;

  md.init();
  for( int i=0; i<1000000; i++) {
    md.update( "a" );
  }
  md.final(&result);
  test_vector = a1000000_vector;
  bool eq = (test_vector == result) ;
  ASSERT_TRUE( eq ) ;
}

} //namespace ecl