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

/** Namespace for Message Digest classes.
 */
namespace digest {

/** Implementation of SHA256.
 * The Secure Hash Algorithm as defined in \cite fips180
 */
class Sha256 {
 public:
  /** Constructor.
   * The object is initialized and ready to update
   */
  Sha256();
  /** Destructor */
  virtual ~Sha256();

  /** Reinitializes the object.
   */
  void init();

  /** Updates the hash.
   * Feeds the string into the hash context.
   * @param[in] str (partial) string to hash
   */
  void update(const sstring &str);

  /** Updates the hash.
   * Feeds the input buffer into the hash context.
   * @param[in] buff (partial) buffer to hash
   */
  void update(const Buffer &buff);

  /** Get the hash value.
   * Finalizes the hash
   * @param[out] hash message digest
   */
  void final(FixedSizedBuffer<32> *hash);

  /** Performs a full hash of a string
   * @param[in] str (full) string to hash
   * @param[out] hash message digest
   */
  void hash(FixedSizedBuffer<32> *hash, const sstring &str);

  /** Performs a full hash of a buffer
   * @param[in] buff (full) buffer to hash
   * @param[out] hash message digest
   */
  void hash(FixedSizedBuffer<32> *hash, const Buffer &buff);

 private:
  void compress(Buffer::const_iterator it);
  void compress();
  Buffer internal_buff;
  uint32_t H[8];

  size_t message_len;
};

} /* namespace ecl */
} /* namespace digest */

#endif /* ECL_INCLUDE_SYMMETRIC_DIGEST_SHA256_H_ */

