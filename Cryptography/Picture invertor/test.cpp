#include "pinv.hpp"
#include <iostream>
using namespace std;

void main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	string patch;
	size_t bits;
	char color;

	cout << "Укажите путь к файлу или перетащите его сюда: ";
	getline(cin, patch);

	cout << "Сколько бит подвергнуть инвертированию: ";
	cin >> bits;

	bool repeat = true;
	do {
		cout << "Над каким цветом производить инвертирование(RGB): ";
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