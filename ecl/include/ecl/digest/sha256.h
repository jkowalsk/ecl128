/*
 * @file sha256.h
 * @author Julien Kowalski
 */

#ifndef ECL_INCLUDE_SYMMETRIC_DIGEST_SHA256_H_
#define ECL_INCLUDE_SYMMETRIC_DIGEST_SHA256_H_

#include <stdint.h>
#include <string>

#include "ecl/config.h"
#include "ecl/errcode.h"
#include "ecl/types.h"

namespace ecl {
namespace digest {

class Sha256 {
 public:
  Sha256();
  virtual ~Sha256() ;

  void init();
  void update(const sstring &str) ;
  void update(const Buffer &buff);
  void final(FixedSizedBuffer<32> *hash);

  void hash(FixedSizedBuffer<32> *hash, const Buffer &buff) ;


 private:
  void compress();
  Buffer internal_buff;
  uint32_t H[8];

  size_t message_len;
};

} /* namespace ecl */
} /* namespace digest */

#endif /* ECL_INCLUDE_SYMMETRIC_DIGEST_SHA256_H_ */

