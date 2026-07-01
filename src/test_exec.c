#define _GNU_SOURCE

#include <stdio.h>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

int main(void)
{
	char* const argv[] = {"__test__", NULL};
	char* const envp[] = {NULL};
	
	execve("/__prefam_test_execve__", argv, envp);
	printf("ok: execve\n");
	
	fexecve(999, argv, envp);
	printf("ok: fexecve\n");
	
	execv("/__prefam_test_execv__", argv);
	printf("ok: execv\n");
	
	execle("/__prefam_test_execle__", "__test__", "a1", "a2", (char*)NULL, envp);
	printf("ok: execle\n");
	
	execl("/__prefam_test_execl__", "__test__", "a1", "a2", (char*)NULL);
	printf("ok: execl\n");
	
	execvp("__prefam_test_execvp__", argv);
	printf("ok: execvp\n");
	
	execlp("__prefam_test_execlp__", "__test__", "a1", "a2", (char*)NULL);
	printf("ok: execlp\n");
	
	execvpe("__prefam_test_execvpe__", argv, envp);
	printf("ok: execvpe\n");
	
	pid_t pid;
	int ret = posix_spawn(&pid, "/__prefam_test_posix_spawn__", NULL, NULL, argv, envp);
	if (ret == 0)
	{
		waitpid(pid, NULL, 0);
	}
	printf("ok: posix_spawn\n");
	
	ret = posix_spawnp(&pid, "__prefam_test_posix_spawnp__", NULL, NULL, argv, envp);
	if (ret == 0)
	{
		waitpid(pid, NULL, 0);
	}
	printf("ok: posix_spawnp\n");
	
	return 0;
}
