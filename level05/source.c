#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
  char buff[100];
  unsigned int i = 0; 

  fgets(buff, 100, stdin); // Is protected
  for (i = 0; i < strlen(buff); ++i) // Iterates through the buffer
    if (buff[i] >= 'A' /* 65 */ && buff[i] <= 'Z' /* 90 */) // Selects any uppercase letter
      buff[i] ^= 0b0100000; // Bitwise XOR with 32
  printf(buff); // Vulnerable to string exploit
  exit(0); // Has no return?
}