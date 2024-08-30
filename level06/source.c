#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

bool auth(char *s, int len)
{
	int i;
	int val;
	int local_len;

	s[strcspn(s, "\n")] = 0;
	local_len = strnlen(s, 32);
	if (local_len <= 5)
		return 1;
	if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1)
	{
		puts("\x1B[32m.---------------------------.");
		puts("\x1B[31m| !! TAMPERING DETECTED !!  |");
		puts("\x1B[32m'---------------------------'");
		return 1;
	}
	else
	{
		val = (s[3] ^ 0b0001001100110111) + 0b10111101110110111101101;
		for (i = 0; i < local_len; ++i)
		{
			if (s[i] <= 31)
				return 1;
			val += (val ^ (unsigned int)s[i]) % 1337;
		}
		return len != val;
	}
}

int main()
{
	int len;
	char s[28];

	puts("***********************************");
	puts("*\t\tlevel06\t\t  *");
	puts("***********************************");
	printf("-> Enter Login: ");
	fgets(s, 32, stdin);
	puts("***********************************");
	puts("***** NEW ACCOUNT DETECTED ********");
	puts("***********************************");
	printf("-> Enter Serial: ");
	scanf(s, &len);
	if (auth(s, len))
		return 1;
	puts("Authenticated!");
	system("/bin/sh");
	return 0;
}
