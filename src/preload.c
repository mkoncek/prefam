#define _GNU_SOURCE

#include "record.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

#include <stdio.h>
#include <dirent.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <spawn.h>

static _Thread_local char* static_argv[128];

#define DECLARE_FUNCTION_POINTER(name) static __typeof__(name)* name##_orig = NULL
#define ASSIGN_FUNCTION_POINTER(name) name##_orig = (__typeof__(name##_orig))dlsym(RTLD_NEXT, #name)

DECLARE_FUNCTION_POINTER(open);
DECLARE_FUNCTION_POINTER(open64);
DECLARE_FUNCTION_POINTER(openat);
DECLARE_FUNCTION_POINTER(openat64);

DECLARE_FUNCTION_POINTER(fopen);
DECLARE_FUNCTION_POINTER(fopen64);
DECLARE_FUNCTION_POINTER(freopen);
DECLARE_FUNCTION_POINTER(freopen64);
DECLARE_FUNCTION_POINTER(opendir);
DECLARE_FUNCTION_POINTER(fdopendir);

DECLARE_FUNCTION_POINTER(execve);
DECLARE_FUNCTION_POINTER(fexecve);
DECLARE_FUNCTION_POINTER(execv);
DECLARE_FUNCTION_POINTER(execle);
DECLARE_FUNCTION_POINTER(execl);
DECLARE_FUNCTION_POINTER(execvp);
DECLARE_FUNCTION_POINTER(execlp);
DECLARE_FUNCTION_POINTER(execvpe);

DECLARE_FUNCTION_POINTER(posix_spawn);
DECLARE_FUNCTION_POINTER(posix_spawnp);

__attribute__((constructor))
static void constructor(void)
{
	ASSIGN_FUNCTION_POINTER(open);
	ASSIGN_FUNCTION_POINTER(open64);
	ASSIGN_FUNCTION_POINTER(openat);
	ASSIGN_FUNCTION_POINTER(openat64);
	
	ASSIGN_FUNCTION_POINTER(fopen);
	ASSIGN_FUNCTION_POINTER(fopen64);
	ASSIGN_FUNCTION_POINTER(freopen);
	ASSIGN_FUNCTION_POINTER(freopen64);
	ASSIGN_FUNCTION_POINTER(opendir);
	ASSIGN_FUNCTION_POINTER(fdopendir);
	
	ASSIGN_FUNCTION_POINTER(execve);
	ASSIGN_FUNCTION_POINTER(fexecve);
	ASSIGN_FUNCTION_POINTER(execv);
	ASSIGN_FUNCTION_POINTER(execle);
	ASSIGN_FUNCTION_POINTER(execl);
	ASSIGN_FUNCTION_POINTER(execvp);
	ASSIGN_FUNCTION_POINTER(execlp);
	ASSIGN_FUNCTION_POINTER(execvpe);
	
	ASSIGN_FUNCTION_POINTER(posix_spawn);
	ASSIGN_FUNCTION_POINTER(posix_spawnp);
}

int open(const char* file, int oflag, ...)
{
	int errno_orig = errno;
	mode_t mode = 0;
	if (oflag & (O_CREAT | __O_TMPFILE))
	{
		va_list args;
		va_start(args, oflag);
		mode = va_arg(args, mode_t);
		va_end(args);
	}
	record_path(file);
	errno = errno_orig;
	return open_orig(file, oflag, mode);
}

int open64(const char* file, int oflag, ...)
{
	int errno_orig = errno;
	mode_t mode = 0;
	if (oflag & (O_CREAT | __O_TMPFILE))
	{
		va_list args;
		va_start(args, oflag);
		mode = va_arg(args, mode_t);
		va_end(args);
	}
	record_path(file);
	errno = errno_orig;
	return open64_orig(file, oflag, mode);
}

int openat(int fd, const char* file, int oflag, ...)
{
	int errno_orig = errno;
	mode_t mode = 0;
	if (oflag & (O_CREAT | __O_TMPFILE))
	{
		va_list args;
		va_start(args, oflag);
		mode = va_arg(args, mode_t);
		va_end(args);
	}
	record_openat_path(fd, file);
	errno = errno_orig;
	return openat_orig(fd, file, oflag, mode);
}

int openat64(int fd, const char* file, int oflag, ...)
{
	int errno_orig = errno;
	mode_t mode = 0;
	if (oflag & (O_CREAT | __O_TMPFILE))
	{
		va_list args;
		va_start(args, oflag);
		mode = va_arg(args, mode_t);
		va_end(args);
	}
	record_openat_path(fd, file);
	errno = errno_orig;
	return openat64_orig(fd, file, oflag, mode);
}

FILE* fopen(const char* path, const char* mode)
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return fopen_orig(path, mode);
}

FILE* fopen64(const char* path, const char* mode)
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return fopen64_orig(path, mode);
}

FILE* freopen(const char* path, const char* mode, FILE* stream)
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return freopen_orig(path, mode, stream);
}

FILE* freopen64(const char* path, const char* mode, FILE* stream)
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return freopen64_orig(path, mode, stream);
}

DIR* opendir(const char* name)
{
	int errno_orig = errno;
	record_path(name);
	errno = errno_orig;
	return opendir_orig(name);
}

DIR* fdopendir(int fd)
{
	int errno_orig = errno;
	record_fd(fd);
	errno = errno_orig;
	return fdopendir_orig(fd);
}

int execve(const char* path, char* const argv[], char* const envp[])
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return execve_orig(path, argv, envp);
}

int fexecve(int fd, char* const argv[], char* const envp[])
{
	int errno_orig = errno;
	record_fd(fd);
	errno = errno_orig;
	return fexecve_orig(fd, argv, envp);
}

int execv(const char* path, char* const argv[])
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return execv_orig(path, argv);
}

int execle(const char* path, const char* arg, ...)
{
	int errno_orig = errno;
	const size_t limit = sizeof(static_argv) / sizeof(static_argv[0]);
	va_list args;
	va_start(args, arg);
	static_argv[0] = (char*)arg;
	for (size_t i = 1; i < limit; ++i)
	{
		static_argv[i] = va_arg(args, char*);
		if (static_argv[i] == NULL)
		{
			char* const* envp = va_arg(args, char* const*);
			va_end(args);
			record_path(path);
			errno = errno_orig;
			return execve_orig(path, static_argv, envp);
		}
	}
	va_end(args);
	errno = E2BIG;
	return -1;
}

int execl(const char* path, const char* arg, ...)
{
	int errno_orig = errno;
	const size_t limit = sizeof(static_argv) / sizeof(static_argv[0]);
	va_list args;
	va_start(args, arg);
	static_argv[0] = (char*)arg;
	for (size_t i = 1; i < limit; ++i)
	{
		static_argv[i] = va_arg(args, char*);
		if (static_argv[i] == NULL)
		{
			va_end(args);
			record_path(path);
			errno = errno_orig;
			return execv_orig(path, static_argv);
		}
	}
	va_end(args);
	errno = E2BIG;
	return -1;
}

int execvp(const char* file, char* const argv[])
{
	int errno_orig = errno;
	record_path_search(file);
	errno = errno_orig;
	return execvp_orig(file, argv);
}

int execlp(const char* file, const char* arg, ...)
{
	int errno_orig = errno;
	const size_t limit = sizeof(static_argv) / sizeof(static_argv[0]);
	va_list args;
	va_start(args, arg);
	static_argv[0] = (char*)arg;
	for (size_t i = 1; i < limit; ++i)
	{
		static_argv[i] = va_arg(args, char*);
		if (static_argv[i] == NULL)
		{
			va_end(args);
			record_path_search(file);
			errno = errno_orig;
			return execvp_orig(file, static_argv);
		}
	}
	va_end(args);
	errno = E2BIG;
	return -1;
}

int execvpe(const char* file, char* const argv[], char* const envp[])
{
	int errno_orig = errno;
	record_path_search(file);
	errno = errno_orig;
	return execvpe_orig(file, argv, envp);
}

int posix_spawn(pid_t* pid, const char* path,
	const posix_spawn_file_actions_t* file_actions,
	const posix_spawnattr_t* attrp,
	char* const argv[], char* const envp[])
{
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return posix_spawn_orig(pid, path, file_actions, attrp, argv, envp);
}

int posix_spawnp(pid_t* pid, const char* file,
	const posix_spawn_file_actions_t* file_actions,
	const posix_spawnattr_t* attrp,
	char* const argv[], char* const envp[])
{
	int errno_orig = errno;
	record_path_search(file);
	errno = errno_orig;
	return posix_spawnp_orig(pid, file, file_actions, attrp, argv, envp);
}
