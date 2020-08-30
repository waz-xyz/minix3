/* Function prototypes. */

struct mproc;
struct stat;
struct mem_map;
struct memory;

#include <timers.h>

/* alloc.c */
phys_clicks alloc_mem(phys_clicks clicks);
void free_mem(phys_clicks base, phys_clicks clicks);
void mem_init(struct memory *chunks, phys_clicks *free);
#if ENABLE_SWAP
int swap_on(char *file, u32_t offset, u32_t size);
int swap_off(void);
void swap_in(void);
void swap_inqueue(struct mproc *rmp);
#else /* !SWAP */
#define swap_in()		((void)0)
#define swap_inqueue(rmp)	((void)0)
#endif /* !SWAP */
int mem_holes_copy(struct hole *, size_t *, u32_t *);

/* break.c */
int adjust(struct mproc *rmp, vir_clicks data_clicks, vir_bytes sp);
int do_brk(void);
int size_ok(int file_type, vir_clicks tc, vir_clicks dc, vir_clicks sc, vir_clicks dvir, vir_clicks s_vir);

/* devio.c */
int do_dev_io(void);
int do_dev_io(void);

/* dmp.c */
int do_fkey_pressed(void);

/* exec.c */
int do_exec(void);
void rw_seg(int rw, int fd, int proc, int seg, phys_bytes seg_bytes);
struct mproc *find_share(struct mproc *mp_ign, Ino_t ino, Dev_t dev, time_t ctime);

/* forkexit.c */
int do_fork(void);
int do_pm_exit(void);
int do_waitpid(void);
void pm_exit(struct mproc *rmp, int exit_status);

/* getset.c */
int do_getset(void);

/* main.c */
int main(void);

/* misc.c */
int do_reboot(void);
int do_procstat(void);
int do_getsysinfo(void);
int do_getprocnr(void);
int do_svrctl(void);
int do_allocmem(void);
int do_freemem(void);
int do_getsetpriority(void);
ssize_t _read_pm(int _fd, void *_buf, size_t _n, int s, int e);
ssize_t _write_pm(int _fd, void *_buf, size_t _n, int s, int e);


#if (MACHINE == MACINTOSH)
phys_clicks start_click(void);
#endif

void setreply(int proc_nr, int result);

/* signal.c */
int do_alarm(void);
int do_kill(void);
int ksig_pending(void);
int do_pause(void);
int set_alarm(int proc_nr, int sec);
int check_sig(pid_t proc_id, int signo);
void sig_proc(struct mproc *rmp, int sig_nr);
int do_sigaction(void);
int do_sigpending(void);
int do_sigprocmask(void);
int do_sigreturn(void);
int do_sigsuspend(void);
void check_pending(struct mproc *rmp);

/* time.c */
int do_stime(void);
int do_time(void);
int do_times(void);
int do_gettimeofday(void);

/* timers.c */
void pm_set_timer(timer_t *tp, int delta, tmr_func_t watchdog, int arg);
void pm_expire_timers(clock_t now);
void pm_cancel_timer(timer_t *tp);

/* trace.c */
int do_trace(void);
void stop_proc(struct mproc *rmp, int sig_nr);

/* utility.c */
pid_t get_free_pid(void);
int allowed(char *name_buf, struct stat *s_buf, int mask);
int no_sys(void);
void panic(char *who, char *mess, int num);
void tell_fs(int what, int p1, int p2, int p3);
int get_stack_ptr(int proc_nr, vir_bytes *sp);
int get_mem_map(int proc_nr, struct mem_map *mem_map);
char *find_param(const char *key);
int proc_from_pid(pid_t p);
int pm_isokendpt(int ep, int *proc);
