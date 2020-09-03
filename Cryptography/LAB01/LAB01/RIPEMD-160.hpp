#pragma once

#include <vector>
#include <string>

namespace RIPEMD_160 
{
	using namespace std;

	typedef unsigned UINT;

	class RIPEMD 
	{
	public:
		string RIPEMD160(const string& patch);
	private:
		bool read(const string& patch);
		UINT word(const string& str);
		UINT rol(UINT left, const UINT& right);
		UINT f(const UINT& j, const UINT& x, const UINT& y, const UINT& z);
	private:
		// block < words < bytes > >
		vector<vector<string>> M;	// Message
		const pair<UINT, UINT> K[5] =
		{
			make_pair(0x00000000,0x50A28BE6),
			make_pair(0x5A827999,0x5C4DD124),
			make_pair(0x6ED9EBA1,0x6D703EF3),
			make_pair(0x8F1BBCDC,0x7A6D76E9),
			make_pair(0xA953FD4E,0x00000000)
		};
		const pair<vector<UINT>, vector<UINT>> R =
		{
			make_pair(
				vector<UINT> { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
							  7,  4, 13,  1, 10,  6, 15,  3, 12,  0,  9,  5,  2, 14, 11,  8,
							  3, 10, 14,  4,  9, 15,  8,  1,  2,  7,  0,  6, 13, 11,  5, 12,
							  1,  9, 11, 10,  0,  8, 12,  4, 13,  3,  7, 15, 14,  5,  6,  2,
							  4,  0,  5,  9,  7, 12,  2, 10, 14,  1,  3,  8, 11,  6, 15, 13},

				vector<UINT> { 5, 14,  7,  0,  9,  2, 11,  4, 13,  6, 15,  8,  1, 10,  3, 12,
							  6, 11,  3,  7,  0, 13,  5, 10, 14, 15,  8, 12,  4,  9,  1,  2,
							 15,  5,  1,  3,  7, 14,  6,  9, 11,  8, 12,  2, 10,  0,  4, 13,
							  8,  6,  4,  1,  3, 11, 15,  0,  5, 12,  2, 13,  9,  7, 10, 14,
							 12, 15, 10,  4,  1,  5,  8,  7,  6,  2, 13, 14,  0,  3,  9, 11}
			)
		};
		const pair<vector<UINT>, vector<UINT>> S =
		{
			make_pair(
				vector<UINT> {11, 14, 15, 12,  5,  8,  7,  9, 11, 13, 14, 15,  6,  7,  9,  8,
							  7,  6,  8, 13, 11,  9,  7, 15,  7, 12, 15,  9, 11,  7, 13, 12,
							 11, 13,  6,  7, 14,  9, 13, 15, 14,  8, 13,  6,  5, 12,  7,  5,
							 11, 12, 14, 15, 14, 15,  9,  8,  9, 14,  5,  6,  8,  6,  5, 12,
							  9, 15,  5, 11,  6,  8, 13, 12,  5, 12, 13, 14, 11,  8,  5,  6},

				vector<UINT> { 8,  9,  9, 11, 13, 15, 15,  5,  7,  7,  8, 11, 14, 14, 12,  6,
							  9, 13, 15,  7, 12,  8,  9, 11,  7,  7, 12,  7,  6, 15, 13, 11,
							  9,  7, 15, 11,  8,  6,  6, 14, 12, 13,  5, 14, 13, 13,  7,  5,
							 15,  5,  8, 11, 14, 14,  6, 14,  6,  9, 12,  9, 12,  5, 15,  8,
							  8,  5, 12,  9, 12,  5, 14,  6,  8, 13,  6,  5, 15, 13, 11, 11}
			)
		};
		const vector<UINT> h =
		{
			0x67452301,
			0xEFCDAB89,
			0x98BADCFE,
			0x10325476,
			0xC3D2E1F0
		};
	};
}
