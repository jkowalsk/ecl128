/*
 * @file ibe.h
 * @author Julien Kowalski
 */

#ifndef ECL_IBE_IBE_H_
#define ECL_IBE_IBE_H_

#include <string>

#include "ecl/errcode.h"

using std::string;

namespace ecl {
/** Namespace embedding Identity based encryption systems.
 *
 * All IBE system follow the properties defined in  \cite cryptoeprint:2001:090 :
 *
 * <b>Identity based encryption (IBE) :</b>  An IBE is specified by four probabilistic polynomial time (PPT) algorithms:
 *
 * <b>Setup</b> takes a security parameter \f$ k \f$ and returns the system parameters \f$ params \f$
 * and \f$ master-key \f$ .
 * The system parameters include the description of sets \f$ \mathcal{M} \f$, \f$ \mathcal{C} \f$
 * which denote the set of messages and ciphertexts respectively.
 * \f$ params \f$ is publicly available, while the \f$ master-key \f$ is kept secret by the KGC.
 *
 * <b>Extract</b> takes as inputs \f$ params \f$, \f$ master-key \f$ and an arbitrary string \f$ ID \in \{0,1\}^* \f$
 * and returns a private key \f$ d_{ID} \f$ to the user with identity ID.
 * This must be done over a secure channel, since \f$ d_{ID} \f$ enables to decrypt ciphertexts under the identity ID.
 * <b>Encrypt</b> takes as inputs \f$ params \f$, \f$ ID \in \{0,1\}^* \f$ and \f$ M \in \mathcal{M} \f$.
 * It returns a cipher-text \f$ C \in \mathcal{C} \f$.
 *
 * <b>Decrypt</b> takes as inputs \f$ params \f$, \f$ C \in \mathcal{C} \f$ and a private key \f$ d_{ID} \f$,
 * and it returns \f$ M \in \mathcal{M} \f$ or rejects.
 *
 * The Message, CipherText, PrivateKey, Pkg and Client abstract classes embed these PPT.
 */
namespace ibe {

/** Abstract class for IBE plaintext.
 * Represents the set \f$ \mathcal{M} \f$
 */
class Message {
 public:
  Message() {
  }
  virtual ~Message() = 0;
};

/** Abstract class for IBE ciphertexts
 * Represents the set \f$ \mathcal{C} \f$
 */
class CipherText {
 public:
  CipherText() {
  }
  virtual ~CipherText() = 0;
};

/** Abstract class for IBE private keys
 */
class PrivateKey {
 public:
  PrivateKey() {
  }
  virtual ~PrivateKey() = 0;

};

/** Abstract class for IBE Private Key Generator
 */
class Pkg {
 public:
  Pkg() {
  }
  virtual ~Pkg() = 0;

  /** Sets the master key of the Private Key Generator
   * @param[in] master_key master key
   * @param[in] key_size master key size
   */
  virtual void SetMasterKey(const unsigned char *master_key,
                            size_t key_size) = 0;

  /** Returns a private key from an identity.
   * @param[out] pKey user's private key
   * @param[in] identity user's identity
   * @return ERR_OK in case of success
   * @return ERR_KEY_NOT_SET if master key was not set
   */
  virtual ErrCode Extract(PrivateKey *pKey, const string &identity) = 0;
};

/** Abstract class for IBE user
 */
class Client {
 public:
 public:
  Client() {
  }
  virtual ~Client() = 0;

  /** Set user private key
   * @param key user private key
   */
  virtual void SetKey(const PrivateKey &key) = 0;

  /** Encrypt message
   * @param[out] ciphertext encrypted text
   * @param[in] m clear message
   * @param[in] identity recipient identity
   */
  virtual void Encrypt(CipherText *ciphertext, const Message &m,
                       const string &identity) = 0;

  /** Returns a private key from an identity.
   * @param[out] m clear message
   * @param[in] ciphertext encrypted text
   * @return ERR_OK in case of success
   * @return ERR_KEY_NOT_SET if private key was not set
   * @return ERR_INVALID_VALUE if impossible to decrypt
   */
  virtual ErrCode Decrypt(Message *m, const CipherText &ciphertext) = 0;

};

}  // namespace ibe
}  // namespace ecl

#endif /* ECL_IBE_IBE_H_ */
