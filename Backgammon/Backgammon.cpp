// Backgammon.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>



int main()
{
    std::cout << "Hello World!\n";
}


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


typedef enum bitboard_t : uint32_t {
	NO_SQUARES = 0,
	ALL_SQUARES = ~uint64_t(0)
} Bitboard;

inline Bitboard	 Bit(SqrInt n) { Assert(0 <= n && n < 64); return Bitboard(uint64(1) << n); }

// Isolate lowest set bit -- compiler intrinsic for (b&(-b))
inline Bitboard  blsi(Bitboard  b) { return Bitboard(_blsi_u64(uint64_t(b))); }
// Remove lowest set bit  -- compiler intrinsic for b&(b-1)
inline Bitboard  blsr(Bitboard  b) { return Bitboard(_blsr_u64(b)); }


/// x `and not` y : x & ~y
/// NOTE: intrinsic negates first argument whereas our convention negates the second.
inline Bitboard  andn(Bitboard  x, Bitboard y) { return Bitboard(_andn_u64(uint64(y), uint64(x))); }

// define iteration protocol over a Bitboard enum as a set of Squares
// for( Square sq : <Bitboard expression> ) { ...sq... }
inline Bitboard  begin(Bitboard  set) { return set; }
inline Bitboard  end(Bitboard  set) { return NO_SQUARES; }
inline Square    operator*  (Bitboard  it) { return Square(BSF(it)); }
inline Bitboard& operator++ (Bitboard& it) { return it = blsr(it); }

inline Bitboard  operator&	(Bitboard b1, Bitboard b2) { return Bitboard(uint64(b1) & uint64(b2)); }
inline Bitboard  operator|	(Bitboard b1, Bitboard b2) { return Bitboard(uint64(b1) | uint64(b2)); }
inline Bitboard  operator^	(Bitboard b1, Bitboard b2) { return Bitboard(uint64(b1) ^ uint64(b2)); }
inline Bitboard  operator<<	(Bitboard b1, int n) { return Bitboard(uint64(b1) << n); }
inline Bitboard  operator>>	(Bitboard b1, int n) { return Bitboard(uint64(b1) >> n); }
inline Bitboard  operator~	(Bitboard b) { return Bitboard(~uint64(b)); }

inline Bitboard  operator&	(Bitboard b, Square sq) { return b & Bit(sq); }
inline Bitboard  operator|	(Bitboard b, Square sq) { return b | Bit(sq); }
inline Bitboard  operator|	(Square x, Square y) { return Bit(x) | Bit(y); }
inline Bitboard  operator^	(Bitboard b, Square sq) { return b ^ Bit(sq); }
inline Bitboard& operator&=	(Bitboard& b, Square sq) { return b = b & Bit(sq); }
inline Bitboard& operator|=	(Bitboard& b, Square sq) { return b = b | Bit(sq); }
inline Bitboard& operator^=	(Bitboard& b, Square sq) { return b = b ^ Bit(sq); }

inline bool		 member(Bitboard b, Square sq) { return (b & sq) != 0; }

std::ostream& operator<<(std::ostream& os, Bitboard bb);

constexpr Bitboard all = ALL_SQUARES; //  Bitboard(~uint64_t(0));
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
