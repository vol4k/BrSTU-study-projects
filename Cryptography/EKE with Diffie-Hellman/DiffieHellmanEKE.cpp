#include "DiffieHellmanEKE.hpp"
#include "RC4.hpp"

using namespace crypt;

DiffieHellmanEKE::DiffieHellmanEKE(bool mode)
	: P(rand() + 1)
	, K(0)
	, R()
	, r(0)
	, g(rand() + 1)
	, n(rand() + 1)
	, access(init)
{
	switch (mode)
	{
	case server:
		Step =
		{
		bind(&DiffieHellmanEKE::sStep1, this),
		bind(&DiffieHellmanEKE::sStep2, this),
		bind(&DiffieHellmanEKE::sStep3, this)
		};
		break;
	case client:
		Step =
		{
		bind(&DiffieHellmanEKE::cStep1, this),
		bind(&DiffieHellmanEKE::cStep2, this),
		};
		break;
	}
}

bool crypt::DiffieHellmanEKE::verify(function<void(string&)> getter, function<void(string)> sender)
{
	getMessage = getter;
	sendMessage = sender;

	for (auto& step : Step)
		step();

	return access;
}

string crypt::DiffieHellmanEKE::getK()
{
	return K.str();
}

void crypt::DiffieHellmanEKE::sStep1()
{
	r = rand();
	mp::cpp_int A = mp::pow(g, r) % n;

	sendMessage(A.str());
}

void crypt::DiffieHellmanEKE::sStep2()
{
	pair<string,string> data;
	
	getMessage(data.first);
	getMessage(data.second);

	RC4 Ep(P.str());
	mp::cpp_int Gr(Ep.decode(data.first));

	K = mp::pow(Gr, r) % n;

	RC4 Ek(K.str());

	R.resize(1024);
	for (auto& ch : R)
		ch = rand() % 256;

	sendMessage(Ek.encode(data.second));
	sendMessage(Ek.encode(R));
}

void crypt::DiffieHellmanEKE::sStep3()
{
	string data;

	getMessage(data);
	
	access = (data == R ? allowed : denied);
}

void crypt::DiffieHellmanEKE::cStep1()
{
	string data;

	getMessage(data);

	r = rand();
	K = mp::pow(mp::cpp_int(data), r) % n;
	
	RC4 Ep(P.str());

	R.resize(1024);
	for (auto& ch : R)
		ch = rand() % 256;

	mp::cpp_int B = mp::pow(g, r) % n;

	sendMessage(Ep.encode(B.str()));
	sendMessage(R);
}

void crypt::DiffieHellmanEKE::cStep2()
{
	pair<string, string> data;

	getMessage(data.first);
	getMessage(data.second);

	RC4 Ek(K.str());

	access = (Ek.decode(data.second) == R ? allowed : denied);
	
	sendMessage(Ek.decode(data.first));
}
