#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/user.h>

_UNKNOWN unk_8048900; // weak USED
int (*_init_array_start)() = &enable_timeout_cons; // weak USED


int _CTOR_LIST__ = -1; // weak
int _DTOR_LIST__[] = { -1 }; // weak
int _DTOR_END__ = 0; // weak
int _JCR_LIST__ = 0; // weak
Elf32_Dyn *GLOBAL_OFFSET_TABLE_ = &DYNAMIC; // weak
int (*dword_8049FFC)(void) = NULL; // weak
FILE *stdout; // idb
char completed_6159; // weak
int dtor_idx_6161; // weak

int clear_stdin()
{
  int result; // eax

  do
    result = getchar();
  while (result != 10 && result != 0xFF );
  return result;
}

int get_unum()
{
  int v1[3]; // [esp+1Ch] [ebp-Ch] BYREF

  v1[0] = 0;
  fflush(stdout);
  __isoc99_scanf(&unk_8048900, v1); // global
  clear_stdin();
  return v1[0];
}
// 8048560: using guessed type int __cdecl __isoc99_scanf(_DWORD, _DWORD);
// 8048657: using guessed type int var_C[3];

//----- (0804868F) --------------------------------------------------------
void prog_timeout(int a1)
{
  int v1; // eax

  v1 = sys_exit(1);
}

//----- (080486A0) --------------------------------------------------------
unsigned int enable_timeout_cons() // global
{
  signal(SIGALRM, (sig_t)prog_timeout);
  return alarm(60); // 60 seconds
}

//----- (080486C8) --------------------------------------------------------
int main(int argc, const char **argv, const char **envp)
{
	int stat_loc;
	char s[128];
	int v7;
	int v8;
	pid_t pid;

	pid = fork();
	memset(s, 0, sizeof(s));
	v8 = 0;
	stat_loc = 0;
	if (pid) // Check if parent process
	{
		do {
			wait(&stat_loc);
			if ( (stat_loc & 0x7F) == 0 || (v7 = stat_loc, (char)((stat_loc & 0x7F) + 1) >> 1 > 0) )
			{
				puts("child is exiting...");
				return 0;
			}
			v8 = ptrace(PTRACE_PEEKUSER, pid, 44, 0);
		} while ( v8 != 11 );
		puts("no exec() for you");
		kill(pid, SIGKILL);
	}
	else
	{
		prctl(PR_SET_PDEATHSIG, SIGHUP);
		ptrace(PTRACE_TRACEME, 0, 0, 0);
		puts("Give me some shellcode, k");
		gets(s);
	}
	return 0;
}
