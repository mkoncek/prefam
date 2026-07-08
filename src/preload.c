#define _GNU_SOURCE

#include "record.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <spawn.h>
#include <unistd.h>

static _Thread_local char* static_argv[128];

#define DECLARE_FUNCTION_POINTER(name) static __typeof__(name)* name##_orig = NULL
#define RESOLVE_FUNCTION_POINTER(name) { if (name##_orig == NULL) {\
	name##_orig = (__typeof__(name##_orig))dlsym(RTLD_NEXT, #name); } }

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

DECLARE_FUNCTION_POINTER(readlink);
DECLARE_FUNCTION_POINTER(readlinkat);

DECLARE_FUNCTION_POINTER(execve);
DECLARE_FUNCTION_POINTER(fexecve);
DECLARE_FUNCTION_POINTER(execv);
// DECLARE_FUNCTION_POINTER(execle);
// DECLARE_FUNCTION_POINTER(execl);
DECLARE_FUNCTION_POINTER(execvp);
// DECLARE_FUNCTION_POINTER(execlp);
DECLARE_FUNCTION_POINTER(execvpe);

DECLARE_FUNCTION_POINTER(posix_spawn);
DECLARE_FUNCTION_POINTER(posix_spawnp);

int open(const char* file, int oflag, ...)
{
	RESOLVE_FUNCTION_POINTER(open);
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
	RESOLVE_FUNCTION_POINTER(open64);
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
	RESOLVE_FUNCTION_POINTER(openat);
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
	RESOLVE_FUNCTION_POINTER(openat64);
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
	RESOLVE_FUNCTION_POINTER(fopen);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return fopen_orig(path, mode);
}

FILE* fopen64(const char* path, const char* mode)
{
	RESOLVE_FUNCTION_POINTER(fopen64);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return fopen64_orig(path, mode);
}

FILE* freopen(const char* path, const char* mode, FILE* stream)
{
	RESOLVE_FUNCTION_POINTER(freopen);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return freopen_orig(path, mode, stream);
}

FILE* freopen64(const char* path, const char* mode, FILE* stream)
{
	RESOLVE_FUNCTION_POINTER(freopen64);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return freopen64_orig(path, mode, stream);
}

DIR* opendir(const char* path)
{
	RESOLVE_FUNCTION_POINTER(opendir);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return opendir_orig(path);
}

DIR* fdopendir(int fd)
{
	RESOLVE_FUNCTION_POINTER(fdopendir);
	int errno_orig = errno;
	record_fd(fd);
	errno = errno_orig;
	return fdopendir_orig(fd);
}

ssize_t readlink(const char* restrict path, char* restrict buf, size_t bufsize)
{
	RESOLVE_FUNCTION_POINTER(readlink);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return readlink_orig(path, buf, bufsize);
}

ssize_t readlinkat(int dirfd, const char* restrict path, char* restrict buf, size_t bufsize)
{
	RESOLVE_FUNCTION_POINTER(readlinkat);
	int errno_orig = errno;
	record_openat_path(dirfd, path);
	errno = errno_orig;
	return readlinkat_orig(dirfd, path, buf, bufsize);
}

int execve(const char* path, char* const argv[], char* const envp[])
{
	RESOLVE_FUNCTION_POINTER(execve);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return execve_orig(path, argv, envp);
}

int fexecve(int fd, char* const argv[], char* const envp[])
{
	RESOLVE_FUNCTION_POINTER(fexecve);
	int errno_orig = errno;
	record_fd(fd);
	errno = errno_orig;
	return fexecve_orig(fd, argv, envp);
}

int execv(const char* path, char* const argv[])
{
	RESOLVE_FUNCTION_POINTER(execv);
	int errno_orig = errno;
	record_path(path);
	errno = errno_orig;
	return execv_orig(path, argv);
}

int execle(const char* path, const char* arg, ...)
{
	RESOLVE_FUNCTION_POINTER(execve);
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
	RESOLVE_FUNCTION_POINTER(execv);
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
	RESOLVE_FUNCTION_POINTER(execvp);
	int errno_orig = errno;
	record_path_search(file);
	errno = errno_orig;
	return execvp_orig(file, argv);
}

int execlp(const char* file, const char* arg, ...)
{
	RESOLVE_FUNCTION_POINTER(execvp);
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
	RESOLVE_FUNCTION_POINTER(execvpe);
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
	RESOLVE_FUNCTION_POINTER(posix_spawn);
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
	RESOLVE_FUNCTION_POINTER(posix_spawnp);
	int errno_orig = errno;
	record_path_search(file);
	errno = errno_orig;
	return posix_spawnp_orig(pid, file, file_actions, attrp, argv, envp);
}
