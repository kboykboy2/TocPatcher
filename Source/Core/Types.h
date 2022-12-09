#pragma once
#include<iostream>

namespace RFcore
{
	typedef unsigned char      uint8;
	typedef char               int8;
	typedef unsigned short     uint16;
	typedef short              int16;
	typedef unsigned int       uint32;
	typedef int                int32;
	typedef unsigned long long uint64;
	typedef long long          int64;
	typedef float              float32;
	typedef double             float64;

	static const float fractionbit_lookup[10]{ pow(2, -(1)), pow(2, -(2)), pow(2, -(3)), pow(2, -(4)), pow(2, -(5)), pow(2, -(6)), pow(2, -(7)), pow(2, -(8)), pow(2, -(9)), pow(2, -(10)) };
	static const float exponent_lookup_p[16]{ pow(2, 0), pow(2, 1), pow(2, 2), pow(2, 3), pow(2, 4), pow(2, 5), pow(2, 6), pow(2, 7), pow(2, 8), pow(2, 9), pow(2, 10), pow(2, 11), pow(2, 12), pow(2, 13), pow(2, 14), pow(2, 15) };
	static const float exponent_lookup_n[16]{ pow(2, -0), pow(2, -1), pow(2, -2), pow(2, -3), pow(2, -4), pow(2, -5), pow(2, -6), pow(2, -7), pow(2, -8), pow(2, -9), pow(2, -10), pow(2, -11), pow(2, -12), pow(2, -13), pow(2, -14), pow(2, -15) };
	static const char  sign_lookup[2]{ 1, -1 };
	class float16
	{
		unsigned short fraction_bits : 10, exponent_bits : 5, sign_bit : 1;

		bool* GetFractionBits() {
			bool bits[10]{ 0 }; int ridx = 9;
			for (int idx = 0; idx < 10; idx++, ridx--) bits[ridx] = 0 != (fraction_bits & (1 << idx));
			return bits;
		}
		float ToFloat() {
			float Mantissa = 0;
			float Exponent = 0;
			char  Sign = sign_lookup[sign_bit];
			// calculate exponent
			char exponent = exponent_bits - 15; // normalize (remove bias)
			if (exponent < 0) Exponent = exponent_lookup_n[exponent * -1]; else Exponent = exponent_lookup_p[exponent];
			// get fraction bits
			bool* bits = GetFractionBits();
			for (int idx = 0; idx < 10; ++idx) {
				Mantissa += bits[idx] * fractionbit_lookup[idx];
			}
			// calculate and return float
			return (Mantissa + 1) * Exponent * Sign;
		}
		void FromFloat(float value) {};

	public:
		float16() { fraction_bits = 0; exponent_bits = 0; sign_bit = 0; };
		float16(float value) { FromFloat(value); };
		float16(char* bytes) { fraction_bits = 0; exponent_bits = 0; sign_bit = 0; memcpy(this, bytes, 2); };

		operator float() { return ToFloat(); };
	};



	struct vec2
	{
		float x, y;

		vec2() { x = 0; y = 0; };
		vec2(float xfloat, float yfloat) { x = xfloat; y = yfloat; };
		float& operator[](int idx) { 
			if (idx == 0) return x; 
			if (idx == 1) return y;
		};
	};

	struct vec3
	{
		float x, y, z;

		vec3() { x = 0; y = 0; z = 0; };
		vec3(float xfloat, float yfloat, float zfloat) { x = xfloat; y = yfloat; z = zfloat; };
		float& operator[](int idx) {
			if (idx == 0) return x;
			if (idx == 1) return y;
			if (idx == 2) return z;
		};
	};

	struct vec4
	{
		float x, y, z, w;

		vec4() { x = 0; y = 0; z = 0; w = 0;};
		vec4(float xfloat, float yfloat, float zfloat, float wfloat) { x = xfloat; y = yfloat; z = zfloat; w = wfloat;};
		float& operator[](int idx) {
			if (idx == 0) return x;
			if (idx == 1) return y;
			if (idx == 2) return z;
			if (idx == 3) return w;
		};
	};
}