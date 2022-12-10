#ifndef INTTYP_INCLUDED
#define INTTYP_INCLUDED 1

// Basic integer types
//#undef _HAS_STD_BYTE
#include <stdint.h>
#include "intrinsics.h"

typedef unsigned int                    uint;

typedef int8_t				int8;
typedef uint8_t				uint8;
typedef int16_t				int16;
typedef uint16_t			uint16;
typedef int32_t				int32;
typedef uint32_t			uint32;
typedef int64_t				int64;
typedef uint64_t			uint64;

// integer booleans - 0 is false, non-zero true
typedef uint32 bool32;	// 32 bit integer representing a boolean
typedef uint16 bool16;	// 16 bit integer representing a boolean

#define lo8(x) uint8(x)
#define hi8(x) uint8((x)>>8)
#define lo16(x) uint16(x)
#define hi16(x) uint16((x)>>16)

typedef int				index;
typedef int		        bindex;	// index for bit shift operations

#ifdef _DEBUG
template<class T>
inline void Assert(T x) { _ASSERT(x); }
#else
#define Assert(x)
#endif
#define DIM(x) (sizeof(x)/sizeof(x[0]))

#if 0
template<class T>
T LSB(T b) { return T(b & (-b)); }

#define ClearSet(xs,b)	(xs ^= (b))
#define ClearLsb(xs)	(xs &= xs-1)
#define GreatestSq(x)   BSR(x)
#define LeastSq(x)      BSF(x)
#define LeastBit(x)	    (blsi(x))
#endif

template<typename T>
inline T& operator+=(T& d1, T d2) { return d1 = d1 + d2; }
template<typename T>
inline T& operator+=(T& d1, int d2) { return d1 = d1 + d2; }
template<typename T>
inline T& operator-=(T& d1, T d2) { return d1 = d1 - d2; }
template<typename T>
inline T& operator-=(T& d1, int d2) { return d1 = d1 - d2; }

template<typename T>
inline T& operator|=(T& d1, T d2) { return d1 = d1 | d2; }
template<typename T>
inline T& operator&=(T& d1, T d2) { return d1 = d1 & d2; }
template<typename T>
inline T& operator^=(T& d1, T d2) { return d1 = d1 ^ d2; }
template<typename T>
inline T& operator<<=(T& d1, int n) { return d1 = d1 << n; }
template<typename T>
inline T& operator>>=(T& d1, int n) { return d1 = d1 >> n; }

//////////////////////////////////////////////////////////////////////////////
/// Bitboard interpreted as Set of Pips

using Pip = int32_t;
constexpr Pip w_bar = Pip(0);

typedef enum bitboard_t : Pip {
	NO_SQUARES = 0,
	BOARD	= 0x01fffffe,	// the 24 bits corresponding to pips on the board
	BAREOFF = 0x7ffffffe,	// Board + 6 bits past the end, targets for bare-off
} Bitboard;

inline Bitboard	 Bit(Pip n) { Assert(0 <= n && n < 32); return Bitboard(uint32(1) << n); }

// Isolate lowest set bit -- compiler intrinsic for (b&(-b))
inline Bitboard  blsi		(Bitboard  b)				{ return Bitboard(_blsi_u32(uint64_t(b))); }
// Remove lowest set bit  -- compiler intrinsic for b&(b-1)
inline Bitboard  blsr		(Bitboard  b)				{ return Bitboard(_blsr_u32(b)); }
/// x `and not` y : x & ~y
/// NOTE: intrinsic negates first argument whereas our convention negates the second.
inline Bitboard  andn		(Bitboard  x, Bitboard y)	{ return Bitboard(_andn_u32(uint32(y), uint32(x))); }
inline Bitboard  operator-	(Bitboard  x, Bitboard y)	{ return andn(x,y); }

// define iteration protocol over a Bitboard enum as a set of Pips
// for( Pip pip : <Bitboard expression> ) { ...pip... }
inline Bitboard  begin		(Bitboard  set)				{ return set; }
inline Bitboard  end		(Bitboard  set)				{ return NO_SQUARES; }
inline Pip		 operator*  (Bitboard  it)				{ return Pip(BSF(it)); }
inline Bitboard& operator++ (Bitboard& it)				{ return it = blsr(it); }

inline Bitboard  operator&	(Bitboard b1, Bitboard b2)	{ return Bitboard(uint32(b1) & uint32(b2)); }
inline Bitboard  operator|	(Bitboard b1, Bitboard b2)	{ return Bitboard(uint32(b1) | uint32(b2)); }
inline Bitboard  operator^	(Bitboard b1, Bitboard b2)	{ return Bitboard(uint32(b1) ^ uint32(b2)); }
inline Bitboard  operator<<	(Bitboard b1, int n)		{ return Bitboard(uint32(b1) << n); }
inline Bitboard  operator>>	(Bitboard b1, int n)		{ return Bitboard(uint32(b1) >> n); }

inline Bitboard  operator~	(Bitboard b)				{ return Bitboard(~uint32(b)); }

inline Bitboard  operator&	(Bitboard b, Pip sq)		{ return b & Bit(sq); }
inline Bitboard  operator|	(Bitboard b, Pip sq)		{ return b | Bit(sq); }
inline Bitboard  operator+	(Bitboard b1, Pip p)		{ return b1 | p; }
inline Bitboard  operator-	(Bitboard b1, Pip p)		{ return b1 - Bit(p); }

inline Bitboard  operator^	(Bitboard b, Pip sq)		{ return b ^ Bit(sq); }
inline Bitboard& operator&=	(Bitboard& b, Pip sq)		{ return b = b & Bit(sq); }
inline Bitboard& operator|=	(Bitboard& b, Pip sq)		{ return b = b | Bit(sq); }
inline Bitboard& operator^=	(Bitboard& b, Pip sq)		{ return b = b ^ Bit(sq); }

inline bool		 member		(Bitboard b, Pip sq)		{ return (b & sq) != 0; }

// std::ostream& operator<<(std::ostream& os, Bitboard bb);

#endif // INTTYP_INCLUDED
