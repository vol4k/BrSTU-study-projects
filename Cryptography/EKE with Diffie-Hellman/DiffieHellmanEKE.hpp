#pragma once
#include <vector>
#include <boost/multiprecision/cpp_int.hpp>

namespace crypt
{
	using namespace std;
	namespace mp = boost::multiprecision;
	enum { client, server };

	class DiffieHellmanEKE
	{
	public:
		DiffieHellmanEKE(bool mode = server);

		DiffieHellmanEKE() = delete;
		DiffieHellmanEKE(const DiffieHellmanEKE&) = delete;
		DiffieHellmanEKE operator = (const DiffieHellmanEKE&) = delete;

		bool verify(function<void(string&)> getter, function<void(string)> sender);
		string getK();

	private:
		//////////////////////////////////////
		////////client-server tools///////////
		//////////////////////////////////////
		function<void(string&)> getMessage;
		function<void(string)> sendMessage;

		//////////////////////////////////////
		/////////callback-iterator////////////
		//////////////////////////////////////
		vector<function<void (void)>> Step;

		//////////////////////////////////////
		////////////////server////////////////
		//////////////////////////////////////
		void sStep1();
		void sStep2();
		void sStep3();

		//////////////////////////////////////
		////////////////client////////////////
		//////////////////////////////////////
		void cStep1();
		void cStep2();
		
	private:
		enum { denied, allowed, init} access;
		mp::cpp_int P;
		mp::cpp_int K;
		string R;
		unsigned r;
		mp::cpp_int g;
		mp::cpp_int n;
	}; 
}