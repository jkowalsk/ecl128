/*
 * @file sha256.cpp
 * @author Julien Kowalski
 */

#include "ecl/config.h"
#include "ecl/digest/sha256.h"

#include "../asm/arch.h"

namespace ecl {
namespace digest {


#define SHR(x, n) (x >> n)

#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))

#define SIGMA0(x)       (ROT32R(x, 2)  ^ ROT32R(x, 13) ^ ROT32R(x, 22))
#define SIGMA1(x)       (ROT32R(x, 6)  ^ ROT32R(x, 11) ^ ROT32R(x, 25))
#define sigma0(x)       (ROT32R(x, 7)  ^ ROT32R(x, 18) ^ SHR(x, 3))
#define sigma1(x)       (ROT32R(x, 17) ^ ROT32R(x, 19) ^ SHR(x, 10))

#define RND(a,b,c,d,e,f,g,h,i,ki)                    \
    t0 = h + SIGMA1(e) + Ch(e, f, g) + ki + W[i];   \
    t1 = SIGMA0(a) + Maj(a, b, c);                  \
    d += t0;                                        \
    h  = t0 + t1;

Sha256::Sha256() {
  init();
}

Sha256::~Sha256() {

}

void Sha256::init() {
  H[0] = 0x6A09E667UL;
  H[1] = 0xBB67AE85UL;
  H[2] = 0x3C6EF372UL;
  H[3] = 0xA54FF53AUL;
  H[4] = 0x510E527FUL;
  H[5] = 0x9B05688CUL;
  H[6] = 0x1F83D9ABUL;
  H[7] = 0x5BE0CD19UL;

  message_len = 0;
}

void Sha256::compress() {
  compress(internal_buff.begin());
  internal_buff.erase(internal_buff.begin(), internal_buff.begin() + 64);
}

void Sha256::compress(Buffer::const_iterator it) {
  uint32_t S[8], W[64], t0, t1;
  int i;

  // Prepare the message schedule, {Wt}:
  for (i = 0; i < 16; i++) {
    W[i] = *it++ << 24;
    W[i] |= *it++ << 16;
    W[i] |= *it++ << 8;
    W[i] |= *it++;
  }
  for (i = 16; i < 64; i++) {
    W[i] = sigma1(W[i - 2]) + W[i - 7] + sigma0(W[i - 15]) + W[i - 16];
  }

  // Initialize the eight working variables, a, b, c, d, e, f, g, and h,
  // with the (i-1)st hash value:
  // Here a..h are S[0] .. S[7]
  for (i = 0; i < 8; i++) {
    S[i] = H[i];
  }

  // For t=0 to 63:
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 0, 0x428a2f98);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 1, 0x71374491);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 2, 0xb5c0fbcf);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 3, 0xe9b5dba5);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 4, 0x3956c25b);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 5, 0x59f111f1);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 6, 0x923f82a4);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 7, 0xab1c5ed5);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 8, 0xd807aa98);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 9, 0x12835b01);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 10, 0x243185be);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 11, 0x550c7dc3);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 12, 0x72be5d74);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 13, 0x80deb1fe);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 14, 0x9bdc06a7);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 15, 0xc19bf174);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 16, 0xe49b69c1);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 17, 0xefbe4786);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 18, 0x0fc19dc6);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 19, 0x240ca1cc);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 20, 0x2de92c6f);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 21, 0x4a7484aa);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 22, 0x5cb0a9dc);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 23, 0x76f988da);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 24, 0x983e5152);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 25, 0xa831c66d);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 26, 0xb00327c8);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 27, 0xbf597fc7);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 28, 0xc6e00bf3);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 29, 0xd5a79147);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 30, 0x06ca6351);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 31, 0x14292967);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 32, 0x27b70a85);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 33, 0x2e1b2138);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 34, 0x4d2c6dfc);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 35, 0x53380d13);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 36, 0x650a7354);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 37, 0x766a0abb);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 38, 0x81c2c92e);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 39, 0x92722c85);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 40, 0xa2bfe8a1);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 41, 0xa81a664b);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 42, 0xc24b8b70);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 43, 0xc76c51a3);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 44, 0xd192e819);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 45, 0xd6990624);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 46, 0xf40e3585);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 47, 0x106aa070);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 48, 0x19a4c116);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 49, 0x1e376c08);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 50, 0x2748774c);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 51, 0x34b0bcb5);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 52, 0x391c0cb3);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 53, 0x4ed8aa4a);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 54, 0x5b9cca4f);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 55, 0x682e6ff3);
  RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], 56, 0x748f82ee);
  RND(S[7], S[0], S[1], S[2], S[3], S[4], S[5], S[6], 57, 0x78a5636f);
  RND(S[6], S[7], S[0], S[1], S[2], S[3], S[4], S[5], 58, 0x84c87814);
  RND(S[5], S[6], S[7], S[0], S[1], S[2], S[3], S[4], 59, 0x8cc70208);
  RND(S[4], S[5], S[6], S[7], S[0], S[1], S[2], S[3], 60, 0x90befffa);
  RND(S[3], S[4], S[5], S[6], S[7], S[0], S[1], S[2], 61, 0xa4506ceb);
  RND(S[2], S[3], S[4], S[5], S[6], S[7], S[0], S[1], 62, 0xbef9a3f7);
  RND(S[1], S[2], S[3], S[4], S[5], S[6], S[7], S[0], 63, 0xc67178f2);

  // Compute the ith intermediate hash value H(i)
  for (i = 0; i < 8; i++) {
    H[i] += S[i];
  }

  message_len += 64;

}

void Sha256::update(const sstring &str) {
  Buffer buff(str.begin(), str.end());
  update(buff);
}

void Sha256::update(const Buffer &buff) {
  // feed into internal_buff
  Buffer::const_iterator it;
  for (it = buff.begin(); it + 64 <= buff.end(); it += 64) {
    compress(it);
  }
  if (it < buff.end()) {
    internal_buff.insert(internal_buff.end(), it, buff.end());
    if (internal_buff.size() > 64) {
      compress();
    }
  }
}

void Sha256::final(FixedSizedBuffer<32> *res) {
  uint64_t length = (message_len + internal_buff.size()) * 8;
  // Append the bit â€œ1â€� to the end of the  message
  internal_buff.push_back(0x80);
  if (internal_buff.size() > 56) {
    while (internal_buff.size() < 64) {
      internal_buff.push_back(0x00);
    }
    compress();
  }
  while (internal_buff.size() < 56) {
    internal_buff.push_back(0x00);
  }
  internal_buff.push_back((length >> 56) & 0xFF);
  internal_buff.push_back((length >> 48) & 0xFF);
  internal_buff.push_back((length >> 40) & 0xFF);
  internal_buff.push_back((length >> 32) & 0xFF);
  internal_buff.push_back((length >> 24) & 0xFF);
  internal_buff.push_back((length >> 16) & 0xFF);
  internal_buff.push_back((length >> 8) & 0xFF);
  internal_buff.push_back((length) & 0xFF);

  compress();

  // get result
  for (int i = 0; i < 8; i++) {
    (*res)[4 * i] = static_cast<unsigned char>((H[i] >> 24) & 0xFF);
    (*res)[4 * i + 1] = static_cast<unsigned char>((H[i] >> 16) & 0xFF);
    (*res)[4 * i + 2] = static_cast<unsigned char>((H[i] >> 8) & 0xFF);
    (*res)[4 * i + 3] = static_cast<unsigned char>((H[i]) & 0xFF);
  }
}

void Sha256::hash(FixedSizedBuffer<32> *res, const Buffer &buff) {
  init();
  update(buff);
  final(res);
}

void Sha256::hash(FixedSizedBuffer<32> *res, const sstring &str) {
  init();
  update(str);
  final(res);
}

} /* namespace ecl */
} /* namespace digest */

