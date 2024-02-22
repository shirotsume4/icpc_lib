#include <bits/stdc++.h>
#include <immintrin.h>
using namespace std;

namespace nyaan {

template<uint32_t mod> struct LazyMontgomeryModInt {
   using mint = LazyMontgomeryModInt;
   using i32 = int32_t;
   using u32 = uint32_t;
   using u64 = uint64_t;

   static constexpr u32 get_r() {
      u32 ret = mod;
      for(i32 i = 0; i < 4; ++i) ret *= 2 - mod * ret;
      return ret;
   }

   static constexpr u32 r = get_r();
   static constexpr u32 n2 = -u64(mod) % mod;
   static_assert(mod < (1 << 30), "invalid, mod >= 2 ^ 30");
   static_assert((mod & 1) == 1, "invalid, mod % 2 == 0");
   static_assert(r * mod == 1, "this code has bugs.");

   u32 a;

   constexpr LazyMontgomeryModInt() : a(0) {}
   constexpr LazyMontgomeryModInt(const int64_t& b) : a(reduce(u64(b % mod + mod) * n2)){};

   static constexpr u32 reduce(const u64& b) { return (b + u64(u32(b) * u32(-r)) * mod) >> 32; }

   constexpr mint& operator+=(const mint& b) {
      if(i32(a += b.a - 2 * mod) < 0) a += 2 * mod;
      return *this;
   }

   constexpr mint& operator-=(const mint& b) {
      if(i32(a -= b.a) < 0) a += 2 * mod;
      return *this;
   }

   constexpr mint& operator*=(const mint& b) {
      a = reduce(u64(a) * b.a);
      return *this;
   }

   constexpr mint& operator/=(const mint& b) {
      *this *= b.inverse();
      return *this;
   }

   constexpr mint operator+(const mint& b) const { return mint(*this) += b; }
   constexpr mint operator-(const mint& b) const { return mint(*this) -= b; }
   constexpr mint operator*(const mint& b) const { return mint(*this) *= b; }
   constexpr mint operator/(const mint& b) const { return mint(*this) /= b; }
   constexpr bool operator==(const mint& b) const { return (a >= mod ? a - mod : a) == (b.a >= mod ? b.a - mod : b.a); }
   constexpr bool operator!=(const mint& b) const { return (a >= mod ? a - mod : a) != (b.a >= mod ? b.a - mod : b.a); }
   constexpr mint operator-() const { return mint() - mint(*this); }
   constexpr mint operator+() const { return mint(*this); }

   constexpr mint pow(u64 n) const {
      mint ret(1), mul(*this);
      while(n > 0) {
         if(n & 1) ret *= mul;
         mul *= mul;
         n >>= 1;
      }
      return ret;
   }

   constexpr mint inverse() const {
      int x = get(), y = mod, u = 1, v = 0, t = 0, tmp = 0;
      while(y > 0) {
         t = x / y;
         x -= t * y, u -= t * v;
         tmp = x, x = y, y = tmp;
         tmp = u, u = v, v = tmp;
      }
      return mint{u};
   }

   friend ostream& operator<<(ostream& os, const mint& b) { return os << b.get(); }

   friend istream& operator>>(istream& is, mint& b) {
      int64_t t;
      is >> t;
      b = LazyMontgomeryModInt<mod>(t);
      return (is);
   }

   constexpr u32 get() const {
      u32 ret = reduce(a);
      return ret >= mod ? ret - mod : ret;
   }

   static constexpr u32 get_mod() { return mod; }
};
#line 2 "ntt/ntt-avx2.hpp"

#line 2 "modint/simd-montgomery.hpp"

#line 4 "modint/simd-montgomery.hpp"

__attribute__((target("sse4.2"))) inline __m128i my128_mullo_epu32(const __m128i& a, const __m128i& b) {
   return _mm_mullo_epi32(a, b);
}

__attribute__((target("sse4.2"))) inline __m128i my128_mulhi_epu32(const __m128i& a, const __m128i& b) {
   __m128i a13 = _mm_shuffle_epi32(a, 0xF5);
   __m128i b13 = _mm_shuffle_epi32(b, 0xF5);
   __m128i prod02 = _mm_mul_epu32(a, b);
   __m128i prod13 = _mm_mul_epu32(a13, b13);
   __m128i prod = _mm_unpackhi_epi64(_mm_unpacklo_epi32(prod02, prod13), _mm_unpackhi_epi32(prod02, prod13));
   return prod;
}

__attribute__((target("sse4.2"))) inline __m128i montgomery_mul_128(const __m128i& a,
                                                                    const __m128i& b,
                                                                    const __m128i& r,
                                                                    const __m128i& m1) {
   return _mm_sub_epi32(_mm_add_epi32(my128_mulhi_epu32(a, b), m1),
                        my128_mulhi_epu32(my128_mullo_epu32(my128_mullo_epu32(a, b), r), m1));
}

__attribute__((target("sse4.2"))) inline __m128i montgomery_add_128(const __m128i& a,
                                                                    const __m128i& b,
                                                                    const __m128i& m2,
                                                                    const __m128i& m0) {
   __m128i ret = _mm_sub_epi32(_mm_add_epi32(a, b), m2);
   return _mm_add_epi32(_mm_and_si128(_mm_cmpgt_epi32(m0, ret), m2), ret);
}

__attribute__((target("sse4.2"))) inline __m128i montgomery_sub_128(const __m128i& a,
                                                                    const __m128i& b,
                                                                    const __m128i& m2,
                                                                    const __m128i& m0) {
   __m128i ret = _mm_sub_epi32(a, b);
   return _mm_add_epi32(_mm_and_si128(_mm_cmpgt_epi32(m0, ret), m2), ret);
}

__attribute__((target("avx2"))) inline __m256i my256_mullo_epu32(const __m256i& a, const __m256i& b) {
   return _mm256_mullo_epi32(a, b);
}

__attribute__((target("avx2"))) inline __m256i my256_mulhi_epu32(const __m256i& a, const __m256i& b) {
   __m256i a13 = _mm256_shuffle_epi32(a, 0xF5);
   __m256i b13 = _mm256_shuffle_epi32(b, 0xF5);
   __m256i prod02 = _mm256_mul_epu32(a, b);
   __m256i prod13 = _mm256_mul_epu32(a13, b13);
   __m256i prod = _mm256_unpackhi_epi64(_mm256_unpacklo_epi32(prod02, prod13), _mm256_unpackhi_epi32(prod02, prod13));
   return prod;
}

__attribute__((target("avx2"))) inline __m256i montgomery_mul_256(const __m256i& a,
                                                                  const __m256i& b,
                                                                  const __m256i& r,
                                                                  const __m256i& m1) {
   return _mm256_sub_epi32(_mm256_add_epi32(my256_mulhi_epu32(a, b), m1),
                           my256_mulhi_epu32(my256_mullo_epu32(my256_mullo_epu32(a, b), r), m1));
}

__attribute__((target("avx2"))) inline __m256i montgomery_add_256(const __m256i& a,
                                                                  const __m256i& b,
                                                                  const __m256i& m2,
                                                                  const __m256i& m0) {
   __m256i ret = _mm256_sub_epi32(_mm256_add_epi32(a, b), m2);
   return _mm256_add_epi32(_mm256_and_si256(_mm256_cmpgt_epi32(m0, ret), m2), ret);
}

__attribute__((target("avx2"))) inline __m256i montgomery_sub_256(const __m256i& a,
                                                                  const __m256i& b,
                                                                  const __m256i& m2,
                                                                  const __m256i& m0) {
   __m256i ret = _mm256_sub_epi32(a, b);
   return _mm256_add_epi32(_mm256_and_si256(_mm256_cmpgt_epi32(m0, ret), m2), ret);
}
#line 4 "ntt/ntt-avx2.hpp"

namespace ntt_inner {
using u64 = uint64_t;
constexpr uint32_t get_pr(uint32_t mod) {
   if(mod == 2) return 1;
   u64 ds[32] = {};
   int idx = 0;
   u64 m = mod - 1;
   for(u64 i = 2; i * i <= m; ++i) {
      if(m % i == 0) {
         ds[idx++] = i;
         while(m % i == 0) m /= i;
      }
   }
   if(m != 1) ds[idx++] = m;

   uint32_t pr = 2;
   while(1) {
      int flg = 1;
      for(int i = 0; i < idx; ++i) {
         u64 a = pr, b = (mod - 1) / ds[i], r = 1;
         while(b) {
            if(b & 1) r = r * a % mod;
            a = a * a % mod;
            b >>= 1;
         }
         if(r == 1) {
            flg = 0;
            break;
         }
      }
      if(flg == 1) break;
      ++pr;
   }
   return pr;
}

constexpr int SZ_FFT_BUF = 1 << 23;
uint32_t _buf1[SZ_FFT_BUF] __attribute__((aligned(64)));
uint32_t _buf2[SZ_FFT_BUF] __attribute__((aligned(64)));
}  // namespace ntt_inner

template<typename mint> struct NTT {
   static constexpr uint32_t mod = mint::get_mod();
   static constexpr uint32_t pr = ntt_inner::get_pr(mint::get_mod());
   static constexpr int level = __builtin_ctzll(mod - 1);
   mint dw[level], dy[level];
   mint *buf1, *buf2;

   constexpr NTT() {
      setwy(level);
      union raw_cast {
         mint dat;
         uint32_t _;
      };
      buf1 = &(((raw_cast*)(ntt_inner::_buf1))->dat);
      buf2 = &(((raw_cast*)(ntt_inner::_buf2))->dat);
   }

   constexpr void setwy(int k) {
      mint w[level], y[level];
      w[k - 1] = mint(pr).pow((mod - 1) / (1 << k));
      y[k - 1] = w[k - 1].inverse();
      for(int i = k - 2; i > 0; --i) w[i] = w[i + 1] * w[i + 1], y[i] = y[i + 1] * y[i + 1];
      dw[0] = dy[0] = w[1] * w[1];
      dw[1] = w[1], dy[1] = y[1], dw[2] = w[2], dy[2] = y[2];
      for(int i = 3; i < k; ++i) {
         dw[i] = dw[i - 1] * y[i - 2] * w[i];
         dy[i] = dy[i - 1] * w[i - 2] * y[i];
      }
   }

   __attribute__((target("avx2"))) void ntt(mint* a, int n) {
      int k = n ? __builtin_ctz(n) : 0;
      if(k == 0) return;
      if(k == 1) {
         mint a1 = a[1];
         a[1] = a[0] - a[1];
         a[0] = a[0] + a1;
         return;
      }
      if(k & 1) {
         int v = 1 << (k - 1);
         if(v < 8) {
            for(int j = 0; j < v; ++j) {
               mint ajv = a[j + v];
               a[j + v] = a[j] - ajv;
               a[j] += ajv;
            }
         } else {
            const __m256i m0 = _mm256_set1_epi32(0);
            const __m256i m2 = _mm256_set1_epi32(mod + mod);
            int j0 = 0;
            int j1 = v;
            for(; j0 < v; j0 += 8, j1 += 8) {
               __m256i T0 = _mm256_loadu_si256((__m256i*)(a + j0));
               __m256i T1 = _mm256_loadu_si256((__m256i*)(a + j1));
               __m256i naj = montgomery_add_256(T0, T1, m2, m0);
               __m256i najv = montgomery_sub_256(T0, T1, m2, m0);
               _mm256_storeu_si256((__m256i*)(a + j0), naj);
               _mm256_storeu_si256((__m256i*)(a + j1), najv);
            }
         }
      }
      int u = 1 << (2 + (k & 1));
      int v = 1 << (k - 2 - (k & 1));
      mint one = mint(1);
      mint imag = dw[1];
      while(v) {
         if(v == 1) {
            mint ww = one, xx = one, wx = one;
            for(int jh = 0; jh < u;) {
               ww = xx * xx, wx = ww * xx;
               mint t0 = a[jh + 0], t1 = a[jh + 1] * xx;
               mint t2 = a[jh + 2] * ww, t3 = a[jh + 3] * wx;
               mint t0p2 = t0 + t2, t1p3 = t1 + t3;
               mint t0m2 = t0 - t2, t1m3 = (t1 - t3) * imag;
               a[jh + 0] = t0p2 + t1p3, a[jh + 1] = t0p2 - t1p3;
               a[jh + 2] = t0m2 + t1m3, a[jh + 3] = t0m2 - t1m3;
               xx *= dw[__builtin_ctz((jh += 4))];
            }
         } else if(v == 4) {
            const __m128i m0 = _mm_set1_epi32(0);
            const __m128i m1 = _mm_set1_epi32(mod);
            const __m128i m2 = _mm_set1_epi32(mod + mod);
            const __m128i r = _mm_set1_epi32(mint::r);
            const __m128i Imag = _mm_set1_epi32(imag.a);
            mint ww = one, xx = one, wx = one;
            for(int jh = 0; jh < u;) {
               if(jh == 0) {
                  int j0 = 0;
                  int j1 = v;
                  int j2 = j1 + v;
                  int j3 = j2 + v;
                  int je = v;
                  for(; j0 < je; j0 += 4, j1 += 4, j2 += 4, j3 += 4) {
                     const __m128i T0 = _mm_loadu_si128((__m128i*)(a + j0));
                     const __m128i T1 = _mm_loadu_si128((__m128i*)(a + j1));
                     const __m128i T2 = _mm_loadu_si128((__m128i*)(a + j2));
                     const __m128i T3 = _mm_loadu_si128((__m128i*)(a + j3));
                     const __m128i T0P2 = montgomery_add_128(T0, T2, m2, m0);
                     const __m128i T1P3 = montgomery_add_128(T1, T3, m2, m0);
                     const __m128i T0M2 = montgomery_sub_128(T0, T2, m2, m0);
                     const __m128i T1M3 = montgomery_mul_128(montgomery_sub_128(T1, T3, m2, m0), Imag, r, m1);
                     _mm_storeu_si128((__m128i*)(a + j0), montgomery_add_128(T0P2, T1P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j1), montgomery_sub_128(T0P2, T1P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j2), montgomery_add_128(T0M2, T1M3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j3), montgomery_sub_128(T0M2, T1M3, m2, m0));
                  }
               } else {
                  ww = xx * xx, wx = ww * xx;
                  const __m128i WW = _mm_set1_epi32(ww.a);
                  const __m128i WX = _mm_set1_epi32(wx.a);
                  const __m128i XX = _mm_set1_epi32(xx.a);
                  int j0 = jh * v;
                  int j1 = j0 + v;
                  int j2 = j1 + v;
                  int j3 = j2 + v;
                  int je = j1;
                  for(; j0 < je; j0 += 4, j1 += 4, j2 += 4, j3 += 4) {
                     const __m128i T0 = _mm_loadu_si128((__m128i*)(a + j0));
                     const __m128i T1 = _mm_loadu_si128((__m128i*)(a + j1));
                     const __m128i T2 = _mm_loadu_si128((__m128i*)(a + j2));
                     const __m128i T3 = _mm_loadu_si128((__m128i*)(a + j3));
                     const __m128i MT1 = montgomery_mul_128(T1, XX, r, m1);
                     const __m128i MT2 = montgomery_mul_128(T2, WW, r, m1);
                     const __m128i MT3 = montgomery_mul_128(T3, WX, r, m1);
                     const __m128i T0P2 = montgomery_add_128(T0, MT2, m2, m0);
                     const __m128i T1P3 = montgomery_add_128(MT1, MT3, m2, m0);
                     const __m128i T0M2 = montgomery_sub_128(T0, MT2, m2, m0);
                     const __m128i T1M3 = montgomery_mul_128(montgomery_sub_128(MT1, MT3, m2, m0), Imag, r, m1);
                     _mm_storeu_si128((__m128i*)(a + j0), montgomery_add_128(T0P2, T1P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j1), montgomery_sub_128(T0P2, T1P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j2), montgomery_add_128(T0M2, T1M3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j3), montgomery_sub_128(T0M2, T1M3, m2, m0));
                  }
               }
               xx *= dw[__builtin_ctz((jh += 4))];
            }
         } else {
            const __m256i m0 = _mm256_set1_epi32(0);
            const __m256i m1 = _mm256_set1_epi32(mod);
            const __m256i m2 = _mm256_set1_epi32(mod + mod);
            const __m256i r = _mm256_set1_epi32(mint::r);
            const __m256i Imag = _mm256_set1_epi32(imag.a);
            mint ww = one, xx = one, wx = one;
            for(int jh = 0; jh < u;) {
               if(jh == 0) {
                  int j0 = 0;
                  int j1 = v;
                  int j2 = j1 + v;
                  int j3 = j2 + v;
                  int je = v;
                  for(; j0 < je; j0 += 8, j1 += 8, j2 += 8, j3 += 8) {
                     const __m256i T0 = _mm256_loadu_si256((__m256i*)(a + j0));
                     const __m256i T1 = _mm256_loadu_si256((__m256i*)(a + j1));
                     const __m256i T2 = _mm256_loadu_si256((__m256i*)(a + j2));
                     const __m256i T3 = _mm256_loadu_si256((__m256i*)(a + j3));
                     const __m256i T0P2 = montgomery_add_256(T0, T2, m2, m0);
                     const __m256i T1P3 = montgomery_add_256(T1, T3, m2, m0);
                     const __m256i T0M2 = montgomery_sub_256(T0, T2, m2, m0);
                     const __m256i T1M3 = montgomery_mul_256(montgomery_sub_256(T1, T3, m2, m0), Imag, r, m1);
                     _mm256_storeu_si256((__m256i*)(a + j0), montgomery_add_256(T0P2, T1P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j1), montgomery_sub_256(T0P2, T1P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j2), montgomery_add_256(T0M2, T1M3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j3), montgomery_sub_256(T0M2, T1M3, m2, m0));
                  }
               } else {
                  ww = xx * xx, wx = ww * xx;
                  const __m256i WW = _mm256_set1_epi32(ww.a);
                  const __m256i WX = _mm256_set1_epi32(wx.a);
                  const __m256i XX = _mm256_set1_epi32(xx.a);
                  int j0 = jh * v;
                  int j1 = j0 + v;
                  int j2 = j1 + v;
                  int j3 = j2 + v;
                  int je = j1;
                  for(; j0 < je; j0 += 8, j1 += 8, j2 += 8, j3 += 8) {
                     const __m256i T0 = _mm256_loadu_si256((__m256i*)(a + j0));
                     const __m256i T1 = _mm256_loadu_si256((__m256i*)(a + j1));
                     const __m256i T2 = _mm256_loadu_si256((__m256i*)(a + j2));
                     const __m256i T3 = _mm256_loadu_si256((__m256i*)(a + j3));
                     const __m256i MT1 = montgomery_mul_256(T1, XX, r, m1);
                     const __m256i MT2 = montgomery_mul_256(T2, WW, r, m1);
                     const __m256i MT3 = montgomery_mul_256(T3, WX, r, m1);
                     const __m256i T0P2 = montgomery_add_256(T0, MT2, m2, m0);
                     const __m256i T1P3 = montgomery_add_256(MT1, MT3, m2, m0);
                     const __m256i T0M2 = montgomery_sub_256(T0, MT2, m2, m0);
                     const __m256i T1M3 = montgomery_mul_256(montgomery_sub_256(MT1, MT3, m2, m0), Imag, r, m1);
                     _mm256_storeu_si256((__m256i*)(a + j0), montgomery_add_256(T0P2, T1P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j1), montgomery_sub_256(T0P2, T1P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j2), montgomery_add_256(T0M2, T1M3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j3), montgomery_sub_256(T0M2, T1M3, m2, m0));
                  }
               }
               xx *= dw[__builtin_ctz((jh += 4))];
            }
         }
         u <<= 2;
         v >>= 2;
      }
   }

   __attribute__((target("avx2"))) void intt(mint* a, int n, int normalize = true) {
      int k = n ? __builtin_ctz(n) : 0;
      if(k == 0) return;
      if(k == 1) {
         mint a1 = a[1];
         a[1] = a[0] - a[1];
         a[0] = a[0] + a1;
         if(normalize) {
            a[0] *= mint(2).inverse();
            a[1] *= mint(2).inverse();
         }
         return;
      }
      int u = 1 << (k - 2);
      int v = 1;
      mint one = mint(1);
      mint imag = dy[1];
      while(u) {
         if(v == 1) {
            mint ww = one, xx = one, yy = one;
            u <<= 2;
            for(int jh = 0; jh < u;) {
               ww = xx * xx, yy = xx * imag;
               mint t0 = a[jh + 0], t1 = a[jh + 1];
               mint t2 = a[jh + 2], t3 = a[jh + 3];
               mint t0p1 = t0 + t1, t2p3 = t2 + t3;
               mint t0m1 = (t0 - t1) * xx, t2m3 = (t2 - t3) * yy;
               a[jh + 0] = t0p1 + t2p3, a[jh + 2] = (t0p1 - t2p3) * ww;
               a[jh + 1] = t0m1 + t2m3, a[jh + 3] = (t0m1 - t2m3) * ww;
               xx *= dy[__builtin_ctz(jh += 4)];
            }
         } else if(v == 4) {
            const __m128i m0 = _mm_set1_epi32(0);
            const __m128i m1 = _mm_set1_epi32(mod);
            const __m128i m2 = _mm_set1_epi32(mod + mod);
            const __m128i r = _mm_set1_epi32(mint::r);
            const __m128i Imag = _mm_set1_epi32(imag.a);
            mint ww = one, xx = one, yy = one;
            u <<= 2;
            for(int jh = 0; jh < u;) {
               if(jh == 0) {
                  int j0 = 0;
                  int j1 = v;
                  int j2 = v + v;
                  int j3 = j2 + v;
                  for(; j0 < v; j0 += 4, j1 += 4, j2 += 4, j3 += 4) {
                     const __m128i T0 = _mm_loadu_si128((__m128i*)(a + j0));
                     const __m128i T1 = _mm_loadu_si128((__m128i*)(a + j1));
                     const __m128i T2 = _mm_loadu_si128((__m128i*)(a + j2));
                     const __m128i T3 = _mm_loadu_si128((__m128i*)(a + j3));
                     const __m128i T0P1 = montgomery_add_128(T0, T1, m2, m0);
                     const __m128i T2P3 = montgomery_add_128(T2, T3, m2, m0);
                     const __m128i T0M1 = montgomery_sub_128(T0, T1, m2, m0);
                     const __m128i T2M3 = montgomery_mul_128(montgomery_sub_128(T2, T3, m2, m0), Imag, r, m1);
                     _mm_storeu_si128((__m128i*)(a + j0), montgomery_add_128(T0P1, T2P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j2), montgomery_sub_128(T0P1, T2P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j1), montgomery_add_128(T0M1, T2M3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j3), montgomery_sub_128(T0M1, T2M3, m2, m0));
                  }
               } else {
                  ww = xx * xx, yy = xx * imag;
                  const __m128i WW = _mm_set1_epi32(ww.a);
                  const __m128i XX = _mm_set1_epi32(xx.a);
                  const __m128i YY = _mm_set1_epi32(yy.a);
                  int j0 = jh * v;
                  int j1 = j0 + v;
                  int j2 = j1 + v;
                  int j3 = j2 + v;
                  int je = j1;
                  for(; j0 < je; j0 += 4, j1 += 4, j2 += 4, j3 += 4) {
                     const __m128i T0 = _mm_loadu_si128((__m128i*)(a + j0));
                     const __m128i T1 = _mm_loadu_si128((__m128i*)(a + j1));
                     const __m128i T2 = _mm_loadu_si128((__m128i*)(a + j2));
                     const __m128i T3 = _mm_loadu_si128((__m128i*)(a + j3));
                     const __m128i T0P1 = montgomery_add_128(T0, T1, m2, m0);
                     const __m128i T2P3 = montgomery_add_128(T2, T3, m2, m0);
                     const __m128i T0M1 = montgomery_mul_128(montgomery_sub_128(T0, T1, m2, m0), XX, r, m1);
                     __m128i T2M3 = montgomery_mul_128(montgomery_sub_128(T2, T3, m2, m0), YY, r, m1);
                     _mm_storeu_si128((__m128i*)(a + j0), montgomery_add_128(T0P1, T2P3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j2),
                                      montgomery_mul_128(montgomery_sub_128(T0P1, T2P3, m2, m0), WW, r, m1));
                     _mm_storeu_si128((__m128i*)(a + j1), montgomery_add_128(T0M1, T2M3, m2, m0));
                     _mm_storeu_si128((__m128i*)(a + j3),
                                      montgomery_mul_128(montgomery_sub_128(T0M1, T2M3, m2, m0), WW, r, m1));
                  }
               }
               xx *= dy[__builtin_ctz(jh += 4)];
            }
         } else {
            const __m256i m0 = _mm256_set1_epi32(0);
            const __m256i m1 = _mm256_set1_epi32(mod);
            const __m256i m2 = _mm256_set1_epi32(mod + mod);
            const __m256i r = _mm256_set1_epi32(mint::r);
            const __m256i Imag = _mm256_set1_epi32(imag.a);
            mint ww = one, xx = one, yy = one;
            u <<= 2;
            for(int jh = 0; jh < u;) {
               if(jh == 0) {
                  int j0 = 0;
                  int j1 = v;
                  int j2 = v + v;
                  int j3 = j2 + v;
                  for(; j0 < v; j0 += 8, j1 += 8, j2 += 8, j3 += 8) {
                     const __m256i T0 = _mm256_loadu_si256((__m256i*)(a + j0));
                     const __m256i T1 = _mm256_loadu_si256((__m256i*)(a + j1));
                     const __m256i T2 = _mm256_loadu_si256((__m256i*)(a + j2));
                     const __m256i T3 = _mm256_loadu_si256((__m256i*)(a + j3));
                     const __m256i T0P1 = montgomery_add_256(T0, T1, m2, m0);
                     const __m256i T2P3 = montgomery_add_256(T2, T3, m2, m0);
                     const __m256i T0M1 = montgomery_sub_256(T0, T1, m2, m0);
                     const __m256i T2M3 = montgomery_mul_256(montgomery_sub_256(T2, T3, m2, m0), Imag, r, m1);
                     _mm256_storeu_si256((__m256i*)(a + j0), montgomery_add_256(T0P1, T2P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j2), montgomery_sub_256(T0P1, T2P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j1), montgomery_add_256(T0M1, T2M3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j3), montgomery_sub_256(T0M1, T2M3, m2, m0));
                  }
               } else {
                  ww = xx * xx, yy = xx * imag;
                  const __m256i WW = _mm256_set1_epi32(ww.a);
                  const __m256i XX = _mm256_set1_epi32(xx.a);
                  const __m256i YY = _mm256_set1_epi32(yy.a);
                  int j0 = jh * v;
                  int j1 = j0 + v;
                  int j2 = j1 + v;
                  int j3 = j2 + v;
                  int je = j1;
                  for(; j0 < je; j0 += 8, j1 += 8, j2 += 8, j3 += 8) {
                     const __m256i T0 = _mm256_loadu_si256((__m256i*)(a + j0));
                     const __m256i T1 = _mm256_loadu_si256((__m256i*)(a + j1));
                     const __m256i T2 = _mm256_loadu_si256((__m256i*)(a + j2));
                     const __m256i T3 = _mm256_loadu_si256((__m256i*)(a + j3));
                     const __m256i T0P1 = montgomery_add_256(T0, T1, m2, m0);
                     const __m256i T2P3 = montgomery_add_256(T2, T3, m2, m0);
                     const __m256i T0M1 = montgomery_mul_256(montgomery_sub_256(T0, T1, m2, m0), XX, r, m1);
                     const __m256i T2M3 = montgomery_mul_256(montgomery_sub_256(T2, T3, m2, m0), YY, r, m1);
                     _mm256_storeu_si256((__m256i*)(a + j0), montgomery_add_256(T0P1, T2P3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j2),
                                         montgomery_mul_256(montgomery_sub_256(T0P1, T2P3, m2, m0), WW, r, m1));
                     _mm256_storeu_si256((__m256i*)(a + j1), montgomery_add_256(T0M1, T2M3, m2, m0));
                     _mm256_storeu_si256((__m256i*)(a + j3),
                                         montgomery_mul_256(montgomery_sub_256(T0M1, T2M3, m2, m0), WW, r, m1));
                  }
               }
               xx *= dy[__builtin_ctz(jh += 4)];
            }
         }
         u >>= 4;
         v <<= 2;
      }
      if(k & 1) {
         v = 1 << (k - 1);
         if(v < 8) {
            for(int j = 0; j < v; ++j) {
               mint ajv = a[j] - a[j + v];
               a[j] += a[j + v];
               a[j + v] = ajv;
            }
         } else {
            const __m256i m0 = _mm256_set1_epi32(0);
            const __m256i m2 = _mm256_set1_epi32(mod + mod);
            int j0 = 0;
            int j1 = v;
            for(; j0 < v; j0 += 8, j1 += 8) {
               const __m256i T0 = _mm256_loadu_si256((__m256i*)(a + j0));
               const __m256i T1 = _mm256_loadu_si256((__m256i*)(a + j1));
               __m256i naj = montgomery_add_256(T0, T1, m2, m0);
               __m256i najv = montgomery_sub_256(T0, T1, m2, m0);
               _mm256_storeu_si256((__m256i*)(a + j0), naj);
               _mm256_storeu_si256((__m256i*)(a + j1), najv);
            }
         }
      }
      if(normalize) {
         mint invn = mint(n).inverse();
         for(int i = 0; i < n; i++) a[i] *= invn;
      }
   }

   __attribute__((target("avx2"))) void inplace_multiply(int l1, int l2, int zero_padding = true) {
      int l = l1 + l2 - 1;
      int M = 4;
      while(M < l) M <<= 1;
      if(zero_padding) {
         for(int i = l1; i < M; i++) ntt_inner::_buf1[i] = 0;
         for(int i = l2; i < M; i++) ntt_inner::_buf2[i] = 0;
      }
      const __m256i m0 = _mm256_set1_epi32(0);
      const __m256i m1 = _mm256_set1_epi32(mod);
      const __m256i r = _mm256_set1_epi32(mint::r);
      const __m256i N2 = _mm256_set1_epi32(mint::n2);
      for(int i = 0; i < l1; i += 8) {
         __m256i a = _mm256_loadu_si256((__m256i*)(ntt_inner::_buf1 + i));
         __m256i b = montgomery_mul_256(a, N2, r, m1);
         _mm256_storeu_si256((__m256i*)(ntt_inner::_buf1 + i), b);
      }
      for(int i = 0; i < l2; i += 8) {
         __m256i a = _mm256_loadu_si256((__m256i*)(ntt_inner::_buf2 + i));
         __m256i b = montgomery_mul_256(a, N2, r, m1);
         _mm256_storeu_si256((__m256i*)(ntt_inner::_buf2 + i), b);
      }
      ntt(buf1, M);
      ntt(buf2, M);
      for(int i = 0; i < M; i += 8) {
         __m256i a = _mm256_loadu_si256((__m256i*)(ntt_inner::_buf1 + i));
         __m256i b = _mm256_loadu_si256((__m256i*)(ntt_inner::_buf2 + i));
         __m256i c = montgomery_mul_256(a, b, r, m1);
         _mm256_storeu_si256((__m256i*)(ntt_inner::_buf1 + i), c);
      }
      intt(buf1, M, false);
      const __m256i INVM = _mm256_set1_epi32((mint(M).inverse()).a);
      for(int i = 0; i < l; i += 8) {
         __m256i a = _mm256_loadu_si256((__m256i*)(ntt_inner::_buf1 + i));
         __m256i b = montgomery_mul_256(a, INVM, r, m1);
         __m256i c = my256_mulhi_epu32(my256_mullo_epu32(b, r), m1);
         __m256i d = _mm256_and_si256(_mm256_cmpgt_epi32(c, m0), m1);
         __m256i e = _mm256_sub_epi32(d, c);
         _mm256_storeu_si256((__m256i*)(ntt_inner::_buf1 + i), e);
      }
   }

   void ntt(vector<mint>& a) {
      int M = (int)a.size();
      for(int i = 0; i < M; i++) buf1[i].a = a[i].a;
      ntt(buf1, M);
      for(int i = 0; i < M; i++) a[i].a = buf1[i].a;
   }

   void intt(vector<mint>& a) {
      int M = (int)a.size();
      for(int i = 0; i < M; i++) buf1[i].a = a[i].a;
      intt(buf1, M, true);
      for(int i = 0; i < M; i++) a[i].a = buf1[i].a;
   }

   vector<mint> multiply(const vector<mint>& a, const vector<mint>& b) {
      if(a.size() == 0 && b.size() == 0) return vector<mint>{};
      int l = a.size() + b.size() - 1;
      if(min<int>(a.size(), b.size()) <= 40) {
         vector<mint> s(l);
         for(int i = 0; i < (int)a.size(); ++i)
            for(int j = 0; j < (int)b.size(); ++j) s[i + j] += a[i] * b[j];
         return s;
      }
      assert(l <= ntt_inner::SZ_FFT_BUF);
      int M = 4;
      while(M < l) M <<= 1;
      for(int i = 0; i < (int)a.size(); ++i) buf1[i].a = a[i].a;
      for(int i = (int)a.size(); i < M; ++i) buf1[i].a = 0;
      for(int i = 0; i < (int)b.size(); ++i) buf2[i].a = b[i].a;
      for(int i = (int)b.size(); i < M; ++i) buf2[i].a = 0;
      ntt(buf1, M);
      ntt(buf2, M);
      for(int i = 0; i < M; ++i) buf1[i].a = mint::reduce(uint64_t(buf1[i].a) * buf2[i].a);
      intt(buf1, M, false);
      vector<mint> s(l);
      mint invm = mint(M).inverse();
      for(int i = 0; i < l; ++i) s[i] = buf1[i] * invm;
      return s;
   }

   void ntt_doubling(vector<mint>& a) {
      int M = (int)a.size();
      for(int i = 0; i < M; i++) buf1[i].a = a[i].a;
      intt(buf1, M);
      mint r = 1, zeta = mint(pr).pow((mint::get_mod() - 1) / (M << 1));
      for(int i = 0; i < M; i++) buf1[i] *= r, r *= zeta;
      ntt(buf1, M);
      a.resize(2 * M);
      for(int i = 0; i < M; i++) a[M + i].a = buf1[i].a;
   }
};

}  // namespace nyaan
// #include "atcoder/convolution"

#include "test/template.hpp"

namespace fft1 {
#include "src/modint/modint.hpp"
#include "src/FPS/FFT.hpp"
}  // namespace fft1
namespace fft2 {
#include "src/extra/modint_fast.hpp"
#include "src/FPS/FFT.hpp"
}  // namespace fft2

int main() {
   using namespace chrono;
   const ll N = 1 << 20;
   {
      vector<fft1::mm> A(N), B(N);
      rep(i, 0, N) A[i] = B[i] = i;
      auto s = system_clock::now();
      auto C = fft1::conv(A, B);
      cout << format("my FFT: {} ms", duration_cast<milliseconds>(system_clock::now() - s).count()) << endl;
   }
   {
      vector<fft2::mm> A(N), B(N);
      rep(i, 0, N) A[i] = B[i] = i;
      auto s = system_clock::now();
      auto C = fft2::conv(A, B);
      cout << format("my FFT with fast modint: {} ms", duration_cast<milliseconds>(system_clock::now() - s).count())
           << endl;
   }
   {
      vector<atcoder::modint998244353> A(N), B(N);
      rep(i, 0, N) A[i] = B[i] = i;
      auto s = system_clock::now();
      auto C = atcoder::convolution(A, B);
      cout << format("ACL: {} ms", duration_cast<milliseconds>(system_clock::now() - s).count()) << endl;
   }
   {
      const int MOD = 998244353;
      using mint = nyaan::LazyMontgomeryModInt<MOD>;
      nyaan::NTT<mint> ntt;
      vector<mint> A(N), B(N);
      rep(i, 0, N) A[i] = B[i] = i;
      auto s = system_clock::now();
      auto C = ntt.multiply(A, B);
      cout << format("nyaan AVX2: {} ms", duration_cast<milliseconds>(system_clock::now() - s).count()) << endl;
   }
}
