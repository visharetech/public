#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
  #include <cstdlib>
  #include <cstdarg>
  #include <cstdio>
  #include <new>
#else
  #include <stdlib.h>
  #include <stdarg.h>
  #include <stdio.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "syscall.h"
#include "io_map.h"
#include "recursive_mutex.h"
#if ASIM_CALL
    #include "../hls_apcall.h"
#endif

//Disable profile for io function
#if HW_PROFILE
#define PAUSE_PROFILE()   riscv_pause_profile()
#define RESUME_PROFILE()  riscv_resume_profile()
#else
#define PAUSE_PROFILE()
#define RESUME_PROFILE()
#endif

//-Wl,–wrap=__wrap_symbol
void *__dso_handle;
#ifndef MIN
 #define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif

#if NETFILE
#include "../netfile/fileio.hpp"
#endif

#ifdef __cplusplus
using namespace std;
extern  "C" {
#endif

#if HLS_CMDR
extern void hls_init(void);
#endif

typedef void (*initFunc)();
extern initFunc __init_array_start[];
extern initFunc __init_array_end[];

#if SUPPORT_MULTI_THREAD
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

#if _rvTranslate
    static recursive_mutex_t malloc_mutex = RECURSIVE_MUTEX_INITIALIZER;
#else
    static pthread_mutex_t malloc_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#endif

//compile error if MAX_ARGC declared as 'static const unsigned int' in C
#ifdef __cplusplus
static const unsigned int MAX_ARGC = 80;
#else
enum {MAX_ARGC = 80};
#endif

//Hardware version
enum
{
    VER_MAJOR,
    VER_MINOR,
    VER_PATCH,
    VER_YY,
    VER_MM,
    VER_DD,
    ICACHE_WAY_NUM,
    ICACHE_SET_NUM,
    DCACHE_WAY_NUM,
    DCACHE_SET_NUM,
    ENABLE_HLS
};

static char *argv_str[MAX_ARGC];

//-------------------------- new_hander() -------------------------------
#ifdef __cplusplus
void riscv_new_handler()
{
    printf("Cannot allocate memory for new operator\n");
    exit(1);
}

//Not used standard set_new_handler() to prevent unsupported "amoswap.w.aq" instruction.
new_handler riscv_set_new_handler()
{
    int *pc   = (int*)set_new_handler;
    int ins   = *pc;
    int *fptr = NULL;
    int prev  = 0;

    //Original set_new_handler() assembly code:
    //   addi            a5,gp,-116 # 2314fc <_ZN12_GLOBAL__N_113__new_handlerE>
    //   fence           iorw,ow
    //   amoswap.w.aq    a0,a0,(a5)
    //   ret

    //Get function pointer of default new_handler()
    asm volatile ("add %0, gp, %1" : "=r"(fptr) : "r"(ins >> 20));    
    //Get previous new_handler
    prev = *fptr;
    //Update new_handler() with riscv_new_hander()
    *fptr = (int)riscv_new_handler;
    
    printf("First instruction of set_new_handler(): 0x%x PC=0x%x\n", ins, (int)pc);
    printf("Write 0x%x @ 0x%x\n", (int)riscv_new_handler, (int)fptr);
    
    return (new_handler)prev;
}
#endif

//-------------------------- memset() & memcpy() with DMA -------------------------------
#if 0
void *__wrap_memset(void *dst, int val, size_t len)
{
    uint8_t *d = (uint8_t*)dst;    
    for (int i = 0; i < len; i += 65535)
    {
        int n = len - i;
        if (n > 65535) n = 65535;
        mtdma_memset(d, val, n, MTDMA_BLOCKING);
        d += n;
    }
    return dst;
}
void *__wrap_memcpy(void *dst, void *src, size_t len)
{
    uint8_t *d = (uint8_t*)dst;
    uint8_t *s = (uint8_t*)src;
    for (int i = 0; i < len; i += 65535)
    {
        int n = len - i;
        if (n > 65535) n = 65535;
        mtdma_memcpy(d, s, n, MTDMA_BLOCKING);
        d += n;
        s += n;
        
    }
    return dst;
}
#endif
//-------------------------- malloc_lock() & malloc_unlock() -------------------------------
void __wrap___malloc_lock(struct _reent *r)
{
#if SUPPORT_MULTI_THREAD
    #if _rvTranslate
        recursive_mutex_lock(&malloc_mutex);
    #else    
        pthread_mutex_lock(&malloc_mutex);
    #endif
#endif
}
void __wrap___malloc_unlock(struct _reent *r)
{
#if SUPPORT_MULTI_THREAD
    #if _rvTranslate
        recursive_mutex_unlock(&malloc_mutex);
    #else
        pthread_mutex_unlock(&malloc_mutex);
    #endif
#endif
}

//-------------------------- gettimeofday -------------------------------
int __wrap_gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if (tv != NULL)
    {
        struct timeval t;
        uint64_t us = (cycle64() * SYSTEM_PERIOD) / 1000;
        t.tv_sec = us / 1000000;
        t.tv_usec = us % 1000000;
        *tv = t;
    }        
    return 0;
}

//-------------------------- printf() -------------------------------
int __wrap_printf(const char *format, ...)
{
    PAUSE_PROFILE();
    va_list args;
    va_start(args,format);
#if SUPPORT_MULTI_THREAD
    pthread_mutex_lock(&print_mutex);
#endif
    int result = vprintf(format, args);
#if SUPPORT_MULTI_THREAD    
    pthread_mutex_unlock(&print_mutex);
#endif
    va_end(args);
    RESUME_PROFILE();
    return result;
}

//-------------------------- access() -------------------------------
int __wrap_access(const char *path, int amode)
{
    // Force to return success
    __wrap_printf("%s: Force to return OK (path:%s)\n", __func__, path);
    return 0;
}

//-------------------------- fopen(), fclose(), fread(), fwrite(), ... -------------------------------
#if NETFILE
// category: fopen, fwrite, fputs, fwrite related functions  (FILE*)
FILE * __wrap_fopen(const char *filename, const char *mode)
{
    PAUSE_PROFILE();
    FILE * ret = nf_fopen(filename, mode);
    RESUME_PROFILE();
    return ret;
}
int __wrap_fopen_s(FILE** pFile, const char *filename, const char *mode)
{
    PAUSE_PROFILE();
    *pFile = nf_fopen(filename, mode);
    if (!(*pFile)) {
        RESUME_PROFILE();
        return -1;
    }
    RESUME_PROFILE();
    return 0;		//return 0 means success
}
int __wrap_fclose(FILE *stream)
{
    PAUSE_PROFILE();
    int ret = nf_fclose(stream);
    RESUME_PROFILE();
    return ret;
}
int __wrap_feof(FILE *stream)
{
    PAUSE_PROFILE();
    int ret = nf_feof(stream);
    RESUME_PROFILE();
    return ret;
}
int __wrap_fflush(FILE *stream)
{
    PAUSE_PROFILE();
    int ret = nf_fflush(stream);
    RESUME_PROFILE();
    return ret;
}
int __wrap_fseek(FILE *stream, long int offset, int whence)
{
    PAUSE_PROFILE();
    int ret = nf_fseek(stream, offset, whence);
    RESUME_PROFILE();
    return ret;
}
long int __wrap_ftell(FILE *stream)
{
    PAUSE_PROFILE();
    long int ret = nf_ftell(stream);
    RESUME_PROFILE();
    return ret;
}
char * __wrap_fgets(char *str, int n, FILE *stream)
{
    PAUSE_PROFILE();
    char * ret = nf_fgets(str, n, stream);
    RESUME_PROFILE();
    return ret;
}
int __wrap_fputs(const char *str, FILE *stream)
{
    PAUSE_PROFILE();
    if(stream == stdout || stream == stderr) {
        __wrap_printf("%s", str);
        RESUME_PROFILE();
        return 0;
    }
    else {
        int ret = nf_fputs(str, stream);
        RESUME_PROFILE();
        return ret;
    }
}
size_t __wrap_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    PAUSE_PROFILE();
    size_t ret = nf_fread(ptr, size, nmemb, stream);
    RESUME_PROFILE();
    return ret;
}
size_t __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    PAUSE_PROFILE();
    if(stream == stdout || stream == stderr) {
        size_t totalcount = size*nmemb;
        while(totalcount--){
            __wrap_printf("%c", *((char*)ptr));
            ptr = (char*)ptr + 1;
        }
        RESUME_PROFILE();
        return size * nmemb;
    } else {
        size_t ret = nf_fwrite(ptr, size, nmemb, stream);
        RESUME_PROFILE();
        return ret;
    }
}
int __wrap_fscanf(FILE *stream, const char *format, ...)
{       
    PAUSE_PROFILE();    
    va_list args;
    va_start(args,format);
    int result = nf_fscanf(stream, format, args);
    va_end(args);    
    RESUME_PROFILE();
    return result;
    
}
int __wrap_fprintf(FILE *stream, const char *format, ...)
{
    PAUSE_PROFILE();
    int result;
    va_list args;
    va_start(args,format);
    if(stream == stdout || stream == stderr)
    {
#if SUPPORT_MULTI_THREAD
        pthread_mutex_lock(&print_mutex);
#endif
        result = vprintf(format, args);
#if SUPPORT_MULTI_THREAD
        pthread_mutex_unlock(&print_mutex);
#endif
    }
    else
    {
        result = nf_vfprintf(stream, format, args);
    }
    va_end(args);    
    RESUME_PROFILE();
    return result;
}
int __wrap_fseeko(FILE *stream, off_t offset, int whence)
{
    PAUSE_PROFILE();
    int ret = nf_fseeko(stream, offset, whence);
    RESUME_PROFILE();
    return ret;
}
int __wrap_ferror(FILE *stream)
{
    PAUSE_PROFILE();
    int ret = nf_ferror(stream);
    RESUME_PROFILE();
    return ret;
}

// category: file related function without FILE*, e.g. rename, unlink, stat
int __wrap_rename(const char *old_filename, const char *new_filename)
{
    PAUSE_PROFILE();
    int ret = nf_rename(old_filename, new_filename);
    RESUME_PROFILE();
    return ret;
}
int __wrap_unlink(const char *pathname)
{
    PAUSE_PROFILE();
    int ret = nf_unlink(pathname);
    RESUME_PROFILE();
    return ret;
}
int __wrap_stat(const char */*restrict*/ pathname, struct stat */*restrict*/ statbuf)
{
    PAUSE_PROFILE();
    int ret = nf_stat(pathname, statbuf);
    RESUME_PROFILE();
    return ret;
}

// cateogry: file related function use int as fd, e.g. open, close, read, write
int __wrap_open(const char * pathname, int flags, mode_t mode)
{
    PAUSE_PROFILE();
    printf("%s, file:%s, flags:0x%x, mode:0x%x\n", __func__, pathname, flags, mode);

    char m[5];
    char *pm = m;

    if(flags & O_RDWR) {
        *pm++ = 'r';
        *pm++ = 'w';
    } else if(flags & O_WRONLY){
        *pm++ = 'w';
    } else {
        *pm++ = 'r';    //O_RDONLY = 0x0000
    } 

    if(flags & O_APPEND) {
        *pm++ = 'a';
    }

    //open as binary mode
    *pm++ = 'b';

    if(flags & O_APPEND){
        *pm++ = '+';
    }

    *pm = '\0';

    int ret = (int)nf_fopen(pathname, m);
    RESUME_PROFILE();
    return ret;
}
FILE * __wrap_fdopen(int fildes, const char *mode)
{
    PAUSE_PROFILE();
    printf("%s, fildes:%d, mode:%s\n", __func__, fildes, mode);
    FILE * ret = (FILE*)fildes;
    RESUME_PROFILE();
    return ret;
}
int __wrap_close(int fd)
{
    PAUSE_PROFILE();
    printf("%s\n", __func__);
    int ret = nf_fclose((FILE*)fd);
    RESUME_PROFILE();
    return ret;
}
int __wrap_fcntl(int fd, int cmd, ... /* arg */ )
{
    PAUSE_PROFILE();
    printf("%s, cmd:%d\n", __func__, cmd);
    RESUME_PROFILE();
    return fd;
}
ssize_t __wrap_read(int fd, void *buf, size_t count)
{
    PAUSE_PROFILE();
    printf("%s, fd:%d, count:%d\n", __func__, fd, count);
    ssize_t ret = nf_fread(buf, 1, count, (FILE*)fd);
    RESUME_PROFILE();
    return ret;
}
ssize_t __wrap_write(int fd, const void *buf, size_t count)
{
    PAUSE_PROFILE();
    printf("%s, fd:%d, count:%d\n", __func__, fd, count);
    ssize_t ret = nf_fwrite(buf, 1, count, (FILE*)fd);
    RESUME_PROFILE();
    return ret;
}
off_t __wrap_lseek(int fd, off_t offset, int whence)
{
    PAUSE_PROFILE();
    printf("%s, fd:%d, off_t:%d whence:%d\n", __func__, fd, offset, whence);
    off_t ret = nf_fseek((FILE*)fd, offset, whence);
    RESUME_PROFILE();
    return ret;
}
int __wrap_fstat(int fildes, struct stat *buf)
{
    PAUSE_PROFILE();
    printf("%s\n", __func__);

#if 0
    int res = nf_fstat(fildes, buf);
    printf("fstat: st_size:%d\n", buf->st_size);
    printf("       st_blksize:%d\n", buf->st_blksize);
    printf("       st_blocks:%d\n", buf->st_blocks);
    printf("       st_mode:0x%x\n", buf->st_mode);
    RESUME_PROFILE();
    return res;

#elif 0
    printf("fstat return dummy value\n");
    buf->st_size = 1225924;
    buf->st_blksize = 4096;
    buf->st_blocks = 2400;
    buf->st_mode = 0x81B6;  //100666
    RESUME_PROFILE();
    return 0;
#else
    RESUME_PROFILE();
    return -1;  //not support fstat, then openHEVC will choose to use fseek function.
#endif
}
#endif


#if NETFILE
static FILE *fargv;

void nf_argv_open(const char *filepath) {
    fargv = fopen(filepath, "rt");
    if (fargv == NULL) {
        printf("Cannot open %s\n", filepath);
        exit(1);
    }
}

void nf_argv_close() {
    if (fargv != NULL) {
        fclose(fargv);
    }
}

bool nf_argv_load(int *argc_ptr, char ***argv_ptr) {
    static int line_idx = 0;
    static int argc = 0;
    static char* argv_from_file[MAX_ARGC];

    if (argc_ptr != NULL && argv_ptr != NULL)
    {
        static char buf[1024];
        char* sbuf = buf;

        do {
            if (fgets(buf, sizeof(buf) - 1, fargv) == NULL) {
                if (line_idx == 0) {
                    printf("Cannot read the line from argv. line_num:%d\n", line_idx+1);
                } else {
                    printf("Load from argv file: EOF detected\n");
                }
                return false;
            }
            ++line_idx;
        } while (buf[0] == '#' || buf[0] == '\r' || buf[0] == '\n');

        buf[sizeof(buf) - 1] = '\0';
        buf[strcspn(buf, "\r\n")] = '\0';       // remove \r\n or \n

        printf("buf:%s\n", buf);

        argv_from_file[0] = sbuf;
        argc = 1;
        while (*sbuf != '\0') {
            sbuf = strchr(sbuf, ' ');
            if (sbuf) {
                *sbuf = '\0';
                argv_from_file[argc] = sbuf + 1;
                argc++;
                sbuf++;

                if (argc >= MAX_ARGC)
                {
                    printf("Argument number is too larger. Maximum is %d\n", MAX_ARGC);
                    exit(1);
                }
            }
            else {
                break;
            }
        }

        printf("parsed argc:%d\n", argc);

        for (int i = 0; i < argc; i++) {
            printf("- argv[%d]:%s\n", i, argv_from_file[i]);
        }

        *argc_ptr = argc;
        *argv_ptr = argv_from_file;
    }
    return true;
}
#endif


//-------------------------- riscv initialization -------------------------------
void riscv_init()
{
#if SUPPORT_MULTI_THREAD
    //Print mutex
    pthread_mutex_init(&print_mutex, NULL);
    
    //Malloc mutex
    #ifndef _rvTranslate 
        pthread_mutex_init(&malloc_mutex, NULL);
    #endif
#endif

    //Set new_handler() for new operator
#if defined(__cplusplus)
    riscv_set_new_handler();
#endif

#if ASIM_CALL
    asim_set_hls_handler(asim_hls_handler);
#endif

#if NETFILE
    //Netfile initialization
    nf_init();
#endif

    //Call functions in .init_array
    printf("__init_array_start=%x\n", (int)__init_array_start);
    printf("__init_array_end=%x\n", (int)__init_array_end);
    for (int i = 0; i < ((int)__init_array_end - (int)__init_array_start) / 4; i++)
    {
        //printf(".init_array call:%x\n", (int)__init_array_start[i]);
        __init_array_start[i]();
    }
    
    //Print RISCV version
    volatile int * hw_ver = (int*)(HW_VERSION);
    printf("------------------------------------------------------------------------------\n");
    printf("SW Build Time: %s %s\n", __DATE__, __TIME__);
    if (marchid() >= 1) {
        printf("System Build Version: %d.%d.%d\n", hw_ver[VER_MAJOR], hw_ver[VER_MINOR], hw_ver[VER_PATCH]);
        printf("System Build Date (yy-mm-dd): %d-%d-%d\n", hw_ver[VER_YY], hw_ver[VER_MM], hw_ver[VER_DD]);
    }
    printf("RISCV Vendor ID: 0x%x\n", mvendorid());
	printf("RISCV Architecture ID: 0x%x (0:C-simulator 1:RTL-implemenation 2:RTL-simulation\n", marchid());
	printf("RISCV Implemenation ID (Version): 0x%x (v.%d.%d)\n", mimpid(), mimpid() >> 16, mimpid() & 0xffff);
    printf("RISCV Cores: %d\n", get_core_num());    
    if (marchid() >= 1) {
        printf("RISCV ICache: way=%d set=%d\n", hw_ver[ICACHE_WAY_NUM], hw_ver[ICACHE_SET_NUM]);
        printf("RISCV DCache: way=%d set=%d\n", hw_ver[DCACHE_WAY_NUM], hw_ver[DCACHE_SET_NUM]);
        printf("HLS Enable: %d\n", hw_ver[ENABLE_HLS]);    
    }
    printf("------------------------------------------------------------------------------\n");
    
#if HLS_CMDR
    //HLS initialization
    hls_init();
#endif
}
void riscv_exit()
{
#if NETFILE
    //Netfile exit
    nf_exit();
#endif
}
void riscv_writeback_dcache_all()
{
    volatile int * hw_ver = (int*)(HW_VERSION);
    const int way_num = hw_ver[DCACHE_WAY_NUM];
    const int set_num = hw_ver[DCACHE_SET_NUM];
    const int line_bytes = 32;        
    volatile uint8_t *src = 0;    
    int tmp = 0;
    //Read a dummy memory to writeback data in dcache
    for (int i = 0; i < way_num * set_num * line_bytes; i += line_bytes)
    {
        //edward 2024-07-10: use assemby
        uint8_t *ptr = (uint8_t*)&src[i];
        asm volatile("lb %0,0(%1)" : "=r"(tmp) : "r"(ptr));
    }
}
void riscv_writeback_dcache(uint8_t *adr)
{
#if BULLET_RISCV
    const int line_bytes = 32; 
    asm volatile ("lw zero,0(%0)"::"r"(adr));
    asm volatile ("lw zero,0(%0)"::"r"(adr + line_bytes));
#else
    volatile int * hw_ver = (int*)(HW_VERSION);
    const int way_num = hw_ver[DCACHE_WAY_NUM];
    const int set_num = hw_ver[DCACHE_SET_NUM];
    const int line_bytes = 32; 
    volatile uint8_t *src = 0;
    int tmp = 0;        
    int adr_i = (uint32_t)adr & (set_num * line_bytes - 1);
    //Read a same set to writeback data in dcache
    for (int i = 0; i < way_num; i++)
    {
        //edward 2024-07-10: use assemby
        volatile uint8_t *ptr = &src[adr_i + i * set_num * line_bytes];
        asm volatile("lb %0,0(%1)" : "=r"(tmp) : "r"(ptr));
    }
#endif
}
//Flush cache lines
void riscv_writeback_dcache_lines(const uint8_t *buf, int len)
{
    const int sets = 512;
    const int byte_per_line = 32;
#if BULLET_RISCV
    wait_mtdma_done(0);
    len += byte_per_line;
    //edward 2024-07-10: use hardware flush command for any length
    for (int i = 0; i < len; i += byte_per_line) {
        const uint8_t * addr = buf + i;
        asm volatile ("lw zero,0(%0)"::"r"(addr));
    }
#else
    //Assume 2 ways cache    
    const uint8_t * ptr0 = buf + (1 * 1024 * 1024);
    const uint8_t * ptr1 = buf + (2 * 1024 * 1024);
    int tmp = 0;    
    if (len > (sets * byte_per_line))
        len = sets * byte_per_line;
    for (int i = 0; i < (len + byte_per_line); i += byte_per_line)
    {            
        asm volatile("lb %0,0(%1)" : "=r"(tmp) : "r"(ptr0));
        asm volatile("lb %0,0(%1)" : "=r"(tmp) : "r"(ptr1));
        ptr0 += byte_per_line;
        ptr1 += byte_per_line;
    }
#endif
}

#if FUNCLOG
void __cyg_profile_func_enter(void *this_fn, void *call_site) {
    int hart_id = mhartid();
    //if (hart_id != 0) {
        printf("%d]c,%p,%p\n", hart_id, this_fn, call_site);
    //}
}

void __cyg_profile_func_exit(void *this_fn, void *call_site) {
    int hart_id = mhartid();
    //if (hart_id != 0) {
        printf("%d]r,%p,%p\n", hart_id, this_fn, call_site);
    //}
}
#endif

#ifdef __cplusplus
}
#endif
