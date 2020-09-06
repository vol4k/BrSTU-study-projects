#include "RIPEMD-160.hpp"
#include <iostream>
using namespace std;

int main() 
{
	string patch;
	cout << "Input patch: ";
	getline(cin, patch);
	RIPEMD_160::RIPEMD RIP;
	cout << RIP.RIPEMD160(patch) << endl;

	return 0;
}