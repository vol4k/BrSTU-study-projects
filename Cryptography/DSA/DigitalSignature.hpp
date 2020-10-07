#pragma once

#include <boost/random.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include "SHA1.hpp"

namespace DS {

	namespace mp = boost::multiprecision;

	class DSA {
	public:
		DSA(bool init = true);

		DSA() = delete;
		DSA(const DSA&) = delete;

		void setData(
			const mp::cpp_int G,
			const mp::cpp_int P,
			const mp::cpp_int q,
			const mp::cpp_int Y,
			const mp::cpp_int X = 0);

		// make digital signature
		std::pair<mp::cpp_int, mp::cpp_int> S(const std::string& M);

		std::vector<mp::cpp_int> getData();

		bool checkS(std::string M, std::pair<mp::cpp_int, mp::cpp_int> rs);

	private:
		bool is_prime(mp::cpp_int n, mp::cpp_int k = 50);

		std::string to_str(mp::cpp_int seed);

		std::pair<mp::cpp_int, mp::cpp_int> extended_euclid(mp::cpp_int a, mp::cpp_int b);

		mp::cpp_int inverse(mp::cpp_int a, mp::cpp_int n);

		mp::cpp_int pow(mp::cpp_int x, mp::cpp_int n, mp::cpp_int mod = 0);

	private:
		mp::cpp_int G;	// % 2^1024	<=> % mp::pow((mp::cpp_int)2, 1024)		public	prime
		mp::cpp_int P;	// % 2^1024	<=> % mp::pow((mp::cpp_int)2, 1024)		public	prime
		mp::cpp_int q;	// % 2^160	<=> % mp::pow((mp::cpp_int)2, 160)		public	prime

		mp::cpp_int Y;	// % 2^160	<=> % mp::pow((mp::cpp_int)2, 160)		public	key
		mp::cpp_int X;	// % 2^160	<=> % mp::pow((mp::cpp_int)2, 160)		private	key

		boost::random::mt19937 rgn;
	};
}