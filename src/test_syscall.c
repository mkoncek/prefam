#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "%s: expected 1 argument (test directory)\n", argv[0]);
		return 1;
	}
	const char* testdir = argv[1];
	char path[4096];
	char buf[4096];
	
#ifdef SYS_open
	snprintf(path, sizeof(path), "%s/file_syscall_open", testdir);
	int fd = (int)syscall(SYS_open, path, O_RDONLY);
	printf("ok: SYS_open\n");
	if (fd >= 0) close(fd);
#endif
	
	snprintf(path, sizeof(path), "%s/file_syscall_openat", testdir);
	int dirfd = (int)syscall(SYS_openat, AT_FDCWD, path, O_RDONLY);
	printf("ok: SYS_openat\n");
	if (dirfd >= 0) close(dirfd);
	
	snprintf(path, sizeof(path), "%s/file_syscall_readlinkat", testdir);
	syscall(SYS_readlinkat, AT_FDCWD, path, buf, sizeof(buf));
	printf("ok: SYS_readlinkat\n");
	
#ifdef SYS_readlink
	snprintf(path, sizeof(path), "%s/file_syscall_readlink", testdir);
	syscall(SYS_readlink, path, buf, sizeof(buf));
	printf("ok: SYS_readlink\n");
#endif
	
	syscall(SYS_execve, "/__prefam_test_syscall_execve__", (char*[]){NULL}, (char*[]){NULL});
	printf("ok: SYS_execve\n");
	
	return 0;
}
