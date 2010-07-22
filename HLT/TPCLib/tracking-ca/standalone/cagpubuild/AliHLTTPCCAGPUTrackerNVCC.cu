// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//					David Rohr <drohr@kip.uni-heidelberg.de>				*
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include "AliHLTTPCCAGPUTrackerNVCC.h"

#ifdef HLTCA_GPUCODE
#include <cuda.h>
#include <sm_11_atomic_functions.h>
#include <sm_12_atomic_functions.h>
#endif

#ifdef R__WIN32
#else
#include <sys/syscall.h>
#include <semaphore.h>
#include <fcntl.h>
#endif

#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCAGPUConfig.h"


#include <iostream>

//Disable assertions since they produce errors in GPU Code
#ifdef assert
#undef assert
#endif
#define assert(param)

__constant__ float4 gAliHLTTPCCATracker[HLTCA_GPU_TRACKER_CONSTANT_MEM / sizeof( float4 )];
#ifdef HLTCA_GPU_TEXTURE_FETCH
texture<ushort2, 1, cudaReadModeElementType> gAliTexRefu2;
texture<unsigned short, 1, cudaReadModeElementType> gAliTexRefu;
texture<signed short, 1, cudaReadModeElementType> gAliTexRefs;
#endif

//Include CXX Files, GPUd() macro will then produce CUDA device code out of the tracker source code
#include "AliHLTTPCCATrackParam.cxx"
#include "AliHLTTPCCATrack.cxx" 

#include "AliHLTTPCCAHitArea.cxx"
#include "AliHLTTPCCAGrid.cxx"
#include "AliHLTTPCCARow.cxx"
#include "AliHLTTPCCAParam.cxx"
#include "AliHLTTPCCATracker.cxx"

#include "AliHLTTPCCAProcess.h"

#include "AliHLTTPCCATrackletSelector.cxx"
#include "AliHLTTPCCANeighboursFinder.cxx"
#include "AliHLTTPCCANeighboursCleaner.cxx"
#include "AliHLTTPCCAStartHitsFinder.cxx"
#include "AliHLTTPCCAStartHitsSorter.cxx"
#include "AliHLTTPCCATrackletConstructor.cxx"

#include "MemoryAssignmentHelpers.h"

#ifndef HLTCA_STANDALONE
#include "AliHLTDefinitions.h"
#include "AliHLTSystem.h"
#endif

ClassImp( AliHLTTPCCAGPUTrackerNVCC )

#define SemLockName "AliceHLTTPCCAGPUTrackerInitLockSem"

AliHLTTPCCAGPUTrackerNVCC::AliHLTTPCCAGPUTrackerNVCC() :
	fGpuTracker(NULL),
	fGPUMemory(NULL),
	fHostLockedMemory(NULL),
	fDebugLevel(0),
	fDebugMask(0xFFFFFFFF),
	fOutFile(NULL),
	fGPUMemSize(0),
	fpCudaStreams(NULL),
	fSliceCount(HLTCA_GPU_DEFAULT_MAX_SLICE_COUNT),
	fOutputControl(NULL),
	fCudaInitialized(0),
	fPPMode(0),
	fCudaContext()
	{};

AliHLTTPCCAGPUTrackerNVCC::~AliHLTTPCCAGPUTrackerNVCC() {};

void AliHLTTPCCAGPUTrackerNVCC::ReleaseGlobalLock(void* sem)
{
	//Release the global named semaphore that locks GPU Initialization
#ifdef R__WIN32
	HANDLE* h = (HANDLE*) sem;
	ReleaseSemaphore(*h, 1, NULL);
	CloseHandle(*h);
	delete h;
#else
	sem_t* pSem = (sem_t*) sem;
	sem_post(pSem);
	sem_unlink(SemLockName);
#endif
}

int AliHLTTPCCAGPUTrackerNVCC::CheckMemorySizes(int sliceCount)
{
	//Check constants for correct memory sizes
  if (sizeof(AliHLTTPCCATracker) * sliceCount > HLTCA_GPU_TRACKER_OBJECT_MEMORY)
  {
	  HLTError("Insufficiant Tracker Object Memory for %d slices", sliceCount);
	  return(1);
  }

  if (fgkNSlices * AliHLTTPCCATracker::CommonMemorySize() > HLTCA_GPU_COMMON_MEMORY)
  {
	  HLTError("Insufficiant Common Memory");
	  return(1);
  }

  if (fgkNSlices * (HLTCA_ROW_COUNT + 1) * sizeof(AliHLTTPCCARow) > HLTCA_GPU_ROWS_MEMORY)
  {
	  HLTError("Insufficiant Row Memory");
	  return(1);
  }

  if (fDebugLevel >= 3)
  {
	  HLTInfo("Memory usage: Tracker Object %d / %d, Common Memory %d / %d, Row Memory %d / %d", sizeof(AliHLTTPCCATracker) * sliceCount, HLTCA_GPU_TRACKER_OBJECT_MEMORY, fgkNSlices * AliHLTTPCCATracker::CommonMemorySize(), HLTCA_GPU_COMMON_MEMORY, fgkNSlices * (HLTCA_ROW_COUNT + 1) * sizeof(AliHLTTPCCARow), HLTCA_GPU_ROWS_MEMORY);
  }
  return(0);
}

int AliHLTTPCCAGPUTrackerNVCC::InitGPU(int sliceCount, int forceDeviceID)
{
	//Find best CUDA device, initialize and allocate memory

	if (sliceCount == -1) sliceCount = fSliceCount;

	if (CheckMemorySizes(sliceCount)) return(1);

#ifdef R__WIN32
	HANDLE* semLock = new HANDLE;
	*semLock = CreateSemaphore(NULL, 1, 1, SemLockName);
	if (*semLock == NULL)
	{
		HLTError("Error creating GPUInit Semaphore");
		return(1);
	}
	WaitForSingleObject(*semLock, INFINITE);
#else
	sem_t* semLock = sem_open(SemLockName, O_CREAT, 0x01B6, 1);
	if (semLock == SEM_FAILED)
	{
		HLTError("Error creating GPUInit Semaphore");
		return(1);
	}
	timespec semtime;
	clock_gettime(CLOCK_REALTIME, &semtime);
	semtime.tv_sec += 10;
	while (sem_timedwait(semLock, &semtime) != 0)
	{
		HLTError("Global Lock for GPU initialisation was not released for 10 seconds, assuming another thread died");
		HLTWarning("Resetting the global lock");
		sem_post(semLock);
	}
#endif

	fThreadId = GetThread();

	cudaDeviceProp fCudaDeviceProp;

	fGPUMemSize = HLTCA_GPU_ROWS_MEMORY + HLTCA_GPU_COMMON_MEMORY + sliceCount * (HLTCA_GPU_SLICE_DATA_MEMORY + HLTCA_GPU_GLOBAL_MEMORY);

#ifndef CUDA_DEVICE_EMULATION
	int count, bestDevice = -1;
	long long int bestDeviceSpeed = 0, deviceSpeed;
	if (CudaFailedMsg(cudaGetDeviceCount(&count)))
	{
		HLTError("Error getting CUDA Device Count");
		ReleaseGlobalLock(semLock);
		return(1);
	}
	if (fDebugLevel >= 2) HLTInfo("Available CUDA devices:");
	for (int i = 0;i < count;i++)
	{
		if (fDebugLevel >= 4) printf("Examining device %d\n", i);
		unsigned int free, total;
		cuInit(0);
		CUdevice tmpDevice;
		cuDeviceGet(&tmpDevice, i);
		CUcontext tmpContext;
		cuCtxCreate(&tmpContext, 0, tmpDevice);
		if(cuMemGetInfo(&free, &total)) std::cout << "Error\n";
		cuCtxDestroy(tmpContext);
		if (fDebugLevel >= 4) printf("Obtained current memory usage for device %d\n", i);
		if (CudaFailedMsg(cudaGetDeviceProperties(&fCudaDeviceProp, i))) continue;
		if (fDebugLevel >= 4) printf("Obtained device properties for device %d\n", i);
		int deviceOK = sliceCount <= fCudaDeviceProp.multiProcessorCount && fCudaDeviceProp.major < 9 && !(fCudaDeviceProp.major < 1 || (fCudaDeviceProp.major == 1 && fCudaDeviceProp.minor < 2)) && free >= fGPUMemSize;

		if (fDebugLevel >= 2) HLTInfo("%s%2d: %s (Rev: %d.%d - Mem Avail %d / %lld)%s", deviceOK ? " " : "[", i, fCudaDeviceProp.name, fCudaDeviceProp.major, fCudaDeviceProp.minor, free, (long long int) fCudaDeviceProp.totalGlobalMem, deviceOK ? "" : " ]");
		deviceSpeed = (long long int) fCudaDeviceProp.multiProcessorCount * (long long int) fCudaDeviceProp.clockRate * (long long int) fCudaDeviceProp.warpSize * (long long int) free * (long long int) fCudaDeviceProp.major * (long long int) fCudaDeviceProp.major;
		if (deviceOK && deviceSpeed > bestDeviceSpeed)
		{
			bestDevice = i;
			bestDeviceSpeed = deviceSpeed;
		}
	}
	if (bestDevice == -1)
	{
		HLTWarning("No %sCUDA Device available, aborting CUDA Initialisation", count ? "appropriate " : "");
		HLTInfo("Requiring Revision 1.3, Mem: %d, Multiprocessors: %d", fGPUMemSize, sliceCount);
		ReleaseGlobalLock(semLock);
		return(1);
	}

  if (forceDeviceID == -1)
	  fCudaDevice = bestDevice;
  else
	  fCudaDevice = forceDeviceID;
#else
	fCudaDevice = 0;
#endif

  cudaGetDeviceProperties(&fCudaDeviceProp ,fCudaDevice ); 

  if (fDebugLevel >= 1)
  {
	  HLTInfo("Using CUDA Device %s with Properties:", fCudaDeviceProp.name);
	  HLTInfo("totalGlobalMem = %lld", (unsigned long long int) fCudaDeviceProp.totalGlobalMem);
	  HLTInfo("sharedMemPerBlock = %lld", (unsigned long long int) fCudaDeviceProp.sharedMemPerBlock);
	  HLTInfo("regsPerBlock = %d", fCudaDeviceProp.regsPerBlock);
	  HLTInfo("warpSize = %d", fCudaDeviceProp.warpSize);
	  HLTInfo("memPitch = %lld", (unsigned long long int) fCudaDeviceProp.memPitch);
	  HLTInfo("maxThreadsPerBlock = %d", fCudaDeviceProp.maxThreadsPerBlock);
	  HLTInfo("maxThreadsDim = %d %d %d", fCudaDeviceProp.maxThreadsDim[0], fCudaDeviceProp.maxThreadsDim[1], fCudaDeviceProp.maxThreadsDim[2]);
	  HLTInfo("maxGridSize = %d %d %d", fCudaDeviceProp.maxGridSize[0], fCudaDeviceProp.maxGridSize[1], fCudaDeviceProp.maxGridSize[2]);
	  HLTInfo("totalConstMem = %lld", (unsigned long long int) fCudaDeviceProp.totalConstMem);
	  HLTInfo("major = %d", fCudaDeviceProp.major);
	  HLTInfo("minor = %d", fCudaDeviceProp.minor);
	  HLTInfo("clockRate = %d", fCudaDeviceProp.clockRate);
	  HLTInfo("multiProcessorCount = %d", fCudaDeviceProp.multiProcessorCount);
	  HLTInfo("textureAlignment = %lld", (unsigned long long int) fCudaDeviceProp.textureAlignment);
  }

  if (fCudaDeviceProp.major < 1 || (fCudaDeviceProp.major == 1 && fCudaDeviceProp.minor < 2))
  {
	HLTError( "Unsupported CUDA Device" );
	ReleaseGlobalLock(semLock);
	return(1);
  }

  if (cuCtxCreate(&fCudaContext, CU_CTX_SCHED_AUTO, fCudaDevice) != CUDA_SUCCESS)
  {
	  HLTError("Could not set CUDA Device!");
	  ReleaseGlobalLock(semLock);
	  return(1);
  }

  if (fGPUMemSize > fCudaDeviceProp.totalGlobalMem || CudaFailedMsg(cudaMalloc(&fGPUMemory, (size_t) fGPUMemSize)))
  {
	  HLTError("CUDA Memory Allocation Error");
	  cudaThreadExit();
	  ReleaseGlobalLock(semLock);
	  return(1);
  }
  ReleaseGlobalLock(semLock);
  if (fDebugLevel >= 1) HLTInfo("GPU Memory used: %d", (int) fGPUMemSize);
  int hostMemSize = HLTCA_GPU_ROWS_MEMORY + HLTCA_GPU_COMMON_MEMORY + sliceCount * (HLTCA_GPU_SLICE_DATA_MEMORY + HLTCA_GPU_TRACKS_MEMORY) + HLTCA_GPU_TRACKER_OBJECT_MEMORY;
  if (CudaFailedMsg(cudaMallocHost(&fHostLockedMemory, hostMemSize)))
  {
	  cudaFree(fGPUMemory);
	  cudaThreadExit();
	  HLTError("Error allocating Page Locked Host Memory");
	  return(1);
  }
  if (fDebugLevel >= 1) HLTInfo("Host Memory used: %d", hostMemSize);

  if (fDebugLevel >= 1)
  {
	  CudaFailedMsg(cudaMemset(fGPUMemory, 143, (size_t) fGPUMemSize));
  }

  fSliceCount = sliceCount;
  //Don't run constructor / destructor here, this will be just local memcopy of Tracker in GPU Memory
  fGpuTracker = (AliHLTTPCCATracker*) TrackerMemory(fHostLockedMemory, 0);

  for (int i = 0;i < fgkNSlices;i++)
  {
    fSlaveTrackers[i].SetGPUTracker();
	fSlaveTrackers[i].SetGPUTrackerCommonMemory((char*) CommonMemory(fHostLockedMemory, i));
	fSlaveTrackers[i].SetGPUSliceDataMemory(SliceDataMemory(fHostLockedMemory, i), RowMemory(fHostLockedMemory, i));
  }

  fpCudaStreams = malloc(CAMath::Max(3, fSliceCount) * sizeof(cudaStream_t));
  cudaStream_t* const cudaStreams = (cudaStream_t*) fpCudaStreams;
  for (int i = 0;i < CAMath::Max(3, fSliceCount);i++)
  {
	if (CudaFailedMsg(cudaStreamCreate(&cudaStreams[i])))
	{
	    cudaFree(fGPUMemory);
	    cudaFreeHost(fHostLockedMemory);
	    cudaThreadExit();
	    HLTError("Error creating CUDA Stream");
	    return(1);
	}
  }

  cuCtxPopCurrent(&fCudaContext);
  fCudaInitialized = 1;
  HLTImportant("CUDA Initialisation successfull (Device %d: %s, Thread %d, Max slices: %d)", fCudaDevice, fCudaDeviceProp.name, fThreadId, fSliceCount);

#if defined(HLTCA_STANDALONE) & !defined(CUDA_DEVICE_EMULATION)
  if (fDebugLevel < 2)
  {
	  //Do one initial run for Benchmark reasons
	  const int useDebugLevel = fDebugLevel;
	  fDebugLevel = 0;
	  AliHLTTPCCAClusterData* tmpCluster = new AliHLTTPCCAClusterData[sliceCount];

	  std::ifstream fin;

	  AliHLTTPCCAParam tmpParam;
	  AliHLTTPCCASliceOutput::outputControlStruct tmpOutputControl;

	  fin.open("events/settings.dump");
	  int tmpCount;
	  fin >> tmpCount;
	  for (int i = 0;i < sliceCount;i++)
	  {
		fSlaveTrackers[i].SetOutputControl(&tmpOutputControl);
		tmpParam.ReadSettings(fin);
		InitializeSliceParam(i, tmpParam);
	  }
	  fin.close();

	  fin.open("eventspbpbc/event.0.dump", std::ifstream::binary);
	  for (int i = 0;i < sliceCount;i++)
	  {
		tmpCluster[i].StartReading(i, 0);
		tmpCluster[i].ReadEvent(fin);
		tmpCluster[i].FinishReading();
	  }
	  fin.close();

	  AliHLTTPCCASliceOutput **tmpOutput = new AliHLTTPCCASliceOutput*[sliceCount];
	  memset(tmpOutput, 0, sliceCount * sizeof(AliHLTTPCCASliceOutput*));

	  Reconstruct(tmpOutput, tmpCluster, 0, sliceCount);
	  for (int i = 0;i < sliceCount;i++)
	  {
		  free(tmpOutput[i]);
		  tmpOutput[i] = NULL;
	  	  fSlaveTrackers[i].SetOutputControl(NULL);
	  }
	  delete[] tmpOutput;
	  delete[] tmpCluster;
	  fDebugLevel = useDebugLevel;
  }
#endif
  return(0);
}

template <class T> inline T* AliHLTTPCCAGPUTrackerNVCC::alignPointer(T* ptr, int alignment)
{
	//Macro to align Pointers.
	//Will align to start at 1 MB segments, this should be consistent with every alignment in the tracker
	//(As long as every single data structure is <= 1 MB)

	size_t adr = (size_t) ptr;
	if (adr % alignment)
	{
		adr += alignment - (adr % alignment);
	}
	return((T*) adr);
}

bool AliHLTTPCCAGPUTrackerNVCC::CudaFailedMsg(cudaError_t error)
{
	//Check for CUDA Error and in the case of an error display the corresponding error string
	if (error == cudaSuccess) return(false);
	HLTWarning("CUDA Error: %d / %s", error, cudaGetErrorString(error));
	return(true);
}

int AliHLTTPCCAGPUTrackerNVCC::CUDASync(char* state, int sliceLocal, int slice)
{
	//Wait for CUDA-Kernel to finish and check for CUDA errors afterwards

	if (fDebugLevel == 0) return(0);
	cudaError cuErr;
	cuErr = cudaGetLastError();
	if (cuErr != cudaSuccess)
	{
		HLTError("Cuda Error %s while running kernel (%s) (Slice %d; %d/%d)", cudaGetErrorString(cuErr), state, sliceLocal, slice, fgkNSlices);
		return(1);
	}
	if (CudaFailedMsg(cudaThreadSynchronize()))
	{
		HLTError("CUDA Error while synchronizing (%s) (Slice %d; %d/%d)", state, sliceLocal, slice, fgkNSlices);
		return(1);
	}
	if (fDebugLevel >= 3) HLTInfo("CUDA Sync Done");
	return(0);
}

void AliHLTTPCCAGPUTrackerNVCC::SetDebugLevel(const int dwLevel, std::ostream* const NewOutFile)
{
	//Set Debug Level and Debug output File if applicable
	fDebugLevel = dwLevel;
	if (NewOutFile) fOutFile = NewOutFile;
}

int AliHLTTPCCAGPUTrackerNVCC::SetGPUTrackerOption(char* OptionName, int OptionValue)
{
	//Set a specific GPU Tracker Option
	if (strcmp(OptionName, "PPMode") == 0)
	{
		fPPMode = OptionValue;
	}
	else if (strcmp(OptionName, "DebugMask") == 0)
	{
		fDebugMask = OptionValue;
	}
	else
	{
		HLTError("Unknown Option: %s", OptionName);
		return(1);
	}
	return(0);
}

#ifdef HLTCA_STANDALONE
void AliHLTTPCCAGPUTrackerNVCC::StandalonePerfTime(int iSlice, int i)
{
  //Run Performance Query for timer i of slice iSlice
  if (fDebugLevel >= 1)
  {
	  AliHLTTPCCAStandaloneFramework::StandaloneQueryTime( fSlaveTrackers[iSlice].PerfTimer(i));
  }
}
#else
void AliHLTTPCCAGPUTrackerNVCC::StandalonePerfTime(int /*iSlice*/, int /*i*/) {}
#endif

void AliHLTTPCCAGPUTrackerNVCC::DumpRowBlocks(AliHLTTPCCATracker* tracker, int iSlice, bool check)
{
	//Dump Rowblocks to File
	if (fDebugLevel >= 4)
	{
		*fOutFile << "RowBlock Tracklets (Slice " << tracker[iSlice].Param().ISlice() << " (" << iSlice << " of reco))";
		*fOutFile << " after Tracklet Reconstruction";
		*fOutFile << std::endl;
	
		int4* rowBlockPos = (int4*) malloc(sizeof(int4) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * 2);
		int* rowBlockTracklets = (int*) malloc(sizeof(int) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2);
		uint2* blockStartingTracklet = (uint2*) malloc(sizeof(uint2) * HLTCA_GPU_BLOCK_COUNT);
		CudaFailedMsg(cudaMemcpy(rowBlockPos, fGpuTracker[iSlice].RowBlockPos(), sizeof(int4) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * 2, cudaMemcpyDeviceToHost));
		CudaFailedMsg(cudaMemcpy(rowBlockTracklets, fGpuTracker[iSlice].RowBlockTracklets(), sizeof(int) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2, cudaMemcpyDeviceToHost));
		CudaFailedMsg(cudaMemcpy(blockStartingTracklet, fGpuTracker[iSlice].BlockStartingTracklet(), sizeof(uint2) * HLTCA_GPU_BLOCK_COUNT, cudaMemcpyDeviceToHost));
		CudaFailedMsg(cudaMemcpy(tracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));

		int k = tracker[iSlice].GPUParameters()->fScheduleFirstDynamicTracklet;
		for (int i = 0; i < tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1;i++)
		{
			*fOutFile << "Rowblock: " << i << ", up " << rowBlockPos[i].y << "/" << rowBlockPos[i].x << ", down " << 
				rowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].y << "/" << rowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].x << std::endl << "Phase 1: ";
			for (int j = 0;j < rowBlockPos[i].x;j++)
			{
				//Use Tracker Object to calculate Offset instead of fGpuTracker, since *fNTracklets of fGpuTracker points to GPU Mem!
				*fOutFile << rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] << ", ";
#ifdef HLTCA_GPU_SCHED_FIXED_START
				if (check && rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] != k)
				{
					HLTError("Wrong starting Row Block %d, entry %d, is %d, should be %d", i, j, rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j], k);
				}
#endif //HLTCA_GPU_SCHED_FIXED_START
				k++;
				if (rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] == -1)
				{
					HLTError("Error, -1 Tracklet found");
				}
			}
			*fOutFile << std::endl << "Phase 2: ";
			for (int j = 0;j < rowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].x;j++)
			{
				*fOutFile << rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(1, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] << ", ";
			}
			*fOutFile << std::endl;
		}

		if (check)
		{
			*fOutFile << "Starting Threads: (First Dynamic: " << tracker[iSlice].GPUParameters()->fScheduleFirstDynamicTracklet << ")" << std::endl;
			for (int i = 0;i < HLTCA_GPU_BLOCK_COUNT;i++)
			{
				*fOutFile << i << ": " << blockStartingTracklet[i].x << " - " << blockStartingTracklet[i].y << std::endl;
			}
		}

		free(rowBlockPos);
		free(rowBlockTracklets);
		free(blockStartingTracklet);
	}
}

__global__ void PreInitRowBlocks(int4* const RowBlockPos, int* const RowBlockTracklets, int* const SliceDataHitWeights, int nSliceDataHits)
{
	//Initialize GPU RowBlocks and HitWeights
	if (blockIdx.x >= HLTCA_GPU_BLOCK_COUNT || threadIdx.x >= HLTCA_GPU_THREAD_COUNT) return;
	int4* const rowBlockTracklets4 = (int4*) RowBlockTracklets;
	int4* const sliceDataHitWeights4 = (int4*) SliceDataHitWeights;
	const int stride = blockDim.x * gridDim.x;
	int4 i0, i1;
	i0.x = i0.y = i0.z = i0.w = 0;
	i1.x = i1.y = i1.z = i1.w = -1;
	for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < sizeof(int4) * 2 * (HLTCA_ROW_COUNT / HLTCA_GPU_SCHED_ROW_STEP + 1) / sizeof(int4);i += stride)
		RowBlockPos[i] = i0;
	for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < sizeof(int) * (HLTCA_ROW_COUNT / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2 / sizeof(int4);i += stride)
		rowBlockTracklets4[i] = i1;
	for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < nSliceDataHits * sizeof(int) / sizeof(int4);i += stride)
		sliceDataHitWeights4[i] = i0;
}

int AliHLTTPCCAGPUTrackerNVCC::SelfHealReconstruct(AliHLTTPCCASliceOutput** pOutput, AliHLTTPCCAClusterData* pClusterData, int firstSlice, int sliceCountLocal)
{
	static bool selfHealing = false;
	if (selfHealing)
	{
		HLTError("Selfhealing failed, giving up");
		cuCtxPopCurrent(&fCudaContext);
		return(1);
	}
	else
	{
		HLTError("Unsolvable CUDA error occured, trying to reinitialize GPU");
	}			
	selfHealing = true;
	ExitGPU();
	if (InitGPU(fSliceCount, fCudaDevice))
	{
		HLTError("Could not reinitialize CUDA device, disabling GPU tracker");
		ExitGPU();
		return(1);
	}
	HLTInfo("GPU tracker successfully reinitialized, restarting tracking");
	int retVal = Reconstruct(pOutput, pClusterData, firstSlice, sliceCountLocal);
	selfHealing = false;
	return(retVal);
}

int AliHLTTPCCAGPUTrackerNVCC::Reconstruct(AliHLTTPCCASliceOutput** pOutput, AliHLTTPCCAClusterData* pClusterData, int firstSlice, int sliceCountLocal)
{
	//Primary reconstruction function

	cudaStream_t* const cudaStreams = (cudaStream_t*) fpCudaStreams;

	if (sliceCountLocal == -1) sliceCountLocal = fSliceCount;
	
	if (!fCudaInitialized)
	{
	    HLTError("GPUTracker not initialized");
	    return(1);
	}
	if (sliceCountLocal > fSliceCount)
	{
	    HLTError("GPU Tracker was initialized to run with %d slices but was called to process %d slices", fSliceCount, sliceCountLocal);
	    return(1);
	}
	if (fThreadId != GetThread())
	{
	    HLTWarning("CUDA thread changed, migrating context, Previous Thread: %d, New Thread: %d", fThreadId, GetThread());
		fThreadId = GetThread();
	}

	if (fDebugLevel >= 2) HLTInfo("Running GPU Tracker (Slices %d to %d)", fSlaveTrackers[firstSlice].Param().ISlice(), fSlaveTrackers[firstSlice].Param().ISlice() + sliceCountLocal);

	if (sliceCountLocal * sizeof(AliHLTTPCCATracker) > HLTCA_GPU_TRACKER_CONSTANT_MEM)
	{
		HLTError("Insuffissant constant memory (Required %d, Available %d, Tracker %d, Param %d, SliceData %d)", sliceCountLocal * (int) sizeof(AliHLTTPCCATracker), (int) HLTCA_GPU_TRACKER_CONSTANT_MEM, (int) sizeof(AliHLTTPCCATracker), (int) sizeof(AliHLTTPCCAParam), (int) sizeof(AliHLTTPCCASliceData));
		return(1);
	}

	cuCtxPushCurrent(fCudaContext);
	if (fPPMode)
	{
		int retVal = ReconstructPP(pOutput, pClusterData, firstSlice, sliceCountLocal);
		cuCtxPopCurrent(&fCudaContext);
		return(retVal);
	}

	memcpy(fGpuTracker, &fSlaveTrackers[firstSlice], sizeof(AliHLTTPCCATracker) * sliceCountLocal);

	if (fDebugLevel >= 3) HLTInfo("Allocating GPU Tracker memory and initializing constants");

#ifdef HLTCA_GPU_TIME_PROFILE
	unsigned long long int a, b, c, d;
	AliHLTTPCCAStandaloneFramework::StandaloneQueryFreq(&c);
	AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&d);
#endif
	
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		//Make this a GPU Tracker
		fGpuTracker[iSlice].SetGPUTracker();
		fGpuTracker[iSlice].SetGPUTrackerCommonMemory((char*) CommonMemory(fGPUMemory, iSlice));
		fGpuTracker[iSlice].SetGPUSliceDataMemory(SliceDataMemory(fGPUMemory, iSlice), RowMemory(fGPUMemory, iSlice));
		fGpuTracker[iSlice].SetPointersSliceData(&pClusterData[iSlice], false);

		//Set Pointers to GPU Memory
		char* tmpMem = (char*) GlobalMemory(fGPUMemory, iSlice);

		if (fDebugLevel >= 3) HLTInfo("Initialising GPU Hits Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerHitsMemory(tmpMem, pClusterData[iSlice].NumberOfClusters());
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fDebugLevel >= 3) HLTInfo("Initialising GPU Tracklet Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTrackletsMemory(tmpMem, HLTCA_GPU_MAX_TRACKLETS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets()*/);
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fDebugLevel >= 3) HLTInfo("Initialising GPU Track Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTracksMemory(tmpMem, HLTCA_GPU_MAX_TRACKS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets()*/, pClusterData[iSlice].NumberOfClusters());
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fGpuTracker[iSlice].TrackMemorySize() >= HLTCA_GPU_TRACKS_MEMORY)
		{
			HLTError("Insufficiant Track Memory");
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		if (tmpMem - (char*) GlobalMemory(fGPUMemory, iSlice) > HLTCA_GPU_GLOBAL_MEMORY)
		{
			HLTError("Insufficiant Global Memory");
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		if (fDebugLevel >= 3)
		{
			HLTInfo("GPU Global Memory Used: %d/%d, Page Locked Tracks Memory used: %d / %d", tmpMem - (char*) GlobalMemory(fGPUMemory, iSlice), HLTCA_GPU_GLOBAL_MEMORY, fGpuTracker[iSlice].TrackMemorySize(), HLTCA_GPU_TRACKS_MEMORY);
		}

		//Initialize Startup Constants
		*fSlaveTrackers[firstSlice + iSlice].NTracklets() = 0;
		*fSlaveTrackers[firstSlice + iSlice].NTracks() = 0;
		*fSlaveTrackers[firstSlice + iSlice].NTrackHits() = 0;
		fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount = HLTCA_GPU_BLOCK_COUNT * (iSlice + 1) / sliceCountLocal - HLTCA_GPU_BLOCK_COUNT * (iSlice) / sliceCountLocal;
		if (fDebugLevel >= 3) HLTInfo("Blocks for Slice %d: %d", iSlice, fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount);
		fGpuTracker[iSlice].GPUParametersConst()->fGPUiSlice = iSlice;
		fGpuTracker[iSlice].GPUParametersConst()->fGPUnSlices = sliceCountLocal;
		fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError = 0;
		fGpuTracker[iSlice].SetGPUTextureBase(fGpuTracker[0].Data().Memory());
	}

#ifdef HLTCA_GPU_TEXTURE_FETCH
		cudaChannelFormatDesc channelDescu2 = cudaCreateChannelDesc<ushort2>();
		size_t offset;
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefu2, fGpuTracker[0].Data().Memory(), &channelDescu2, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture ushort2 (Offset %d)", (int) offset);
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
		cudaChannelFormatDesc channelDescu = cudaCreateChannelDesc<unsigned short>();
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefu, fGpuTracker[0].Data().Memory(), &channelDescu, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture ushort (Offset %d)", (int) offset);
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
		cudaChannelFormatDesc channelDescs = cudaCreateChannelDesc<signed short>();
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefs, fGpuTracker[0].Data().Memory(), &channelDescs, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture short (Offset %d)", (int) offset);
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
#endif

	//Copy Tracker Object to GPU Memory
	if (fDebugLevel >= 3) HLTInfo("Copying Tracker objects to GPU");
#ifdef HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE
	char* tmpMem;
	if (CudaFailedMsg(cudaMalloc(&tmpMem, 100000000)))
	{
		HLTError("Error allocating CUDA profile memory");
		cuCtxPopCurrent(&fCudaContext);
		return(1);
	}
	fGpuTracker[0].fStageAtSync = tmpMem;
	CudaFailedMsg(cudaMemset(fGpuTracker[0].StageAtSync(), 0, 100000000));
#endif
	CudaFailedMsg(cudaMemcpyToSymbolAsync(gAliHLTTPCCATracker, fGpuTracker, sizeof(AliHLTTPCCATracker) * sliceCountLocal, 0, cudaMemcpyHostToDevice, cudaStreams[0]));
	if (CUDASync("Initialization (1)", 0, firstSlice))
	{
		cuCtxPopCurrent(&fCudaContext);
		return(1);
	}

	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		StandalonePerfTime(firstSlice + iSlice, 0);

		//Initialize GPU Slave Tracker
		if (fDebugLevel >= 3) HLTInfo("Creating Slice Data");
		fSlaveTrackers[firstSlice + iSlice].SetGPUSliceDataMemory(SliceDataMemory(fHostLockedMemory, iSlice), RowMemory(fHostLockedMemory, firstSlice + iSlice));
#ifdef HLTCA_GPU_TIME_PROFILE
			AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&a);
#endif
		fSlaveTrackers[firstSlice + iSlice].ReadEvent(&pClusterData[iSlice]);

		if (fDebugLevel >= 4)
		{
			*fOutFile << std::endl << std::endl << "Reconstruction: " << iSlice << "/" << sliceCountLocal << " Total Slice: " << fSlaveTrackers[firstSlice + iSlice].Param().ISlice() << " / " << fgkNSlices << std::endl;
			if (fDebugMask & 1) fSlaveTrackers[firstSlice + iSlice].DumpSliceData(*fOutFile);
		}

#ifdef HLTCA_GPU_TIME_PROFILE
		AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&b);
		printf("Read %f %f\n", ((double) b - (double) a) / (double) c, ((double) a - (double) d) / (double) c);
#endif
		if (fSlaveTrackers[firstSlice + iSlice].Data().MemorySize() > HLTCA_GPU_SLICE_DATA_MEMORY)
		{
			HLTError("Insufficiant Slice Data Memory");
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		if (fDebugLevel >= 3)
		{
			HLTInfo("GPU Slice Data Memory Used: %d/%d", fSlaveTrackers[firstSlice + iSlice].Data().MemorySize(), HLTCA_GPU_SLICE_DATA_MEMORY);
		}

		//Initialize temporary memory where needed
		if (fDebugLevel >= 3) HLTInfo("Copying Slice Data to GPU and initializing temporary memory");		
		PreInitRowBlocks<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT, 0, cudaStreams[2]>>>(fGpuTracker[iSlice].RowBlockPos(), fGpuTracker[iSlice].RowBlockTracklets(), fGpuTracker[iSlice].Data().HitWeights(), fSlaveTrackers[firstSlice + iSlice].Data().NumberOfHitsPlusAlign());
		if (CUDASync("Initialization (2)", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		//Copy Data to GPU Global Memory
		CudaFailedMsg(cudaMemcpyAsync(fGpuTracker[iSlice].CommonMemory(), fSlaveTrackers[firstSlice + iSlice].CommonMemory(), fSlaveTrackers[firstSlice + iSlice].CommonMemorySize(), cudaMemcpyHostToDevice, cudaStreams[iSlice & 1]));
		CudaFailedMsg(cudaMemcpyAsync(fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyHostToDevice, cudaStreams[iSlice & 1]));
		CudaFailedMsg(cudaMemcpyAsync(fGpuTracker[iSlice].SliceDataRows(), fSlaveTrackers[firstSlice + iSlice].SliceDataRows(), (HLTCA_ROW_COUNT + 1) * sizeof(AliHLTTPCCARow), cudaMemcpyHostToDevice, cudaStreams[iSlice & 1]));

		if (fDebugLevel >= 4)
		{
			if (fDebugLevel >= 5) HLTInfo("Allocating Debug Output Memory");
			fSlaveTrackers[firstSlice + iSlice].SetGPUTrackerTrackletsMemory(reinterpret_cast<char*> ( new uint4 [ fGpuTracker[iSlice].TrackletMemorySize()/sizeof( uint4 ) + 100] ), HLTCA_GPU_MAX_TRACKLETS);
			fSlaveTrackers[firstSlice + iSlice].SetGPUTrackerHitsMemory(reinterpret_cast<char*> ( new uint4 [ fGpuTracker[iSlice].HitMemorySize()/sizeof( uint4 ) + 100]), pClusterData[iSlice].NumberOfClusters() );
		}
		
		if (CUDASync("Initialization (3)", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
		StandalonePerfTime(firstSlice + iSlice, 1);

		if (fDebugLevel >= 3) HLTInfo("Running GPU Neighbours Finder (Slice %d/%d)", iSlice, sliceCountLocal);
		AliHLTTPCCAProcess<AliHLTTPCCANeighboursFinder> <<<fSlaveTrackers[firstSlice + iSlice].Param().NRows(), 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);

		if (CUDASync("Neighbours finder", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		StandalonePerfTime(firstSlice + iSlice, 2);

		if (fDebugLevel >= 4)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugMask & 2) fSlaveTrackers[firstSlice + iSlice].DumpLinks(*fOutFile);
		}

		if (fDebugLevel >= 3) HLTInfo("Running GPU Neighbours Cleaner (Slice %d/%d)", iSlice, sliceCountLocal);
		AliHLTTPCCAProcess<AliHLTTPCCANeighboursCleaner> <<<fSlaveTrackers[firstSlice + iSlice].Param().NRows()-2, 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);
		if (CUDASync("Neighbours Cleaner", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		StandalonePerfTime(firstSlice + iSlice, 3);

		if (fDebugLevel >= 4)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugMask & 4) fSlaveTrackers[firstSlice + iSlice].DumpLinks(*fOutFile);
		}

		if (fDebugLevel >= 3) HLTInfo("Running GPU Start Hits Finder (Slice %d/%d)", iSlice, sliceCountLocal);
		AliHLTTPCCAProcess<AliHLTTPCCAStartHitsFinder> <<<fSlaveTrackers[firstSlice + iSlice].Param().NRows()-6, 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);
		if (CUDASync("Start Hits Finder", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		StandalonePerfTime(firstSlice + iSlice, 4);

		if (fDebugLevel >= 3) HLTInfo("Running GPU Start Hits Sorter (Slice %d/%d)", iSlice, sliceCountLocal);
		AliHLTTPCCAProcess<AliHLTTPCCAStartHitsSorter> <<<30, 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);
		if (CUDASync("Start Hits Sorter", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}

		StandalonePerfTime(firstSlice + iSlice, 5);

		if (fDebugLevel >= 2)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugLevel >= 3) HLTInfo("Obtaining Number of Start Hits from GPU: %d (Slice %d)", *fSlaveTrackers[firstSlice + iSlice].NTracklets(), iSlice);
			if (*fSlaveTrackers[firstSlice + iSlice].NTracklets() > HLTCA_GPU_MAX_TRACKLETS)
			{
				HLTError("HLTCA_GPU_MAX_TRACKLETS constant insuffisant");
				cuCtxPopCurrent(&fCudaContext);
				return(1);
			}
		}

		if (fDebugLevel >= 4)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].TrackletStartHits(), fGpuTracker[iSlice].TrackletTmpStartHits(), pClusterData[iSlice].NumberOfClusters() * sizeof(AliHLTTPCCAHitId), cudaMemcpyDeviceToHost));
			if (fDebugMask & 8)
			{
				*fOutFile << "Temporary ";
				fSlaveTrackers[firstSlice + iSlice].DumpStartHits(*fOutFile);
			}
			uint3* tmpMemory = (uint3*) malloc(sizeof(uint3) * fSlaveTrackers[firstSlice + iSlice].Param().NRows());
			CudaFailedMsg(cudaMemcpy(tmpMemory, fGpuTracker[iSlice].RowStartHitCountOffset(), fSlaveTrackers[firstSlice + iSlice].Param().NRows() * sizeof(uint3), cudaMemcpyDeviceToHost));
			if (fDebugMask & 16)
			{
				*fOutFile << "Start Hits Sort Vector:" << std::endl;
				for (int i = 0;i < fSlaveTrackers[firstSlice + iSlice].Param().NRows();i++)
				{
					*fOutFile << "Row: " << i << ", Len: " << tmpMemory[i].x << ", Offset: " << tmpMemory[i].y << ", New Offset: " << tmpMemory[i].z << std::endl;
				}
			}
			free(tmpMemory);

			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].HitMemory(), fGpuTracker[iSlice].HitMemory(), fSlaveTrackers[firstSlice + iSlice].HitMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugMask & 32) fSlaveTrackers[firstSlice + iSlice].DumpStartHits(*fOutFile);
		}

		StandalonePerfTime(firstSlice + iSlice, 6);
		
		fSlaveTrackers[firstSlice + iSlice].SetGPUTrackerTracksMemory((char*) TracksMemory(fHostLockedMemory, iSlice), HLTCA_GPU_MAX_TRACKS, pClusterData[iSlice].NumberOfClusters());
	}

	StandalonePerfTime(firstSlice, 7);

	int nHardCollisions = 0;

RestartTrackletConstructor:
	if (fDebugLevel >= 3) HLTInfo("Initialising Tracklet Constructor Scheduler");
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		AliHLTTPCCATrackletConstructorInit<<<HLTCA_GPU_MAX_TRACKLETS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets() */ / HLTCA_GPU_THREAD_COUNT + 1, HLTCA_GPU_THREAD_COUNT>>>(iSlice);
		if (CUDASync("Tracklet Initializer", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
		if (fDebugMask & 64) DumpRowBlocks(&fSlaveTrackers[firstSlice], iSlice);
	}

	if (fDebugLevel >= 3) HLTInfo("Running GPU Tracklet Constructor");
	AliHLTTPCCATrackletConstructorGPU<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>();
	if (CUDASync("Tracklet Constructor", 0, firstSlice))
	{
		cuCtxPopCurrent(&fCudaContext);
		return(1);
	}
	
	StandalonePerfTime(firstSlice, 8);

	if (fDebugLevel >= 4)
	{
		for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
		{
			if (fDebugMask & 64) DumpRowBlocks(&fSlaveTrackers[firstSlice], iSlice, false);
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugLevel >= 5)
			{
				HLTInfo("Obtained %d tracklets", *fSlaveTrackers[firstSlice + iSlice].NTracklets());
			}
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].TrackletMemory(), fGpuTracker[iSlice].TrackletMemory(), fGpuTracker[iSlice].TrackletMemorySize(), cudaMemcpyDeviceToHost));
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].HitMemory(), fGpuTracker[iSlice].HitMemory(), fGpuTracker[iSlice].HitMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugMask & 128) fSlaveTrackers[firstSlice + iSlice].DumpTrackletHits(*fOutFile);
		}
	}

	int runSlices = 0;
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice += runSlices)
	{
		if (runSlices < HLTCA_GPU_TRACKLET_SELECTOR_SLICE_COUNT) runSlices++;
		if (fDebugLevel >= 3) HLTInfo("Running HLT Tracklet selector (Slice %d to %d)", iSlice, iSlice + runSlices);
		AliHLTTPCCAProcessMulti<AliHLTTPCCATrackletSelector><<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT, 0, cudaStreams[iSlice]>>>(iSlice, CAMath::Min(runSlices, sliceCountLocal - iSlice));
		if (CUDASync("Tracklet Selector", iSlice, iSlice + firstSlice))
		{
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
	}
	StandalonePerfTime(firstSlice, 9);

	int tmpSlice = 0, tmpSlice2 = 0;
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 3) HLTInfo("Transfering Tracks from GPU to Host");

		while(tmpSlice < sliceCountLocal && (tmpSlice == iSlice || cudaStreamQuery(cudaStreams[tmpSlice]) == CUDA_SUCCESS))
		{
			if (CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + tmpSlice].CommonMemory(), fGpuTracker[tmpSlice].CommonMemory(), fGpuTracker[tmpSlice].CommonMemorySize(), cudaMemcpyDeviceToHost, cudaStreams[tmpSlice])))
			{
				return(SelfHealReconstruct(pOutput, pClusterData, firstSlice, sliceCountLocal));
			}
			tmpSlice++;
		}

		while (tmpSlice2 < tmpSlice && (tmpSlice2 == iSlice ? cudaStreamSynchronize(cudaStreams[tmpSlice2]) : cudaStreamQuery(cudaStreams[tmpSlice2])) == CUDA_SUCCESS)
		{
			CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + tmpSlice2].Tracks(), fGpuTracker[tmpSlice2].Tracks(), sizeof(AliHLTTPCCATrack) * *fSlaveTrackers[firstSlice + tmpSlice2].NTracks(), cudaMemcpyDeviceToHost, cudaStreams[tmpSlice2]));
			CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + tmpSlice2].TrackHits(), fGpuTracker[tmpSlice2].TrackHits(), sizeof(AliHLTTPCCAHitId) * *fSlaveTrackers[firstSlice + tmpSlice2].NTrackHits(), cudaMemcpyDeviceToHost, cudaStreams[tmpSlice2]));
			tmpSlice2++;
		}

		if (CudaFailedMsg(cudaStreamSynchronize(cudaStreams[iSlice])))
		{
			return(SelfHealReconstruct(pOutput, pClusterData, firstSlice, sliceCountLocal));
		}

		if (fDebugLevel >= 4)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Data().HitWeights(), fGpuTracker[iSlice].Data().HitWeights(), fSlaveTrackers[firstSlice + iSlice].Data().NumberOfHitsPlusAlign() * sizeof(int), cudaMemcpyDeviceToHost));
			if (fDebugMask & 256) fSlaveTrackers[firstSlice + iSlice].DumpHitWeights(*fOutFile);
			if (fDebugMask & 512) fSlaveTrackers[firstSlice + iSlice].DumpTrackHits(*fOutFile);
		}

		if (fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError)
		{
			if ((fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError == HLTCA_GPU_ERROR_SCHEDULE_COLLISION || fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError == HLTCA_GPU_ERROR_WRONG_ROW)&& nHardCollisions++ < 10)
			{
				if (fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError == HLTCA_GPU_ERROR_SCHEDULE_COLLISION)
				{
					HLTWarning("Hard scheduling collision occured, rerunning Tracklet Constructor");
				}
				else
				{
					HLTWarning("Tracklet Constructor returned invalid row");
				}
				for (int i = 0;i < sliceCountLocal;i++)
				{
					cudaThreadSynchronize();
					CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + i].CommonMemory(), fGpuTracker[i].CommonMemory(), fGpuTracker[i].CommonMemorySize(), cudaMemcpyDeviceToHost));
					*fSlaveTrackers[firstSlice + i].NTracks() = 0;
					*fSlaveTrackers[firstSlice + i].NTrackHits() = 0;
					fSlaveTrackers[firstSlice + i].GPUParameters()->fGPUError = HLTCA_GPU_ERROR_NONE;
					CudaFailedMsg(cudaMemcpy(fGpuTracker[i].CommonMemory(), fSlaveTrackers[firstSlice + i].CommonMemory(), fGpuTracker[i].CommonMemorySize(), cudaMemcpyHostToDevice));
					PreInitRowBlocks<<<30, 256>>>(fGpuTracker[i].RowBlockPos(), fGpuTracker[i].RowBlockTracklets(), fGpuTracker[i].Data().HitWeights(), fSlaveTrackers[firstSlice + i].Data().NumberOfHitsPlusAlign());
				}
				goto RestartTrackletConstructor;
			}
			HLTError("GPU Tracker returned Error Code %d", fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError);
			cuCtxPopCurrent(&fCudaContext);
			return(1);
		}
		if (fDebugLevel >= 3) HLTInfo("Tracks Transfered: %d / %d", *fSlaveTrackers[firstSlice + iSlice].NTracks(), *fSlaveTrackers[firstSlice + iSlice].NTrackHits());

		fSlaveTrackers[firstSlice + iSlice].SetOutput(&pOutput[iSlice]);
#ifdef HLTCA_GPU_TIME_PROFILE
		AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&a);
#endif
		fSlaveTrackers[firstSlice + iSlice].WriteOutput();
#ifdef HLTCA_GPU_TIME_PROFILE
		AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&b);
		printf("Write %f %f\n", ((double) b - (double) a) / (double) c, ((double) a - (double) d) / (double) c);
#endif

		if (fDebugLevel >= 4)
		{
			delete[] fSlaveTrackers[firstSlice + iSlice].HitMemory();
			delete[] fSlaveTrackers[firstSlice + iSlice].TrackletMemory();
		}
	}

	StandalonePerfTime(firstSlice, 10);

	if (fDebugLevel >= 3) HLTInfo("GPU Reconstruction finished");

#ifdef HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE
	char* stageAtSync = (char*) malloc(100000000);
	CudaFailedMsg(cudaMemcpy(stageAtSync, fGpuTracker[0].StageAtSync(), 100 * 1000 * 1000, cudaMemcpyDeviceToHost));
	cudaFree(fGpuTracker[0].StageAtSync());

	FILE* fp = fopen("profile.txt", "w+");
	FILE* fp2 = fopen("profile.bmp", "w+b");
	int nEmptySync = 0, fEmpty;

	const int bmpheight = 1000;
	BITMAPFILEHEADER bmpFH;
	BITMAPINFOHEADER bmpIH;
	ZeroMemory(&bmpFH, sizeof(bmpFH));
	ZeroMemory(&bmpIH, sizeof(bmpIH));
	
	bmpFH.bfType = 19778; //"BM"
	bmpFH.bfSize = sizeof(bmpFH) + sizeof(bmpIH) + (HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT / 32 * 33 - 1) * bmpheight ;
	bmpFH.bfOffBits = sizeof(bmpFH) + sizeof(bmpIH);

	bmpIH.biSize = sizeof(bmpIH);
	bmpIH.biWidth = HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT / 32 * 33 - 1;
	bmpIH.biHeight = bmpheight;
	bmpIH.biPlanes = 1;
	bmpIH.biBitCount = 32;

	fwrite(&bmpFH, 1, sizeof(bmpFH), fp2);
	fwrite(&bmpIH, 1, sizeof(bmpIH), fp2); 	

	for (int i = 0;i < bmpheight * HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT;i += HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT)
	{
		fEmpty = 1;
		for (int j = 0;j < HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT;j++)
		{
			fprintf(fp, "%d\t", stageAtSync[i + j]);
			int color = 0;
			if (stageAtSync[i + j] == 1) color = RGB(255, 0, 0);
			if (stageAtSync[i + j] == 2) color = RGB(0, 255, 0);
			if (stageAtSync[i + j] == 3) color = RGB(0, 0, 255);
			if (stageAtSync[i + j] == 4) color = RGB(255, 255, 0);
			fwrite(&color, 1, sizeof(int), fp2);
			if (j > 0 && j % 32 == 0)
			{
				color = RGB(255, 255, 255);
				fwrite(&color, 1, 4, fp2);
			}
			if (stageAtSync[i + j]) fEmpty = 0;
		}
		fprintf(fp, "\n");
		if (fEmpty) nEmptySync++;
		else nEmptySync = 0;
		//if (nEmptySync == HLTCA_GPU_SCHED_ROW_STEP + 2) break;
	}

	fclose(fp);
	fclose(fp2);
	free(stageAtSync);
#endif 

	cuCtxPopCurrent(&fCudaContext);
	return(0);
}

__global__ void ClearPPHitWeights(int sliceCount)
{
	//Clear HitWeights
	
	for (int k = 0;k < sliceCount;k++)
	{
		AliHLTTPCCATracker &tracker = ((AliHLTTPCCATracker*) gAliHLTTPCCATracker)[k];
		int4* const pHitWeights = (int4*) tracker.Data().HitWeights();
		const int dwCount = tracker.Data().NumberOfHitsPlusAlign();
		const int stride = blockDim.x * gridDim.x;
		int4 i0;
		i0.x = i0.y = i0.z = i0.w = 0;
	
		for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < dwCount * sizeof(int) / sizeof(int4);i += stride)
		{
			pHitWeights[i] = i0;
		}
	}
}

int AliHLTTPCCAGPUTrackerNVCC::ReconstructPP(AliHLTTPCCASliceOutput** pOutput, AliHLTTPCCAClusterData* pClusterData, int firstSlice, int sliceCountLocal)
{
	//Primary reconstruction function for small events (PP)

	memcpy(fGpuTracker, &fSlaveTrackers[firstSlice], sizeof(AliHLTTPCCATracker) * sliceCountLocal);

	if (fDebugLevel >= 3) HLTInfo("Allocating GPU Tracker memory and initializing constants");

#ifdef HLTCA_GPU_TIME_PROFILE
	unsigned long long int a, b, c, d;
	AliHLTTPCCAStandaloneFramework::StandaloneQueryFreq(&c);
	AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&d);
#endif

	char* tmpSliceMemHost = (char*) SliceDataMemory(fHostLockedMemory, 0);
	char* tmpSliceMemGpu = (char*) SliceDataMemory(fGPUMemory, 0);

	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		StandalonePerfTime(firstSlice + iSlice, 0);

		//Initialize GPU Slave Tracker
		if (fDebugLevel >= 3) HLTInfo("Creating Slice Data");
		fSlaveTrackers[firstSlice + iSlice].SetGPUSliceDataMemory(tmpSliceMemHost, RowMemory(fHostLockedMemory, firstSlice + iSlice));
#ifdef HLTCA_GPU_TIME_PROFILE
			AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&a);
#endif
		fSlaveTrackers[firstSlice + iSlice].ReadEvent(&pClusterData[iSlice]);
#ifdef HLTCA_GPU_TIME_PROFILE
			AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&b);
		printf("Read %f %f\n", ((double) b - (double) a) / (double) c, ((double) a - (double) d) / (double) c);
#endif
		if (fSlaveTrackers[firstSlice + iSlice].Data().MemorySize() > HLTCA_GPU_SLICE_DATA_MEMORY)
		{
			HLTError("Insufficiant Slice Data Memory");
			return(1);
		}

		//Make this a GPU Tracker
		fGpuTracker[iSlice].SetGPUTracker();
		fGpuTracker[iSlice].SetGPUTrackerCommonMemory((char*) CommonMemory(fGPUMemory, iSlice));


		fGpuTracker[iSlice].SetGPUSliceDataMemory(tmpSliceMemGpu, RowMemory(fGPUMemory, iSlice));
		fGpuTracker[iSlice].SetPointersSliceData(&pClusterData[iSlice], false);

		tmpSliceMemHost += fSlaveTrackers[firstSlice + iSlice].Data().MemorySize();
		tmpSliceMemHost = alignPointer(tmpSliceMemHost, 64 * 1024);
		tmpSliceMemGpu += fSlaveTrackers[firstSlice + iSlice].Data().MemorySize();
		tmpSliceMemGpu = alignPointer(tmpSliceMemGpu, 64 * 1024);


		//Set Pointers to GPU Memory
		char* tmpMem = (char*) GlobalMemory(fGPUMemory, iSlice);

		if (fDebugLevel >= 3) HLTInfo("Initialising GPU Hits Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerHitsMemory(tmpMem, pClusterData[iSlice].NumberOfClusters());
		tmpMem = alignPointer(tmpMem, 64 * 1024);

		if (fDebugLevel >= 3) HLTInfo("Initialising GPU Tracklet Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTrackletsMemory(tmpMem, HLTCA_GPU_MAX_TRACKLETS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets()*/);
		tmpMem = alignPointer(tmpMem, 64 * 1024);

		if (fDebugLevel >= 3) HLTInfo("Initialising GPU Track Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTracksMemory(tmpMem, HLTCA_GPU_MAX_TRACKS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets()*/, pClusterData[iSlice].NumberOfClusters());
		tmpMem = alignPointer(tmpMem, 64 * 1024);

		if (fGpuTracker[iSlice].TrackMemorySize() >= HLTCA_GPU_TRACKS_MEMORY)
		{
			HLTError("Insufficiant Track Memory");
			return(1);
		}

		if (tmpMem - (char*) GlobalMemory(fGPUMemory, iSlice) > HLTCA_GPU_GLOBAL_MEMORY)
		{
			HLTError("Insufficiant Global Memory");
			return(1);
		}

		//Initialize Startup Constants
		*fSlaveTrackers[firstSlice + iSlice].NTracklets() = 0;
		*fSlaveTrackers[firstSlice + iSlice].NTracks() = 0;
		*fSlaveTrackers[firstSlice + iSlice].NTrackHits() = 0;
		fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError = 0;

		fGpuTracker[iSlice].SetGPUTextureBase(fGpuTracker[0].Data().Memory());

		if (CUDASync("Initialization", iSlice, iSlice + firstSlice)) return(1);
		StandalonePerfTime(firstSlice + iSlice, 1);
	}

#ifdef HLTCA_GPU_TEXTURE_FETCH
		cudaChannelFormatDesc channelDescu2 = cudaCreateChannelDesc<ushort2>();
		size_t offset;
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefu2, fGpuTracker[0].Data().Memory(), &channelDescu2, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture ushort2 (Offset %d)", (int) offset);
			return(1);
		}
		cudaChannelFormatDesc channelDescu = cudaCreateChannelDesc<unsigned short>();
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefu, fGpuTracker[0].Data().Memory(), &channelDescu, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture ushort (Offset %d)", (int) offset);
			return(1);
		}
		cudaChannelFormatDesc channelDescs = cudaCreateChannelDesc<signed short>();
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefs, fGpuTracker[0].Data().Memory(), &channelDescs, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture short (Offset %d)", (int) offset);
			return(1);
		}
#endif

	//Copy Tracker Object to GPU Memory
	if (fDebugLevel >= 3) HLTInfo("Copying Tracker objects to GPU");
	CudaFailedMsg(cudaMemcpyToSymbol(gAliHLTTPCCATracker, fGpuTracker, sizeof(AliHLTTPCCATracker) * sliceCountLocal, 0, cudaMemcpyHostToDevice));

	//Copy Data to GPU Global Memory
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		CudaFailedMsg(cudaMemcpy(fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyHostToDevice));
		//printf("%lld %lld %d %d\n", (size_t) (char*) fGpuTracker[iSlice].Data().Memory(), (size_t) (char*) fSlaveTrackers[firstSlice + iSlice].Data().Memory(), (int) (size_t) fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), (int) (size_t) fSlaveTrackers[firstSlice + iSlice].Data().MemorySize());
	}
	//CudaFailedMsg(cudaMemcpy(SliceDataMemory(fGPUMemory, 0), SliceDataMemory(fHostLockedMemory, 0), tmpSliceMemHost - (char*) SliceDataMemory(fHostLockedMemory, 0), cudaMemcpyHostToDevice));
	//printf("%lld %lld %d\n", (size_t) (char*) SliceDataMemory(fGPUMemory, 0), (size_t) (char*) SliceDataMemory(fHostLockedMemory, 0), (int) (size_t) (tmpSliceMemHost - (char*) SliceDataMemory(fHostLockedMemory, 0)));
	CudaFailedMsg(cudaMemcpy(fGpuTracker[0].CommonMemory(), fSlaveTrackers[firstSlice].CommonMemory(), fSlaveTrackers[firstSlice].CommonMemorySize() * sliceCountLocal, cudaMemcpyHostToDevice));
	CudaFailedMsg(cudaMemcpy(fGpuTracker[0].SliceDataRows(), fSlaveTrackers[firstSlice].SliceDataRows(), (HLTCA_ROW_COUNT + 1) * sizeof(AliHLTTPCCARow) * sliceCountLocal, cudaMemcpyHostToDevice));

	if (fDebugLevel >= 3) HLTInfo("Running GPU Neighbours Finder");
	AliHLTTPCCAProcessMultiA<AliHLTTPCCANeighboursFinder> <<<30, 256>>>(0, sliceCountLocal, fSlaveTrackers[firstSlice].Param().NRows());
	if (CUDASync("Neighbours finder", 0, firstSlice)) return 1;
	StandalonePerfTime(firstSlice, 2);
	if (fDebugLevel >= 3) HLTInfo("Running GPU Neighbours Cleaner");
	AliHLTTPCCAProcessMultiA<AliHLTTPCCANeighboursCleaner> <<<30, 256>>>(0, sliceCountLocal, fSlaveTrackers[firstSlice].Param().NRows() - 2);
	if (CUDASync("Neighbours Cleaner", 0, firstSlice)) return 1;
	StandalonePerfTime(firstSlice, 3);
	if (fDebugLevel >= 3) HLTInfo("Running GPU Start Hits Finder");
	AliHLTTPCCAProcessMultiA<AliHLTTPCCAStartHitsFinder> <<<30, 256>>>(0, sliceCountLocal, fSlaveTrackers[firstSlice].Param().NRows() - 6);
	if (CUDASync("Start Hits Finder", 0, firstSlice)) return 1;
	StandalonePerfTime(firstSlice, 4);

	ClearPPHitWeights <<<30, 256>>>(sliceCountLocal);
	if (CUDASync("Clear Hit Weights", 0, firstSlice)) return 1;

	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		fSlaveTrackers[firstSlice + iSlice].SetGPUTrackerTracksMemory((char*) TracksMemory(fHostLockedMemory, iSlice), HLTCA_GPU_MAX_TRACKS, pClusterData[iSlice].NumberOfClusters());
	}

	StandalonePerfTime(firstSlice, 7);

	if (fDebugLevel >= 3) HLTInfo("Running GPU Tracklet Constructor");
	AliHLTTPCCATrackletConstructorGPUPP<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>(0, sliceCountLocal);
	if (CUDASync("Tracklet Constructor PP", 0, firstSlice)) return 1;
	
	StandalonePerfTime(firstSlice, 8);

	AliHLTTPCCAProcessMulti<AliHLTTPCCATrackletSelector><<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>(0, sliceCountLocal);
	if (CUDASync("Tracklet Selector", 0, firstSlice)) return 1;
	StandalonePerfTime(firstSlice, 9);

	CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice].CommonMemory(), fGpuTracker[0].CommonMemory(), fSlaveTrackers[firstSlice].CommonMemorySize() * sliceCountLocal, cudaMemcpyDeviceToHost));

	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 3) HLTInfo("Transfering Tracks from GPU to Host");

		CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Tracks(), fGpuTracker[iSlice].Tracks(), sizeof(AliHLTTPCCATrack) * *fSlaveTrackers[firstSlice + iSlice].NTracks(), cudaMemcpyDeviceToHost));
		CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].TrackHits(), fGpuTracker[iSlice].TrackHits(), sizeof(AliHLTTPCCAHitId) * *fSlaveTrackers[firstSlice + iSlice].NTrackHits(), cudaMemcpyDeviceToHost));

		if (fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError)
		{
			HLTError("GPU Tracker returned Error Code %d", fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError);
			return(1);
		}
		if (fDebugLevel >= 3) HLTInfo("Tracks Transfered: %d / %d", *fSlaveTrackers[firstSlice + iSlice].NTracks(), *fSlaveTrackers[firstSlice + iSlice].NTrackHits());

		fSlaveTrackers[firstSlice + iSlice].SetOutput(&pOutput[iSlice]);
#ifdef HLTCA_GPU_TIME_PROFILE
		AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&a);
#endif
		fSlaveTrackers[firstSlice + iSlice].WriteOutput();
#ifdef HLTCA_GPU_TIME_PROFILE
		AliHLTTPCCAStandaloneFramework::StandaloneQueryTime(&b);
		printf("Write %f %f\n", ((double) b - (double) a) / (double) c, ((double) a - (double) d) / (double) c);
#endif
	}

	StandalonePerfTime(firstSlice, 10);

	if (fDebugLevel >= 3) HLTInfo("GPU Reconstruction finished");

	return(0);
}

int AliHLTTPCCAGPUTrackerNVCC::InitializeSliceParam(int iSlice, AliHLTTPCCAParam &param)
{
	//Initialize Slice Tracker Parameter for a slave tracker
	fSlaveTrackers[iSlice].Initialize(param);
	if (fSlaveTrackers[iSlice].Param().NRows() != HLTCA_ROW_COUNT)
	{
		HLTError("Error, Slice Tracker %d Row Count of %d exceeds Constant of %d", iSlice, fSlaveTrackers[iSlice].Param().NRows(), HLTCA_ROW_COUNT);
		return(1);
	}
	return(0);
}

int AliHLTTPCCAGPUTrackerNVCC::ExitGPU()
{
	//Uninitialize CUDA
	cuCtxPushCurrent(fCudaContext);

	cudaThreadSynchronize();
	if (fGPUMemory)
	{
		cudaFree(fGPUMemory);
		fGPUMemory = NULL;
	}
	if (fHostLockedMemory)
	{
		for (int i = 0;i < CAMath::Max(3, fSliceCount);i++)
		{
			cudaStreamDestroy(((cudaStream_t*) fpCudaStreams)[i]);
		}
		free(fpCudaStreams);
		fGpuTracker = NULL;
		cudaFreeHost(fHostLockedMemory);
	}

	if (CudaFailedMsg(cudaThreadExit()))
	{
		HLTError("Could not uninitialize GPU");
		return(1);
	}
	HLTInfo("CUDA Uninitialized");
	fCudaInitialized = 0;
	return(0);
}

void AliHLTTPCCAGPUTrackerNVCC::SetOutputControl( AliHLTTPCCASliceOutput::outputControlStruct* val)
{
	//Set Output Control Pointers
	fOutputControl = val;
	for (int i = 0;i < fgkNSlices;i++)
	{
		fSlaveTrackers[i].SetOutputControl(val);
	}
}

int AliHLTTPCCAGPUTrackerNVCC::GetThread()
{
	//Get Thread ID
#ifdef R__WIN32
	return((int) (size_t) GetCurrentThread());
#else
	return((int) syscall (SYS_gettid));
#endif
}

unsigned long long int* AliHLTTPCCAGPUTrackerNVCC::PerfTimer(int iSlice, unsigned int i)
{
	//Returns pointer to PerfTimer i of slice iSlice
	return(fSlaveTrackers ? fSlaveTrackers[iSlice].PerfTimer(i) : NULL);
}

const AliHLTTPCCASliceOutput::outputControlStruct* AliHLTTPCCAGPUTrackerNVCC::OutputControl() const
{
	//Return Pointer to Output Control Structure
	return fOutputControl;
}

int AliHLTTPCCAGPUTrackerNVCC::GetSliceCount() const
{
	//Return max slice count processable
	return(fSliceCount);
}

AliHLTTPCCAGPUTracker* AliHLTTPCCAGPUTrackerNVCCCreate()
{
	return new AliHLTTPCCAGPUTrackerNVCC;
}

void AliHLTTPCCAGPUTrackerNVCCDestroy(AliHLTTPCCAGPUTracker* ptr)
{
	delete ptr;
}

