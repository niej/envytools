/*
 * Copyright 2019 Intel Corporation
 * SPDX-License-Identifier: MIT
 *
 * File operations helpers
 */

#ifndef _OS_FILE_H_
#define _OS_FILE_H_

#include <stdbool.h>
#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * Create a new file and opens it for writing-only.
 * If the given filename already exists, nothing is done and NULL is returned.
 * `errno` gets set to the failure reason; if that is not EEXIST, the caller
 * might want to do something other than trying again.
 */
FILE *
os_file_create_unique(const char *filename, int filemode);

/*
 * Duplicate a file descriptor, making sure not to keep it open after an exec*()
 */
int
os_dupfd_cloexec(int fd);

/*
 * Read a file.
 * Returns a char* that the caller must free(), or NULL and sets errno.
 * If size is not null and no error occured it's set to the size of the
 * file.
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
static ssize_t
readN(int fd, char *buf, size_t len)
{
   int err = -ENODATA;
   size_t total = 0;
   do {
      ssize_t ret = read(fd, buf + total, len - total);

      if (ret < 0)
         ret = -errno;

      if (ret == -EINTR || ret == -EAGAIN)
         continue;

      if (ret <= 0) {
         err = ret;
         break;
      }

      total += ret;
   } while (total != len);

   return total ? (ssize_t)total : err;
}

static inline
char *
os_read_file(const char *filename, size_t *size)
{
   /* Note that this also serves as a slight margin to avoid a 2x grow when
    * the file is just a few bytes larger when we read it than when we
    * fstat'ed it.
    * The string's NULL terminator is also included in here.
    */
   size_t len = 64;

   int fd = open(filename, O_RDONLY);
   if (fd == -1) {
      /* errno set by open() */
      return NULL;
   }

   /* Pre-allocate a buffer at least the size of the file if we can read
    * that information.
    */
   struct stat stat;
   if (fstat(fd, &stat) == 0)
      len += stat.st_size;

   char *buf = malloc(len);
   if (!buf) {
      close(fd);
      errno = -ENOMEM;
      return NULL;
   }

   ssize_t actually_read;
   size_t offset = 0, remaining = len - 1;
   while ((actually_read = readN(fd, buf + offset, remaining)) == (ssize_t)remaining) {
      char *newbuf = realloc(buf, 2 * len);
      if (!newbuf) {
         free(buf);
         close(fd);
         errno = -ENOMEM;
         return NULL;
      }

      buf = newbuf;
      len *= 2;
      offset += actually_read;
      remaining = len - offset - 1;
   }

   close(fd);

   if (actually_read > 0)
      offset += actually_read;

   /* Final resize to actual size */
   len = offset + 1;
   char *newbuf = realloc(buf, len);
   if (!newbuf) {
      free(buf);
      errno = -ENOMEM;
      return NULL;
   }
   buf = newbuf;

   buf[offset] = '\0';

   if (size)
      *size = offset;

   return buf;
}

/*
 * Try to determine if two file descriptors reference the same file description
 *
 * Return values:
 * - 0:   They reference the same file description
 * - > 0: They do not reference the same file description
 * - < 0: Unable to determine whether they reference the same file description
 */
int
os_same_file_description(int fd1, int fd2);

#ifdef __cplusplus
}
#endif

#endif /* _OS_FILE_H_ */
