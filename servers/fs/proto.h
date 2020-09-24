/* Function prototypes. */

#include "timers.h"

/* Structs used in prototypes must be declared as such first. */
struct buf;
struct filp;		
struct inode;
struct super_block;

/* cache.c */
zone_t alloc_zone(Dev_t dev, zone_t z);
void flushall(Dev_t dev);
void free_zone(Dev_t dev, zone_t numb);
struct buf *get_block(Dev_t dev, block_t block,int only_search);
void invalidate(Dev_t device);
void put_block(struct buf *bp, int block_type);
void rw_scattered(Dev_t dev, struct buf **bufq, int bufqsize, int rw_flag);

#if ENABLE_CACHE2
/* cache2.c */
void init_cache2(unsigned long size);
int get_block2(struct buf *bp, int only_search);
void put_block2(struct buf *bp);
void invalidate2(Dev_t device);
#endif

/* device.c */
int dev_open(Dev_t dev, int proc, int flags);
void dev_close(Dev_t dev);
int dev_io(int op, Dev_t dev, int proc, void *buf, off_t pos, int bytes, int flags);
int gen_opcl(int op, Dev_t dev, int proc, int flags);
int gen_io(int task_nr, message *mess_ptr);
int no_dev(int op, Dev_t dev, int proc, int flags);
int no_dev_io(int, message *);
int tty_opcl(int op, Dev_t dev, int proc, int flags);
int ctty_opcl(int op, Dev_t dev, int proc, int flags);
int clone_opcl(int op, Dev_t dev, int proc, int flags);
int ctty_io(int task_nr, message *mess_ptr);
int do_ioctl(void);
int do_setsid(void);
void dev_status(message *);
void dev_up(int major);

/* dmp.c */
int do_fkey_pressed(void);

/* dmap.c */
int do_devctl(void);
void build_dmap(void);
int map_driver(int major, int proc_nr, int dev_style);
int dmap_driver_match(int proc, int major);
void dmap_unmap_by_endpt(int proc_nr);
void dmap_endpt_up(int proc_nr);

/* filedes.c */
struct filp *find_filp(struct inode *rip, mode_t bits);
int get_fd(int start, mode_t bits, int *k, struct filp **fpt);
struct filp *get_filp(int fild);
int inval_filp(struct filp *);

/* inode.c */
struct inode *alloc_inode(dev_t dev, mode_t bits);
void dup_inode(struct inode *ip);
void free_inode(Dev_t dev, Ino_t numb);
struct inode *get_inode(Dev_t dev, int numb);
void put_inode(struct inode *rip);
void update_times(struct inode *rip);
void rw_inode(struct inode *rip, int rw_flag);
void wipe_inode(struct inode *rip);

/* link.c */
int do_link(void);
int do_unlink(void);
int do_rename(void);
int do_truncate(void);
int do_ftruncate(void);
int truncate_inode(struct inode *rip, off_t len);
int freesp_inode(struct inode *rip, off_t st, off_t end);

/* lock.c */
int lock_op(struct filp *f, int req);
void lock_revive(void);

/* main.c */
int main(void);
void reply(int whom, int result);

/* misc.c */
int do_dup(void);
int do_exit(void);
int do_fcntl(void);
int do_fork(void);
int do_exec(void);
int do_revive(void);
int do_set(void);
int do_sync(void);
int do_fsync(void);
int do_reboot(void);
int do_svrctl(void);
int do_getsysinfo(void);

/* mount.c */
int do_mount(void);
int do_umount(void);
int unmount(Dev_t dev);

/* open.c */
int do_close(void);
int do_creat(void);
int do_lseek(void);
int do_mknod(void);
int do_mkdir(void);
int do_open(void);
int do_slink(void);

/* path.c */
struct inode *advance(struct inode **dirp, char string[NAME_MAX]);
int search_dir(struct inode *ldir_ptr, char string [NAME_MAX], ino_t *numb, int flag);
struct inode *eat_path(char *path);
struct inode *last_dir(char *path, char string [NAME_MAX]);
struct inode *parse_path(char *path, char string[NAME_MAX], int action);

/* pipe.c */
int do_pipe(void);
int do_unpause(void);
int pipe_check(struct inode *rip, int rw_flag, int oflags, int bytes, off_t position, int *canwrite, int notouch);
void release(struct inode *ip, int call_nr, int count);
void revive(int proc_nr, int bytes);
void suspend(int task);
int select_request_pipe(struct filp *f, int *ops, int bl);
int select_cancel_pipe(struct filp *f);
int select_match_pipe(struct filp *f);
void unsuspend_by_endpt(int);

/* protect.c */
int do_access(void);
int do_chmod(void);
int do_chown(void);
int do_umask(void);
int forbidden(struct inode *rip, mode_t access_desired);
int read_only(struct inode *ip);

/* read.c */
int do_read(void);
struct buf *rahead(struct inode *rip, block_t baseblock, off_t position, unsigned bytes_ahead);
void read_ahead(void);
block_t read_map(struct inode *rip, off_t pos);
int read_write(int rw_flag);
zone_t rd_indir(struct buf *bp, int index);

/* stadir.c */
int do_chdir(void);
int do_fchdir(void);
int do_chroot(void);
int do_fstat(void);
int do_stat(void);
int do_fstatfs(void);
int do_rdlink(void);
int do_lstat(void);

/* super.c */
bit_t alloc_bit(struct super_block *sp, int map, bit_t origin);
void free_bit(struct super_block *sp, int map, bit_t bit_returned);
struct super_block *get_super(Dev_t dev);
int mounted(struct inode *rip);
int read_super(struct super_block *sp);
int get_block_size(dev_t dev);

/* time.c */
int do_stime(void);
int do_utime(void);

/* utility.c */
time_t clock_time(void);
unsigned conv2(int norm, int w);
long conv4(int norm, long x);
int fetch_name(char *path, int len, int flag);
int no_sys(void);
int isokendpt_f(const char *f, int l, int e, int *p, int ft);
void panic(char *who, char *mess, int num);

#define okendpt(e, p)	isokendpt_f(__FILE__, __LINE__, (e), (p), 1)
#define isokendpt(e, p)	isokendpt_f(__FILE__, __LINE__, (e), (p), 0)

/* write.c */
void clear_zone(struct inode *rip, off_t pos, int flag);
int do_write(void);
struct buf *new_block(struct inode *rip, off_t position);
void zero_block(struct buf *bp);
int write_map(struct inode *, off_t, zone_t, int);

/* select.c */
int do_select(void);
int select_callback(struct filp *, int ops);
void select_forget(int fproc);
void select_timeout_check(timer_t *);
void init_select(void);
void select_unsuspend_by_endpt(int proc);
int select_notified(int major, int minor, int ops);

/* timers.c */
void fs_set_timer(timer_t *tp, int delta, tmr_func_t watchdog, int arg);
void fs_expire_timers(clock_t now);
void fs_cancel_timer(timer_t *tp);
void fs_init_timer(timer_t *tp);

/* cdprobe.c */
int cdprobe(void);
