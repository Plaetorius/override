#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

typedef struct s_message {
	char 	message[140];
	char	username[40];
	int		msg_len;
}			t_message;

int secret_backdoor()
{
	char s[128];

	fgets(s, 128, stdin);
	return system(s);
}

int handle_msg()
{
	t_message mess = {
		.msg_len = 140,
	};

	set_username(&mess);
	set_msg(&mess);
	return puts(">: Msg sent!");
}

char *set_msg(t_message *mess)
{
	char s[1024];

	memset(s, 0, sizeof(s));
	puts(">: Msg @Unix-Dude");
	printf(">>: ");
	fgets(s, 1024, stdin);
	return strncpy(mess->message, s, mess->msg_len);
}

int set_username(t_message *mess)
{
	char s[140];
	int i; 

	memset(s, 0, 128);
	puts(">: Enter your username");
	printf(">>: ");
	fgets(s, 128, stdin);
	for (i = 0; i <= 40 && s[i]; ++i)
		mess->username[i] = s[i];
	return printf(">: Welcome, %s", mess->username);
}

int main()
{
	puts(
	"--------------------------------------------\n"
	"|   ~Welcome to l33t-m$n ~    v1337        |\n"
	"--------------------------------------------");
	handle_msg();
	return 0;
}