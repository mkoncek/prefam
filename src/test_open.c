#define _GNU_SOURCE

#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "%s: expected 1 argument (test directory)\n", argv[0]);
		return 1;
	}
	const char* testdir = argv[1];
	int fd;
	
	int dirfd = open(testdir, O_RDONLY | O_DIRECTORY);
	printf("ok: open\n");
	
	fd = openat(dirfd, "file_openat", O_RDONLY);
	printf("ok: openat dirfd\n");
	close(fd);
	
	fd = openat(dirfd, "/dev/null", O_RDONLY);
	printf("ok: openat dirfd+absolute\n");
	close(fd);
	
	int bigdirfd = dup2(dirfd, 42);
	fd = openat(bigdirfd, "file_openat_bigfd", O_RDONLY);
	printf("ok: openat multi-digit dirfd\n");
	close(fd);
	close(bigdirfd);
	close(dirfd);
	
	fd = openat(999, "file_openat_badfd", O_RDONLY);
	printf("ok: openat invalid dirfd\n");
	
	fd = openat(AT_FDCWD, "/dev/zero", O_RDONLY);
	printf("ok: openat AT_FDCWD\n");
	close(fd);
	
	fd = open("/dev/null", O_WRONLY | O_CREAT, 0644);
	printf("ok: open O_CREAT\n");
	close(fd);
	
	fd = open64("/dev/random", O_RDONLY);
	printf("ok: open64\n");
	close(fd);
	
	dirfd = open(testdir, O_RDONLY | O_DIRECTORY);
	fd = openat64(dirfd, "file_openat64", O_RDONLY);
	printf("ok: openat64 dirfd\n");
	close(fd);
	close(dirfd);
	
	fd = openat64(AT_FDCWD, "/dev/urandom", O_RDONLY);
	printf("ok: openat64 AT_FDCWD\n");
	close(fd);
	
	return 0;
}
