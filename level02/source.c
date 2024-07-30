#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
	char user_pass[96];
	char pass[48]; 
	char user_name[96];
	int len_pass;
	FILE *stream;

	memset(user_name, 0, sizeof(user_name));
	memset(user_pass, 0, sizeof(user_pass));
	memset(pass, 0, 41);
	stream = 0LL;
	len_pass = 0;
	stream = fopen("/home/users/level03/.pass", "r");
	if ( !stream )
	{
		fwrite("ERROR: failed to open password file\n", 1uLL, 0x24uLL, stderr);
		exit(1);
	}
	len_pass = fread(pass, 1uLL, 41, stream);
	pass[strcspn(pass, "\n")] = 0;
	if ( len_pass != 41 )
	{
		fwrite("ERROR: failed to read password file\n", 1uLL, 0x24uLL, stderr);
		fwrite("ERROR: failed to read password file\n", 1uLL, 0x24uLL, stderr);
		exit(1);
	}
	fclose(stream);
	puts("===== [ Secure Access System v1.0 ] =====");
	puts("/***************************************\\");
	puts("| You must login to access this system. |");
	puts("\\**************************************/");
	printf("--[ Username: ");
	fgets(user_name, 100, stdin);
	user_name[strcspn(user_name, "\n")] = 0;
	printf("--[ Password: ");
	fgets(user_pass, 100, stdin);
	user_pass[strcspn(user_pass, "\n")] = 0;
	puts("*****************************************");
	if ( strncmp(pass, user_pass, 41) )
	{
		printf(user_name); // DANGEROUS
		puts(" does not have access!");
		exit(1);
	}
	printf("Greetings, %s!\n", user_name);
	system("/bin/sh");
	return 0;
}