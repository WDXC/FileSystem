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

/* Create a hard link to a file */
int bb_link(const char* path, const char* newpath) {
    char fpath[PATH_MAX], fnewpath[PATH_MAX];

    log_msg("\nbb_link(path=\"%s\", newpath=\"%s\")\n");
    bb_fullpath(fpath, path);
    bb_fullpath(fnewpath, newpath);

    return log_syscall("link", link(fpath, fnewpath), 0);
}

/** Change the permission bits of a file */
int bb_chmod(const char* path, mode_t mode) {
    char fpath[PATH_MAX];

    log_msg("\nbb_chmod(fpath=\"%s\", mode=0%03o)\n", path, mode);
    bb_fullpath(fpath, path);

    return log_syscall("chmod", chmod(fpath, mode), 0);
}

/* Change the owner and group of a file */
int bb_chown(const char* path, uid_t uid, gid_t gid) {
    char fpath[PATH_MAX];
    log_msg("\nbb_chown(path=\"%s\", uid=%d, gid=%d)\n", path, uid, gid);
    bb_fullpath(fpath, path);

    return log_syscall("chown", chown(fpath, uid, gid), 0);
}

/* change the size of a file */
int bb_truncate(const char* path, off_t newsize) {
    char fpath[PATH_MAX];

    log_msg("\nbb_truncate(path=\"%s\", newsize=%lld)\n", path, newsize);
    bb_fullpath(fpath, path);

    return log_syscall("truncate", truncate(fpath, newsize), 0);
}

/*
 * change the access and/or modification times of a file 
 * note  -- I'll want to change this as soon as 2.6 is in debian testing
 * */
int bb_utime(const char* path, struct utimbuf* ubuf) {
    char fpath[PATH_MAX];

    log_msg("\nbb_utime(path=\"%s\", ubuf=0x%08x)\n", path, ubuf);
    bb_fullpath(fpath, path);

    return log_syscall("utime", utime(fpath, ubuf), 0);
}

/*
 * File open operation
 * No creation, or truncation flags (O_CREATE, O_EXCL, O_TRUNC)
 * will be passed to open(). Open should check if the operation
 * is premitted for the given flags. Optionally open may also
 * return an arbitrary filehandle in the fuse_file_info structure,
 * which will be passed to all file operations
 *
 * */
int bb_open(const char* path, struct fuse_file_info* fi) {
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];

    log_msg("\nbb_open(path=\"%s\", fi=0x%08x)\n", path, fi);
    bb_fullpath(fpath, path);

    // if the open call succeeds, my retstat is the file descriptor
    // else it's -errno. I'm makeing sure that in that case the saved
    // file descriptor is exactly -1
    fd = log_syscall("open", open(fpath, fi->flags), 0);
    if (fd < 0) {
        retstat = log_error("open");
    }

    fi->fh = fd;

    log_fi(fi);
    return retstat;
}

/**
 * Read Data from an open file
 *
 * Read should return exactly the number of bytes requested except
 * on EOF or error, otherwise the rest of the data will be
 * substituted with zeroes. An exception to this is when the
 * 'direct_io' mount option is specified, in which case the return value
 * of the read system call will reflect the return value of this operation
 * */
int bb_read(const char* path, char* buf, size_t size, off_t offset,
            struct fuse_file_info* fi) {
    int retstat = 0;
    log_msg("\nbb_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
            path, buf, size, offset, fi);

    log_fi(fi);

    return log_syscall("pread", pread(fi->fh, buf, size, offset), 0);
}

/**
 * Write data to an open file 
 * write should return exactly the number of bytes requested
 * expect on error. Anexception to this is when the 'direct_io'
 * mount option is specified (see read opertion)
 * */

int bb_write(const char* path, const char* buf, size_t size, off_t offset,
        struct fuse_file_info* fi) {
    int retstat = 0;
    log_msg("\nbb_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
            path, buf, size, offset, fi);
    log_fi(fi);

    return log_syscall("pwrite", pwrite(fi->fh, buf, size, offset), 0);
}

/**
 * Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' field are ignored
 *
 * Replaced 'struct statfs' parameter with 'struct stavfs' in version 2.5
 * */
int bb_statfs(const char* path, struct statvfs* statv) {
    int retstat = 0;
    char fpath[PATH_MAX];

    log_msg("\nbb_statfs(path=\"%s\", statv=0x%08x)\n", path, statv);
    bb_fullpath(fpath, path);

    // get stats for underlying filesystem
    retstat = log_syscall("statfs", statvfs(fpath, statv), 0);
    log_statvfs(statv);
    return retstat;
}


/***
 * Possibly fulsh cached data
 *
 * BIG NOTE: this is not equivalent to fsync(). It's not a request to sync dirty
 * data.
 *
 * Flush is called on each close() of a file descriptor. So if a filesystem
 * wants to return write errors in close() and the file has cached dirty data,
 * this is a good place to write back data and return any errors. since many
 * applications ignore close() errors this is not always useful.
 *
 * Note: The flush method may be called more than noce for each open(). This
 * happens if more than one file descriptor refers to an opened file due to
 * dup(), dup2() or fork() calls. It is not possible to determine if a flush is
 * final, so each flush should be treated equally. Multiple write-flush
 * sequences are relatively rare, so this shouldn't be a problem
 *
 * Filesystem shouldn't assume that flush will always be called after some
 * writes, or that if will be called at all
 */
/* this is a no-op in BBFS, It just logs the call and return access*/
int bb_flush(const char* path, struct fuse_file_info* fi) {
    log_msg("\nbb_flush(path=\"%s\", fi=0x%08x)\n", path, fi);
    log_fi(fi);
    return 0;
}

/***
 * Release an open file
 *
 * Release is called when there are no more references to an open
 * file: all file descriptors are closed and all memory mappings are unmapped.
 *
 * For every open() call there will be exactly one release() call
 * with the same flags and file descriptor. It is possible to 
 * have a file opened more than once, in whic case only the last release will
 * mean, that no more read/writes will happen on the file.
 * the return value of release is ignored.
 */

int bb_release(const char* path, struct fuse_file_info* fi) {
    log_msg("\nbb_release(path=\"%s\", fi=0x%08x)\n", path, fi);
    log_fi(fi);

    /*
     * We need to close the file. Had we allocate any resources
     * (buffers etc) we'd need to free them here as well
     * */
    return log_syscall("close", close(fi->fh), 0);
}

/**
 * Synchronize file contents
 * 
 * If the datasync parameter is non-zero, then only the user data
 * should be flushed, not the meta data
 * */
int bb_fsync(const char* path, int datasync, struct fuse_file_info* fi) {
    log_msg("\nbb_fsync(path=\"%s\", datasync=%d, fi=0x%08x)\n", path, datasync, fi);
    log_fi(fi);

#ifdef HAVE_FDATASYNC
    if (datasync) {
        return log_syscall("fdatasync", fdatasync(fi->fh), 0);
    }
    else
#endif
        return log_syscall("fsync" , fsync(fi->fh), 0);
}
#ifdef HAVE_SYS_XATTR_H

/**
 * Note that my implementations of the various xattr functions use the 'l-'
 * version of the functions (eg bb_setxattr() calls lsetxattr() not setxattr(),
 * etc). This is because it appears any symbolic links are resolved before the
 * actual call takes place, so I only need to use the system-provided calls that
 * don't follow them
 * */

/** Set extended attributes  */
int bb_setxattr(const char* path, const char* name, const char* value, size_t size, int flags) {
    char fpath[PATH_MAX];
    log_msg("\nbb_setxattr(path=\"%s\", name=\"%s\", value=\"%s\", size=%d, flags=0x%08x)\n",
            path, name, value, size, flags);
    bb_fullpath(fpath, path);
    return log_syscall("lsetxattr", lsetxattr(fpath, name, value, size, flags), 0);
}

/** Removed extended attibutes */
int bb_removexattr(const char* path, const char* name) {
    char fpath[PATH_MAX];

    log_msg("\nbb_removexattr(path=\"%s\", name=\"%s\")\n", path, name);
    bb_fullpath(fpath, path);

    return log_syscall("lremovexattr", lremovexattr(fpath, name), 0);
}

#endif

/**
 * Open directory
 *
 * This method should check is the open operation is permited ofr this directory
 *
 * */

int bb_opendir(const char* path, struct fuse_file_info* fi) {
    DIR* dp;
    int retstat = 0;
    char fpath[PATH_MAX];

    log_msg("\nbb_opendir(path=\"%s\", fi=0x%08x)\n", path, fi);
    bb_fullpath(fpath, path);

    // since opendir returns a pointer, takes some custom handling of return
    // status
    dp = opendir(fpath);
    log_msg("     opendir returned 0x%p\n", dp);
    if (dp == NULL) {
        retstat = log_error("bb_opendir opendir");
    }
    fi->fh = (intptr_t) dp;
    log_fi(fi);
    return retstat;
}


/**
 * Read directory
 *
 * This supersedes the old getidr() interface. New applications
 * should use this
 *
 * The filesystem man choose between two modes of operation
 *
 * 1) the readdir implemetation ignores the offset parameter, and
 * passes zero to the filler function's offset. The filler
 * function will not return '1' (unless an error happends), so the
 * whole diretory is read in a single readdir operation. this
 * works just linke the old getdir() method
 *
 * 2) The readdir implementation keeps track of the offsets of the
 * directory entries . It uses the offset parameter and always 
 * passes non-zero offset to the filler function. When the buffer
 * is full (or an error happends) the filler function will return '1'
 *
 * */

int bb_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset,
        struct fuse_file_info* fi) {
    int retstat = 0;
    DIR* dp;
    struct dirent* de;

    log_msg("\nbb_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, fi=0x%08x)\n",
            path, buf, filler, offset, fi);

    dp = (DIR*) (uintptr_t) fi->fh;

    // every directory contains at least two entries: . and .. if my
    // first call to the system readdir() returns NULL I've got ans error;
    // near as I call tell, that's the only condition under which I can get
    // anerror from readdir()
}
