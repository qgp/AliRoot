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

#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCAGPUConfig.h"

//#include <cutil.h>
#ifndef CUDA_DEVICE_EMULATION
//#include <cutil_inline_runtime.h>
#else
//#include <cutil_inline.h>
#endif
#include <sm_11_atomic_functions.h>
#include <sm_12_atomic_functions.h>

#include <iostream>

//Disable assertions since they produce errors in GPU Code
#ifdef assert
#undef assert
#endif
#define assert(param)

#include "AliHLTTPCCAGPUTracker.h"

__constant__ float4 gAliHLTTPCCATracker[HLTCA_GPU_TRACKER_CONSTANT_MEM / sizeof( float4 )];

#include "AliHLTTPCCAHit.h"

//Include CXX Files, GPUd() macro will then produce CUDA device code out of the tracker source code
#include "AliHLTTPCCATrackParam.cxx"
#include "AliHLTTPCCATrack.cxx" 

#include "AliHLTTPCCATrackletSelector.cxx"

#include "AliHLTTPCCAHitArea.cxx"
#include "AliHLTTPCCAGrid.cxx"
#include "AliHLTTPCCARow.cxx"
#include "AliHLTTPCCAParam.cxx"
#include "AliHLTTPCCATracker.cxx"

#include "AliHLTTPCCAOutTrack.cxx"

#include "AliHLTTPCCAProcess.h"

#include "AliHLTTPCCANeighboursFinder.cxx"

#include "AliHLTTPCCANeighboursCleaner.cxx"
#include "AliHLTTPCCAStartHitsFinder.cxx"
#include "AliHLTTPCCAStartHitsSorter.cxx"
#include "AliHLTTPCCATrackletConstructor.cxx"
#include "AliHLTTPCCASliceOutput.cxx"

#include "MemoryAssignmentHelpers.h"

//Find best CUDA device, initialize and allocate memory
int AliHLTTPCCAGPUTracker::InitGPU(int sliceCount, int forceDeviceID)
{
	cudaDeviceProp fCudaDeviceProp;

#ifndef CUDA_DEVICE_EMULATION
	int count, bestDevice = -1, bestDeviceSpeed = 0;
	if (CUDA_FAILED_MSG(cudaGetDeviceCount(&count)))
	{
		std::cout << "Error getting CUDA Device Count" << std::endl;
		return(1);
	}
	if (fDebugLevel >= 2) std::cout << "Available CUDA devices: ";
	for (int i = 0;i < count;i++)
	{
		cudaGetDeviceProperties(&fCudaDeviceProp, i);
		if (fDebugLevel >= 2) std::cout << fCudaDeviceProp.name << " (" << i << ")     ";
		if (fCudaDeviceProp.major < 9 && !(fCudaDeviceProp.major < 1 || (fCudaDeviceProp.major == 1 && fCudaDeviceProp.minor < 2)) && fCudaDeviceProp.multiProcessorCount * fCudaDeviceProp.clockRate > bestDeviceSpeed)
		{
			bestDevice = i;
			bestDeviceSpeed = fCudaDeviceProp.multiProcessorCount * fCudaDeviceProp.clockRate;
		}
	}
	if (fDebugLevel >= 2) std::cout << std::endl;

	if (bestDevice == -1)
	{
		std::cout << "No CUDA Device available, aborting CUDA Initialisation" << std::endl;
		return(1);
	}

  int cudaDevice;
  if (forceDeviceID == -1)
	  cudaDevice = bestDevice;
  else
	  cudaDevice = forceDeviceID;
#else
	int cudaDevice = 0;
#endif

  cudaGetDeviceProperties(&fCudaDeviceProp ,cudaDevice ); 

  if (fDebugLevel >= 1)
  {
	  std::cout<<"CUDA Device Properties: "<<std::endl;
	  std::cout<<"name = "<<fCudaDeviceProp.name<<std::endl;
	  std::cout<<"totalGlobalMem = "<<fCudaDeviceProp.totalGlobalMem<<std::endl;
	  std::cout<<"sharedMemPerBlock = "<<fCudaDeviceProp.sharedMemPerBlock<<std::endl;
	  std::cout<<"regsPerBlock = "<<fCudaDeviceProp.regsPerBlock<<std::endl;
	  std::cout<<"warpSize = "<<fCudaDeviceProp.warpSize<<std::endl;
	  std::cout<<"memPitch = "<<fCudaDeviceProp.memPitch<<std::endl;
	  std::cout<<"maxThreadsPerBlock = "<<fCudaDeviceProp.maxThreadsPerBlock<<std::endl;
	  std::cout<<"maxThreadsDim = "<<fCudaDeviceProp.maxThreadsDim[0]<<" "<<fCudaDeviceProp.maxThreadsDim[1]<<" "<<fCudaDeviceProp.maxThreadsDim[2]<<std::endl;
	  std::cout<<"maxGridSize = "  <<fCudaDeviceProp.maxGridSize[0]<<" "<<fCudaDeviceProp.maxGridSize[1]<<" "<<fCudaDeviceProp.maxGridSize[2]<<std::endl;
	  std::cout<<"totalConstMem = "<<fCudaDeviceProp.totalConstMem<<std::endl;
	  std::cout<<"major = "<<fCudaDeviceProp.major<<std::endl;
	  std::cout<<"minor = "<<fCudaDeviceProp.minor<<std::endl;
	  std::cout<<"clockRate = "<<fCudaDeviceProp.clockRate<<std::endl;
	  std::cout<<"textureAlignment = "<<fCudaDeviceProp.textureAlignment<<std::endl;
  }

  if (fCudaDeviceProp.major < 1 || (fCudaDeviceProp.major == 1 && fCudaDeviceProp.minor < 2))
  {
	  std::cout << "Unsupported CUDA Device\n";
	  return(1);
  }

  if (CUDA_FAILED_MSG(cudaSetDevice(cudaDevice)))
  {
	  std::cout << "Could not set CUDA Device!\n";
	  return(1);
  }

  fGPUMemSize = (long long int) fCudaDeviceProp.totalGlobalMem - 400 * 1024 * 1024;
  if (fGPUMemSize > 1024 * 1024 * 1024) fGPUMemSize = 1024 * 1024 * 1024;
  if (CUDA_FAILED_MSG(cudaMalloc(&fGPUMemory, (size_t) fGPUMemSize)))
  {
	  std::cout << "CUDA Memory Allocation Error\n";
	  cudaThreadExit();
	  return(1);
  }
  if (fDebugLevel >= 1)
  {
	  CUDA_FAILED_MSG(cudaMemset(fGPUMemory, 143, (size_t) fGPUMemSize));
  }
  std::cout << "CUDA Initialisation successfull\n";

  if ((fGpuTracker = new AliHLTTPCCATracker[sliceCount]) == NULL)
  {
	printf("Error Creating GpuTrackers\n");
	cudaThreadExit();
	return(1);
  }
  for (int i = 0;i < sliceCount;i++) fGpuTracker[i].SetGPUTracker();
  fSliceCount = sliceCount;

  pCudaStreams = malloc(2 * sizeof(cudaStream_t));
  cudaStream_t* const cudaStreams = (cudaStream_t*) pCudaStreams;
  for (int i = 0;i < 2;i++)
  {
	if (CUDA_FAILED_MSG(cudaStreamCreate(&cudaStreams[i])))
	{
		std::cout << "Error creating CUDA Stream" << std::endl;
		return(1);
	}
  }

	return(0);
}

//Macro to align Pointers.
//Will align to start at 1 MB segments, this should be consistent with every alignment in the tracker
//(As long as every single data structure is <= 1 MB)
template <class T> inline T* AliHLTTPCCAGPUTracker::alignPointer(T* ptr, int alignment)
{
	size_t adr = (size_t) ptr;
	if (adr % alignment)
	{
		adr += alignment - (adr % alignment);
	}
	return((T*) adr);
}

//Check for CUDA Error and in the case of an error display the corresponding error string
bool AliHLTTPCCAGPUTracker::CUDA_FAILED_MSG(cudaError_t error)
{
	if (error == cudaSuccess) return(false);
	printf("CUDA Error: %d / %s\n", error, cudaGetErrorString(error));
	return(true);
}

//Wait for CUDA-Kernel to finish and check for CUDA errors afterwards
int AliHLTTPCCAGPUTracker::CUDASync(char* state)
{
	if (fDebugLevel == 0) return(0);
	cudaError cuErr;
	cuErr = cudaGetLastError();
	if (cuErr != cudaSuccess)
	{
		printf("Cuda Error %s while invoking kernel (%s)\n", cudaGetErrorString(cuErr), state);
		return(1);
	}
	if (CUDA_FAILED_MSG(cudaThreadSynchronize()))
	{
		printf("CUDA Error while synchronizing (%s)\n", state);
		return(1);
	}
	if (fDebugLevel >= 5) printf("CUDA Sync Done\n");
	return(0);
}

void AliHLTTPCCAGPUTracker::SetDebugLevel(int dwLevel, std::ostream *NewOutFile)
{
	fDebugLevel = dwLevel;
	if (NewOutFile) fOutFile = NewOutFile;
}

int AliHLTTPCCAGPUTracker::SetGPUTrackerOption(char* OptionName, int OptionValue)
{
	if (strcmp(OptionName, "SingleBlock") == 0)
	{
		fOptionSingleBlock = OptionValue;
	}
	else if (strcmp(OptionName, "SimpleSched") == 0)
	{
		fOptionSimpleSched = OptionValue;
	}
	else
	{
		printf("Unknown Option: %s\n", OptionName);
		return(1);
	}
	return(0);
}

#ifdef HLTCA_STANDALONE
void AliHLTTPCCAGPUTracker::StandalonePerfTime(int i)
{
  if (fDebugLevel >= 1)
  {
	  fGpuTracker[0].StandaloneQueryTime( fGpuTracker[0].PerfTimer(i));
  }
}
#else
void AliHLTTPCCAGPUTracker::StandalonePerfTime(int /*i*/) {}
#endif

void* AliHLTTPCCAGPUTracker::gpuHostMallocPageLocked(size_t size)
{
	void* ptr;
	if (cudaMallocHost(&ptr, size)) ptr = 0;
	return(ptr);
}

void AliHLTTPCCAGPUTracker::gpuHostFreePageLocked(void* ptr)
{
	cudaFreeHost(ptr);
}

void AliHLTTPCCAGPUTracker::DumpRowBlocks(AliHLTTPCCATracker* tracker, int iSlice, bool check)
{
	if (fDebugLevel >= 4)
	{
		*fOutFile << "RowBlock Tracklets" << std::endl;
	
		int4* RowBlockPos = (int4*) malloc(sizeof(int4) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * 2);
		int* RowBlockTracklets = (int*) malloc(sizeof(int) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2);
		uint2* BlockStartingTracklet = (uint2*) malloc(sizeof(uint2) * HLTCA_GPU_BLOCK_COUNT);
		CUDA_FAILED_MSG(cudaMemcpy(RowBlockPos, fGpuTracker[iSlice].RowBlockPos(), sizeof(int4) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * 2, cudaMemcpyDeviceToHost));
		CUDA_FAILED_MSG(cudaMemcpy(RowBlockTracklets, fGpuTracker[iSlice].RowBlockTracklets(), sizeof(int) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2, cudaMemcpyDeviceToHost));
		CUDA_FAILED_MSG(cudaMemcpy(BlockStartingTracklet, fGpuTracker[iSlice].BlockStartingTracklet(), sizeof(uint2) * HLTCA_GPU_BLOCK_COUNT, cudaMemcpyDeviceToHost));
		CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), tracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));

		int k = tracker[iSlice].GPUParameters()->fScheduleFirstDynamicTracklet;
		for (int i = 0; i < tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1;i++)
		{
			*fOutFile << "Rowblock: " << i << ", up " << RowBlockPos[i].y << "/" << RowBlockPos[i].x << ", down " << 
				RowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].y << "/" << RowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].x << endl << "Phase 1: ";
			for (int j = 0;j < RowBlockPos[i].x;j++)
			{
				//Use Tracker Object to calculate Offset instead of fGpuTracker, since *fNTracklets of fGpuTracker points to GPU Mem!
				*fOutFile << RowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] << ", ";
				if (check && RowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] != k)
				{
					printf("Wrong starting Row Block %d, entry %d, is %d, should be %d\n", i, j, RowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j], k);
				}
				k++;
				if (RowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] == -1)
				{
					printf("Error, -1 Tracklet found\n");
				}
			}
			*fOutFile << endl << "Phase 2: ";
			for (int j = 0;j < RowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].x;j++)
			{
				*fOutFile << RowBlockTracklets[(tracker[iSlice].RowBlockTracklets(1, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] << ", ";
			}
			*fOutFile << endl;
		}

		if (check)
		{
			*fOutFile << "Starting Threads: (First Dynamic: " << tracker[iSlice].GPUParameters()->fScheduleFirstDynamicTracklet << ")" << std::endl;
			for (int i = 0;i < HLTCA_GPU_BLOCK_COUNT;i++)
			{
				*fOutFile << i << ": " << BlockStartingTracklet[i].x << " - " << BlockStartingTracklet[i].y << std::endl;
			}
		}

		free(RowBlockPos);
		free(RowBlockTracklets);
		free(BlockStartingTracklet);
	}
}

__global__ void PreInitRowBlocks(int4* const RowBlockPos, int* const RowBlockTracklets, int* const SliceDataHitWeights, int nSliceDataHits)
{
	int4* const RowBlockTracklets4 = (int4*) RowBlockTracklets;
	int4* const SliceDataHitWeights4 = (int4*) SliceDataHitWeights;
	const int stride = blockDim.x * gridDim.x;
	int4 i0, i1;
	i0.x = i0.y = i0.z = i0.w = 0;
	i1.x = i1.y = i1.z = i1.w = -1;
	for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < sizeof(int4) * 2 * (HLTCA_ROW_COUNT / HLTCA_GPU_SCHED_ROW_STEP + 1) / sizeof(int4);i += stride)
		RowBlockPos[i] = i0;
	for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < sizeof(int) * (HLTCA_ROW_COUNT / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2 / sizeof(int4);i += stride)
		RowBlockTracklets4[i] = i1;
	for (int i = blockIdx.x * blockDim.x + threadIdx.x;i < nSliceDataHits * sizeof(int) / sizeof(int4);i += stride)
		SliceDataHitWeights4[i] = i0;
}

//Primary reconstruction function
int AliHLTTPCCAGPUTracker::Reconstruct(AliHLTTPCCATracker* tracker, int fSliceCountLocal)
{
    //int nThreads;
    //int nBlocks;
	int size;

	if (fSliceCountLocal == -1) fSliceCountLocal = this->fSliceCount;

	if (fSliceCountLocal * sizeof(AliHLTTPCCATracker) > HLTCA_GPU_TRACKER_CONSTANT_MEM)
	{
		printf("Insuffissant constant memory (Required %d, Available %d, Tracker %d, Param %d, SliceData %d)\n", fSliceCountLocal * (int) sizeof(AliHLTTPCCATracker), (int) HLTCA_GPU_TRACKER_CONSTANT_MEM, (int) sizeof(AliHLTTPCCATracker), (int) sizeof(AliHLTTPCCAParam), (int) sizeof(AliHLTTPCCASliceData));
		return(1);
	}

	int cudaDevice;
	cudaDeviceProp fCudaDeviceProp;
	cudaGetDevice(&cudaDevice);
	cudaGetDeviceProperties(&fCudaDeviceProp, cudaDevice);

	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		if (tracker[iSlice].Param().NRows() != HLTCA_ROW_COUNT)
		{
			printf("Error, Slice Tracker %d Row Count of %d exceeds Constant of %d\n", iSlice, tracker[iSlice].Param().NRows(), HLTCA_ROW_COUNT);
			return(1);
		}
		if (tracker[iSlice].CheckEmptySlice())
		{
			if (fDebugLevel >= 5) printf("Slice Empty, not running GPU Tracker\n");
			if (fSliceCountLocal == 1)
				return(0);
		}

		if (fDebugLevel >= 4)
		{
			*fOutFile << endl << endl << "Slice: " << tracker[iSlice].Param().ISlice() << endl;
		}
	}

	if (fDebugLevel >= 5) printf("\n\nInitialising GPU Tracker\n");
	memcpy(fGpuTracker, tracker, sizeof(AliHLTTPCCATracker) * fSliceCountLocal);

	StandalonePerfTime(0);

	char* tmpMem = alignPointer((char*) fGPUMemory, 1024 * 1024);


	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		fGpuTracker[iSlice].SetGPUTracker();

		if (fDebugLevel >= 5) printf("Initialising GPU Common Memory\n");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerCommonMemory(tmpMem);
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fDebugLevel >= 5) printf("Initialising GPU Hits Memory\n");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerHitsMemory(tmpMem, tracker[iSlice].NHitsTotal(), fOptionSimpleSched);
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fDebugLevel >= 5) printf("Initialising GPU Slice Data Memory\n");
		tmpMem = fGpuTracker[iSlice].SetGPUSliceDataMemory(tmpMem, fGpuTracker[iSlice].ClusterData());
		tmpMem = alignPointer(tmpMem, 1024 * 1024);
		if (tmpMem - (char*) fGPUMemory > fGPUMemSize)
		{
			printf("Out of CUDA Memory\n");
			return(1);
		}

	#ifdef HLTCA_STANDALONE
		if (fDebugLevel >= 6)
		{
			if (CUDA_FAILED_MSG(cudaMalloc((void**) &fGpuTracker[iSlice].fGPUDebugMem, 100 * 1024 * 1024)))
			{
				printf("Out of CUDA Memory\n");
				return(1);
			}
			CUDA_FAILED_MSG(cudaMemset(fGpuTracker[iSlice].fGPUDebugMem, 0, 100 * 1024 * 1024));
		}
	#endif

		if (fDebugLevel >= 5) printf("Initialising GPU Track Memory\n");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTracksMemory(tmpMem, HLTCA_GPU_MAX_TRACKLETS /**tracker[iSlice].NTracklets()*/, tracker[iSlice].NHitsTotal(), fOptionSimpleSched);
		tmpMem = alignPointer(tmpMem, 1024 * 1024);
		if (tmpMem - (char*) fGPUMemory > fGPUMemSize)
		{
			printf("Out of CUDA Memory\n");
			return(1);
		}

		*tracker[iSlice].NTracklets() = 0;
		*tracker[iSlice].NTracks() = 0;
		*tracker[iSlice].NTrackHits() = 0;
		fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount = HLTCA_GPU_BLOCK_COUNT * (iSlice + 1) / fSliceCountLocal - HLTCA_GPU_BLOCK_COUNT * (iSlice) / fSliceCountLocal;
		if (fDebugLevel >= 5) printf("Blocks for Slice %d: %d\n", iSlice, fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount);
		fGpuTracker[iSlice].GPUParametersConst()->fGPUiSlice = iSlice;
		fGpuTracker[iSlice].GPUParametersConst()->fGPUnSlices = fSliceCountLocal;
		tracker[iSlice].GPUParameters()->fStaticStartingTracklets = 1;
		tracker[iSlice].GPUParameters()->fGPUError = 0;
		tracker[iSlice].GPUParameters()->fGPUSchedCollisions = 0;
#ifdef HLTCA_GPU_SCHED_FIXED_START
		tracker[iSlice].GPUParameters()->fNextTracklet = fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount * HLTCA_GPU_THREAD_COUNT;
#else
		tracker[iSlice].GPUParameters()->fNextTracklet = 0;
#endif

		CUDA_FAILED_MSG(cudaMemcpy(fGpuTracker[iSlice].CommonMemory(), tracker[iSlice].CommonMemory(), tracker[iSlice].CommonMemorySize(), cudaMemcpyHostToDevice));
		CUDA_FAILED_MSG(cudaMemcpy(fGpuTracker[iSlice].SliceDataMemory(), tracker[iSlice].SliceDataMemory(), tracker[iSlice].SliceDataMemorySize(), cudaMemcpyHostToDevice));
#ifdef SLICE_DATA_EXTERN_ROWS
		CUDA_FAILED_MSG(cudaMemcpy(fGpuTracker[iSlice].SliceDataRows(), tracker[iSlice].SliceDataRows(), HLTCA_ROW_COUNT * sizeof(AliHLTTPCCARow), cudaMemcpyHostToDevice));
#endif
		if (!fOptionSimpleSched)
		{
			CUDA_FAILED_MSG(cudaMemset(fGpuTracker[iSlice].RowBlockPos(), 0, sizeof(int4) * 2 * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1)));
			CUDA_FAILED_MSG(cudaMemset(fGpuTracker[iSlice].RowBlockTracklets(), -1, sizeof(int) * (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * HLTCA_GPU_MAX_TRACKLETS * 2));
		}
	}
#ifdef HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE
	if (CUDA_FAILED_MSG(cudaMalloc(&fGpuTracker[0].fStageAtSync, 100000000))) return(1);
	CUDA_FAILED_MSG(cudaMemset(fGpuTracker[0].fStageAtSync, 0, 100000000));
#endif
	CUDA_FAILED_MSG(cudaMemcpyToSymbol(gAliHLTTPCCATracker, fGpuTracker, sizeof(AliHLTTPCCATracker) * fSliceCountLocal));
	if (fDebugLevel >= 1)
	{
		static int showMemInfo = true;
		if (showMemInfo)
			printf("GPU Memory used: %d bytes\n", (int) (tmpMem - (char*) fGPUMemory));
		showMemInfo = false;
	}

	CUDASync("Initialization");
	StandalonePerfTime(1);

	if (fDebugLevel >= 5) printf("Running GPU Neighbours Finder\n");
	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		AliHLTTPCCAProcess<AliHLTTPCCANeighboursFinder> <<<fGpuTracker[iSlice].Param().NRows(), 256>>>(iSlice);
		if (CUDASync("Neighbours finder")) return 1;
	}

	StandalonePerfTime(2);

	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 4)
		{
			*fOutFile << "Neighbours Finder:" << endl;
			CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].SliceDataMemory(), fGpuTracker[iSlice].SliceDataMemory(), tracker[iSlice].SliceDataMemorySize(), cudaMemcpyDeviceToHost));
			tracker[iSlice].DumpLinks(*fOutFile);
		}

		if (fDebugLevel >= 5) printf("Running GPU Neighbours Cleaner\n");
		AliHLTTPCCAProcess<AliHLTTPCCANeighboursCleaner> <<<fGpuTracker[iSlice].Param().NRows()-2, 256>>>(iSlice);
		if (CUDASync("Neighbours Cleaner")) return 1;
	}

	StandalonePerfTime(3);

	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 4)
		{
			*fOutFile << "Neighbours Cleaner:" << endl;
			CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].SliceDataMemory(), fGpuTracker[iSlice].SliceDataMemory(), tracker[iSlice].SliceDataMemorySize(), cudaMemcpyDeviceToHost));
			tracker[iSlice].DumpLinks(*fOutFile);
		}

		if (fDebugLevel >= 5) printf("Running GPU Start Hits Finder\n");
		AliHLTTPCCAProcess<AliHLTTPCCAStartHitsFinder> <<<fGpuTracker[iSlice].Param().NRows()-4, 256>>>(iSlice);
		if (CUDASync("Start Hits Finder")) return 1;
	}

	StandalonePerfTime(4);

#ifdef HLTCA_GPU_SORT_STARTHITS
	if (!fOptionSimpleSched)
	{
		for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
		{
			if (fDebugLevel >= 5) printf("Running GPU Start Hits Sorter\n");
			AliHLTTPCCAProcess<AliHLTTPCCAStartHitsSorter> <<<30, 256>>>(iSlice);
			if (CUDASync("Start Hits Sorter")) return 1;
		}
	}
#endif

	StandalonePerfTime(5);

	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 5) printf("Obtaining Number of Start Hits from GPU: ");
		CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), tracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
		if (fDebugLevel >= 5) printf("%d\n", *tracker[iSlice].NTracklets());
		else if (fDebugLevel >= 2) printf("%3d ", *tracker[iSlice].NTracklets());

#ifdef HLTCA_GPU_SORT_STARTHITS
		if (!fOptionSimpleSched && fDebugLevel >= 4)
		{
			*fOutFile << "Start Hits Tmp: (" << *tracker[iSlice].NTracklets() << ")" << endl;
			CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].TrackletStartHits(), fGpuTracker[iSlice].TrackletTmpStartHits(), tracker[iSlice].NHitsTotal() * sizeof(AliHLTTPCCAHit), cudaMemcpyDeviceToHost));
			tracker[iSlice].DumpStartHits(*fOutFile);
			uint3* tmpMemory = (uint3*) malloc(sizeof(uint3) * tracker[iSlice].Param().NRows());
			CUDA_FAILED_MSG(cudaMemcpy(tmpMemory, fGpuTracker[iSlice].RowStartHitCountOffset(), tracker[iSlice].Param().NRows() * sizeof(uint3), cudaMemcpyDeviceToHost));
			*fOutFile << "Start Hits Sort Vector:" << std::endl;
			for (int i = 0;i < tracker[iSlice].Param().NRows();i++)
			{
				*fOutFile << "Row: " << i << ", Len: " << tmpMemory[i].x << ", Offset: " << tmpMemory[i].y << ", New Offset: " << tmpMemory[i].z << std::endl;
			}
			free(tmpMemory);
		}
#endif

		if (fDebugLevel >= 4)
		{
			*fOutFile << "Start Hits: (" << *tracker[iSlice].NTracklets() << ")" << endl;
			CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].HitMemory(), fGpuTracker[iSlice].HitMemory(), tracker[iSlice].HitMemorySize(), cudaMemcpyDeviceToHost));
			tracker[iSlice].DumpStartHits(*fOutFile);
		}

		/*tracker[iSlice].RunNeighboursFinder();
		tracker[iSlice].RunNeighboursCleaner();
		tracker[iSlice].RunStartHitsFinder();*/

		if (*tracker[iSlice].NTracklets() > HLTCA_GPU_MAX_TRACKLETS)
		{
			printf("HLTCA_GPU_MAX_TRACKLETS constant insuffisant\n");
			return(1);
		}

		CUDA_FAILED_MSG(cudaMemset(fGpuTracker[iSlice].SliceDataHitWeights(), 0, tracker[iSlice].NHitsTotal() * sizeof(int)));
		//tracker[iSlice].ClearSliceDataHitWeights();
		//CUDA_FAILED_MSG(cudaMemcpy(fGpuTracker[iSlice].SliceDataHitWeights(), tracker[iSlice].SliceDataHitWeights(), tracker[iSlice].NHitsTotal() * sizeof(int), cudaMemcpyHostToDevice));

		if (fDebugLevel >= 5) printf("Initialising Slice Tracker (CPU) Track Memory\n");
		tracker[iSlice].TrackMemory() = reinterpret_cast<char*> ( new uint4 [ fGpuTracker[iSlice].TrackMemorySize()/sizeof( uint4 ) + 100] );
		tracker[iSlice].SetPointersTracks( *tracker[iSlice].NTracklets(), tracker[iSlice].NHitsTotal() );

		/*tracker[iSlice].RunTrackletConstructor();
		if (fDebugLevel >= 4)
		{
			*fOutFile << "Tracklet Hits:" << endl;
			tracker[iSlice].DumpTrackletHits(*fOutFile);
		}*/
	}

	StandalonePerfTime(6);

	if (fOptionSimpleSched)
	{
		StandalonePerfTime(7);
		AliHLTTPCCATrackletConstructorNewGPUSimple<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>();
		if (CUDASync("Tracklet Constructor Simple Sched")) return 1;
	}
	else
	{
#ifdef HLTCA_GPU_PREFETCHDATA
		for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
		{
			if (tracker[iSlice].Data().GPUSharedDataReq() * sizeof(ushort_v) > ALIHLTTPCCATRACKLET_CONSTRUCTOR_TEMP_MEM / 4 * sizeof(uint4))
			{
				printf("Insufficiant GPU shared Memory, required: %d, available %d\n", tracker[iSlice].Data().GPUSharedDataReq() * sizeof(ushort_v), ALIHLTTPCCATRACKLET_CONSTRUCTOR_TEMP_MEM / 4 * sizeof(uint4));
				return(1);
			}
			if (fDebugLevel >= 1)
			{
				static int infoShown = 0;
				if (!infoShown)
				{
					printf("GPU Shared Memory Cache Size: %d\n", 2 * tracker[iSlice].Data().GPUSharedDataReq() * sizeof(ushort_v));
					infoShown = 1;
				}
			}
		}
#endif

		if (fDebugLevel >= 5) printf("Running GPU Tracklet Constructor\n");

		for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
		{
			AliHLTTPCCATrackletConstructorInit<<<*tracker[iSlice].NTracklets() / HLTCA_GPU_THREAD_COUNT + 1, HLTCA_GPU_THREAD_COUNT>>>(iSlice);
			if (CUDASync("Tracklet Initializer")) return 1;
			DumpRowBlocks(tracker, iSlice);
		}
		StandalonePerfTime(7);

#ifdef HLTCA_GPU_SCHED_HOST_SYNC
		for (int i = 0;i < (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * 2;i++)
		{
			if (fDebugLevel >= 4) *fOutFile << "Scheduled Tracklet Constructor Iteration " << i << std::endl;
			AliHLTTPCCATrackletConstructorNewGPU<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>();
			if (CUDASync("Tracklet Constructor (new)")) return 1;
			for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
			{
				AliHLTTPCCATrackletConstructorUpdateRowBlockPos<<<HLTCA_GPU_BLOCK_COUNT, (tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1) * 2 / HLTCA_GPU_BLOCK_COUNT + 1>>>(iSlice);
				if (CUDASync("Tracklet Constructor (update)")) return 1;
				DumpRowBlocks(tracker, iSlice, false);
			}
		}
#else
		AliHLTTPCCATrackletConstructorNewGPU<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>();
		if (CUDASync("Tracklet Constructor (new)")) return 1;
		for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
		{
			DumpRowBlocks(tracker, iSlice, false);
		}
#endif
	}
	
	StandalonePerfTime(8);

	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 4)
		{
			*fOutFile << "Tracklet Hits:" << endl;
			CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), tracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugLevel >= 5)
			{
				printf("Obtained %d tracklets\n", *tracker[iSlice].NTracklets());
			}
			CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].Tracklets(), fGpuTracker[iSlice].Tracklets(), fGpuTracker[iSlice].TrackMemorySize(), cudaMemcpyDeviceToHost));
			tracker[iSlice].DumpTrackletHits(*fOutFile);
		}

		//tracker[iSlice].RunTrackletSelector();
		
		/*nThreads = HLTCA_GPU_THREAD_COUNT;
		nBlocks = *tracker[iSlice].NTracklets()/nThreads + 1;
		if( nBlocks<30 ){
		  nBlocks = HLTCA_GPU_BLOCK_COUNT;  
		  nThreads = *tracker[iSlice].NTracklets()/nBlocks+1;
		  nThreads = (nThreads/32+1)*32;
		}
		if (nThreads > fCudaDeviceProp.maxThreadsPerBlock || (nThreads) * HLTCA_GPU_REGS > fCudaDeviceProp.regsPerBlock)
		{
			printf("Invalid CUDA Kernel Configuration %d blocks %d threads\n", nBlocks, nThreads);
			return(1);
		}

		if (fDebugLevel >= 5) printf("Running GPU Tracklet Selector\n");
		if (!fOptionSingleBlock)
		{
			AliHLTTPCCAProcessMulti<AliHLTTPCCATrackletSelector><<<nBlocks, nThreads>>>(fSliceCountLocal);
		}
		else
		{
			AliHLTTPCCAProcess<AliHLTTPCCATrackletSelector><<<1, *tracker[iSlice].NTracklets()>>>(iSlice);
		}
		if (CUDASync("Tracklet Selector")) return 1;*/
	}
	AliHLTTPCCAProcessMulti<AliHLTTPCCATrackletSelector><<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>(fSliceCountLocal);
	if (CUDASync("Tracklet Selector")) return 1;

	StandalonePerfTime(9);

	for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
	{
		if (fDebugLevel >= 5) printf("Transfering Tracks from GPU to Host ");
		CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), tracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
		if (tracker[iSlice].GPUParameters()->fGPUError)
		{
			printf("GPU Tracker returned Error Code %d\n", tracker[iSlice].GPUParameters()->fGPUError);
			return(1);
		}
		if (tracker[iSlice].GPUParameters()->fGPUSchedCollisions)
			printf("Collisions: %d\n", tracker[iSlice].GPUParameters()->fGPUSchedCollisions);
		if (fDebugLevel >= 5) printf("%d / %d\n", *tracker[iSlice].NTracks(), *tracker[iSlice].NTrackHits());
		size = sizeof(AliHLTTPCCATrack) * *tracker[iSlice].NTracks();
		CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].Tracks(), fGpuTracker[iSlice].Tracks(), size, cudaMemcpyDeviceToHost));
		size = sizeof(AliHLTTPCCAHitId) * *tracker[iSlice].NTrackHits();
		if (CUDA_FAILED_MSG(cudaMemcpy(tracker[iSlice].TrackHits(), fGpuTracker[iSlice].TrackHits(), size, cudaMemcpyDeviceToHost)))
		{
			printf("CUDA Error during Reconstruction\n");
			return(1);
		}

		if (fDebugLevel >= 4)
		{
			*fOutFile << "Track Hits: (" << *tracker[iSlice].NTracks() << ")" << endl;
			tracker[iSlice].DumpTrackHits(*fOutFile);
		}

		if (fDebugLevel >= 5) printf("Running WriteOutput\n");
		tracker[iSlice].WriteOutput();
	}

	StandalonePerfTime(10);

	if (fDebugLevel >= 5) printf("GPU Reconstruction finished\n");

#ifdef HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE
	char* stageAtSync = (char*) malloc(100000000);
	CUDA_FAILED_MSG(cudaMemcpy(stageAtSync, fGpuTracker[0].fStageAtSync, 100 * 1000 * 1000, cudaMemcpyDeviceToHost));
	cudaFree(fGpuTracker[0].fStageAtSync);

	FILE* fp = fopen("profile.txt", "w+");
	int nEmptySync = 0, fEmpty;

	for (int i = 0;i < 100000000 / HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT;i += HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT)
	{
		fEmpty = 1;
		for (int j = 0;j < HLTCA_GPU_BLOCK_COUNT * HLTCA_GPU_THREAD_COUNT;j++)
		{
			fprintf(fp, "%d\t", stageAtSync[i + j]);
			if (stageAtSync[i + j]) fEmpty = 0;
		}
		fprintf(fp, "\n");
		if (fEmpty) nEmptySync++;
		else nEmptySync = 0;
		if (nEmptySync == HLTCA_GPU_SCHED_ROW_STEP + 2) break;
	}

	fclose(fp);
	free(stageAtSync);
#endif

#ifdef HLTCA_STANDALONE
	if (fDebugLevel >= 6)
	{
		for (int iSlice = 0;iSlice < fSliceCountLocal;iSlice++)
		{
			std::ofstream tmpout("tmpdebug.out");
			int* GPUDebug = (int*) malloc(100 * 1024 * 1024);
			CUDA_FAILED_MSG(cudaMemcpy(GPUDebug, fGpuTracker[iSlice].fGPUDebugMem, 100 * 1024 * 1024, cudaMemcpyDeviceToHost));
			free(GPUDebug);
			cudaFree(fGpuTracker[iSlice].fGPUDebugMem);
			tmpout.close();
		}
	}
#endif
	
	return(0);
}

int AliHLTTPCCAGPUTracker::ExitGPU()
{
	if (fGPUMemory)
	{
		cudaFree(fGPUMemory);
		fGPUMemory = NULL;
	}
	if (fGpuTracker)
	{
#ifndef CUDA_DEVICE_EMULATION
		delete[] fGpuTracker;
#endif
		for (int i = 0;i < 2;i++)
		{
			cudaStreamDestroy(((cudaStream_t*) pCudaStreams)[i]);
		}
		free(pCudaStreams);
		fGpuTracker = NULL;

	}
	cudaThreadExit();
	return(0);
}
