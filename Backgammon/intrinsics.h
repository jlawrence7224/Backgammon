
#pragma once
#ifndef INTRINSICS_INCLUDED
#define INTRINSICS_INCLUDED 1
//#undef _HAS_STD_BYTE
#include <intrin.h>
#include <immintrin.h>
#include <nmmintrin.h>
#include <crtdbg.h>
//#pragma intrinsic(_BitScanForward32)
//#pragma intrinsic(_BitScanReverse32)
//#pragma intrinsic(_pext_u32)
//#pragma intrinsic(_pdep_u32)
//#pragma intrinsic(_mm_popcnt_u32)
//#pragma intrinsic(_mm_prefetch)
//#pragma intrinsic(_blsi_u32)
//#pragma intrinsic(_blsr_u32)
// #pragma intrinsic(_tzcnt_u32)

	//__forceinline
	//uint64_t Pext(uint64_t v, uint64_t m) { return _pext_u64(v, m); }
	//#define Pext(v,m) _pext_u64((v),(m))
	//#define Pdep(v,m) _pdep_u64((v),(m))

	__forceinline uint32_t PopCnt(uint32_t a) {
		return _mm_popcnt_u32(a);
	}
	__forceinline unsigned long BSR(uint32_t a)
	{
		unsigned long v;

		_BitScanReverse(&v, a);
		return v;
	}

#if 1 // USE_BSF_INSTR
	__forceinline unsigned long BSF(uint32_t a)
	{
		unsigned long v;

		_BitScanForward(&v, a);
		return v;
	}
#else
	__forceinline unsigned long BSF(uint32_t a)
	{
		return _tzcnt_u32(a);
	}
#endif
#if 0
	__forceinline int Popcnt(uint32_t a)
	{
		int c = 0;
		if (a) do { c++; } while (a &= a - 1);

		return c;
	}

	__forceinline uint32 _pext_u32(uint32 val, uint32 mask)
	{
		uint32 res = 0;
		for (uint32 bb = 1; mask; bb += bb)
		{
			if (val & mask & -mask)
				res |= bb;
			mask &= mask - 1;
		}
		return res;
	}

	__forceinline uint32 _pdep_u32(uint32 val, uint32 mask)
	{
		uint32 res = 0;
		for (uint32 bb = 1; mask; bb += bb)
		{
			if (val & bb)
				res |= mask & -mask;
			mask &= mask - 1;
		}
		return res;
	}
#endif


#endif // INTRINSICS_INCLUDED
