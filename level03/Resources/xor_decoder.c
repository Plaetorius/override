#include <stdio.h>
#include <string.h>

void print_bits(unsigned char c) {
	c = c ^ 'B';
    for (int i = 7; i >= 0; i--) {
        unsigned char mask = 1 << i;
        if (c & mask)
            printf("1");
        else
            printf("0");
    }
    printf("\n");
}

void xor_strings() {
	char congrats[] = "Congratulations!";
	char xored[] = "Q}|u`sfg~sf{}|a3";
	size_t len = strlen(xored);
	char result[len + 1];

	memset(result, 0, len + 1);
	for (int i = 0; i < len; i++) {
		result[i] = xored[i] ^ congrats[i];
	}
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)result[i]);
    }
    printf("\n");
}

int main() {
    // char c = 'C'; 
    // print_bits((unsigned char)c);
	xor_strings();
    return 0;
}