#pragma once
#include <vector>
using namespace std;

class Jacobi {
private:
	vector<pair<int, int>> J;
	int res;
public:
	int Calculate(int a, int p) {
		// [p] не может быть чётным
		if (!(p % 2)) return 404;

		J.clear();
		res = 1;

		J.push_back(make_pair(abs(a), p));

		if(a < 0)
			J.push_back(make_pair(-1, p));

		// [a] и [p] должны быть взаимно простыми
		if (GCD() != 1) return 0;

		do {
			Simplify();
		} while (!J.empty());

		return res;
	}

private:
	vector<int> Prime(int num)
	{
		vector<int> res;
		unsigned i;
			if (num == 1) {
				res.push_back(1);
				return res;
			}

			if (num % 2 == 0) 
			{
				res.push_back(2);
				
				res.push_back(num /= 2);

				return res;
			}

			for (i = 3; i * i <= num && num % i; i += 2)
				;
			if (i * i <= num)
				res.push_back(i);
			else {
				for (i = 3; i * i != num && num % i; i += 2)
					;
				if (i == num)
					res.push_back(i);
			}
			return res;
	}

	void Simplify() {

		vector<int> repeat;

		for (auto j = J.begin(); j < J.end(); j++) {
			// 5-е свойство
			if (j->first == -1) {
				res *= (int)pow(-1, (j->second - 1) / 2) % j->second;
				j = J.erase(j);
				if (j != J.begin()) j--;
				else if (J.empty()) return;
			}
			// 6-е свойство
			if (j->first == 2) {
				res *= (int)pow(-1, (pow(j->second,2) - 1) / 8);
				j = J.erase(j);
				if(j != J.begin()) j--;
				else if (J.empty()) return;
			}

			// 1-е свойство
			auto jTemp = j->first % j->second;
			if (jTemp != j->first) 
			{
				j->first = jTemp;
			}
			// 2-е свойство
			vector<int> vTemp = Prime(j->first);
			if (vTemp.size() > 1) 
			{
				for (auto v : vTemp) 
				{
					// 4-е свойство
					if (v == 1)
						continue;
					// 3-е свойство
					bool push = true;
					for (auto r = repeat.begin(); r < repeat.end() && repeat.size(); r++) 
					{
						if (v == *r)
						{
							push = false;
							r = repeat.erase(r);
							if(r != repeat.begin()) r--;
							break;
						}

					}
					if (push) 
					{
						auto jTemp = j - J.begin();
						J.push_back(make_pair(v, j->second));
						j = J.begin() + jTemp;
						repeat.push_back(v);
					}
				}
				j = J.erase(j);
				if(j != J.begin()) j--;
				else if (J.empty()) return;
				vTemp.clear();
			}

			// 7-е свойство
			if (j->first % 2) {
				res *= (int)pow(-1, (j->second - 1) / 2);
				swap(j->first, j->second);
			}
		}
	}

	vector<int> GetDividers(int num) {
		vector<int> res;

		while (num > 1) {
			auto temp = Prime(num);
			for (auto t : temp)
				num /= t;
			copy(temp.begin(), temp.end(), back_inserter(res));
		}

		return res;
	}

	int GCD() {
		vector<int> first = GetDividers(J.front().first);
		vector<int> second = GetDividers(J.front().second);

		int GCD = 1;

		if (first.empty()) GCD = 0;

		for (auto f : first)
			for (auto s : second)
				if (f == s && f > GCD)
					GCD = f;
		
		return GCD;
	}
};