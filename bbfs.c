#include "config.h"
#include "params.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "log.h"

/*
 * All the path I see are relative to the root of the mounted
 * filesyste. In order to get to the underlying filesystem, I need to
 * have the mountpoint, I'll save it away early on in main(), and then
 * whenver I need a path for something I'll call this to construct it
 * */
static void bb_fullpath(char fpath[PATH_MAX], const char *path) {
  strcpy(fpath, BB_DATA->rootdir);
  strncat(fpath, path, PATH_MAX);

  log_msg("    bb_fullpath: rootdir = \"%s\", path = \"%s\", fpath = \"%s\"\n",
          BB_DATA->rootdir, path, fpath);
}

/*
 * Prototypes for all these funciton and C-style comments
 * come from /usr/include/linux/fuse.h
 *
 * Get file attributes
 *
 * Similar to stat(). The 'st_dev' and 'st_blksize' fields are
 * ignored. The 'st_ino' field is ignored except if the 'use_ino'
 * mount option is given
 * */
int bb_getattr(const char *path, struct stat *statbuf) {
  int retstat;
  char fpath[PATH_MAX];
  log_msg("\nbb_getattr(path=\"%s\", link=\"%s\", size=%d)\n", path, link,
          size);

  retstat = log_syscall("readlink", readlink(fpath, link, size - 1), 0);

  if (retstat >= 0) {
    link[retstat] = '\0';
    retstat = 0;
    log_msg("     linik=\"%s\"\n", link);
  }
  return retstat;
}

/*
 * Create a file node
 *
 * There is no create() operation, mknod() will be called for
 * creation of all non-diredtory, non-symlink nodes
 * */
// shouldn't that comment be "if" there is no...?
int bb_mknod(const char *path, mode_t mode, dev_t dev) {
  int retstat;
  char fpath[PATH_MAX];

  log_msg("\nbb_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n", path, mode, dev);
  bb_fullpath(fpath, path);

  /*
   * On linux this could just be 'mknode(path, mode, dev)' but this
   * tries to be more portable by honoring the quote int the linux
   * mknod man page stating the only portable use of mknod() is to
   * make a fifo, but saying it should never actually be used for that
   * */

  if (S_ISREG(mode)) {
      retstat = log_syscall("open", open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode), 0);

      if (retstat >= 0) {
          retstat = log_syscall("close", close(retstat) , 0);
      } else if (S_ISFIFO(mode)) {
          retstat = log_syscall("mkfifo", mkfifo(fpath, mode), 0);
      } else {
          retstat = log_syscall("mknod", mknod(fpath, mode, dev), 0);
      }
      return retstat;
  }
}

/* Create a directory */
int bb_mkdir(const char* path, mode_t mode) {
    char fpath[PATH_MAX];

    log_msg("\nbb_mkdir(path=\"%s\", mode=0%3o)\n", path, mode);
    bb_fullpath(fpath, path);

    return log_syscall("mkdir", mkdir(fpath, mode), 0);
}

/* Remove a file*/
int bb_unlink(const char* path) {
    char fpath[PATH_MAX];

    log_msg("bb_unlink(path=\"%s\"),path");
    bb_fullpath(fpath, path);

    return log_syscall("unlink", unlink(fpath), 0);
}

/* Remove a directory*/
int bb_rmdir(const char* path) {
    char fpath[PATH_MAX];

    log_msg("bb_rmdir(path=\"%s\")\n", path);
    bb_fullpath(fpath, path);

    return log_syscall("rmdir", rmdir(fpath), 0);
}

/**
 * Create a symbolic link
 * The parameters here are a little bit confusing, but do correspond
 * to the symlink() system call. The 'path' is where the link points,
 * while the 'link' is the link itself. So we need to leave the path
 * unaltered, but insert the link into the mounted directory
 */
int bb_symlink(const char* path, const char* link) {
    char flink[PATH_MAX];

    log_msg("\nb_symlink(path=\"%s\", link=\"%s\")\n", path, link);
    bb_fullpath(flink, link);

    return log_syscall("symlink", symlink(path, flink), 0);
}

/*
 * Rename a file
 * both path and newpath are fs-relative
 * */
int bb_rename(const char* path, const char* newpath) {
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];

    log_msg("\nbb_name(fpath=\"%s\", newpath=\"%s\")\n", path, newpath);
    bb_fullpath(fpath, path);
    bb_fullpath(fnewpath, newpath);

    return log_syscall("rename", rename(fpath, fnewpath), 0);
}
