#define _GNU_SOURCE

#include "record.h"

#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <spawn.h>
#include <sys/syscall.h>

static _Thread_local char* static_argv[128];

#define RESOLVE_FUNCTION_POINTER(name)\
	static __typeof__(name)* _Atomic name##_orig_static = NULL;\
	__typeof__(name)* name##_orig = atomic_load_explicit(&name##_orig_static, memory_order_relaxed);\
	if (name##_orig == NULL) {\
		name##_orig = (__typeof__(name##_orig))dlsym(RTLD_NEXT, #name);\
		atomic_store_explicit(&name##_orig_static, name##_orig, memory_order_relaxed);\
	}\
;

int prefam_orig_open(const char* file, int oflag, mode_t mode)
{
	RESOLVE_FUNCTION_POINTER(open);
	return open_orig(file, oflag, mode);
}

ssize_t prefam_orig_readlink(const char* restrict path, char* restrict buf, size_t bufsize)
{
	RESOLVE_FUNCTION_POINTER(readlink);
	return readlink_orig(path, buf, bufsize);
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
	prefam_record_path(file);
	errno = errno_orig;
	return prefam_orig_open(file, oflag, mode);
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
	prefam_record_path(file);
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
	prefam_record_openat_path(fd, file);
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
	prefam_record_openat_path(fd, file);
	errno = errno_orig;
	return openat64_orig(fd, file, oflag, mode);
}

FILE* fopen(const char* path, const char* mode)
{
	RESOLVE_FUNCTION_POINTER(fopen);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return fopen_orig(path, mode);
}

FILE* fopen64(const char* path, const char* mode)
{
	RESOLVE_FUNCTION_POINTER(fopen64);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return fopen64_orig(path, mode);
}

FILE* freopen(const char* path, const char* mode, FILE* stream)
{
	RESOLVE_FUNCTION_POINTER(freopen);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return freopen_orig(path, mode, stream);
}

FILE* freopen64(const char* path, const char* mode, FILE* stream)
{
	RESOLVE_FUNCTION_POINTER(freopen64);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return freopen64_orig(path, mode, stream);
}

DIR* opendir(const char* path)
{
	RESOLVE_FUNCTION_POINTER(opendir);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return opendir_orig(path);
}

DIR* fdopendir(int fd)
{
	RESOLVE_FUNCTION_POINTER(fdopendir);
	int errno_orig = errno;
	prefam_record_fd(fd);
	errno = errno_orig;
	return fdopendir_orig(fd);
}

ssize_t readlink(const char* restrict path, char* restrict buf, size_t bufsize)
{
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return prefam_orig_readlink(path, buf, bufsize);
}

ssize_t readlinkat(int dirfd, const char* restrict path, char* restrict buf, size_t bufsize)
{
	RESOLVE_FUNCTION_POINTER(readlinkat);
	int errno_orig = errno;
	prefam_record_openat_path(dirfd, path);
	errno = errno_orig;
	return readlinkat_orig(dirfd, path, buf, bufsize);
}

int execve(const char* path, char* const argv[], char* const envp[])
{
	RESOLVE_FUNCTION_POINTER(execve);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return execve_orig(path, argv, envp);
}

int fexecve(int fd, char* const argv[], char* const envp[])
{
	RESOLVE_FUNCTION_POINTER(fexecve);
	int errno_orig = errno;
	prefam_record_fd(fd);
	errno = errno_orig;
	return fexecve_orig(fd, argv, envp);
}

int execv(const char* path, char* const argv[])
{
	RESOLVE_FUNCTION_POINTER(execv);
	int errno_orig = errno;
	prefam_record_path(path);
	errno = errno_orig;
	return execv_orig(path, argv);
}

int execle(const char* path, const char* arg, ...)
{
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
			return execve(path, static_argv, envp);
		}
	}
	va_end(args);
	errno = E2BIG;
	return -1;
}

int execl(const char* path, const char* arg, ...)
{
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
			return execv(path, static_argv);
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
	prefam_record_path_search(file);
	errno = errno_orig;
	return execvp_orig(file, argv);
}

int execlp(const char* file, const char* arg, ...)
{
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
			return execvp(file, static_argv);
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
	prefam_record_path_search(file);
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
	prefam_record_path(path);
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
	prefam_record_path_search(file);
	errno = errno_orig;
	return posix_spawnp_orig(pid, file, file_actions, attrp, argv, envp);
}

long syscall(long number, ...)
{
	RESOLVE_FUNCTION_POINTER(syscall);
	va_list args;
	va_start(args, number);
	long a1 = va_arg(args, long);
	long a2 = va_arg(args, long);
	long a3 = va_arg(args, long);
	long a4 = va_arg(args, long);
	long a5 = va_arg(args, long);
	long a6 = va_arg(args, long);
	va_end(args);
	
	int errno_orig = errno;
	switch (number)
	{
#ifdef SYS_open
	case SYS_open:
#endif
#ifdef SYS_creat
	case SYS_creat:
#endif
	case SYS_execve:
		prefam_record_path((const char*)a1);
		break;
#ifdef SYS_readlink
	case SYS_readlink:
		prefam_record_path((const char*)a1);
		break;
#endif
	case SYS_openat:
#ifdef SYS_openat2
	case SYS_openat2:
#endif
	case SYS_readlinkat:
#ifdef SYS_execveat
	case SYS_execveat:
#endif
		prefam_record_openat_path((int)a1, (const char*)a2);
		break;
	}
	errno = errno_orig;
	
	return syscall_orig(number, a1, a2, a3, a4, a5, a6);
}
