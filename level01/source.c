#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Global variable
char g_user_name[100]; 

bool verify_user_name()
{
	puts("verifying username....\n");
	// memcmp returns 0 if it matches
	// if (memcmp != 0) == true, it means it doesn't match 
	// if (memcmp != 0) == false, it means it does match 
	return memcmp(g_user_name, "dat_wil", 7u) != 0; 
}

bool verify_user_pass(const void *pass)
{
	// memcmp returns 0 if it matches
	// if (memcmp != 0) == true, it means it doesn't match 
	// if (memcmp != 0) == false, it means it does match 
	return memcmp(pass, "admin", 5u) != 0;
}

int main()
{
	char pass[64];
	bool is_uncorrect;

	memset(pass, 0, sizeof(pass));
	is_uncorrect = false;
	puts("********* ADMIN LOGIN PROMPT *********");
	printf("Enter Username: ");
	fgets(g_user_name, 256, stdin);		// DANGEROUS, writing 256 in a 100 space
	is_uncorrect = verify_user_name();
	if (is_uncorrect)
	{
		puts("nope, incorrect username...\n");
	}
	else
	{
		puts("Enter Password: ");
		fgets(pass, 100, stdin);			// DANGEROUS, writing 100 in a 64 space
		is_uncorrect = verify_user_pass(pass); // No matter the password, will print the incorrect password message
		puts("nope, incorrect password...\n");
	}
	return 1;
}