#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/user.h>

int main(int argc, const char **argv, const char **envp)
{
	int stat_loc;
	char input[128];
	int ptrace_result;
	pid_t pid;

	pid = fork();
	memset(input, 0, sizeof(input));
	ptrace_result = 0;
	stat_loc = 0;
	if (pid) // Check if parent process
	{
		do {
			wait(&stat_loc);
			if ((stat_loc & 0b01111111) == 0 
			|| ((char)((stat_loc & 0b01111111) + 1) >> 1 > 0)) 
			{
				puts("child is exiting...");
				return 0;
			}
			ptrace_result = ptrace(PTRACE_PEEKUSER, pid, 44, 0); // 44 offsets to the eip register
		} while (ptrace_result != 11); // Checks if the syscall captured is different from execve
		puts("no exec() for you");
		kill(pid, SIGKILL);
	}
	else
	{
		prctl(PR_SET_PDEATHSIG, SIGHUP); // If the process is killed, send SIGHUP
		ptrace(PTRACE_TRACEME, 0, 0, 0); // Tell the parent process to trace the child
		puts("Give me some shellcode, k");
		gets(input);
	}
	return 0;
}
