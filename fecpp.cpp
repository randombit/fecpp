/*
 * fec.c -- forward error correction based on Vandermonde matrices
 *
 * (C) 1997-98 Luigi Rizzo (luigi@iet.unipi.it)
 * (C) 2009 Jack Lloyd (lloyd@randombit.net)
 * Distributed under the terms given in license.txt
 */

#include "fecpp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdexcept>
#include <vector>

namespace {


/* Tables for arithetic in GF(2^8) using 1+x^2+x^3+x^4+x^8
 *
 * See Lin & Costello, Appendix A, and Lee & Messerschmitt, p. 453.
 *
 * Generate GF(2**m) from the irreducible polynomial p(X) in p[0]..p[m]
 * Lookup tables:
 *     index->polynomial form           gf_exp[] contains j= \alpha^i;
 *     polynomial form -> index form    gf_log[ j = \alpha^i ] = i
 * \alpha=x is the primitive element of GF(2^m)
 *
 * For efficiency, gf_exp[] has size 2*GF_SIZE, so that a simple
 * multiplication of two numbers can be resolved without calling modnn
 */
const byte GF_EXP[510] = {
0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1D, 0x3A, 0x74,
0xE8, 0xCD, 0x87, 0x13, 0x26, 0x4C, 0x98, 0x2D, 0x5A, 0xB4, 0x75,
0xEA, 0xC9, 0x8F, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x9D,
0x27, 0x4E, 0x9C, 0x25, 0x4A, 0x94, 0x35, 0x6A, 0xD4, 0xB5, 0x77,
0xEE, 0xC1, 0x9F, 0x23, 0x46, 0x8C, 0x05, 0x0A, 0x14, 0x28, 0x50,
0xA0, 0x5D, 0xBA, 0x69, 0xD2, 0xB9, 0x6F, 0xDE, 0xA1, 0x5F, 0xBE,
0x61, 0xC2, 0x99, 0x2F, 0x5E, 0xBC, 0x65, 0xCA, 0x89, 0x0F, 0x1E,
0x3C, 0x78, 0xF0, 0xFD, 0xE7, 0xD3, 0xBB, 0x6B, 0xD6, 0xB1, 0x7F,
0xFE, 0xE1, 0xDF, 0xA3, 0x5B, 0xB6, 0x71, 0xE2, 0xD9, 0xAF, 0x43,
0x86, 0x11, 0x22, 0x44, 0x88, 0x0D, 0x1A, 0x34, 0x68, 0xD0, 0xBD,
0x67, 0xCE, 0x81, 0x1F, 0x3E, 0x7C, 0xF8, 0xED, 0xC7, 0x93, 0x3B,
0x76, 0xEC, 0xC5, 0x97, 0x33, 0x66, 0xCC, 0x85, 0x17, 0x2E, 0x5C,
0xB8, 0x6D, 0xDA, 0xA9, 0x4F, 0x9E, 0x21, 0x42, 0x84, 0x15, 0x2A,
0x54, 0xA8, 0x4D, 0x9A, 0x29, 0x52, 0xA4, 0x55, 0xAA, 0x49, 0x92,
0x39, 0x72, 0xE4, 0xD5, 0xB7, 0x73, 0xE6, 0xD1, 0xBF, 0x63, 0xC6,
0x91, 0x3F, 0x7E, 0xFC, 0xE5, 0xD7, 0xB3, 0x7B, 0xF6, 0xF1, 0xFF,
0xE3, 0xDB, 0xAB, 0x4B, 0x96, 0x31, 0x62, 0xC4, 0x95, 0x37, 0x6E,
0xDC, 0xA5, 0x57, 0xAE, 0x41, 0x82, 0x19, 0x32, 0x64, 0xC8, 0x8D,
0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xDD, 0xA7, 0x53, 0xA6, 0x51,
0xA2, 0x59, 0xB2, 0x79, 0xF2, 0xF9, 0xEF, 0xC3, 0x9B, 0x2B, 0x56,
0xAC, 0x45, 0x8A, 0x09, 0x12, 0x24, 0x48, 0x90, 0x3D, 0x7A, 0xF4,
0xF5, 0xF7, 0xF3, 0xFB, 0xEB, 0xCB, 0x8B, 0x0B, 0x16, 0x2C, 0x58,
0xB0, 0x7D, 0xFA, 0xE9, 0xCF, 0x83, 0x1B, 0x36, 0x6C, 0xD8, 0xAD,
0x47, 0x8E,

0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1D, 0x3A, 0x74,
0xE8, 0xCD, 0x87, 0x13, 0x26, 0x4C, 0x98, 0x2D, 0x5A, 0xB4, 0x75,
0xEA, 0xC9, 0x8F, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x9D,
0x27, 0x4E, 0x9C, 0x25, 0x4A, 0x94, 0x35, 0x6A, 0xD4, 0xB5, 0x77,
0xEE, 0xC1, 0x9F, 0x23, 0x46, 0x8C, 0x05, 0x0A, 0x14, 0x28, 0x50,
0xA0, 0x5D, 0xBA, 0x69, 0xD2, 0xB9, 0x6F, 0xDE, 0xA1, 0x5F, 0xBE,
0x61, 0xC2, 0x99, 0x2F, 0x5E, 0xBC, 0x65, 0xCA, 0x89, 0x0F, 0x1E,
0x3C, 0x78, 0xF0, 0xFD, 0xE7, 0xD3, 0xBB, 0x6B, 0xD6, 0xB1, 0x7F,
0xFE, 0xE1, 0xDF, 0xA3, 0x5B, 0xB6, 0x71, 0xE2, 0xD9, 0xAF, 0x43,
0x86, 0x11, 0x22, 0x44, 0x88, 0x0D, 0x1A, 0x34, 0x68, 0xD0, 0xBD,
0x67, 0xCE, 0x81, 0x1F, 0x3E, 0x7C, 0xF8, 0xED, 0xC7, 0x93, 0x3B,
0x76, 0xEC, 0xC5, 0x97, 0x33, 0x66, 0xCC, 0x85, 0x17, 0x2E, 0x5C,
0xB8, 0x6D, 0xDA, 0xA9, 0x4F, 0x9E, 0x21, 0x42, 0x84, 0x15, 0x2A,
0x54, 0xA8, 0x4D, 0x9A, 0x29, 0x52, 0xA4, 0x55, 0xAA, 0x49, 0x92,
0x39, 0x72, 0xE4, 0xD5, 0xB7, 0x73, 0xE6, 0xD1, 0xBF, 0x63, 0xC6,
0x91, 0x3F, 0x7E, 0xFC, 0xE5, 0xD7, 0xB3, 0x7B, 0xF6, 0xF1, 0xFF,
0xE3, 0xDB, 0xAB, 0x4B, 0x96, 0x31, 0x62, 0xC4, 0x95, 0x37, 0x6E,
0xDC, 0xA5, 0x57, 0xAE, 0x41, 0x82, 0x19, 0x32, 0x64, 0xC8, 0x8D,
0x07, 0x0E, 0x1C, 0x38, 0x70, 0xE0, 0xDD, 0xA7, 0x53, 0xA6, 0x51,
0xA2, 0x59, 0xB2, 0x79, 0xF2, 0xF9, 0xEF, 0xC3, 0x9B, 0x2B, 0x56,
0xAC, 0x45, 0x8A, 0x09, 0x12, 0x24, 0x48, 0x90, 0x3D, 0x7A, 0xF4,
0xF5, 0xF7, 0xF3, 0xFB, 0xEB, 0xCB, 0x8B, 0x0B, 0x16, 0x2C, 0x58,
0xB0, 0x7D, 0xFA, 0xE9, 0xCF, 0x83, 0x1B, 0x36, 0x6C, 0xD8, 0xAD,
0x47, 0x8E };

const byte GF_LOG[256] = {
0xFF, 0x00, 0x01, 0x19, 0x02, 0x32, 0x1A, 0xC6, 0x03, 0xDF, 0x33,
0xEE, 0x1B, 0x68, 0xC7, 0x4B, 0x04, 0x64, 0xE0, 0x0E, 0x34, 0x8D,
0xEF, 0x81, 0x1C, 0xC1, 0x69, 0xF8, 0xC8, 0x08, 0x4C, 0x71, 0x05,
0x8A, 0x65, 0x2F, 0xE1, 0x24, 0x0F, 0x21, 0x35, 0x93, 0x8E, 0xDA,
0xF0, 0x12, 0x82, 0x45, 0x1D, 0xB5, 0xC2, 0x7D, 0x6A, 0x27, 0xF9,
0xB9, 0xC9, 0x9A, 0x09, 0x78, 0x4D, 0xE4, 0x72, 0xA6, 0x06, 0xBF,
0x8B, 0x62, 0x66, 0xDD, 0x30, 0xFD, 0xE2, 0x98, 0x25, 0xB3, 0x10,
0x91, 0x22, 0x88, 0x36, 0xD0, 0x94, 0xCE, 0x8F, 0x96, 0xDB, 0xBD,
0xF1, 0xD2, 0x13, 0x5C, 0x83, 0x38, 0x46, 0x40, 0x1E, 0x42, 0xB6,
0xA3, 0xC3, 0x48, 0x7E, 0x6E, 0x6B, 0x3A, 0x28, 0x54, 0xFA, 0x85,
0xBA, 0x3D, 0xCA, 0x5E, 0x9B, 0x9F, 0x0A, 0x15, 0x79, 0x2B, 0x4E,
0xD4, 0xE5, 0xAC, 0x73, 0xF3, 0xA7, 0x57, 0x07, 0x70, 0xC0, 0xF7,
0x8C, 0x80, 0x63, 0x0D, 0x67, 0x4A, 0xDE, 0xED, 0x31, 0xC5, 0xFE,
0x18, 0xE3, 0xA5, 0x99, 0x77, 0x26, 0xB8, 0xB4, 0x7C, 0x11, 0x44,
0x92, 0xD9, 0x23, 0x20, 0x89, 0x2E, 0x37, 0x3F, 0xD1, 0x5B, 0x95,
0xBC, 0xCF, 0xCD, 0x90, 0x87, 0x97, 0xB2, 0xDC, 0xFC, 0xBE, 0x61,
0xF2, 0x56, 0xD3, 0xAB, 0x14, 0x2A, 0x5D, 0x9E, 0x84, 0x3C, 0x39,
0x53, 0x47, 0x6D, 0x41, 0xA2, 0x1F, 0x2D, 0x43, 0xD8, 0xB7, 0x7B,
0xA4, 0x76, 0xC4, 0x17, 0x49, 0xEC, 0x7F, 0x0C, 0x6F, 0xF6, 0x6C,
0xA1, 0x3B, 0x52, 0x29, 0x9D, 0x55, 0xAA, 0xFB, 0x60, 0x86, 0xB1,
0xBB, 0xCC, 0x3E, 0x5A, 0xCB, 0x59, 0x5F, 0xB0, 0x9C, 0xA9, 0xA0,
0x51, 0x0B, 0xF5, 0x16, 0xEB, 0x7A, 0x75, 0x2C, 0xD7, 0x4F, 0xAE,
0xD5, 0xE9, 0xE6, 0xE7, 0xAD, 0xE8, 0x74, 0xD6, 0xF4, 0xEA, 0xA8,
0x50, 0x58, 0xAF };

const byte GF_INVERSE[256] = {
0x00, 0x01, 0x8E, 0xF4, 0x47, 0xA7, 0x7A, 0xBA, 0xAD, 0x9D, 0xDD,
0x98, 0x3D, 0xAA, 0x5D, 0x96, 0xD8, 0x72, 0xC0, 0x58, 0xE0, 0x3E,
0x4C, 0x66, 0x90, 0xDE, 0x55, 0x80, 0xA0, 0x83, 0x4B, 0x2A, 0x6C,
0xED, 0x39, 0x51, 0x60, 0x56, 0x2C, 0x8A, 0x70, 0xD0, 0x1F, 0x4A,
0x26, 0x8B, 0x33, 0x6E, 0x48, 0x89, 0x6F, 0x2E, 0xA4, 0xC3, 0x40,
0x5E, 0x50, 0x22, 0xCF, 0xA9, 0xAB, 0x0C, 0x15, 0xE1, 0x36, 0x5F,
0xF8, 0xD5, 0x92, 0x4E, 0xA6, 0x04, 0x30, 0x88, 0x2B, 0x1E, 0x16,
0x67, 0x45, 0x93, 0x38, 0x23, 0x68, 0x8C, 0x81, 0x1A, 0x25, 0x61,
0x13, 0xC1, 0xCB, 0x63, 0x97, 0x0E, 0x37, 0x41, 0x24, 0x57, 0xCA,
0x5B, 0xB9, 0xC4, 0x17, 0x4D, 0x52, 0x8D, 0xEF, 0xB3, 0x20, 0xEC,
0x2F, 0x32, 0x28, 0xD1, 0x11, 0xD9, 0xE9, 0xFB, 0xDA, 0x79, 0xDB,
0x77, 0x06, 0xBB, 0x84, 0xCD, 0xFE, 0xFC, 0x1B, 0x54, 0xA1, 0x1D,
0x7C, 0xCC, 0xE4, 0xB0, 0x49, 0x31, 0x27, 0x2D, 0x53, 0x69, 0x02,
0xF5, 0x18, 0xDF, 0x44, 0x4F, 0x9B, 0xBC, 0x0F, 0x5C, 0x0B, 0xDC,
0xBD, 0x94, 0xAC, 0x09, 0xC7, 0xA2, 0x1C, 0x82, 0x9F, 0xC6, 0x34,
0xC2, 0x46, 0x05, 0xCE, 0x3B, 0x0D, 0x3C, 0x9C, 0x08, 0xBE, 0xB7,
0x87, 0xE5, 0xEE, 0x6B, 0xEB, 0xF2, 0xBF, 0xAF, 0xC5, 0x64, 0x07,
0x7B, 0x95, 0x9A, 0xAE, 0xB6, 0x12, 0x59, 0xA5, 0x35, 0x65, 0xB8,
0xA3, 0x9E, 0xD2, 0xF7, 0x62, 0x5A, 0x85, 0x7D, 0xA8, 0x3A, 0x29,
0x71, 0xC8, 0xF6, 0xF9, 0x43, 0xD7, 0xD6, 0x10, 0x73, 0x76, 0x78,
0x99, 0x0A, 0x19, 0x91, 0x14, 0x3F, 0xE6, 0xF0, 0x86, 0xB1, 0xE2,
0xF1, 0xFA, 0x74, 0xF3, 0xB4, 0x6D, 0x21, 0xB2, 0x6A, 0xE3, 0xE7,
0xB5, 0xEA, 0x03, 0x8F, 0xD3, 0xC9, 0x42, 0xD4, 0xE8, 0x75, 0x7F,
0xFF, 0x7E, 0xFD };

/*
 * modnn(x) computes x % GF_SIZE, where GF_SIZE is 2**GF_BITS - 1,
 * without a slow divide.
 */
inline byte modnn(int x)
   {
   while(x >= 0xFF)
      {
      x -= 0xFF;
      x = (x >> 8) + (x & 0xFF);
      }
   return x;
   }

/*
* gf_mul(x,y) multiplies two numbers. If GF_BITS<=8, it is much
* faster to use a multiplication table.
*
* USE_GF_MULC, GF_MULC0(c) and GF_ADDMULC(x) can be used when multiplying
* many numbers by the same constant. In this case the first
* call sets the constant, and others perform the multiplications.
* A value related to the multiplication is held in a local variable
* declared with USE_GF_MULC . See usage in addmul1().
*/
#if 1
static byte gf_mul_table[0xFF + 1][0xFF + 1];

#define gf_mul(x,y) gf_mul_table[x][y]

#define USE_GF_MULC register byte * __gf_mulc_
#define GF_MULC0(c) __gf_mulc_ = gf_mul_table[c]
#define GF_ADDMULC(dst, x) dst ^= __gf_mulc_[x]

void
init_mul_table()
   {
   int i, j;
   for(i=0; i< 0xFF+1; i++)
      for(j=0; j< 0xFF+1; j++)
         gf_mul_table[i][j] = GF_EXP[modnn(GF_LOG[i] + GF_LOG[j]) ] ;

   for(j=0; j< 0xFF+1; j++)
      gf_mul_table[0][j] = gf_mul_table[j][0] = 0;
   }
#else
// for larger values of GF_BITS
inline byte
gf_mul(byte x, byte y)
   {
   if((x) == 0 || (y)==0) return 0;

   return GF_EXP[GF_LOG[x] + GF_LOG[y] ] ;
   }
#define init_mul_table()

#define USE_GF_MULC register const byte * __gf_mulc_
#define GF_MULC0(c) __gf_mulc_ = &GF_EXP[ GF_LOG[c] ]
#define GF_ADDMULC(dst, x) { if(x) dst ^= __gf_mulc_[ GF_LOG[x] ] ; }
#endif

/*
* Various linear algebra operations that i use often.
*/

/*
* addmul() computes dst[] = dst[] + c * src[]
* This is used often, so better optimize it! Currently the loop is
* unrolled 16 times, a good value for 486 and pentium-class machines.
* The case c=0 is also optimized, whereas c=1 is not. These
* calls are unfrequent in my typical apps so I did not bother.
*
* Note that gcc on
*/
#define addmul(dst, src, c, sz)                 \
   if(c != 0) addmul1(dst, src, c, sz)

#define UNROLL 16 /* 1, 4, 8, 16 */
void
addmul1(byte *dst1, byte *src1, byte c, int sz)
   {
   USE_GF_MULC;
   register byte *dst = dst1, *src = src1;
   byte *lim = &dst[sz - UNROLL + 1];

   GF_MULC0(c);

#if(UNROLL > 1) /* unrolling by 8/16 is quite effective on the pentium */
   for(; dst < lim; dst += UNROLL, src += UNROLL)
      {
      GF_ADDMULC(dst[0] , src[0]);
      GF_ADDMULC(dst[1] , src[1]);
      GF_ADDMULC(dst[2] , src[2]);
      GF_ADDMULC(dst[3] , src[3]);
#if(UNROLL > 4)
      GF_ADDMULC(dst[4] , src[4]);
      GF_ADDMULC(dst[5] , src[5]);
      GF_ADDMULC(dst[6] , src[6]);
      GF_ADDMULC(dst[7] , src[7]);
#endif
#if(UNROLL > 8)
      GF_ADDMULC(dst[8] , src[8]);
      GF_ADDMULC(dst[9] , src[9]);
      GF_ADDMULC(dst[10] , src[10]);
      GF_ADDMULC(dst[11] , src[11]);
      GF_ADDMULC(dst[12] , src[12]);
      GF_ADDMULC(dst[13] , src[13]);
      GF_ADDMULC(dst[14] , src[14]);
      GF_ADDMULC(dst[15] , src[15]);
#endif
      }
#endif
   lim += UNROLL - 1;
   for(; dst < lim; dst++, src++)            /* final components */
      GF_ADDMULC(*dst , *src);
   }

/*
* computes C = AB where A is n*k, B is k*m, C is n*m
*/
void
matmul(byte *a, byte *b, byte *c, int n, int k, int m)
   {
   int row, col, i;

   for(row = 0; row < n; row++)
      {
      for(col = 0; col < m; col++)
         {
         byte *pa = &a[ row * k ];
         byte *pb = &b[ col ];
         byte acc = 0;
         for(i = 0; i < k; i++, pa++, pb += m)
            acc ^= gf_mul(*pa, *pb);
         c[ row * m + col ] = acc;
         }
      }
   }

/*
* i use malloc so many times, it is easier to put checks all in
* one place.
*/
void *
my_malloc(int sz, const char *err_string)
   {
   void *p = malloc(sz);
   if(!p)
      throw std::bad_alloc();
   return p;
   }

/*
* invert_mat() takes a matrix and produces its inverse
* k is the size of the matrix.
* (Gauss-Jordan, adapted from Numerical Recipes in C)
* Return non-zero if singular.
*/
void invert_mat(byte *src, int k)
   {
   byte c, *p;
   int irow, icol, row, col, i, ix;

   std::vector<int> indxc(k);
   std::vector<int> indxr(k);
   std::vector<int> ipiv(k);
   std::vector<byte> id_row(k);
   std::vector<byte> temp_row(k);

   /*
   * ipiv marks elements already used as pivots.
   */
   for(i = 0; i < k; i++)
      ipiv[i] = 0;

   for(col = 0; col < k; col++)
      {
      byte *pivot_row;
      /*
      * Zeroing column 'col', look for a non-zero element.
      * First try on the diagonal, if it fails, look elsewhere.
      */
      irow = icol = -1;

      if(ipiv[col] != 1 && src[col*k + col] != 0)
         {
         irow = col;
         icol = col;
         goto found_piv;
         }

      for(row = 0; row < k; row++)
         {
         if(ipiv[row] != 1)
            {
            for(ix = 0; ix < k; ix++)
               {
               if(ipiv[ix] == 0)
                  {
                  if(src[row*k + ix] != 0)
                     {
                     irow = row;
                     icol = ix;
                     goto found_piv;
                     }
                  }
               else if(ipiv[ix] > 1)
                  throw std::invalid_argument("singlar matrix");
               }
            }
         }

      if(icol == -1)
         throw std::invalid_argument("pivot not found in invert_mat");

      found_piv:
      ++(ipiv[icol]);
      /*
      * swap rows irow and icol, so afterwards the diagonal
      * element will be correct. Rarely done, not worth
      * optimizing.
      */
      if(irow != icol)
         {
         for(ix = 0; ix < k; ix++)
            {
            std::swap(src[irow*k + ix], src[icol*k + ix]);
            }
         }
      indxr[col] = irow;
      indxc[col] = icol;
      pivot_row = &src[icol*k];
      c = pivot_row[icol];

      if(c == 0)
         throw std::invalid_argument("singlar matrix");

      if(c != 1)
         { /* otherwhise this is a NOP */
         /*
         * this is done often , but optimizing is not so
         * fruitful, at least in the obvious ways (unrolling)
         */
         c = GF_INVERSE[ c ];
         pivot_row[icol] = 1;
         for(ix = 0; ix < k; ix++)
            pivot_row[ix] = gf_mul(c, pivot_row[ix]);
         }
      /*
      * from all rows, remove multiples of the selected row
      * to zero the relevant entry (in fact, the entry is not zero
      * because we know it must be zero).
      * (Here, if we know that the pivot_row is the identity,
      * we can optimize the addmul).
      */
      id_row[icol] = 1;
      if(memcmp(pivot_row, &id_row[0], k*sizeof(byte)) != 0)
         {
         for(p = src, ix = 0; ix < k; ix++, p += k)
            {
            if(ix != icol)
               {
               c = p[icol];
               p[icol] = 0;
               addmul(p, pivot_row, c, k);
               }
	    }
         }
      id_row[icol] = 0;
      } /* done all columns */
   for(col = k-1; col >= 0; col--)
      {
      if(indxr[col] <0 || indxr[col] >= k)
         fprintf(stderr, "AARGH, indxr[col] %d\n", indxr[col]);
      else if(indxc[col] <0 || indxc[col] >= k)
         fprintf(stderr, "AARGH, indxc[col] %d\n", indxc[col]);
      else
         if(indxr[col] != indxc[col])
            {
	    for(row = 0; row < k; row++)
               {
               std::swap(src[row*k + indxr[col]], src[row*k + indxc[col]]);
               }
            }
      }
   }

/*
* fast code for inverting a vandermonde matrix.
* XXX NOTE: It assumes that the matrix
* is not singular and _IS_ a vandermonde matrix. Only uses
* the second column of the matrix, containing the p_i's.
*
* Algorithm borrowed from "Numerical recipes in C" -- sec.2.8, but
* largely revised for my purposes.
* p = coefficients of the matrix (p_i)
* q = values of the polynomial (known)
*/

int
invert_vdm(byte *src, int k)
   {
   int i, j, row, col;
   byte t, xx;

   if(k == 1) 	/* degenerate case, matrix must be p^0 = 1 */
      return 0;
   /*
   * c holds the coefficient of P(x) = Prod (x - p_i), i=0..k-1
   * b holds the coefficient for the matrix inversion
   */
   std::vector<byte> c(k), b(k), p(k);

   for(j=1, i = 0; i < k; i++, j+=k)
      {
      c[i] = 0;
      p[i] = src[j];    /* p[i] */
      }
   /*
   * construct coeffs. recursively. We know c[k] = 1 (implicit)
   * and start P_0 = x - p_0, then at each stage multiply by
   * x - p_i generating P_i = x P_{i-1} - p_i P_{i-1}
   * After k steps we are done.
   */
   c[k-1] = p[0];	/* really -p(0), but x = -x in GF(2^m) */
   for(i = 1; i < k; i++)
      {
      byte p_i = p[i]; /* see above comment */
      for(j = k-1  - (i - 1); j < k-1; j++)
         c[j] ^= gf_mul(p_i, c[j+1]);
      c[k-1] ^= p_i;
      }

   for(row = 0; row < k; row++)
      {
      /*
      * synthetic division etc.
      */
      xx = p[row];
      t = 1;
      b[k-1] = 1; /* this is in fact c[k] */
      for(i = k-2; i >= 0; i--)
         {
         b[i] = c[i+1] ^ gf_mul(xx, b[i+1]);
         t = gf_mul(xx, t) ^ b[i];
         }
      for(col = 0; col < k; col++)
         src[col*k + row] = gf_mul(GF_INVERSE[t], b[col]);
      }

   return 0;
   }

void init_fec()
   {
   static int fec_initialized = 0;

   if(!fec_initialized)
      {
      init_mul_table();
      fec_initialized = 1;
      }
   }

/*
* shuffle move src packets in their position
*/
int
shuffle(byte *pkt[], int index[], int k)
   {
   for(int i = 0; i < k;)
      {
      if(index[i] >= k || index[i] == i)
         i++;
      else
         {
         /*
         * put pkt in the right position (first check for conflicts).
         */
         int c = index[i];

         if(index[c] == c)
            {
            return 1;
	    }
         std::swap(index[i], index[c]);
         std::swap(pkt[i], pkt[c]);
         }
      }
   return 0;
   }

/*
* build_decode_matrix constructs the encoding matrix given the
* indexes. The matrix must be already allocated as
* a vector of k*k elements, in row-major order
*/
byte *
build_decode_matrix(struct fec_parms *code, byte *pkt[], int index[])
   {
   int i , k = code->k;
   byte *p;
   byte* matrix = (byte*)my_malloc(k * k, "gf");

   for(i = 0, p = matrix; i < k; i++, p += k)
      {
#if 1 /* this is simply an optimization, not very useful indeed */
      if(index[i] < k)
         {
         memset(p, 0, k*sizeof(byte));
         p[i] = 1;
         } else
#endif
         if(index[i] < code->n)
            memcpy(p, &(code->enc_matrix[index[i]*k]), k*sizeof(byte));
         else
            {
	    fprintf(stderr, "decode: invalid index %d (max %d)\n",
                    index[i], code->n - 1);
	    free(matrix);
	    return NULL;
            }
      }

   try
      {
      invert_mat(matrix, k);
      return matrix;
      }
   catch(std::exception& e)
      {
      fprintf(stderr, "%s", e.what());
      free(matrix);
      return NULL;
      }

   }

}

/*
* This section contains the proper FEC encoding/decoding routines.
* The encoding matrix is computed starting with a Vandermonde matrix,
* and then transforming it into a systematic matrix.
*/

#define FEC_MAGIC	0xFECC0DEC

void
fec_free(struct fec_parms *p)
   {
   if(p==NULL ||
       p->magic != (((FEC_MAGIC ^ p->k) ^ p->n)))
      {
      fprintf(stderr, "bad parameters to fec_free\n");
      return;
      }
   free(p->enc_matrix);
   free(p);
   }

/*
* create a new encoder, returning a descriptor. This contains k,n and
* the encoding matrix.
*/
struct fec_parms *
fec_new(int k, int n)
   {
   int row, col;

   struct fec_parms *retval;

   init_fec();

   if(k > 0xFF + 1 || n > 0xFF + 1 || k > n)
      {
      fprintf(stderr, "Invalid parameters k %d n %d GF_SIZE %d\n",
              k, n, 0xFF);
      return NULL;
      }
   retval = (fec_parms*)my_malloc(sizeof(struct fec_parms), "new_code");
   retval->k = k;
   retval->n = n;
   retval->enc_matrix = (byte*)my_malloc(n * k, "gf");
   retval->magic = ((FEC_MAGIC ^ k) ^ n);

   std::vector<byte> tmp_m(n * k);
   /*
   * fill the matrix with powers of field elements, starting from 0.
   * The first row is special, cannot be computed with exp. table.
   */
   tmp_m[0] = 1;
   for(col = 1; col < k; col++)
      tmp_m[col] = 0;
   for(byte* p = &tmp_m[k], row = 0; row < n-1; row++, p += k)
      {
      for(col = 0; col < k; col ++)
         p[col] = GF_EXP[modnn(row*col)];
      }

   /*
   * quick code to build systematic matrix: invert the top
   * k*k vandermonde matrix, multiply right the bottom n-k rows
   * by the inverse, and construct the identity matrix at the top.
   */
   invert_vdm(&tmp_m[0], k); /* much faster than invert_mat */
   matmul(&tmp_m[k*k], &tmp_m[0], retval->enc_matrix + k*k, n - k, k, k);
   /*
   * the upper matrix is I so do not bother with a slow multiply
   */
   memset(retval->enc_matrix, 0, k*k*sizeof(byte));
   for(byte* p = retval->enc_matrix, col = 0; col < k; col++, p += k+1)
      *p = 1;

   return retval;
   }

/*
* fec_encode accepts as input pointers to n data packets of size sz,
* and produces as output a packet pointed to by fec, computed
* with index "index".
*/
void
fec_encode(struct fec_parms *code, byte *src[], byte *fec, int index, int sz)
   {
   int i, k = code->k;
   byte *p;

   if(index < k)
      memcpy(fec, src[index], sz*sizeof(byte));
   else if(index < code->n)
      {
      p = &(code->enc_matrix[index*k]);
      memset(fec, 0, sz*sizeof(byte));
      for(i = 0; i < k; i++)
         addmul(fec, src[i], p[i], sz);
      }
   else
      fprintf(stderr, "Invalid index %d (max %d)\n",
              index, code->n - 1);
   }

/*
* fec_decode receives as input a vector of packets, the indexes of
* packets, and produces the correct vector as output.
*
* Input:
*	code: pointer to code descriptor
*	pkt:  pointers to received packets. They are modified
*	      to store the output packets (in place)
*	index: pointer to packet indexes (modified)
*	sz:    size of each packet
*/
int
fec_decode(struct fec_parms *code, byte *pkt[], int index[], int sz)
   {
   byte *m_dec;
   byte **new_pkt;
   int row, col , k = code->k;

   if(shuffle(pkt, index, k))	/* error if true */
      return 1;
   m_dec = build_decode_matrix(code, pkt, index);

   if(m_dec == NULL)
      return 1; /* error */
   /*
   * do the actual decoding
   */
   new_pkt = (byte**)my_malloc (k * sizeof (byte *), "new pkt pointers");
   for(row = 0; row < k; row++)
      {
      if(index[row] >= k)
         {
         new_pkt[row] = (byte*)my_malloc (sz * sizeof (byte), "new pkt buffer");
         memset(new_pkt[row], 0, sz * sizeof(byte));
         for(col = 0; col < k; col++)
            addmul(new_pkt[row], pkt[col], m_dec[row*k + col], sz);
         }
      }
   /*
   * move pkts to their final destination
   */
   for(row = 0; row < k; row++)
      {
      if(index[row] >= k)
         {
         memcpy(pkt[row], new_pkt[row], sz*sizeof(byte));
         free(new_pkt[row]);
         }
      }
   free(new_pkt);
   free(m_dec);

   return 0;
   }
