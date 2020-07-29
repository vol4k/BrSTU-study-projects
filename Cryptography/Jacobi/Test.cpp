#include <iostream>
#include "Jacobi.h"
using namespace std;

void main() {
	int a = 0, p = 0;
	Jacobi j;
	while (true) {
		cout << "Input [a] and [p]: ";
		cin >> a >> p;
		while (!(p % 2)) {
			cout << "# [p] must be odd: ";
			cin >> p;
		}
		cout << "Jacobi("<< a << ',' << p << ") is " << j.Calculate(a, p) << "\n\n";
		cin.get();
	}
}