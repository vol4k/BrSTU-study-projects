#include <iostream>

int main(){
	uint16_t value, start;					// любое 16-битное число кроме нуля
	unsigned period = 0;
	
	do {
		printf("Input value [1..65535]: ");
		std::cin >> start;

	} while (!start);

	value = start;

	do {
		unsigned bit = value & 1;	// получаем входной бит
		value >>= 1;				// сдвигаем регистр на 1 бит вправо
		if (bit == 1)				// выполняем операцию XOR только если входной бит = 1
			value ^= 0x80E3u;       // в битовом представлении число 0x80E3u соответствует полиндрому
									// x^16 + x^15 + x^11 + x^10 + x^9 + x + 1, где степени соответсвуют битам = 1
									// 1000 0000 1110 0011

		printf("%10d: %d\n", ++period, value);

	} while (value != start);

	return 0;
}
