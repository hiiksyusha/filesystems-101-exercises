#include <solution.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <fuse.h>


static void *my_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    
    (void) conn;
    
    cfg->kernel_cache = 1;
    cfg->set_uid = 1;
    cfg->uid = getgid();
    cfg->set_gid = 1;
    cfg->gid = getgid();
    cfg->set_mode = 1;
    cfg->umask = ~S_IRUSR;
    
    return NULL;
}

static int my_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    
    (void) fi;
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        
    }
    else if (strcmp(path+1, "/hello") == 0) {
        stbuf->st_mode = S_IFREG | S_IRUSR;
        stbuf->st_nlink = 1;
        stbuf->st_size = 32;
    }
    
    else
        res = -ENOENT;

    return res;
}

static int my_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, "hello", NULL, 0, 0);

    return 0;
}

static int my_open(const char *path, struct fuse_file_info *fi) {
    
    void(fi);

    if ((fi->flags & O_ACCMODE) != O_RDONLY)
        return -EROFS;
    
    if(strcmp(path, "/hello") != 0)
        return -ENOENT;

    return 0;
}

static int my_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    
    (void) fi;
    if(strcmp(path+1, "/hello") != 0)
        return -ENOENT;

    struct fuse_context* ctx = fuse_get_context();
    char kek[512] = {0};
    sprintf(kek, "hello, %d\n", ctx->pid);
    const size_t len = strlen(kek);
    
    if ((size_t)offset < len) {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, kek + offset, size);
    }
    else
        size = 0;

    return size;
}

static int my_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    
    (void) path;
    (void) buf;
    (void) size;
    (void) offset;
    (void) fi;
    
    return -EROFS;
    
}

static int my_rename(const char* path, const char* new_name, unsigned int flags) {
    
    (void) path;
    (void) new_name;
    (void) flags;

    return -EROFS;
}

static int my_mkdir(const char *path, mode_t mode) {
    
    (void) path;
    (void) mode;

    return -EROFS;
}

static int my_unlink(const char *path) {
    
    (void) path;

    return -EROFS;
}
    
static int my_setxattr(const char *path, const char *name, const char *value, size_t size, int flags){
    
    (void)path;
    (void)name;
    (void)value;
    (void)size;
    (void)flags;
    
    return -EROFS;
}
    
static int my_removexattr(const char* path, const char* name) {
    
    (void) path;
    (void) name;

    return -EROFS;
}

static int my_write_buf(const char *path, struct fuse_bufvec *buf, off_t off, struct fuse_file_info *fi) {
    
    (void)path;
    (void)buf;
    (void)off;
    (void)fi;

    return -EROFS;
}
    
static int my_create(const char *path, mode_t mode, struct fuse_file_info *fi){
    
    (void) path;
    (void) mode;
    (void) fi;

    return -EROFS;
}

static const struct fuse_operations hellofs_ops = {
    .init = my_init,
    .getattr = my_getattr,
    .readdir = my_readdir,
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .rename = my_rename,
    .mkdir = my_mkdir,
    .unlink = my_unlink,
    .setxattr = my_setxattr,
    .removexattr = my_removexattr,
    .write_buf = my_write_buf,
    .create = my_create
};

int helloworld(const char *mntp)
{
	char *argv[] = {"exercise", "-f", (char *)mntp, NULL};
	return fuse_main(3, argv, &hellofs_ops, NULL);
}
