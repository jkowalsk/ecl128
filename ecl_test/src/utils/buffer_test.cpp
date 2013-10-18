#include <gtest/gtest.h>

#include "config.h"
#include "ecl/config.h"
#include "ecl/types.h"

using namespace ecl;

TEST(Buffer, Basic) {
  FixedSizedBuffer<4> a, b, zero;

  for(int i=0; i<4; i++) {
    a[i] = i;
    zero[i] = 0;
  }

  b = a;
  ASSERT_TRUE( a == b ) ;
  b^=b;
  ASSERT_TRUE( zero == b ) ;
}
