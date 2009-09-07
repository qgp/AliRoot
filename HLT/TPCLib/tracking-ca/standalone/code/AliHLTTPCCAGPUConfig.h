#ifndef ALIHLTTPCCAGPUCONFIG_H
#define ALIHLTTPCCAGPUCONFIG_H

#define HLTCA_GPU_BLOCK_COUNT 30
#define HLTCA_GPU_THREAD_COUNT 256

#define HLTCA_GPU_WARP_SIZE 32
#define HLTCA_GPU_REGS 64
#define HLTCA_ROW_COUNT 159

#define HLTCA_GPU_ROWALIGNMENT int
#define HLTCA_GPU_ROWCOPY int
#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS 32
//#define HLTCA_GPU_PREFETCHDATA
#define HLTCA_GPU_PREFETCH_ROWBLOCK_ONLY
#define HLTCA_GPU_SORT_STARTHITS
//#define HLTCA_GPU_SORT_STARTHITS_2

#define HLTCA_GPU_SCHED_ROW_STEP 40
#define HLTCA_GPU_SCHED_FIXED_START
//#define HLTCA_GPU_SCHED_FIXED_SLICE
//#define HLTCA_GPU_SCHED_HOST_SYNC
#define HLTCA_GPU_RESCHED

//#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE

#define HLTCA_GPU_TRACKLET_SELECTOR_HITS_REG_SIZE 12

//#define HLTCA_GPU_SORT_DUMPDATA

#define HLTCA_GPU_MAX_TRACKLETS 20480					//Should be divisible by 16 at least

//#define HLTCA_GPU_EMULATION_SINGLE_TRACKLET 1313
//#define HLTCA_GPU_EMULATION_DEBUG_TRACKLET 1313

#define HLTCA_GPU_TRACKER_CONSTANT_MEM 65000

#ifndef HLTCA_GPU_RESCHED
#ifdef HLTCA_GPU_SCHED_HOST_SYNC
#undef HLTCA_GPU_SCHED_HOST_SYNC
#endif
#endif

#ifndef HLTCA_GPUCODE
#ifdef HLTCA_GPU_SORT_STARTHITS
#undef HLTCA_GPU_SORT_STARTHITS
#endif
#ifdef HLTCA_GPU_PREFETCHDATA
#undef HLTCA_GPU_PREFETCHDATA
#endif
#undef HLTCA_GPU_TRACKLET_SELECTOR_HITS_REG_SIZE
#define HLTCA_GPU_TRACKLET_SELECTOR_HITS_REG_SIZE 0
#endif

#if !defined(HLTCA_GPU_PREFETCHDATA) | !defined(HLTCA_GPU_RESCHED)
#undef HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS
#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS 0
#endif

#define HLTCA_GPU_ERROR_NONE 0
#define HLTCA_GPU_ERROR_ROWBLOCK_TRACKLET_OVERFLOW 1

#endif