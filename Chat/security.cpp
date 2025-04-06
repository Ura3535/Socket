#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <cmath>

static std::vector<int> generatePrimes(int n) {
    std::vector<int> primes;
    int x = 2;
    while (primes.size() < n) {
        bool isPrime = true;
        for (int p : primes) {
            if (x % p == 0) {
                isPrime = false;
                break;
            }
            if (p * p > x)
                break;
        }
        if (isPrime)
            primes.push_back(x);
        ++x;
    }
    return primes;
}

inline uint32_t frac(double x) {
    return static_cast<uint32_t>((x - static_cast<uint32_t>(x)) * (1ULL << 32));
}

static std::vector<uint32_t> generate_constants(int num, double (*root)(double)) {
    std::vector<uint32_t> constants;

    for (const auto x : generatePrimes(num)) {
        constants.push_back(frac(root(x)));
    }
    return constants;
}

const auto K_ = generate_constants(64, std::cbrt);
/*
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
*/

const auto H_ = generate_constants(8, std::sqrt);
/*
       0x6a09e667, 0xbb67ae85,
       0x3c6ef372, 0xa54ff53a,
       0x510e527f, 0x9b05688c,
       0x1f83d9ab, 0x5be0cd19
*/

inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

inline uint32_t choose(uint32_t e, uint32_t f, uint32_t g) {
    return (e & f) ^ (~e & g);
}

inline uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
    return (a & b) ^ (a & c) ^ (b & c);
}

inline uint32_t sig0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline uint32_t sig1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline uint32_t theta0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline uint32_t theta1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}


std::string sha256(const std::string& input) {
	std::vector<uint32_t> H(H_.begin(), H_.end());

   std::vector<uint8_t> data(input.begin(), input.end());

   data.push_back(0x80);

   while ((data.size() + 8) % 64 != 0)
       data.push_back(0x00);

   uint64_t bitsLen = input.size() * 8;
   for (int i = 7; i >= 0; --i)
       data.push_back((bitsLen >> (i * 8)) & 0xff);

   for (size_t chunk = 0; chunk < data.size(); chunk += 64) {
       std::vector<uint32_t> w(64, 0);

       for (int i = 0; i < 16; ++i) {
           w[i] = (data[chunk + i * 4] << 24) |
               (data[chunk + i * 4 + 1] << 16) |
               (data[chunk + i * 4 + 2] << 8) |
               (data[chunk + i * 4 + 3]);
       }

       for (int i = 16; i < 64; ++i) {
           w[i] = theta1(w[i - 2]) + w[i - 7] + theta0(w[i - 15]) + w[i - 16];
       }

       uint32_t a = H[0], b = H[1], c = H[2], d = H[3];
       uint32_t e = H[4], f = H[5], g = H[6], h = H[7];

       for (int i = 0; i < 64; ++i) {
           uint32_t temp1 = h + sig1(e) + choose(e, f, g) + K_[i] + w[i];
           uint32_t temp2 = sig0(a) + majority(a, b, c);

           h = g;
           g = f;
           f = e;
           e = d + temp1;
           d = c;
           c = b;
           b = a;
           a = temp1 + temp2;
       }

       H[0] += a; H[1] += b; H[2] += c; H[3] += d;
       H[4] += e; H[5] += f; H[6] += g; H[7] += h;
   }

   std::vector<uint8_t> hash(32);
   for (int i = 0; i < 8; ++i) {
       hash[i * 4] = (H[i] >> 24) & 0xff;
       hash[i * 4 + 1] = (H[i] >> 16) & 0xff;
       hash[i * 4 + 2] = (H[i] >> 8) & 0xff;
       hash[i * 4 + 3] = H[i] & 0xff;
   }

   std::stringstream ss;
   for (const auto& x : hash) {
       ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(x);
   }
   return ss.str();
}
