/**
 * Field_test.cc
 *
 * Part of the eclv2 library.
 *
 * Copyright 2013 Julien Kowalski.
 *
 */

#include <gtest/gtest.h>

#include "config.h"
#include "rand.h"
#include "clock.h"

#include "ecl/config.h"
#include "ecl/errcode.h"
#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

using namespace ecl::field;

/** @ingroup Field
 @defgroup GenericField Generic test suite for field implementation.
 The tests in this category aim at verifying the correct common behavior of
 every field implementation.
 @addtogroup GenericField
 @{
 */
template<class Field>
class FieldTest : public testing::Test {
 protected:
  FieldTest() {
    int degre = Field::getExtensionDegre();
    /* Initialize the field GFp with prime */
    switch (degre) {
      case 1:
        field = (Field *) new GFp(
            "b64000000000ff2f2200000085fd5480b0001f44b6b88bf142bc818f95e3e6af");
        break;
      case 2:
        field = (Field *) new Fp2(
            "b64000000000ff2f2200000085fd5480b0001f44b6b88bf142bc818f95e3e6af");
        break;
      case 6:
        field = (Field *) new Fp6(
            "b64000000000ff2f2200000085fd5480b0001f44b6b88bf142bc818f95e3e6af");
        break;
      case 12:
        field = (Field *) new Fp12(
            "b64000000000ff2f2200000085fd5480b0001f44b6b88bf142bc818f95e3e6af");
        break;
      default:
        field = NULL;
        break;
    }
    assert(field != NULL);
    field->set(&one, 1);
    field->set(&zero, 0);
  }

  virtual ~FieldTest() {
    delete field;
    field = NULL;
  }

 public:
  typename GFp::Element gfp_elem;
  Field *field;
  typename Field::Element a, b, c, res1, res2, one, zero;
  typename Field::Double r;
};

// First, define a test fixture class template.  Here we just reuse
// the FieldTest fixture defined earlier:
template<class Field>
class GenericFieldTest : public FieldTest<Field> {
 public:
  GenericFieldTest() {}
  virtual ~GenericFieldTest() {}
};

// Then, declare the test case.  The argument is the name of the test
// fixture, and also the name of the test case (as usual).  The _P
// suffix is for "parameterized" or "pattern".
TYPED_TEST_CASE_P(GenericFieldTest);

// Next, use TYPED_TEST_P(TestCaseName, TestName) to define a test,
// similar to what you do with TEST_F.

TYPED_TEST_P(GenericFieldTest, Set){
this->field->set(&this->a, 1);
ASSERT_TRUE(this->field->isOne(this->a) );
}

/** Tests that if the function result is also an operand, the result is still right.
 The test performs the following operations NB_TESTS times :
 */
TYPED_TEST_P(GenericFieldTest, Target){

for(int i=0; i<NBTESTS; i++) {
  /** Generate two random elements a and b in field */
  do {
    this->field->rand(&this->a, my_rand, NULL);
  }while( this->field->cmp(this->a, this->zero) == 0);
  do {
    this->field->rand(&this->b, my_rand, NULL);
  }while( this->field->cmp(this->b, this->zero) == 0);

  this->field->set(&this->res1, 0);
  this->field->set(&this->res2, 0);

  /** Test  for addition : */
  /** <ul><li> if res := a+b and a := a+b then a == res */
  this->field->add(&this->res1, this->a, this->b);
  this->field->add(&this->a, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );
  /** <li> if res := a+b and b := a+b then b == res */
  this->field->add(&this->res1, this->a, this->b);
  this->field->add(&this->b, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->b, this->res1) );
  /** <li> if res := a+a and a := a+a then a == res */
  this->field->add(&this->res1, this->a, this->a);
  this->field->add(&this->a, this->a, this->a);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );
  /** </ul> */

  /** Test  for substraction : */
  /** <ul><li> if res := a-b and a := a-b then a == res */
  this->field->sub(&this->res1, this->a, this->b);
  this->field->sub(&this->a, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );
  /** <li> if res := a-b and b := a-b then b == res */
  this->field->sub(&this->res1, this->a, this->b);
  this->field->sub(&this->b, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->b,this->res1) );
  /**</ul>*/

  /** Test  for multiplication : */
  /** <ul><li> if res := a*b and a := a*b then a == res */
  this->field->mul(&this->res1, this->a, this->b);
  this->field->mul(&this->a, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );
  /** <li> if res := a*b and b := a*b then b == res */
  this->field->mul(&this->res1, this->a, this->b);
  this->field->mul(&this->b, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->b,this->res1) );

  this->field->mul(&this->res1, this->a, 3);
  this->field->mul(&this->a, this->a, 3);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );
  /**</ul>*/

  /** Test  for inverse : */
  /** <ul><li> if res := a^{-1} and a := a^{-1} then a == res */
  do {
    this->field->rand(&this->a, my_rand, NULL);
  }while( this->field->cmp(this->a, this->zero) == 0);
  do {
    this->field->rand(&this->b, my_rand, NULL);
  }while( this->field->cmp(this->b, this->zero) == 0);
  this->field->inv(&this->res1, this->a);
  this->field->inv(&this->a, this->a);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );
  /**</ul>*/

  /** Test  for division : */
  /** <ul><li> if res := a/b and a := a/b then a == res */
  this->field->div(&this->res1, this->a, this->b);
  this->field->div(&this->a, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->a, this->res1) );
  /** <li> if res := a/b and b := a/b then b == res */
  this->field->div(&this->res1, this->a, this->b);
  this->field->div(&this->b, this->a, this->b);
  ASSERT_EQ(0, this->field->cmp(this->b,this->res1) );
  /**</ul>*/
}
}

/** Tests the properties of the addition in a (commutative) field.
 The test performs the following operations NB_TESTS times :
 */
TYPED_TEST_P(GenericFieldTest, AddSub){
for(int i=0; i<NBTESTS; i++) {
  /** <ul><li> Generate three random elements a, b and c in field */
  this->field->rand(&this->a, my_rand, NULL);
  this->field->rand(&this->b, my_rand, NULL);

  this->field->set(&this->res1, 0);
  this->field->set(&this->res2, 0);

  /** <li> Associativity : verify  that (a+b)+c = a+(b+c) */
  this->field->add(&this->res1, this->a, this->b);
  this->field->add(&this->res1, this->res1, this->c);
  this->field->add(&this->res2, this->b, this->c);
  this->field->add(&this->res2, this->a, this->res2);
  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );

  /** <li> Commutativity : verify  that a+b = b+a */
  this->field->add(&this->res1, this->a, this->b);
  this->field->add(&this->res2, this->b, this->a);
  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );

  /** <li> Presence of neutral element : verify  that a+0 = a */
  this->field->add(&this->res1, this->a, this->zero);
  ASSERT_EQ(0, this->field->cmp(this->res1, this->a) );

  /** <li> Neutral element : verify  that a-a = 0 */
  this->field->sub(&this->res1, this->a, this->a);
  ASSERT_TRUE( this->field->isZero(this->res1) );

  /** <li> Inverse operation: verify  that a+b - b = a */
  this->field->add(&this->res1, this->a, this->b);
  this->field->sub(&this->res1, this->res1, this->b);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );

  /** <li> Verify  that a-b = -(b-a) */
  this->field->sub(&this->res1, this->a, this->b);
  this->field->sub(&this->res2, this->b, this->a);
  this->field->opp(&this->res2, this->res2);
  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );
  /** </ul> */
}

}

/** Tests the properties of the multiplication in a (commutative) field.
 The test performs the following operations NB_TESTS times :
 */
TYPED_TEST_P(GenericFieldTest, MulInv){
for(int i=0; i<NBTESTS; i++) {
  /** <ul><li> Generate 3 random elements a, b, c in field */
  do {
    this->field->rand(&this->a, my_rand, NULL);
  }while( this->field->cmp(this->a, this->zero) == 0);
  do {
    this->field->rand(&this->b, my_rand, NULL);
  }while( this->field->cmp(this->b, this->zero) == 0);
  do {
    this->field->rand(&this->c, my_rand, NULL);
  }while( this->field->cmp(this->c, this->zero) == 0);

  this->field->set(&this->res1, 0);
  this->field->set(&this->res2, 0);

  /** <li> Neutral element : verify  that a*1 = a */
  this->field->mul(&this->res1, this->a, this->one);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );

  /** <li> Additive neutral element : verify  that a*0 = 0 */
  this->field->mul(&this->res1, this->a, this->zero);
  ASSERT_TRUE( this->field->isZero(this->res1) );

  /** <li> Commutativity : verify  that a*b = b*a */
  this->field->mul(&this->res1, this->a, this->b);
  this->field->mul(&this->res2, this->b, this->a);
  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );

  /** <li> Associativity : verify  that (a*b)*c = a*(b*c) */
  this->field->mul(&this->res1, this->a, this->b);
  this->field->mul(&this->res1, this->res1, this->c);
  this->field->mul(&this->res2, this->b, this->c);
  this->field->mul(&this->res2, this->a, this->res2);

  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );

  /** <li> Consistency of the square operation : check (a*a)= a^2 */
  this->field->sqr(&this->res2, this->a);
  this->field->mul(&this->res1, this->a, this->a);
  ASSERT_EQ(0, this->field->cmp(this->res1, this->res2) );

  /** <li> Distributivity: verify  that (a+b) * c = ac + bc */
  this->field->add(&this->res1, this->a, this->b);
  this->field->mul(&this->res1, this->res1, this->c);
  this->field->mul(&this->res2, this->a, this->c);
  this->field->mul(&this->a, this->b, this->c);
  this->field->add(&this->res2, this->res2, this->a);
  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );

  /** <li> Inverse : verify  that a * a^-1 = 1 */
  this->field->inv(&this->res2, this->a);
  this->field->mul(&this->res1, this->a, this->res2);
  ASSERT_EQ(0, this->field->cmp(this->res1, this->one) );

  /** <li> Inverse : verify  that a^2 * a^-1 = a */
  this->field->inv(&this->res2, this->a);
  this->field->mul(&this->res1, this->a, this->a);
  this->field->mul(&this->res1, this->res1, this->res2);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res1) );

  /** <li> Inverse : verify  that (a * b) / b = a */
  this->field->mul(&this->res1, this->a, this->b);
  this->field->div(&this->res2, this->res1, this->b);
  ASSERT_EQ(0, this->field->cmp(this->a,this->res2) );
}
/** </ul> */
}

TYPED_TEST_P(GenericFieldTest, Scalar){
ecl_digit scal;
for(int i=0; i<NBTESTS; i++) {
  scal = rand() & 0xff;
  this->field->rand(&this->a, my_rand, NULL);

  this->field->set(&this->res1, 0);
  this->field->set(&this->res2, 0);

  this->field->mul(&this->res1, this->a, scal);
  for(ecl_digit j=0; j<scal; j++) {
    this->field->add(&this->res2, this->res2, this->a);
  }
  ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );
}
}

TYPED_TEST_P(GenericFieldTest, Exponentiation){

for(int i=0; i<NBTESTS; i++) {
  this->field->rand(&this->a, my_rand, NULL);
  this->field->copy(&this->res1, this->a);

  for(unsigned int j=2; j<1000; j++) {
    this->gfp_elem.set(j);
    this->field->mul(&this->res1, this->res1, this->a);
    this->field->exp(&this->res2, this->a, this->gfp_elem);
    ASSERT_EQ(0, this->field->cmp(this->res2,this->res1) );
  }
}
}

TYPED_TEST_P(GenericFieldTest, Frobenius){

// not implemented
if( this->field->getExtensionDegre() == 6) return;

this->field->get_characteristic(&this->gfp_elem);

for(int i=0; i<NBTESTS; i++) {
  this->field->rand(&this->a, my_rand, NULL);
  this->field->set(&this->res1, 0);
  this->field->set(&this->res2, 0);

  for(int j=1; j<4; j++) {
    this->field->frobenius(&this->res1, this->a, j);
    this->field->copy(&this->res2, this->a);
    for(int k=0; k<j; k++) {
      this->field->exp(&this->res2, this->res2, this->gfp_elem);
    }
    this->field->cmp(this->res2,this->res1);
  }
}
}

TYPED_TEST_P(GenericFieldTest, Performance){
uint64_t overhead;

this->field->rand(&this->a, my_rand, NULL);
this->field->rand(&this->b, my_rand, NULL);

GET_OVERHEAD(overhead);
GET_PERF_CLOCKS("      addition", this->field->add(&this->c, this->c, this->b), overhead);
GET_PERF_CLOCKS("  substraction", this->field->sub(&this->c, this->c, this->b), overhead);
GET_PERF_CLOCKS("multiplication", this->field->mul(&this->c, this->c, this->b), overhead);
GET_PERF_CLOCKS("mult no reduce", this->field->mul(&this->r, this->c, this->b), overhead);
GET_PERF_CLOCKS("        square", this->field->sqr(&this->c, this->c), overhead);
GET_PERF_CLOCKS(" sqr no reduce", this->field->sqr(&this->r, this->a), overhead);
GET_PERF_CLOCKS("        reduce", this->field->reduce(&this->c, this->r), overhead);
GET_PERF_CLOCKS("      division", this->field->div(&this->c, this->a, this->c), overhead);
GET_PERF_CLOCKS("       inverse", this->field->inv(&this->c, this->c), overhead);

GET_PERF("      addition", this->field->add(&this->c, this->c, this->b));
GET_PERF("  substraction", this->field->sub(&this->c, this->c, this->b));
GET_PERF("multiplication", this->field->mul(&this->c, this->c, this->b));
GET_PERF("mult no reduce", this->field->mul(&this->r, this->c, this->b));
GET_PERF("        square", this->field->sqr(&this->c, this->c));
GET_PERF(" sqr no reduce", this->field->sqr(&this->r, this->a));
GET_PERF("        reduce", this->field->reduce(&this->c, this->r));
GET_PERF("      division", this->field->div(&this->c, this->a, this->c));
GET_PERF("       inverse", this->field->inv(&this->c, this->c));

}

// Type-parameterized tests involve one extra step: you have to
// enumerate the tests you defined:
REGISTER_TYPED_TEST_CASE_P(
    GenericFieldTest,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    Set,
    Target, AddSub, MulInv, Scalar, Exponentiation, Frobenius,
    Performance);

INSTANTIATE_TYPED_TEST_CASE_P(GFp, GenericFieldTest, GFp);
INSTANTIATE_TYPED_TEST_CASE_P(Fp2, GenericFieldTest, Fp2);
INSTANTIATE_TYPED_TEST_CASE_P(Fp6, GenericFieldTest, Fp6);
INSTANTIATE_TYPED_TEST_CASE_P(Fp12, GenericFieldTest, Fp12);
/**@}*/

