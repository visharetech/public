#ifndef SYSCALL_H
#define SYSCALL_H


#ifdef __GNUC__
//Use always_inline to ensure the function is inlined
#define ALWAYS_INLINE __attribute__((always_inline, no_instrument_function)) inline
#elif defined(_MSC_VER)
#define ALWAYS_INLINE __forceinline
#else
#define ALWAYS_INLINE inline
#endif

#ifdef ENABLE_VU19P
	#define SYSTEM_FREQUENCY          100000000 //Hz
	#define SYSTEM_PERIOD             10        //ns
#else 
	#ifdef SYSCLK_100M
		#define SYSTEM_FREQUENCY          100000000 //Hz
		#define SYSTEM_PERIOD             10       //ns
	#else 
		#define SYSTEM_FREQUENCY          80000000 //Hz
		#define SYSTEM_PERIOD             14       //ns
	#endif 
#endif 

#define TRANSLATOR_CPU_CORE_NUM   8

//RISCV COREs
#define MAX_CORE_NUM              16
#define SCHED_CORE0               0x100
#define SCHED_CORE1               0x101
#define SCHED_CORE2               0x102
#define SCHED_CORE3               0x103
#define SCHED_CORE4               0x104
#define SCHED_CORE5               0x105
#define SCHED_CORE6               0x106
#define SCHED_CORE7               0x107

//CSR
#define CSR_THREAD_START          0x800
#define CSR_THREAD_PC             0x801
#define CSR_THREAD_STACKADDR      0x802
#define CSR_THREAD_STACKSIZE      0x803
#define CSR_THREAD_ARG            0x804
#define CSR_THREAD_SCHEDCORE      0x805
#define CSR_THREAD_PID            0x806
#define CSR_THREAD_DONE           0x807
#define CSR_HW_FUNC_START         0x808
#define CSR_HW_FUNC_ARG           0x809

#define CSR_INIT_KERNEL_PC             0x80a
#define CSR_KERNEL_GET_NEXT_THREAD     0x80b
#define CSR_KERNEL_GET_NEXT_PAGE       0x80c
#define CSR_MPR_USE_NEXT_PAGE          0x80d
#define CSR_KERNEL_ACTIVATE_THREAD     0x80e
#define CSR_THREAD_START_V2            0x80f
#define CSR_INIT_KERNEL_STACK_POINTER  0x810
#define CSR_KERNEL_IRP_STORE           0x811
#define CSR_KERNEL_IRP_LOAD            0x812
#define CSR_THREAD_JOIN                0x813
#define CSR_MUTEX_LOCK                 0x814
#define CSR_MUTEX_UNLOCK               0x815
#define CSR_THREAD_COND_UNLOCK_AND_WAIT  0x816
#define CSR_THREAD_COND_SIGNAL         0x817
#define CSR_THREAD_EXIT                0x81a
#define CSR_GET_SPECIAL_TRIGGER_REGISTER 0x81b
#define CSR_SET_SPECIAL_TRIGGER_REGISTER 0x81c
#define CSR_SET_BREAKPOINT               0x81d
#define CSR_CLEAR_BREAKPOINT             0x81e
#define CSR_GET_BREAKPOINT_INDEX         0x81f
#define CSR_SET_THREAD_SCHEDULE_COUNT    0x820

#define CSR_MCACHE_LOCK                  0x822
#define CSR_MCACHE_UNLOCK                0x823
#define CSR_MCACHE_CORES_WRITE_DATA      0x824
#define CSR_MCACHE_CORES_REGISTER        0x825

#define CSR_HW_FUNC_GET_TRIGGER          0x826
#define CSR_HW_FUNC_CLR_TRIGGER          0x827
#define CSR_HW_FUNC_SET_RESPONSE         0x828


#define CSR_TRACE_RESET                0x830
#define CSR_TRACE_SET_TRIGGER          0x831
#define CSR_TRACE_SET_DELAY            0x832
#define CSR_TRACE_SET_SIZE             0x833
#define CSR_TRACE_POP                  0x834

#define CSR_KERNEL_GET_NEXT_PRIORITY_THREAD  0x835
#define CSR_DISABLE_KERNEL_TIRRGER           0x836
#define CSR_COMMANDER_PC                     0x837
#define CSR_COMMANDER_STOP                   0x838
#define CSR_COMMANDER_INST                   0x839
#define CSR_COMMANDER_RETURN                 0x83a
#define CSR_COMMANDER_NUM                    0x83b

#define CSR_IOBUF_INIT                 0x840
#define CSR_IOBUF_TRIGGER              0x841
#define CSR_IOBUF_PUSH                 0x842
#define CSR_IOBUF_POP                  0x843
#define CSR_IOBUF_FULL                 0x844
#define CSR_IOBUF_EMPTY                0x845

#define CSR_SET_MPU            0x850
#define CSR_CLEAR_MPU          0x851
#define CSR_GET_MPU            0x852

//PROFILING (previous version)
#define CSR_PROFILE_FUNC_ENABLE          0x860
#define CSR_PROFILE_FUNC_SET_FUNC_ADDR   0x861
#define CSR_PROFILE_FUNC_GET_INFO        0x862
#define CSR_PROFILE_FUNC_SW_RST          0x863

//so add (2023-10-25)
#define CSR_PTREE_PROFILER_SET_PC_START		0x864
#define CSR_PTREE_PROFILER_SET_PC_MAX		0x865
#define CSR_PTREE_PROFILER_ADD_TREE_NODE	0x866
#define CSR_PTREE_PROFILER_ENABLE			0x867
#define CSR_PTREE_PROFILER_SW_RST			0x868
#define CSR_PTREE_PROFILER_FUNC_IDX			0x869
#define CSR_PTREE_PROFILE_FUNC_GET_INFO		0x870
#define CSR_PTREE_PROFILER_PUSH_LAST 		0x871	


//Call/return interface CSR
#define CSR_CALL_VLD       0x883
#define CSR_CALL_DAT       0x884

// Misc
#define CSR_PRINT_ACTIVE_THREADS_STATS 0x890
#define CSR_THREAD_SELF 0x891
#define CSR_X86_FUNC_CALL 0x892

//For translator (asim XMEM check)
#define CSR_XMEM_SIZE               0x8B0
#define CSR_XMEM_START              0x8B1
#define CSR_SIM_OPERATION           0x8B2
#define CSR_XMEM_SEARCH_RANGE       0x8B3       // csr_write(CSR_XMEM_SEARCH_RANGE, xmem_element_addr)  : it will check which xmem is being used from dynamic array (previous assigned from XMEM_START), then get back current xmem base address by read the CSR_XMEM_AT
#define CSR_XMEM_AT                 0x8B4       // csr_read(CSR_XMEM_AT, &xmem_base)                    : get back xmem_base address, please see CSR_XMEM_SEARCH_RANGE desc
#define CSR_startProfiling          0x8B5
#define CSR_MEM_WR_WATCH            0x8B6
#define CSR_MEM_RD_WATCH            0x8B7
#define CSR_MEMCMP_WATCH            0x8B8

#define CSR_SET_HLS_HANDLER         0x8C0       // csr_write(CSR_SET_HLS_HANDLER, hls_handler)          : set the HLS handler, ecall() will jump into this routine.
#define CSR_GET_APCALL_HLS_ID       0x8C1
#define CSR_GET_APCALL_ARG0         0x8C2       // csr_read(CSR_GET_APCALL_ARG0, &a0)                   : get apcall argument 0 value from longtail_wr2()
#define CSR_GET_APCALL_ARG1         0x8C3       // csr_read(CSR_GET_APCALL_ARG1, &a1)                   : get apcall argument 1 value from longtail_wr2()
#define CSR_GET_APCALL_ARG2         0x8C4       // csr_read(CSR_GET_APCALL_ARG2, &a2)                   : get apcall argument 2 value from longtail_wr2()
#define CSR_GET_APCALL_ARG3         0x8C5       // csr_read(CSR_GET_APCALL_ARG3, &a3)                   : get apcall argument 3 value from longtail_wr2()
#define CSR_GET_APCALL_ARG4         0x8C6       // csr_read(CSR_GET_APCALL_ARG4, &a4)                   : get apcall argument 4 value from longtail_wr2()
#define CSR_GET_APCALL_ARG5         0x8C7       // csr_read(CSR_GET_APCALL_ARG5, &a5)                   : get apcall argument 5 value from longtail_wr2()
#define CSR_GET_APCALL_ARG6         0x8C8       // csr_read(CSR_GET_APCALL_ARG6, &a6)                   : get apcall argument 6 value from longtail_wr2()
#define CSR_GET_APCALL_ARG7         0x8C9       // csr_read(CSR_GET_APCALL_ARG7, &a7)                   : get apcall argument 7 value from longtail_wr2()
#define CSR_SET_APRETURN            0x8CA       // csr_write(CSR_SET_APRETURN, ret)                     : set apcall ap_return
#define CSR_HEVC_CONTEXT            0x8CB       

// Use asim to measure the dcache read write count in HLS functions
#define CSR_PROFILE_DCACHE_SET      0x8CC
#define CSR_PROFILE_DCACHE_CLEAR    0x8CD

//Hardware function
#define HW_SUB4X4_DCT             0x0


// ----- SpecialTrigger -------------------------

#define TRACE_BUFFER_FULL            0x01
#define BREAKPOINT_TRIGGER           0x02

#define IOBUF_LARGER                 0x04
#define IOBUF_SMALLER                0x08
#define IOBUF_LEVEL0                 0x10
#define IOBUF_LEVEL1                 0x20
#define IOBUF_EDGE_RISING            0x40
#define IOBUF_EDGE_FALLING           0x80

#define INVALID_STACK_POINTER_ACCESS 0x100
#define INVALID_MEMORY_LOAD          0x200
#define INVALID_MEMORY_STORE         0x400
#define INVALID_PC_ACCESS            0x800

#define HW_CTRL_TRIGGER              0x1000

// ---------------------------------------------


// Index of the register in mpr which contains thread_id associated with that
// page.
#define REGISTER_PAGE_THREAD_REGISTER_INDEX   50
#define REGISTER_PAGE_PC_REGISTER_INDEX       32
#define REGISTER_PAGE_SP_LIMIT_REGISTER_INDEX 33

// 32 GPR + 1 PC + 1 m_mpu_sp_limit
#define REGISTER_PAGE_NUM_REGISTER_INDEX 34


#define GPR_SP   2
#define GPR_GP   3
#define GPR_RA   1
#define GPR_A0   10


#define ACCESS_TYPE_READ    0
#define ACCESS_TYPE_WRITE   1
#define ACCESS_TYPE_EXECUTE 2

#define REGION_TYPE_INST 0
#define REGION_TYPE_DATA 1

//Hardware profile parameters
#define PROFILE_DC_ACC_LCNT			0
#define PROFILE_DC_ACC_HCNT			1
#define PROFILE_DC_MISS_LCNT		2
#define PROFILE_DC_MISS_HCNT		3
#define PROFILE_DC_MISSING_LCNT		4
#define PROFILE_DC_MISSING_HCNT		5
#define PROFILE_IC_ACC_LCNT			6
#define PROFILE_IC_ACC_HCNT			7
#define PROFILE_IC_MISS_LCNT		8
#define PROFILE_IC_MISS_HCNT		9
#define PROFILE_IC_MISSING_LCNT		10
#define PROFILE_IC_MISSING_HCNT		11
#define PROFILE_INST_LCNT			12
#define PROFILE_INST_HCNT			13
#define PROFILE_CYCLE_LCNT			14
#define PROFILE_CYCLE_HCNT			15
#define PROFILE_CALL_LCNT			16
#define PROFILE_CALL_HCNT			17


//Function start/end
#define PROFILE_START_ADDR       0
#define PROFILE_END_ADDR         1

#ifndef _MSC_VER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t addr1;
    uint32_t addr2;
    uint32_t len;
} memcmp_watch_info_t;

#define MAX_PROF_DCACHE_ARG  6

typedef struct {
    int32_t hls_id;
    intptr_t dc_ptr[MAX_PROF_DCACHE_ARG];
    uint32_t size[MAX_PROF_DCACHE_ARG];
} dcache_info_t;


//Machine Vendor ID
static ALWAYS_INLINE uint32_t mvendorid(void)
{
  uint32_t id;
  asm volatile ("csrr %0, mvendorid" :"=r"(id));
  return id;
}

//Machine Architecture ID
static ALWAYS_INLINE uint32_t marchid(void)
{
  uint32_t id;
  asm volatile ("csrr %0, marchid" :"=r"(id));
  return id;
}

//Machine Implementation ID
static ALWAYS_INLINE uint32_t mimpid(void)
{
  uint32_t id;
  asm volatile ("csrr %0, mimpid" :"=r"(id));
  return id;
}

//Machine hart ID
static ALWAYS_INLINE uint32_t mhartid(void)
{
  uint32_t id;
  asm volatile ("csrr %0, mhartid" :"=r"(id));
  return id;
}

//Cycle count (32 bit)
static ALWAYS_INLINE uint32_t cycle32(void)
{
  uint32_t cyc;
  asm volatile ("csrr %0, cycle" :"=r"(cyc));
  return cyc;
}

//Cycle count H (32 bit)
static ALWAYS_INLINE uint32_t cycle32h(void)
{
  uint32_t cyc;
  asm volatile ("csrr %0, cycleh" :"=r"(cyc));
  return cyc;
}

//Cycle count (64 bit)
static ALWAYS_INLINE uint64_t cycle64(void)
{
  uint32_t cyc, cych;
  asm volatile ("csrr %0, cycle" :"=r"(cyc));
  asm volatile ("csrr %0, cycleh" :"=r"(cych));  
  return ((uint64_t)cych << 32) + cyc;
}

//instRet count (32 bit)
static ALWAYS_INLINE uint32_t instret32(void)
{
  uint32_t cnt;
  asm volatile ("csrr %0, instret" :"=r"(cnt));
  return cnt;
}

//MCycle count (32 bit)
static ALWAYS_INLINE uint32_t mcycle32(void)
{
  uint32_t cyc;
  asm volatile ("csrr %0, mcycle" :"=r"(cyc));
  return cyc;
}

//MCycle count H (32 bit)
static ALWAYS_INLINE uint32_t mcycle32h(void)
{
  uint32_t cyc;
  asm volatile ("csrr %0, mcycleh" :"=r"(cyc));
  return cyc;
}

//Cycle count (64 bit)
static ALWAYS_INLINE uint64_t mcycle64(void)
{
  uint32_t cyc, cych;
  asm volatile ("csrr %0, mcycle" :"=r"(cyc));
  asm volatile ("csrr %0, mcycleh" :"=r"(cych));  
  return ((uint64_t)cych << 32) + cyc;
}

//Request hardware function
static ALWAYS_INLINE int hw_func(int func, void * arg)
{
  int ret;
  //Argument
  asm volatile ("csrw %0, %1" : :"i"(CSR_HW_FUNC_ARG), "r"((uint32_t)arg));
  //Function ID
  asm volatile ("csrrw %0, %1, %2" :"=r"(ret) :"i"(CSR_HW_FUNC_START), "r"(func));
  return 0;
}

static ALWAYS_INLINE void trace_reset() {
  asm volatile ("csrw %0, zero" :: "i"(CSR_TRACE_RESET));
}

static ALWAYS_INLINE void trace_set_trigger(uint32_t pc) {
  asm volatile ("csrw %0, %1" :: "i"(CSR_TRACE_SET_TRIGGER), "r"(pc));
}

static ALWAYS_INLINE void trace_set_delay(int delay) {
  asm volatile ("csrw %0, %1" :: "i"(CSR_TRACE_SET_DELAY), "r"(delay));
}

static ALWAYS_INLINE void trace_set_size(int size) {
  asm volatile ("csrw %0, %1" :: "i"(CSR_TRACE_SET_SIZE), "r"(size));
}

static ALWAYS_INLINE void iobuf_init(uint32_t buf_index, int dir, int size) {
  register int size_ asm("a2") = size;
  asm volatile ("csrw %0, %1" :: "i"(CSR_IOBUF_INIT), "r"(buf_index), "r"(size_));
}

static ALWAYS_INLINE void iobuf_trigger(uint32_t buf_index, int trigger_type, int threshold) {
  register int trigger_type_ asm("a1") = trigger_type;
  register int threshold_ asm("a2") = threshold;
  asm volatile ("csrw %0, %1" :: "i"(CSR_IOBUF_TRIGGER), "r"(buf_index),
                                 "r"(trigger_type_), "r"(threshold_));
}

static ALWAYS_INLINE void iobuf_push(uint32_t buf_index, uint8_t data) {
  register uint32_t data_ asm("a1") = data;
  asm volatile ("csrw %0, %1" :: "i"(CSR_IOBUF_PUSH),
                                 "r"(buf_index), "r"(data_));
}

// Returns 1 if success. In that case *data contains pop'ed value.
// Returns 0 if the buffer was empty.
static ALWAYS_INLINE int iobuf_pop(uint32_t buf_index, uint8_t* data) {
  uint32_t output;
  asm volatile ("csrrw %0, %1, %2" : "=r"(output)
                                   : "i"(CSR_IOBUF_POP), "r"(buf_index));
  *data = output & 0xff;
  return ((output >> 16) & 0x1);
}

// Returns 1 if buffer is full. 0 otherwise.
static ALWAYS_INLINE int iobuf_full(uint32_t buf_index) {
  int output;
  asm volatile ("csrrw %0, %1, %2" : "=r"(output)
                              : "i"(CSR_IOBUF_FULL), "r"(buf_index));
  return output;
}

// Returns 1 if buffer is empty. 0 otherwise.
static ALWAYS_INLINE int iobuf_empty(uint32_t buf_index) {
  int output;
  asm volatile ("csrrw %0, %1, %2" : "=r"(output)
                              : "i"(CSR_IOBUF_EMPTY), "r"(buf_index));
  return output;
}

// @start_addr - inclusive.
// @end_addr   - exclusive.
static ALWAYS_INLINE void set_mpu(int region_index, uint32_t access_type,
                    uint32_t start_addr, uint32_t end_addr) {
  register uint32_t access_type_ asm("a1") = access_type;
  register uint32_t start_addr_ asm("a2") = start_addr;
  register uint32_t end_addr_ asm("a3") = end_addr;
  asm volatile ("csrw %0, %1" :: "i"(CSR_SET_MPU), "r"(region_index),
                                  "r"(access_type_), "r"(start_addr_),
                                  "r"(end_addr_));
}

static ALWAYS_INLINE void clear_mpu(int region_index, uint32_t region_type) {
  register uint32_t region_type_ asm("a1") = region_type;
  asm volatile ("csrw %0, %1" :: "i"(CSR_CLEAR_MPU), "r"(region_index),
                                  "r"(region_type_));
}

static ALWAYS_INLINE void get_mpu(int region_index, uint32_t region_type,
                    uint32_t* access_type, uint32_t* start_addr,
                    uint32_t* end_addr) {
  register uint32_t region_type_ asm("a1") = region_type;
  register uint32_t start_addr_ asm("a5");
  register uint32_t end_addr_ asm("a6");
  asm volatile ("csrrw %0, %3, %4" : "=r"(*access_type), "=r"(start_addr_),
                                     "=r"(end_addr_)
                                   : "i"(CSR_GET_MPU), "r"(region_index),
                                     "r"(region_type_));
  *start_addr = start_addr_;
  *end_addr = end_addr_;
}

#ifdef BULLET_RISCV
//in libcrt_bullet.a
int get_system_core_number();
#endif

static ALWAYS_INLINE uint32_t get_core_num(void) {
#if _rvTranslate
    return TRANSLATOR_CPU_CORE_NUM;
#elif defined(BULLET_RISCV)
    return get_system_core_number();
#else
    int core = mhartid();
    int resp;
    uint32_t data;
    //Mutex lock
    do {
        asm volatile("csrrw %0, %1, %2" : "=r"(resp) : "i"(CSR_MCACHE_LOCK), "r"(core));
    } while (resp != 1);
    //MCACHE_CORE_NUM (0)
    asm volatile("csrrw %0, %1, %2" : "=r"(data) : "i"(CSR_MCACHE_CORES_REGISTER), "r"(core | (0 << 16)));
    //Mutex unlock
    asm volatile("csrrw %0, %1, %2" : "=r"(resp) : "i"(CSR_MCACHE_UNLOCK), "r"(core));
    return data;
#endif
}

static ALWAYS_INLINE void asim_startProfiling() {
#if _rvTranslate
    asm volatile ("csrw %0, %1" :: "i"(CSR_startProfiling), "r"(1));
#endif
}

static ALWAYS_INLINE void asim_notify_xmem_size(unsigned int size) {
#if _rvTranslate
    asm volatile ("csrw %0, %1" :: "i"(CSR_XMEM_SIZE), "r"(size));
#endif
}

// Function to notify the XMEM start
static ALWAYS_INLINE void asim_notify_xmem_start(void *start_address) {
#if _rvTranslate
    asm volatile ("csrw %0, %1" :: "i"(CSR_XMEM_START), "r"(start_address));
#endif
}

// Function to perform range search to find the xmem address
static ALWAYS_INLINE void asim_xmem_search_range(const void *xmem_element_address) {
#if ASIM_CALL 
    asm volatile ("csrw %0, %1" :: "i"(CSR_XMEM_SEARCH_RANGE), "r"(xmem_element_address));
#endif
}

// Function to add the memory write watch during write to memory (SW, SB..etc) for debugging
static ALWAYS_INLINE void asim_add_wr_mem_watch(const void *mem_addr) {
#if ASIM_CALL 
    asm volatile ("csrw %0, %1" :: "i"(CSR_MEM_WR_WATCH), "r"(mem_addr));
#endif
}

// Function to add the memory read watch during read from memory (LW, LB..etc) for debugging
static ALWAYS_INLINE void asim_add_rd_mem_watch(const void *mem_addr) {
#if ASIM_CALL 
    asm volatile ("csrw %0, %1" :: "i"(CSR_MEM_RD_WATCH), "r"(mem_addr));
#endif
}

// Function to add the memory compare watch during write to memory (SW, SB..etc) for debugging
static ALWAYS_INLINE void asim_add_memcmp_watch(const memcmp_watch_info_t *param) {
#if ASIM_CALL 
    asm volatile ("csrw %0, %1" :: "i"(CSR_MEMCMP_WATCH), "r"(param));
#endif
}

// Function to perform range search to find the xmem address
static ALWAYS_INLINE void *asim_xmem_at(void) {
#if ASIM_CALL 
  void *xmem_addr;
  asm volatile ("csrr %0, %1" :"=r"(xmem_addr) : "i"(CSR_XMEM_AT));
  return xmem_addr;
#else
  return NULL;
#endif
}

static ALWAYS_INLINE void asim_set_hls_handler(void (*hls_handler)(void)) {
#if ASIM_CALL 
    asm volatile ("csrw %0, %1" :: "i"(CSR_SET_HLS_HANDLER), "r"(hls_handler));
#endif
}

static ALWAYS_INLINE int asim_get_apcall_hls_id(void) {
#if ASIM_CALL 
  int hls_id;
  asm volatile ("csrr %0, %1" :"=r"(hls_id) : "i"(CSR_GET_APCALL_HLS_ID));
  return hls_id;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg0(void) {
#if ASIM_CALL 
  int a0;
  asm volatile ("csrr %0, %1" :"=r"(a0) : "i"(CSR_GET_APCALL_ARG0));
  return a0;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg1(void) {
#if ASIM_CALL 
  int a1;
  asm volatile ("csrr %0, %1" :"=r"(a1) : "i"(CSR_GET_APCALL_ARG1));
  return a1;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg2(void) {
#if ASIM_CALL 
  int a2;
  asm volatile ("csrr %0, %1" :"=r"(a2) : "i"(CSR_GET_APCALL_ARG2));
  return a2;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg3(void) {
#if ASIM_CALL 
  int a3;
  asm volatile ("csrr %0, %1" :"=r"(a3) : "i"(CSR_GET_APCALL_ARG3));
  return a3;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg4(void) {
#if ASIM_CALL 
  int a4;
  asm volatile ("csrr %0, %1" :"=r"(a4) : "i"(CSR_GET_APCALL_ARG4));
  return a4;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg5(void) {
#if ASIM_CALL 
  int a5;
  asm volatile ("csrr %0, %1" :"=r"(a5) : "i"(CSR_GET_APCALL_ARG5));
  return a5;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg6(void) {
#if ASIM_CALL 
  int a6;
  asm volatile ("csrr %0, %1" :"=r"(a6) : "i"(CSR_GET_APCALL_ARG6));
  return a6;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE int asim_get_apcall_arg7(void) {
#if ASIM_CALL 
  int a7;
  asm volatile ("csrr %0, %1" :"=r"(a7) : "i"(CSR_GET_APCALL_ARG7));
  return a7;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE void asim_set_apreturn(int value) {
#if ASIM_CALL 
    asm volatile ("csrw %0, %1" :: "i"(CSR_SET_APRETURN), "r"(value));
#endif
}

static ALWAYS_INLINE void asim_set_hevc_context(void* context) {
#if ASIM_CALL
    asm volatile ("csrw %0, %1" :: "i"(CSR_HEVC_CONTEXT), "r"(context));
#endif
}

static ALWAYS_INLINE int asim_get_hevc_context(void) {
#if ASIM_CALL 
  int a7;
  asm volatile ("csrr %0, %1" :"=r"(a7) : "i"(CSR_HEVC_CONTEXT));
  return a7;
#else
  return 0xdeadc0de;
#endif
}

static ALWAYS_INLINE void asim_profile_dcache_set(dcache_info_t *dcache_info){
#if ASIM_CALL
    asm volatile ("csrw %0, %1" :: "i"(CSR_PROFILE_DCACHE_SET), "r"(dcache_info));
#endif
}

static ALWAYS_INLINE void asim_profile_dcache_clear(void){
#if ASIM_CALL
    asm volatile ("csrw %0, %1" :: "i"(CSR_PROFILE_DCACHE_CLEAR), "i"(0x00));
#endif
}



//CSR: Enable disable HW profiling
/*
static inline void hw_prof_set_enable(uint8_t func_idx, uint8_t ena) {
    asm volatile("csrw %0, %1" : : "i"( CSR_PROFILE_FUNC_ENABLE ), "r"( func_idx << 8 | ena) );
}
*/
//CSR: Set hardware profiling function start/end PC
/*
static inline void hw_prof_set_addr(uint8_t func_idx, uint8_t start_or_end, uint32_t addr) {
    asm volatile("mv a0, %0; csrw %1, %2" : : "r" ( addr ), "i" ( CSR_PROFILE_FUNC_SET_FUNC_ADDR ), "r" ( func_idx << 8 | start_or_end ) : "a0");
}
*/
//CSR: Get profiling result
/*
static inline uint32_t hw_prof_get_info(uint8_t func_idx, uint8_t param_idx) {
    uint32_t result_value;
    asm volatile("csrrw %0, %1, %2" : "=r"( result_value ) : "i" ( CSR_PROFILE_FUNC_GET_INFO ), "r" ( func_idx << 8 | param_idx ));
    return result_value;

}
*/
//CSR: Reset profiling
/*
static inline void hw_prof_reset(uint8_t func_idx) {
    asm volatile("csrw %0, %1" : : "i"( CSR_PROFILE_FUNC_SW_RST ), "r"( func_idx << 8 ) );
}
*/












int get_thread_id(void);
int get_thread_pc(int thread);

#ifdef __cplusplus
}
#endif

#endif
#endif