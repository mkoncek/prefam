#pragma once

#include <stddef.h>

#include <fcntl.h>
#include <unistd.h>

__attribute__((visibility("hidden"))) int prefam_orig_open(const char* file, int oflag, mode_t mode);
__attribute__((visibility("hidden"))) ssize_t prefam_orig_readlink(const char* restrict pathname, char* restrict buf, size_t bufsiz);

//! Record file access of @p path file path.
//! @param path The file path that was accessed.
__attribute__((visibility("hidden"))) void prefam_record_path(const char* path);

//! Record file access of @p fd file descriptor.
//! This function is responsible for translating the file descriptor to the file path string.
//! @param fd The file descriptor that was accessed.
__attribute__((visibility("hidden"))) void prefam_record_fd(int fd);

//! Record file access of @p path file path relative to the @p fd file descriptor.
//! This function is responsible for translating the file descriptor and the relative file path
//! to the absolute file path.
//! @param fd The file descriptor that was accessed.
//! @param path The path relative to @p fd.
__attribute__((visibility("hidden"))) void prefam_record_openat_path(int fd, const char* path);

//! Record file access of @p path which is the result of PATH search.
//! This function is responsible discovering the absolute path result of the PATH search
//! for @p path.
//! @param path The path that was searched for on all entries present in the PATH.
__attribute__((visibility("hidden"))) void prefam_record_path_search(const char* path);
