#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fmt/format.h>

#include "wideint.h"
typedef wideint<256> int256_t;

std::ostream& operator<<(std::ostream &os, std::vector<uint8_t> const& v) {
  int counter = 0;
  for (auto &i: v) {
    if (!(counter % 16) && counter != 0) {
      os << std::endl;
    }
    os << fmt::format("0x{:02x} ", i);
    counter++;
  }
  return os;
}

std::vector<uint8_t> operator+(std::vector<uint8_t> const& lhs, std::vector<uint8_t> const& rhs) {
  auto i = lhs.begin();
  auto j = rhs.begin();
  std::vector<uint8_t> v;
  size_t t_num = lhs.size() < rhs.size() ? lhs.size() : rhs.size();
  for (int k=0; k<t_num; k++) {
    v.push_back((*i++) + (*j++));
  }
  return v;
}

std::vector<uint8_t> operator^(std::vector<uint8_t> const& lhs, std::vector<uint8_t> const& rhs) {
  auto i = lhs.begin();
  auto j = rhs.begin();
  std::vector<uint8_t> v;
  size_t t_num = lhs.size() < rhs.size() ? lhs.size() : rhs.size();
  for (int k=0; k<t_num; k++) {
    v.push_back((*i++) ^ (*j++));
  }
  return v;
}

uint32_t leftRotate(uint32_t v, uint8_t n) {
  if (n == 16) {
    return (v << 16) | ((v >> 16)&0x0000FFFF);
  } else if (n == 12) {
    return (v << 12) | ((v >> 20)&0x00000FFF);
  } else if (n == 8) {
    return (v << 8)  | ((v >> 24)&0x000000FF);
  } else if (n == 7) {
    return (v << 7)  | ((v >> 25)&0x0000007F);
  } else {
    return 0;
  }
}

class QuarterRound {
  private:
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
  public:
    QuarterRound(void): a(0), b(0), c(0), d(0) {}
    QuarterRound(uint32_t a, uint32_t b, uint32_t c, uint32_t d):
      a(a), b(b), c(c), d(d) {}

    void collectValues(uint32_t values[4]) {
      values[0] = a;
      values[1] = b;
      values[2] = c;
      values[3] = d;
    }
  
    QuarterRound& doQuarterRound(void) {
      a += b;
      d ^= a;
      d = leftRotate(d,16);
      c += d;
      b ^= c;
      b = leftRotate(b,12);
      a += b;
      d ^= a;
      d = leftRotate(d,8);
      c += d;
      b ^= c;
      b = leftRotate(b,7);

      return *this;
    }

    QuarterRound& doSampleQuarterRound(void) {
      c += d;
      b ^= c;
      b = leftRotate(b,7);

      return *this;
    }

    friend std::ostream& operator<<(std::ostream &os, QuarterRound const& q);
};

class ChachaState {
  private:
    uint32_t s[16];
  public:
    ChachaState(void): s{0} {}

    ChachaState(const uint32_t m_s[16]) {
      for (auto &i: s) {
        i = *(m_s++);
      }
    }

    ChachaState operator+(ChachaState const& rhs) {
      uint32_t local_s[16] = {0};
      for (int i=0; i<16; i++) {
        local_s[i] = this->s[i] + rhs.s[i];
      }
      return ChachaState(local_s);
    }

    ChachaState& doQuarterRound(const uint32_t m_i[4]) {
      QuarterRound q(s[m_i[0]], s[m_i[1]], s[m_i[2]], s[m_i[3]]);
      q.doQuarterRound();
      uint32_t local[4];
      q.collectValues(local);
      for (int i=0; i<4; i++)
        s[m_i[i]] = local[i];

      return *this;
    }

    ChachaState& initState(const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]) {
      s[0] = 0x61707865;
      s[1] = 0x3320646e;
      s[2] = 0x79622d32;
      s[3] = 0x6b206574;

      /* value in key[i]: byte0: 0x01, byte1: 0x02, byte2: 0x03, byte3: 0x04 */
      /* s[4+1] = 0x04030201 - read them in little endian */
      
      for (int i=0; i<8; i++) {
        uint8_t *ptr = (uint8_t *)&key[i];
        s[4+i] = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
      }

      s[12] = counter;

      for (int i=0; i<3; i++) {
        uint8_t *ptr = (uint8_t *)&nonce[i];
        s[13+i] = ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24;
      }

      return *this;
    }

    ChachaState& doRound(void) {
      const uint32_t permutations[8][4] = {
        {0, 4,  8, 12},
        {1, 5,  9, 13},
        {2, 6, 10, 14},
        {3, 7, 11, 15},
        {0, 5, 10, 15},
        {1, 6, 11, 12},
        {2, 7,  8, 13},
        {3, 4,  9, 14}
      };
      ChachaState originalState = *this;
      for (int j=0; j<10; j++) {
        for (int i=0; i<8; i++) {
          doQuarterRound(permutations[i]);
        }
      }

      *this = *this + originalState;

      return *this;
    }

    std::vector<uint8_t> serialize(void) {
      std::vector<uint8_t> v;
      for (int i=0; i<16; i++) {
        for (int j=0; j<4; j++) {
          v.push_back( (s[i] >> j*8) & 0xFF );
        }
      }
      return v;
    }

    std::vector<uint8_t> chacha20_block(const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]) {
      initState(key, counter, nonce);
      doRound();
      return serialize();
    }
 
    friend std::ostream& operator<<(std::ostream &os, ChachaState const& s);
};

class Chacha20 {
  private:
    ChachaState state;
  public:
    Chacha20(void) {}

    std::vector<uint8_t> encrypt(const uint32_t key[8], uint32_t counter, const uint32_t nonce[3], std::vector<uint8_t> plaintext) {
      std::vector<uint8_t> encrypted_message;
      for (int j=0; j<plaintext.size()/64; j++) {
        std::vector<uint8_t> key_stream = state.chacha20_block(key, counter+j, nonce);
        std::vector<uint8_t> block(&plaintext[j*64], &plaintext[j*64+64]);
        std::vector<uint8_t> encrypted_block = key_stream ^ block;
        encrypted_message.insert( encrypted_message.end(), encrypted_block.begin(), encrypted_block.end() );
      }
      if ((plaintext.size() % 64) != 0) {
        int j = plaintext.size()/64;
        std::vector<uint8_t> key_stream = state.chacha20_block(key, counter+j, nonce);
        std::vector<uint8_t> block(&plaintext[j*64], &plaintext[plaintext.size()]);
        std::vector<uint8_t> encrypted_block = key_stream ^ block;
        encrypted_message.insert( encrypted_message.end(), encrypted_block.begin(), encrypted_block.end() );
      }
      return encrypted_message;
    }

    std::vector<uint8_t> chacha20_block(const uint32_t key[8], uint32_t counter, const uint32_t nonce[3]) {
      return state.chacha20_block(key, counter, nonce);
    }
};

class Poly1305 {
  private:
    /* (r,s) is the 256 bit key */
    uint8_t r[16];
    uint8_t s[16];

    int256_t r_n, s_n, p, a;

    void clamp(void) {
      r[3]  &= 15;
      r[7]  &= 15;
      r[11] &= 15;
      r[15] &= 15;
      r[4]  &= 252;
      r[8]  &= 252;
      r[12] &= 252;
    }

  public:
    Poly1305(void): r{0}, s{0}, r_n(0), s_n(0), p(0), a(0) {}

    void setKey(const uint8_t key[32]) {
      a = 0;
      /* Partition the key */
      for (int i=0; i<16; i++) {
        r[i] = key[i];
        s[i] = key[i+16];
      }
      /* Clamp r */
      clamp();

      /* Set p = (1<<130)-5 */
      p = 1;
      p <<= 130;
      p -= 5;

      /* Translate r and s to numbers */
      for (int i=0; i<16; i++) {
        int256_t temp = r[i];
        r_n |= temp << 8*i;
        temp = s[i];
        s_n |= temp << 8*i;
      }

      //std::cout << p.to_string(16) << std::endl;
      //std::cout << r_n.to_string(16) << std::endl;
      //std::cout << s_n.to_string(16) << std::endl;
    }

    std::vector<uint8_t> authenticate(std::vector<uint8_t> msg) {
      std::vector<uint8_t> tag;
      /* i=1 upto ceil(msg length in bytes / 16) */
      for (int i=1; i<=std::ceil(msg.size()/16.); i++) {
        /*  n = le_bytes_to_num(msg[((i-1)*16)..(i*16)] | [0x01])
         *  a += n
         *  a = (r * a) % p
         */
        int256_t n = 0;
        int j=0;
        for (j=0; j<16 && (i-1)*16+j<msg.size(); j++)
          n |= ((int256_t)msg[(i-1)*16+j] << j*8);

        //std::cout << "Block: " << i << std::endl;
        n |= ((int256_t)0x01 << j*8);
        //std::cout << n.to_string(16) << std::endl;
        a += n;
        //std::cout << a.to_string(16) << std::endl;
        a = (r_n * a) % p;
        //std::cout << a.to_string(16) << std::endl;
      }

      /* Add the secret */
      a += s_n;

      /* serialize in little endian from the accumulator value */
      for (int i=0; i<16; i++) {
        int256_t temp = (a >> i*8) & 0xFF;
        tag.push_back(std::stoi(temp.to_string()));
      }
      
      return tag;
    }
    
};

class Chacha20Poly1305 {
  private:
    Chacha20 chacha20;
    Poly1305 poly1305;

    std::vector<uint8_t> _mac_data;
    std::vector<uint8_t> _otk;

  public:
    Chacha20Poly1305(void) {}
    std::vector<uint8_t> generatePolyKey(const uint32_t key[8], const uint32_t nonce[3]) {
      uint32_t counter = 0;
      std::vector<uint8_t> key_block = chacha20.chacha20_block(key, counter, nonce);
      /* Key block is now serialised as a 64 byte block but we just need first 32 */
      std::vector<uint8_t> poly_key(&key_block[0], &key_block[32]);
      return poly_key;
    }

  std::vector<uint8_t> aead_construction(std::vector<uint8_t> aad, std::vector<uint8_t> payload) {
    std::vector aad_pad(16-aad.size()%16, 0);
    std::vector payload_pad(16-payload.size()%16, 0);
    size_t aad_size = aad.size();
    size_t payload_size = payload.size();
    aad.insert(aad.end(), aad_pad.begin(), aad_pad.end());
    payload.insert(payload.end(), payload_pad.begin(), payload_pad.end());
    aad.insert(aad.end(), payload.begin(), payload.end());
    std::vector<uint8_t> mac_data = aad;
    /* convert length to little endian bytes and concatenate to mac_data */
    uint8_t aad_payload_lengths[16];
    for (int i=0; i<8; i++) {
      aad_payload_lengths[i] = (aad_size >> (8*i)) & 0xFF;
      aad_payload_lengths[i+8] = (payload_size >> (8*i)) & 0xFF;
    }
    for (int i=0; i<16; i++) {
      mac_data.push_back(aad_payload_lengths[i]);
    }
    return mac_data;
  }

  std::vector<uint8_t> encrypt(std::vector<uint8_t> aad, const uint32_t key[8], const uint32_t iv[2], uint32_t constant, std::vector<uint8_t> plaintext) {
    const uint32_t nonce[3] = {constant, iv[0], iv[1]};
    std::vector<uint8_t> otk = generatePolyKey(key, nonce);
    std::vector<uint8_t> ciphertext = chacha20.encrypt(key, 1, nonce, plaintext);
    /* Store mac_data and otk for later */
    _mac_data = aead_construction(aad, ciphertext);
    _otk = otk;

    return ciphertext;
  }

  std::vector<uint8_t> decrypt(std::vector<uint8_t> aad, const uint32_t key[8], const uint32_t iv[2], uint32_t constant, std::vector<uint8_t> ciphertext, std::vector<uint8_t> tag) {
    std::vector<uint8_t> decrypted_message = encrypt(aad, key, iv, constant, ciphertext);
    /* Make sure the AEAD construction used is on the ciphertext and not on the plaintext - which would be the one constructed in the previous step */
    _mac_data = aead_construction(aad, ciphertext);
    std::vector<uint8_t> reconstructed_tag = authenticate();
    if (!std::equal(tag.begin(), tag.end(), reconstructed_tag.begin(), reconstructed_tag.end())) {
      std::cout << "VERIFICATION OF AUTH TAG FAILED!" << std::endl;
      decrypted_message.clear();
    }
    return decrypted_message;
  }

  std::vector<uint8_t> authenticate(void) {
    if (_mac_data.size() == 0) {
      std::cout << "No data to authenticate!" << std::endl;
    }

    if (_otk.size() != 32) {
      std::cout << "The one time key for authentication has size " << _otk.size() << " which is not 32 as expected!" << std::endl;
    }

    uint8_t otk[32];
    for (int i=0; i<32; i++) {
      otk[i] = _otk[i];
    }
                                  
    poly1305.setKey(otk);
    std::vector<uint8_t> tag = poly1305.authenticate(_mac_data);
    //std::cout << _mac_data << std::endl;
    _mac_data.clear();
    _otk.clear();
    return tag;
  }
};

std::ostream& operator<<(std::ostream &os, QuarterRound const& q) {
  os << fmt::format("(0x{:08x}, 0x{:08x}, 0x{:08x}, 0x{:08x})", q.a, q.b, q.c, q.d);
  return os;
};

std::ostream& operator<<(std::ostream &os, ChachaState const& cs) {
  int counter = 0;
  os << "[";
  for (auto &i: cs.s) {
    os << fmt::format("0x{:08x}", i);
    counter++;
    if (counter % 4) {
      os << ", ";
    } else if (counter != 16) {
      os << std::endl << " ";
    }
  }
  os << "]";
  return os;
}

const uint32_t sampleChachaState[] = {
  0x879531e0, 0xc5ecf37d, 0x516461b1, 0xc9a62f8a,
  0x44c20ef3, 0x3390af7f, 0xd9fc690b, 0x2a5f714c,
  0x53372767, 0xb00a5631, 0x974c541a, 0x359e9963,
  0x5c971061, 0x3d631689, 0x2098d9d6, 0x91dbd320
};

const uint8_t testKey[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

const uint8_t testNonce[] = {
  0x00, 0x00, 0x00, 0x09,
  0x00, 0x00, 0x00, 0x4a,
  0x00, 0x00, 0x00, 0x00
};

const uint8_t testNonce_2[] = {
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x4a,
  0x00, 0x00, 0x00, 0x00
};

const uint32_t testCounter = 1;

const uint8_t testPlaintext[] = {
  0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
  0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
  0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
  0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
  0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
  0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
  0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
  0x74, 0x2e   
};

uint32_t s3_default_sum[16] = {0x0};
uint32_t s4_default_sum[16] = {0x0};

uint32_t s_chacha_state_with_key_setup[] = {
  0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
  0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
  0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c,
  0x00000001, 0x09000000, 0x4a000000, 0x00000000
};

uint32_t s_chacha_state_after_twenty_rounds[] = {
  0x837778ab, 0xe238d763, 0xa67ae21e, 0x5950bb2f,
  0xc4f2d0c7, 0xfc62bb2f, 0x8fa018fc, 0x3f5ec7b7,
  0x335271c2, 0xf29489f3, 0xeabda8fc, 0x82e46ebd,
  0xd19c12b4, 0xb04e16de, 0x9e83d0cb, 0x4e3c50a2
};

uint8_t testPolyKey[] = {
  0x85, 0xd6, 0xbe, 0x78, 0x57, 0x55, 0x6d, 0x33,
  0x7f, 0x44, 0x52, 0xfe, 0x42, 0xd5, 0x06, 0xa8,
  0x01, 0x03, 0x80, 0x8a, 0xfb, 0x0d, 0xb2, 0xfd,
  0x4a, 0xbf, 0xf6, 0xaf, 0x41, 0x49, 0xf5, 0x1b
};

const uint8_t testAuthenticatedMessage[] = {
  0x43, 0x72, 0x79, 0x70, 0x74, 0x6f, 0x67, 0x72, 0x61, 0x70, 0x68, 0x69, 0x63, 0x20, 0x46, 0x6f,
  0x72, 0x75, 0x6d, 0x20, 0x52, 0x65, 0x73, 0x65, 0x61, 0x72, 0x63, 0x68, 0x20, 0x47, 0x72, 0x6f,
  0x75, 0x70                                            
};

const uint8_t testGenPolyKey[] = {
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
};

const uint8_t testNonceAead[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};

const uint8_t testAad[] = {
  0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7
};

const uint8_t testIv[] = {
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47
};

const uint8_t testConstant[] = {
  0x07, 0x00, 0x00, 0x00
};

int main(void) {
#if 0
  QuarterRound q1(0x11111111, 0x01020304, 0x77777777, 0x01234567);
  QuarterRound q2(0x11111111, 0x01020304, 0x9b8d6f43, 0x01234567);
  std::cout << q1 << std::endl;
  std::cout << q1.doSampleQuarterRound() << std::endl;
  std::cout << q2 << std::endl;
  std::cout << q2.doQuarterRound() << std::endl;
  ChachaState s0;
  std::cout << "ChachaState initialised with default: " << std::endl << s0 << std::endl;
  std::fill_n(s3_default_sum, 16, 0x3);
  std::fill_n(s4_default_sum, 16, 0x4);

  ChachaState s3(s_chacha_state_with_key_setup);
  ChachaState s4(s_chacha_state_after_twenty_rounds);
  std::cout << "ChachaState simple sum test:" << std::endl << s3+s4 << std::endl;
  ChachaState s1(sampleChachaState);
  std::cout << "ChachaState initalised with sample state:" << std::endl << s1 << std::endl;
  uint32_t indexes[]{2,7,8,13};
  std::cout << "ChachaState after Qround(2,7,8,13):" << std::endl
            << s1.doQuarterRound(indexes) << std::endl;

  ChachaState s2;
  std::cout << s2.chacha20_block((const uint32_t *)testKey, testCounter, (const uint32_t *)testNonce) << std::endl;
#endif

  std::vector<uint8_t> message;
  for (int i=0; i<sizeof(testAuthenticatedMessage); i++) {
    message.push_back(testAuthenticatedMessage[i]);
  }
  std::vector<uint8_t> plainText;
  for (int i=0; i<sizeof(testPlaintext); i++) {
    plainText.push_back(testPlaintext[i]);
  }
  std::vector<uint8_t> aad;
  for (int i=0; i<sizeof(testAad); i++) {
    aad.push_back(testAad[i]);
  }

#if 0
  Chacha20 cipher;
  std::vector<uint8_t> encrypted_message = cipher.encrypt((const uint32_t *)testKey, testCounter, (const uint32_t *)testNonce_2, plainText);
  std::cout << "Plaintext:" << std::endl << plainText << std::endl;
  std::cout << "Ciphertext:" << std::endl << encrypted_message << std::endl;
  std::vector<uint8_t> decrypted_message = cipher.encrypt((const uint32_t *)testKey, testCounter, (const uint32_t *)testNonce_2, encrypted_message);
  std::cout << "Recovered text in clear:" << std::endl;
  for (auto i: decrypted_message) {
    std::cout << (char)i;
  }
  std::cout << std::endl;

  Poly1305 mac;
  mac.setKey(testPolyKey);
  std::cout << "Authenticated message tag:" << std::endl;
  std::cout << mac.authenticate(message) << std::endl;
#endif

  Chacha20Poly1305 aead;
  std::cout << "Original plaintext:" << std::endl << plainText << std::endl;
  std::cout << "Generate polyKey through Chacha20:" << std::endl;
  std::cout << aead.generatePolyKey((const uint32_t *)testGenPolyKey, (const uint32_t *)testNonceAead) << std::endl;
  std::cout << "Encrypted message:" << std::endl;
  std::vector<uint8_t> cipherText = aead.encrypt(aad, (const uint32_t *)testGenPolyKey, (const uint32_t *)testIv, *((uint32_t *)(testConstant)), plainText);
  std::cout << cipherText << std::endl;
  std::cout << "Authentication tag:" << std::endl;
  std::vector<uint8_t> tag = aead.authenticate();
  std::cout << tag << std::endl;
  std::vector<uint8_t> decrypted_message = aead.decrypt(aad, (const uint32_t *)testGenPolyKey, (const uint32_t *)testIv, *((uint32_t *)(testConstant)), cipherText, tag);
  if (decrypted_message.size()) {
    std::cout << "Decrypted message:" << std::endl << decrypted_message << std::endl;
    std::cout << "Recovered text in clear:" << std::endl;
    for (auto i: decrypted_message) {
      std::cout << (char)i;
    }
    std::cout << std::endl;
  }

  return 0;
}
