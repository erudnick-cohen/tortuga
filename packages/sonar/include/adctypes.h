/**
 * @file adctypes.h
 * Generic datatypes for fixed-with integers
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 * How to use:
 * 
 *   Choose the bit depth of your ADC -- say, 4 bits.
 *   To declare variables that stores ADC values, do the following:
 *
 *     adctype<4>::SIGNED myNum;     // equivalent to char
 *     adctype<4>::UNSIGNED myNum2;  // equivalent to unsigned char
 *   
 *   You may want to have another data type for numbers that are twice as wide
 *   as your ADC for intermediate calculations.
 *
 *     adctype<4>::DOUBLE_WIDE::SIGNED myNum3;    // equivalent to char
 *     adctype<4>::DOUBLE_WIDE::UNSIGNED myNum4;  // equivalent to unsigned char
 *
 *   Or, you might even want a quadruple wide type.
 *   
 *     adctype<4>::QUADRUPLE_WIDE::SIGNED myNum5;   // equivalent to short on most platforms
 *     adctype<4>::QUADRUPLE_WIDE::UNSIGNED myNum6; // equivalent to unsigned short on most platforms
 *   
 *   You don't have to limit yourself to bit depths that are multiples of two.
 *
 *     adctype<3>::SIGNED myNum7;   // char
 *     adctype<22>::DOUBLE_WIDE::UNSIGNED myNum8;    // unsigned int
 *
 *   If you request a type that cannot be created on your machine, then an error
 *   will be generated.
 *
 *     adctype<32>::SIGNED myNum9;                    // works
 *     adctype<32>::DOUBLE_WIDE::SIGNED myNum10;      // works
 *     adctype<32>::QUADRUPLE_WIDE::SIGNED myNum11;   // FAILS
 *     adctype<33>::SIGNED myNum12;                   // works
 *     adctype<33>::DOUBLE_WIDE::SIGNED myNum13;      // FAILS
 *     adctype<128>::SIGNED myNum14;                  // FAILS
 *
 *   You can get useful information about your ADC-specific types;
 *
 *     adctype<5>::SIGNED_MIN   // -32
 *     adctype<5>::SIGNED_MAX   // 31
 *     adctype<5>::DOUBLE_WIDE::UNSIGNED_MAX  // 1023
 *     adctype<5>::QUADRUPLE_WIDE::BITDEPTH   // 20
 *     adctype<5>::QUADRUPLE_WIDE::MACHINE_SIZE  // 4 (sizeof(int))
 *
 */

#ifndef _RAM_SONAR_ADCTYPES_H
#define _RAM_SONAR_ADCTYPES_H

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

namespace ram { namespace sonar {

namespace { // hide messy internals in an anonymous namespace
	template<int bitRange>
	struct adctype1
	{
	};

	template<>
	struct adctype1<0> {
		typedef int8_t SIGNED;
		typedef uint8_t UNSIGNED;
	};

	template<>
	struct adctype1<1> {
		typedef int16_t SIGNED;
		typedef uint16_t UNSIGNED;
	};

	template<>
	struct adctype1<2> {
		typedef int32_t SIGNED;
		typedef int32_t UNSIGNED;
	};

	template<>
	struct adctype1<3> {
		typedef int64_t SIGNED;
		typedef uint64_t UNSIGNED;
	};
}

template<int _bitDepth>
struct adctype
{
private:
	static const int _bitRange = 
	(_bitDepth <= 8 ? 0 : 
	 (_bitDepth <= 16 ? 1 : 
	  (_bitDepth <= 32 ? 2 : 
	   (_bitDepth <= 64 ? 3 : 4))));
	static const uint64_t UINT64_MAX = ULLONG_MAX;
public:
	typedef typename adctype1<_bitRange>::SIGNED SIGNED;
	typedef typename adctype1<_bitRange>::UNSIGNED UNSIGNED;
	typedef struct adctype<1*_bitDepth> SINGLE_WIDE;
	typedef struct adctype<2*_bitDepth> DOUBLE_WIDE;
	typedef struct adctype<4*_bitDepth> QUADRUPLE_WIDE;
	
	static const int BITDEPTH = _bitDepth;
	static const SIGNED SIGNED_MIN = -((int64_t)1 << (_bitDepth - 1));
	static const SIGNED SIGNED_MAX =  ((int64_t)1 << (_bitDepth - 1)) - 1;
	static const UNSIGNED UNSIGNED_MAX = ((~((uint64_t)0)) >> (64 - _bitDepth));
	static const size_t MACHINE_SIZE = sizeof(SIGNED);
};

}} // namespace ram::sonar

#endif