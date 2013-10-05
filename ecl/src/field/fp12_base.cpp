/*
 * @file fp12_base.cpp
 * @author Julien Kowalski
 */

#include <iostream>

#include <cstring>
#include <string>

#include "ecl/field/GFp.h"
#include "ecl/field/Fp2.h"
#include "ecl/field/Fp6.h"
#include "ecl/field/Fp12.h"

using ecl::ErrCode;

namespace ecl {
namespace field {

Fp12::Fp12(string p) {
  fp6 = new Fp6(p);
  fp2 = fp6->getBaseField();
  gfp = fp2->getBasePrimeField();
  precomputeGamma();
}

Fp12::Fp12(const GFp::Element &p) {
  fp6 = new Fp6(p);
  fp2 = fp6->getBaseField();
  gfp = fp2->getBasePrimeField();
  precomputeGamma();
}

Fp12::~Fp12() {
  delete fp6;
}

void Fp12::precomputeGamma() {  // ok verified with code from article
  Fp2::Element xsi, tmp, conjugate;
  GFp::Element expo, six;

  gfp->set(&six, 6);

  gfp->get_characteristic(&expo);
  gfp->sub(&expo, expo, 1);
  gfp->div(&expo, expo, six);

  fp2->get_xsi(&xsi);

  fp2->exp(&tmp, xsi, expo);
  fp2->copy(&(gamma[0][0]), tmp);  // tmp[0] == 0

  for (int j = 1; j < 5; j++) {
    fp2->mul(&(gamma[0][j]), gamma[0][j - 1], gamma[0][0]);
  }

  for (int k = 0; k < 5; k++) {
    fp2->conj(&conjugate, gamma[0][k]);
    fp2->mul(&(gamma[1][k]), gamma[0][k], conjugate);
    fp2->mul(&(gamma[2][k]), gamma[1][k], gamma[0][k]);
  }
}

void Fp12::rand(Element *res, int (*f_rng)(unsigned char *, int, void *),
                void *p_rng) {
  fp6->rand(&((*res)[1]), f_rng, p_rng);
  fp6->rand(&((*res)[0]), f_rng, p_rng);
}

void Fp12::zero(Element *res) {
  fp6->zero(&((*res)[0]));
  fp6->zero(&((*res)[1]));
}

void Fp12::one(Element *res) {
  fp6->one(&((*res)[0]));
  fp6->zero(&((*res)[1]));
}

void Fp12::set(Element *res, const int v) {
  fp6->set(&((*res)[0]), v);
  fp6->zero(&((*res)[1]));
}

void Fp12::copy(Element *res, const Element &a) {
  fp6->copy(&((*res)[0]), a[0]);
  fp6->copy(&((*res)[1]), a[1]);
}

void Fp12::copy(Double *res, const Double &a) {
  fp6->copy(&((*res)[0]), a[0]);
  fp6->copy(&((*res)[1]), a[1]);
}

bool Fp12::isOne(const Element &a) {
  return fp6->isOne(a[0]) && fp6->isZero(a[1]);
}

bool Fp12::isZero(const Element &a) {
  return fp6->isZero(a[0]) && fp6->isZero(a[1]);
}

void Fp12::add(Element *res, const Element &a, const Element &b) {
  fp6->add(&((*res)[0]), a[0], b[0]);
  fp6->add(&((*res)[1]), a[1], b[1]);
}

void Fp12::add(Double *res, const Double &a, const Double &b) {
  fp6->add(&((*res)[0]), a[0], b[0]);
  fp6->add(&((*res)[1]), a[1], b[1]);
}

void Fp12::sub(Element *res, const Element &a, const ecl_digit b) {
  fp6->sub(&((*res)[0]), a[0], b);
}

void Fp12::sub(Element *res, const Element &a, const Element &b) {
  fp6->sub(&((*res)[0]), a[0], b[0]);
  fp6->sub(&((*res)[1]), a[1], b[1]);
}

void Fp12::sub(Double *res, const Double &a, const Double &b) {
  fp6->sub(&((*res)[0]), a[0], b[0]);
  fp6->sub(&((*res)[1]), a[1], b[1]);
}

void Fp12::opp(Element *res, const Element &a) {
  fp6->opp(&((*res)[0]), a[0]);
  fp6->opp(&((*res)[1]), a[1]);
}

void Fp12::opp(Double *res, const Double &a) {
  fp6->opp(&((*res)[0]), a[0]);
  fp6->opp(&((*res)[1]), a[1]);
}

void Fp12::reduce(Element *res, const Double &a) {
  fp6->reduce(&((*res)[0]), a[0]);
  fp6->reduce(&((*res)[1]), a[1]);
}

int Fp12::cmp(const Element &a, const Element &b) {
  return fp6->cmp(a[0], b[0]) | fp6->cmp(a[1], b[1]);
}

int Fp12::cmp(const Double &a, const Double &b) {
  return fp6->cmp(a[0], b[0]) | fp6->cmp(a[1], b[1]);
}

void Fp12::init(Element *res, const Fp6::Element &a0, const Fp6::Element &a1) {
  fp6->copy(&((*res)[0]), a0);
  fp6->copy(&((*res)[1]), a1);
}

void Fp12::conj(Element *res, const Element &a) {
  fp6->copy(&((*res)[0]), a[0]);
  fp6->opp(&((*res)[1]), a[1]);
}

}  // namespace field
}  // namespace ecl

