#ifndef ALIHLTTPCCAGPUCONFIG_H
#define ALIHLTTPCCAGPUCONFIG_H

#define HLTCA_GPU_BLOCK_COUNT 30
#define HLTCA_GPU_THREAD_COUNT 256

#define HLTCA_GPU_WARP_SIZE 32
#define HLTCA_GPU_REGS 64
#define HLTCA_ROW_COUNT 159

#define HLTCA_GPU_ROWALIGNMENT uint4
#define HLTCA_GPU_ROWCOPY int
#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS 32
//#define HLTCA_GPU_PREFETCHDATA
//#define HLTCA_GPU_PREFETCH_ROWBLOCK_ONLY

#define HLTCA_GPU_SCHED_ROW_STEP 32
#define HLTCA_GPU_SCHED_FIXED_START
//#define HLTCA_GPU_SCHED_FIXED_SLICE
#define HLTCA_GPU_RESCHED

//#define HLTCA_GPU_TEXTURE_FETCH

//#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE

#define HLTCA_GPU_TRACKLET_SELECTOR_HITS_REG_SIZE 12
#define HLTCA_GPU_TRACKLET_SELECTOR_SLICE_COUNT 2		//Currently must be smaller than avaiable MultiProcessors on GPU or will result in wrong results

//#define HLTCA_GPU_SORT_DUMPDATA

#define HLTCA_GPU_MAX_TRACKLETS 10240					//Should be divisible by 16 at least
#define HLTCA_GPU_MAX_TRACKS 2048


//#define HLTCA_GPU_EMULATION_SINGLE_TRACKLET 1313
//#define HLTCA_GPU_EMULATION_DEBUG_TRACKLET 1313

#define HLTCA_GPU_TRACKER_CONSTANT_MEM 65000

#define HLTCA_GPU_TRACKER_OBJECT_MEMORY 1024 * 1024
#define HLTCA_GPU_ROWS_MEMORY 1024 * 1024
#define HLTCA_GPU_COMMON_MEMORY 1024 * 1024
#define HLTCA_GPU_SLICE_DATA_MEMORY 6 * 1024 * 1024
#define HLTCA_GPU_GLOBAL_MEMORY 16 * 1024 * 1024
#define HLTCA_GPU_TRACKS_MEMORY 2 * 1024 * 1024

#ifndef HLTCA_GPUCODE
#ifdef HLTCA_GPU_TEXTURE_FETCH
#undef HLTCA_GPU_TEXTURE_FETCH
#endif

#ifdef HLTCA_GPU_PREFETCHDATA
#undef HLTCA_GPU_PREFETCHDATA
#endif

#undef HLTCA_GPU_TRACKLET_SELECTOR_HITS_REG_SIZE
#define HLTCA_GPU_TRACKLET_SELECTOR_HITS_REG_SIZE 0

#else
#define HLTCA_GPU_SORT_STARTHITS
#endif

#if !defined(HLTCA_GPU_PREFETCHDATA) | !defined(HLTCA_GPU_RESCHED)
#undef HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS
#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS 0
#endif

#define HLTCA_GPU_ERROR_NONE 0
#define HLTCA_GPU_ERROR_ROWBLOCK_TRACKLET_OVERFLOW 1
#define HLTCA_GPU_ERROR_TRACKLET_OVERFLOW 2
#define HLTCA_GPU_ERROR_TRACK_OVERFLOW 3
#define HLTCA_GPU_ERROR_SCHEDULE_COLLISION 4

#endif