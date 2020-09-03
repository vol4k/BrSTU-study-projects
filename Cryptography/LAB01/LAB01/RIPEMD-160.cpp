#include "RIPEMD-160.hpp"
#include <iostream>
#include <fstream>
#include <cmath>

namespace RIPEMD_160 {

	bool RIPEMD::read(const string& patch)
	{
		ifstream file;
		try 
		{
			// opening
			file.open(patch, ios::binary);
			if (!file.is_open()) throw "File can't opened";

			// detecting size
			unsigned long long size = 0;
			file.seekg(0, ios::end);
			size = file.tellg();
			file.seekg(0, ios::beg);

			// reading
			for (unsigned long long done = 0; done < size;)
			{
				vector<string> block;
				for (auto i = 0; i < 16 && done < size; i++)
				{
					char word[5];
					file.read(word, 4);

					word[4] = NULL;

					block.push_back(word);
					
					done += 4;

					if (done > size) {
						for (auto count = done - size; count; count--)
							block.back().pop_back();
					}
				}
				M.push_back(block);
			}
			file.close();

			// complement bits
			bool firstBit = true;

			if (M.empty())
				M.push_back({ {} });
			do
			{
				if (M.back().size() < 16)
				{
					if (M.back().back().size() < 4)
					{
						M.back().back().push_back(firstBit ? 128 : 0);
					}
					else
					{
						M.back().push_back({ static_cast<char>(firstBit ? 128 : 0),0,0,0 });
					}
				}
				else
				{
					M.push_back({ { static_cast<char>(firstBit ? 128 : 0),0,0,0 } });
				}
				firstBit = false;

			} while (M.back().size() != 14);

			// complement message size

			M.back().push_back(
				{
					static_cast<char>((size << (0 * 8)) & 0xFF),
					static_cast<char>((size << (1 * 8)) & 0xFF),
					static_cast<char>((size << (2 * 8)) & 0xFF),
					static_cast<char>((size << (3 * 8)) & 0xFF),
				}
			);
			M.back().push_back(
				{
					static_cast<char>((size << (4 * 8)) & 0xFF),
					static_cast<char>((size << (5 * 8)) & 0xFF),
					static_cast<char>((size << (6 * 8)) & 0xFF),
					static_cast<char>((size << (7 * 8)) & 0xFF),
				}
			);

		}
		catch (const char *err)
		{
			cerr << "Error! " << err << endl;
			return false;
		}
		catch (...)
		{
			cerr << "Uncknown error!" << endl;
			file.close();
			return false;
		}

		return true;
	}

	UINT RIPEMD::word(const string& str)
	{
		return (
			(static_cast<UINT>(str[0]) << (0 * 8)) +
			(static_cast<UINT>(str[1]) << (1 * 8)) +
			(static_cast<UINT>(str[2]) << (2 * 8)) +
			(static_cast<UINT>(str[3]) << (3 * 8)));
	}

	UINT RIPEMD::rol(UINT left, const UINT& right)
	{
		for (auto i = 0; i < right; i++)
		{
			auto temp = left >> 31;
			left <<= 1;
			left += temp;
		}
		return left;
	}

	UINT RIPEMD::f(const UINT& index, const UINT& x, const UINT& y, const UINT& z)
	{
		switch (index)
		{
		case 0: return x ^ y ^ z;
		case 1: return (x & y) | ((~x) & z);
		case 2: return (x | (~y)) ^ z;
		case 3: return (x & z) | (y & (~z));
		case 4: return x ^ (y | (~z));
		default:return 0;
		}
	}

	string RIPEMD::RIPEMD160(const string& patch)
	{
		if(!read(move(patch))) return "Reading failed!";

		vector<UINT> h = this->h;

		for (size_t i = 0; i < M.size(); i++)
		{
			UINT A1 = h[0], B1 = h[1], C1 = h[2], D1 = h[3], E1 = h[4];
			UINT A2 = h[0], B2 = h[1], C2 = h[2], D2 = h[3], E2 = h[4];
			
			UINT T;

			for (int j = 0; j < 80; j++)
			{
				T  = A1 + f(j / 16, B1, C1, D1);
				T += word(M[i][R.first.at(j)]);
				T += K[j / 16].first;
				T  = rol(T, S.first[j]);
				T += E1;
				
				A1 = E1;
				E1 = D1;
				D1 = rol(C1, 10);
				C1 = B1; 
				B1 = T;
				
				T  = A2 + f((79 - j) / 16, B2, C2, D2);
				T += word(M[i][R.second.at(j)]);
				T += K[j / 16].second;
				T  = rol(T, S.second[j]);
				T += E2;

				A2 = E2;
				E2 = D2;
				D2 = rol(C2, 10);
				C2 = B2; 
				B2 = T;
			}

			D2 += h[1] + C1;
			h[1] = h[2] + D1 + E2;
			h[2] = h[3] + E1 + A2;
			h[3] = h[4] + A1 + B2;
			h[4] = h[0] + B1 + C2;
			h[0] = D2;

		}

		for (auto h : h)
			cout << hex << h << ' ';
		cout << endl;

		return "Done";
	}
}
