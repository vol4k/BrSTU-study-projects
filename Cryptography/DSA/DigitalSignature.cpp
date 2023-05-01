#include "DigitalSignature.hpp"

namespace DS
{
	//////////////////////////////////////////////////////////////////////
	/////////////////////////////ISSUE////////////////////////////////////
	//////////////////////////////////////////////////////////////////////

	DSA::DSA(bool init)
	{
		if (init)
		{
			bool done = false;
			do {
				mp::cpp_int n(1023 / 160);
				mp::cpp_int b(1023 % 160);
				mp::cpp_int SEED;
				do {
					SEED = 0;
					// GENERATE RANDOM SEED
					for (auto i = 0; i < 160; i++)
						if (rand() & 1)
							SEED += pow(2, i);

					// CALCULATE q VALUE
					q = (mp::cpp_int("0x" + sha1(to_str(SEED))) ^
						mp::cpp_int("0x" + sha1(to_str((SEED + 1))))) %
						pow(2, 160);

					// first and last bit has ben 1
					if (!(q % 2)) q += 1;
					if (q < pow(2, 159)) q += pow(2, 159);
				} while (!is_prime(q));

				mp::cpp_int counter(0), offset(2);
				do {
					mp::cpp_int W(0);
					for (mp::cpp_int k = 0; k <= n; k += 1)
					{
						mp::cpp_int V = mp::cpp_int("0x" + sha1(to_str((SEED + offset + k) % pow(2, 160))));
						W += (V % (2 * b)) * pow(2, k * 159);
					}
					mp::cpp_int x = W + pow(2, 1023);

					mp::cpp_int c = x % (2 * q);
					P = x - (c - 1);

					if (P >= pow(2, 1023))
						done = is_prime(P);

					static std::vector<mp::cpp_int> vec;

					counter += 1; offset += n + 1;
				} while (counter < 4096 && !done);
			} while (!done);

			mp::cpp_int itG(0);
			for (mp::cpp_int h = 2; h < P - 1; h += 1)
			{
				G = pow(h, (P - 1) / q, P);
				if (is_prime(G)) break;
			}
			boost::random::uniform_int_distribution<mp::cpp_int> gen(2, q - 1);

			// X = (1 < {random value} < q)
			X = gen(rgn);

			// Y = G^X % P
			Y = pow(G, X, P);
		}
	}

	bool DSA::is_prime(mp::cpp_int n, mp::cpp_int k)
	{
		if (!(n % 2) || n <= 1) return false;
		
		boost::random::uniform_int_distribution<mp::cpp_int> gen(0, n);

		mp::cpp_int t = n - 1;
		mp::cpp_int s = 0;
		do {
			t /= 2;
			s += 1;
		} while (!(t % 2));

		for (mp::cpp_int i = 0; i < k; i += 1)
		{
			mp::cpp_int a = gen(rgn) + 2;
			mp::cpp_int x = pow(a, t, n);
			
			if (x == 1 || x == n - 1) continue;
			bool _continue = false;
			for (mp::cpp_int k = 0; k < s - 1; k += 1)
			{
				x = pow(x, 2, n);
				if (x == 1) return false;
				if (x == n - 1) _continue = true;
			}
			if (_continue) continue;
			return false;
		}
		return true;
	}

	std::string DSA::to_str(mp::cpp_int seed)
	{
		std::string str;
		str.resize((unsigned)mp::log2(mp::cpp_dec_float_100(seed)) / 8);
		for (unsigned i = 0; seed; i++)
		{
			char ch(0);
			for (auto j = 0; j < 8 && seed; j++)
			{
				if (seed % 2)
					ch += std::pow(2, i);
				seed /= 2;
			}
			str.push_back(ch);
		}
		return std::move(str);
	}

	mp::cpp_int DSA::pow(mp::cpp_int x, mp::cpp_int n, mp::cpp_int mod)
	{
		mp::cpp_int res(1);
		while (n) {
			if (n % 2)
			{
				res *= x;
				if (mod) res %= mod;
			}
			x *= x;
			if (mod) x %= mod;
			n /= 2;
		}
		return res;
	}

	std::pair<mp::cpp_int, mp::cpp_int> DSA::S(const std::string& M)
	{
		// m = h(M), 1 < m < q
		mp::cpp_int m("0x" + sha1(M));
		m %= q;

		boost::random::uniform_int_distribution<mp::cpp_int> gen(2, q - 1);

		// K = (1 < {random value} < q)
		mp::cpp_int K;
		mp::cpp_int r;
		mp::cpp_int s;
		do {
			K = gen(rgn);

			// r = (G^K % P) % q
			r = pow(G, K, P) % q;
			if (!r) continue;

			// s = ((m + r * X) / K) % q
			s = ((m + r * X) * inverse(K, q)) % q; // w = s^(-1) = (K * (m+r*X)^(-1)) 

		} while (!s);

		if (r < 0) r = (r + q) % q;
		if (s < 0) s = (s + q) % q;

		return std::make_pair(r, s);
	}

	std::vector<mp::cpp_int> DSA::getData()
	{
		return { G,P,q,Y };
	}
	
	//////////////////////////////////////////////////////////////////////
	///////////////////////////VERIFICATION///////////////////////////////
	//////////////////////////////////////////////////////////////////////

	void DSA::setData(
		const mp::cpp_int G, 
		const mp::cpp_int P, 
		const mp::cpp_int q, 
		const mp::cpp_int Y, 
		const mp::cpp_int X)
	{
		this->G = G;
		this->P = P;
		this->q = q;
		this->Y = Y;
		this->X = X;
	}

	bool DSA::checkS(std::string M, std::pair<mp::cpp_int, mp::cpp_int> rs)
	{
		// 0 < s < q && 0 < r < q
		if ((rs.first <= 0 || rs.first >= q) || (rs.second <= 0 || rs.second >= q))
			return false;

		// w = (1/s) % q
		mp::cpp_int w = inverse(rs.second, q);

		// m = h(M)
		mp::cpp_int m("0x" + sha1(M));
		m %= q;

		// u1 = (m * w) % q
		// u2 = (r * w) % q
		mp::cpp_int u1 = mp::cpp_int(m * w) % q;
		mp::cpp_int u2 = mp::cpp_int(rs.first * w) % q;

		// v = ((G^u1 * Y^u2) % P) % q <=> (((G^u1 % P) * (Y^u2 % P)) % P) % q

		// first = G^u1 % P
		mp::cpp_int first = pow(G, u1, P);
		
		// second = Y^u2 % P
		mp::cpp_int second = pow(Y,u2,P);
		
		mp::cpp_int v = ((first * second) % P) % q;

		return (v == rs.first);
	}

	std::pair<mp::cpp_int, mp::cpp_int> DSA::extended_euclid(mp::cpp_int a, mp::cpp_int b)
	{
		mp::cpp_int x, y, d;
		mp::cpp_int q, r, x1, x2, y1, y2;
		if (b == 0) {
			d = a, x = 1, y = 0;
			return std::make_pair(x, d);
		}

		x2 = 1, x1 = 0, y2 = 0, y1 = 1;
		while (b > 0) {
			q = a / b, r = a - q * b;
			x = x2 - q * x1, y = y2 - q * y1;
			a = b, b = r;
			x2 = x1, x1 = x, y2 = y1, y1 = y;
		}

		d = a, x = x2, y = y2;
		return std::make_pair(x, d);
	}

	mp::cpp_int DSA::inverse(mp::cpp_int a, mp::cpp_int n)
		/* module inversion calculation */
	{
		auto xd = extended_euclid(a, n);
		if (xd.second == 1) return (xd.first + n) % n;
		return 0;
	}
}