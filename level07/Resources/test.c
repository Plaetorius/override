#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define HIBYTE(w)   ((unsigned char)(((unsigned int)(w) >> 8) & 0xFF))

void clear_stdin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int get_unum()
{
	int input[3];

	input[0] = 0;
	fflush(stdout);
	scanf("%u", input);
	clear_stdin();
	return input[0];
}

int store_number(int *nbs)
{
	unsigned int unum;
	unsigned int index;

	printf(" Number: ");
	unum = get_unum();
	printf(" Index: ");
	index = get_unum();
	printf("Unum: %u\nIndex: %u\n", unum, index);
	if (index == 3 * (index / 3) /* index % 3 == 0 */ || HIBYTE(unum) == 183 /* 0xb7, 3070230528 */)
	{
		printf("Mod: %d / other: %d\n", index == 3 * (index / 3), HIBYTE(unum) == 183);
		puts(" *** ERROR! ***");
		puts("   This index is reserved for wil!");
		puts(" *** ERROR! ***");
		return 1;
	}
	else
	{
		nbs[index] = unum;
		return 0;
	}
}

int read_number(int *nbs)
{
	int index; // Int here when unsigned int in store

	printf(" Index: ");
	index = get_unum();
	printf(" Number at data[%u] is %u\n", index, nbs[index]);
	return 0;
}

int main(int argc, const char **argv, const char **envp)
{
	char nbs[400];
	int number;
	char s[4]; 

	number = 0;
	*s = 0;
	memset(nbs, 0, sizeof(nbs)); // Zero the nbs
	while (*argv) { // Zero the args
		memset((void *)*argv, 0, strlen(*argv)); ++argv;
	}
	while (*envp) {// Zero the environment
		memset((void *)*envp, 0, strlen(*envp)); ++envp;
	}
	puts(
		"----------------------------------------------------\n"
		"  Welcome to wil's crappy number storage service!   \n"
		"----------------------------------------------------\n"
		" Commands:                                          \n"
		"    store - store a number into the data storage    \n"
		"    read  - read a number from the data storage     \n"
		"    quit  - exit the program                        \n"
		"----------------------------------------------------\n"
		"   wil has reserved some storage :>                 \n"
		"----------------------------------------------------\n");
	while (42)
	{
		printf("Input command: ");
		number = 1;
		fgets(s, 20, stdin); // VULN s is only 4 bytes long
		s[strlen(s) - 1] = 0;
		if (!memcmp(s, "store", 5) )
		{
			number = store_number((int*)nbs);
			goto LABEL_13;
		}
		if (!memcmp(s, "read", 4) )
		{
			number = read_number((int*)nbs);
			goto LABEL_13;
		}
		if (!memcmp(s, "quit", 4))
			return 0;
	
		LABEL_13:
		if (number)
			printf(" Failed to do %s command\n", s);
		else
			printf(" Completed %s command successfully\n", s);
		*s = 0;
	}
}
