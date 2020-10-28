#include "RC4.hpp"
using namespace crypt;

RC4::RC4(unsigned n)
	: iterator1(0)
	, iterator2(0)
	, blockLen(pow(2, n))
{}

RC4::RC4(const string& key, unsigned n)
	: iterator1(0)
	, iterator2(0)
	, blockLen(pow(2, n))
{
	init(key);
}

void RC4::init(const string& key)
{
	int keyLenght = key.size();

	S.clear();
	S.resize(blockLen);

	for (int i = 0; i < blockLen; i++)
		S[i] = i;

	for (int i = 0, j = 0; i < blockLen; i++)
	{
		j = (j + S[i] + key[i % keyLenght]) % blockLen;
		swap(S[i], S[j]);
	}
}

char RC4::keyItem()
{
	iterator1 = (iterator1 + 1) % blockLen;
	iterator2 = (iterator2 + 1) % blockLen;

	swap(S[iterator1], S[iterator2]);

	return S[(S[iterator1] + S[iterator2]) % blockLen];
}

string RC4::encode(string M)
{
	for (auto& byte : M)
		byte ^= keyItem();
	return M;
}

string RC4::decode(string M)
{
	return encode(M);
}
