#include <iostream>

int main(){
	uint16_t value, start;					// any 16-bit number, but not zero
	unsigned period = 0;
	
	do {
		printf("Input value [1..65535]: ");
		std::cin >> start;

	} while (!start);

	value = start;

	do {
		unsigned bit = value & 1;	// get input bit
		value >>= 1;				// shift the register 1 bit to the right
		if (bit == 1)				// perform the XOR operation only if the input bit = 1
			value ^= 0x80E3u;       // in bit representation, the number 0x80E3u corresponds to the polydrome
									// x^16 + x^15 + x^11 + x^10 + x^9 + x + 1, where exponents correspond to bits = 1
									// 1000 0000 1110 0011

		printf("%10d: %d\n", ++period, value);

	} while (value != start);

	return 0;
}
