/*
 * @file fp6_base.cpp
 * @author Julien Kowalski
 */

#include <iostream>

#include <cstring>
#include <string>

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

Fp6::Fp6(string p) {
  fp2 = new Fp2(p);
  gfp = fp2->getBasePrimeField();
}

Fp6::Fp6(const GFp::Element &p) {
  fp2 = new Fp2(p);
  gfp = fp2->getBasePrimeField();
}

Fp6::~Fp6() {
  delete fp2;
}

void Fp6::rand(Element *res, int (*f_rng)(unsigned char *, int, void *),
               void *p_rng) {
  fp2->rand(&((*res)[2]), f_rng, p_rng);
  fp2->rand(&((*res)[1]), f_rng, p_rng);
  fp2->rand(&((*res)[0]), f_rng, p_rng);
}

void Fp6::zero(Element *res) {
  fp2->zero(&((*res)[0]));
  fp2->zero(&((*res)[1]));
  fp2->zero(&((*res)[2]));
}

void Fp6::one(Element *res) {
  fp2->one(&((*res)[0]));
  fp2->zero(&((*res)[1]));
  fp2->zero(&((*res)[2]));
}

void Fp6::set(Element *res, const int v) {
  fp2->set(&((*res)[0]), v);
  fp2->zero(&((*res)[1]));
  fp2->zero(&((*res)[2]));
}

void Fp6::copy(Element *res, const Element &a) {
  fp2->copy(&((*res)[0]), a[0]);
  fp2->copy(&((*res)[1]), a[1]);
  fp2->copy(&((*res)[2]), a[2]);
}

void Fp6::copy(Double *res, const Double &a) {
  fp2->copy(&((*res)[0]), a[0]);
  fp2->copy(&((*res)[1]), a[1]);
  fp2->copy(&((*res)[2]), a[2]);
}

bool Fp6::isOne(const Element &a) {
  return fp2->isOne(a[0]) && fp2->isZero(a[1]) && fp2->isZero(a[2]);
}

bool Fp6::isZero(const Element &a) {
  return fp2->isZero(a[0]) && fp2->isZero(a[1]) && fp2->isZero(a[2]);
}

void Fp6::add(Element *res, const Element &a, const Element &b) {
  fp2->add(&((*res)[0]), a[0], b[0]);
  fp2->add(&((*res)[1]), a[1], b[1]);
  fp2->add(&((*res)[2]), a[2], b[2]);
}

void Fp6::add(Double *res, const Double &a, const Double &b) {
  fp2->add(&((*res)[0]), a[0], b[0]);
  fp2->add(&((*res)[1]), a[1], b[1]);
  fp2->add(&((*res)[2]), a[2], b[2]);
}

void Fp6::sub(Element *res, const Element &a, const ecl_digit b) {
  fp2->sub(&((*res)[0]), a[0], b);
}

void Fp6::sub(Element *res, const Element &a, const Element &b) {
  fp2->sub(&((*res)[0]), a[0], b[0]);
  fp2->sub(&((*res)[1]), a[1], b[1]);
  fp2->sub(&((*res)[2]), a[2], b[2]);
}

void Fp6::sub(Double *res, const Double &a, const Double &b) {
  fp2->sub(&((*res)[0]), a[0], b[0]);
  fp2->sub(&((*res)[1]), a[1], b[1]);
  fp2->sub(&((*res)[2]), a[2], b[2]);
}

void Fp6::opp(Element *res, const Element &a) {
  fp2->opp(&((*res)[0]), a[0]);
  fp2->opp(&((*res)[1]), a[1]);
  fp2->opp(&((*res)[2]), a[2]);
}

void Fp6::opp(Double *res, const Double &a) {
  fp2->opp(&((*res)[0]), a[0]);
  fp2->opp(&((*res)[1]), a[1]);
  fp2->opp(&((*res)[2]), a[2]);
}

void Fp6::reduce(Element *res, const Double &a) {
  fp2->reduce(&((*res)[0]), a[0]);
  fp2->reduce(&((*res)[1]), a[1]);
  fp2->reduce(&((*res)[2]), a[2]);
}

int Fp6::cmp(const Element &a, const Element &b) {
  return fp2->cmp(a[0], b[0]) | fp2->cmp(a[1], b[1]) | fp2->cmp(a[2], b[2]);
}

int Fp6::cmp(const Double &a, const Double &b) {
  return fp2->cmp(a[0], b[0]) | fp2->cmp(a[1], b[1]) | fp2->cmp(a[2], b[2]);
}

void Fp6::init(Element *res, const Fp2::Element &a0,
               const Fp2::Element &a1,
               const Fp2::Element &a2) {
  fp2->copy(&((*res)[0]), a0);
  fp2->copy(&((*res)[1]), a1);
  fp2->copy(&((*res)[2]), a2);
}

}  // namespace field
}  // namespace ecl
