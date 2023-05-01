#include "pinv.hpp"
#include <iostream>
using namespace std;

void main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	string patch;
	size_t bits;
	char color;

	cout << "Write file path or drag and drop here: ";
	getline(cin, patch);

	cout << "How mutch bits must be inverted: ";
	cin >> bits;

	bool repeat = true;
	do {
		cout << "What color must been inverted(RGB): ";
		cin >> color;
		switch (color) {
		case 'R':
			pinv::transform(patch, bits, 'R');
			repeat = !repeat;
			break;
		case 'G':
			pinv::transform(patch, bits, 'G');
			repeat = !repeat;
			break;
		case 'B':
			pinv::transform(patch, bits, 'B');
			repeat = !repeat;
			break;
		}
	} while (repeat);
}