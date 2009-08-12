#ifndef ALIHLTTPCCAGPUCONFIG_H
#define ALIHLTTPCCAGPUCONFIG_H

#define HLTCA_GPU_BLOCK_COUNT 30
#define HLTCA_GPU_THREAD_COUNT 256
#define HLTCA_GPU_WARP_SIZE 32
#define HLTCA_GPU_REGS 64
#define HLTCA_GPU_ROWALIGNMENT uint4
#define HLTCA_GPU_ROWCOPY int
#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS 32
//#define HLTCA_GPU_REORDERHITDATA
//#define HLTCA_GPU_PREFETCHDATA
#define HLTCA_GPU_SORT_STARTHITS
//#define HLTCA_GPU_SORT_STARTHITS_2
#define HLTCA_GPU_SCHED_ROW_STEP 20

//#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE

//#define HLTCA_GPU_SORT_DUMPDATA

#define HLTCA_GPU_MAX_TRACKLETS 65536

#ifndef HLTCA_GPUCODE
#ifdef HLTCA_GPU_SORT_STARTHITS
#undef HLTCA_GPU_SORT_STARTHITS
#endif
#ifdef HLTCA_GPU_PREFETCHDATA
#undef HLTCA_GPU_PREFETCHDATA
#endif
#endif

#ifndef HLTCA_GPU_PREFETCHDATA
#undef HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS
#define HLTCA_GPU_TRACKLET_CONSTRUCTOR_NMEMTHREDS 0
#endif
#endif