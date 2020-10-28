#pragma once
#include <string>

namespace crypt
{
	using namespace std;

	class RC4
	{
	public:
		RC4(unsigned n = 8);
		RC4(const string& key, unsigned n = 8);

		RC4() = delete;
		RC4(const RC4&) = delete;
		RC4 operator =(const RC4&) = delete;

		inline void init(const string& key);

		string encode(string M);
		string decode(string M);

	private:
		inline char keyItem();

	private:
		string S;
		unsigned blockLen;
		unsigned iterator1;
		unsigned iterator2;
	};
}