/*	
	Andersen2020 training task "A. Перестановка цифр"
	В заданном числе требуется переставить цифры так, 
	чтобы рядом не находились две одинаковые цифры и число 
	получилось максимально возможным. 
	Полученное число должно быть такой же длины 
	и не содержать лидирующих нулей.
	Если такого числа не существует, то выведите -1

	Andersen2020 training task "A. Permutation of numbers"
	In a given number, you need to rearrange the digits so that
	so that two identical numbers and a number are not next to each other
	turned out to be the best possible.
	The resulting number must be the same length
	and not contain leading zeros.
	If there is no such number, print -1
*/

#include <iostream>
#include <map>
#include <cmath>
using namespace std;

void magic();

int main()
{
	magic();
	return 0;
}

void magic()
{
	map<char, uint64_t> dataMap;
	uint64_t len = 0;

	for (auto i = 0; i < 10; i++)
		dataMap.emplace('0' + i, 0);
	
	bool input = true;
	for (;input;) 
	{
		char key = getchar();
		switch (key)
		{
		case 10: input = false; break;
		default: dataMap.find(key)->second++; len++; break;
		}
	}

	for (auto it = dataMap.begin(); it != dataMap.end();)
	{
		if (!it->second)
			dataMap.erase(it++);
		else
			++it;
	}

	// 1-st condition: char value <=> char priority
	// 2-st condition: ch.count <= other.count
	// 3-st condition: ch.prev != ch.now
	// 4-st condition: zero can't be first

	try 
	{
		char prevChar = '-';
		for (auto element = dataMap.rbegin(); element != dataMap.rend(); element++) // 1-st condition
		{
			if (element->second > ceil(len / 2.f)) // 2-st condition
				throw - 1;
			if (element->second > ceil((len - 1) / 2.f) && element->first == '0') // 4-st condition
				throw - 1;
		}
		do {
			bool emplaced = false;
			for (auto element = dataMap.rbegin(); element != dataMap.rend() & !emplaced; element++) // 1-st condition
			{
				if (element->second > ceil((len - 1) / 2.f)) // 2-st condition
				{
					element->second--;
					putchar(element->first);
					prevChar = element->first;
					emplaced = true;
				}
			}
			if (!emplaced)
			{
				if (prevChar != dataMap.rbegin()->first) // 3-st condition
				{
					dataMap.rbegin()->second--;
					prevChar = dataMap.rbegin()->first;
					putchar(dataMap.rbegin()->first);
				}
				else
				{
					(++dataMap.rbegin())->second--;
					putchar((++dataMap.rbegin())->first);
					prevChar = (++dataMap.rbegin())->first;

				}
			}

			for (auto it = dataMap.begin(); it != dataMap.end();)
			{
				if (!it->second)
					dataMap.erase(it++);
				else
					++it;
			}

		} while (--len);
	}
	catch (int)
	{
		printf("-1");
	}
}