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
#ifdef HLTCA_GPU_TEXTURE_FETCH
texture<ushort2, 1, cudaReadModeElementType> gAliTexRefu2;
texture<unsigned short, 1, cudaReadModeElementType> gAliTexRefu;
texture<signed short, 1, cudaReadModeElementType> gAliTexRefs;
#endif

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

#ifndef HLTCA_STANDALONE
#include "AliHLTDefinitions.h"
#include "AliHLTSystem.h"
#endif

ClassImp( AliHLTTPCCAGPUTracker )

int AliHLTTPCCAGPUTracker::InitGPU(int sliceCount, int forceDeviceID)
{
	//Find best CUDA device, initialize and allocate memory

	cudaDeviceProp fCudaDeviceProp;

#ifndef CUDA_DEVICE_EMULATION
	int count, bestDevice = -1, bestDeviceSpeed = 0;
	if (CudaFailedMsg(cudaGetDeviceCount(&count)))
	{
		HLTError("Error getting CUDA Device Count");
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
		HLTWarning("No CUDA Device available, aborting CUDA Initialisation");
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
	HLTError( "Unsupported CUDA Device" );
	  return(1);
  }

  if (CudaFailedMsg(cudaSetDevice(cudaDevice)))
  {
	  HLTError("Could not set CUDA Device!");
	  return(1);
  }

  if (fgkNSlices * AliHLTTPCCATracker::CommonMemorySize() > HLTCA_GPU_COMMON_MEMORY)
  {
	  HLTError("Insufficiant Common Memory");
	  cudaThreadExit();
	  return(1);
  }

  if (fgkNSlices * (HLTCA_ROW_COUNT + 1) * sizeof(AliHLTTPCCARow) > HLTCA_GPU_ROWS_MEMORY)
  {
	  HLTError("Insufficiant Row Memory");
	  cudaThreadExit();
	  return(1);
  }

  fGPUMemSize = HLTCA_GPU_ROWS_MEMORY + HLTCA_GPU_COMMON_MEMORY + sliceCount * (HLTCA_GPU_SLICE_DATA_MEMORY + HLTCA_GPU_GLOBAL_MEMORY);
  if (fGPUMemSize > fCudaDeviceProp.totalGlobalMem || CudaFailedMsg(cudaMalloc(&fGPUMemory, (size_t) fGPUMemSize)))
  {
	  HLTError("CUDA Memory Allocation Error");
	  cudaThreadExit();
	  return(1);
  }
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
  HLTInfo("CUDA Initialisation successfull");

  //Don't run constructor / destructor here, this will be just local memcopy of Tracker in GPU Memory
  if (sizeof(AliHLTTPCCATracker) * sliceCount > HLTCA_GPU_TRACKER_OBJECT_MEMORY)
  {
	  HLTError("Insufficiant Tracker Object Memory");
	  return(1);
  }
  fSliceCount = sliceCount;
  fGpuTracker = (AliHLTTPCCATracker*) TrackerMemory(fHostLockedMemory, 0);

  for (int i = 0;i < fgkNSlices;i++)
  {
    fSlaveTrackers[i].SetGPUTracker();
	fSlaveTrackers[i].SetGPUTrackerCommonMemory((char*) CommonMemory(fHostLockedMemory, i));
	fSlaveTrackers[i].pData()->SetGPUSliceDataMemory(SliceDataMemory(fHostLockedMemory, i), RowMemory(fHostLockedMemory, i));
  }

  fpCudaStreams = malloc(CAMath::Max(3, fSliceCount) * sizeof(cudaStream_t));
  cudaStream_t* const cudaStreams = (cudaStream_t*) fpCudaStreams;
  for (int i = 0;i < CAMath::Max(3, fSliceCount);i++)
  {
	if (CudaFailedMsg(cudaStreamCreate(&cudaStreams[i])))
	{
		HLTError("Error creating CUDA Stream");
		return(1);
	}
  }

#if defined(HLTCA_STANDALONE) & !defined(CUDA_DEVICE_EMULATION)
  if (fDebugLevel < 2)
  {
	  //Do one initial run for Benchmark reasons
	  const int useDebugLevel = fDebugLevel;
	  fDebugLevel = 0;
	  AliHLTTPCCAClusterData tmpCluster;
	  AliHLTTPCCASliceOutput *tmpOutput = NULL;
	  AliHLTTPCCAParam tmpParam;
	  tmpParam.SetNRows(HLTCA_ROW_COUNT);
	  fSlaveTrackers[0].SetParam(tmpParam);
	  Reconstruct(&tmpOutput, &tmpCluster, 0, 1);
	  free(tmpOutput);
	  fDebugLevel = useDebugLevel;
  }
#endif
  return(0);
}

template <class T> inline T* AliHLTTPCCAGPUTracker::alignPointer(T* ptr, int alignment)
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

bool AliHLTTPCCAGPUTracker::CudaFailedMsg(cudaError_t error)
{
	//Check for CUDA Error and in the case of an error display the corresponding error string
	if (error == cudaSuccess) return(false);
	HLTWarning("CUDA Error: %d / %s", error, cudaGetErrorString(error));
	return(true);
}

int AliHLTTPCCAGPUTracker::CUDASync(char* state)
{
	//Wait for CUDA-Kernel to finish and check for CUDA errors afterwards

	if (fDebugLevel == 0) return(0);
	cudaError cuErr;
	cuErr = cudaGetLastError();
	if (cuErr != cudaSuccess)
	{
		HLTError("Cuda Error %s while invoking kernel (%s)", cudaGetErrorString(cuErr), state);
		return(1);
	}
	if (CudaFailedMsg(cudaThreadSynchronize()))
	{
		HLTError("CUDA Error while synchronizing (%s)", state);
		return(1);
	}
	if (fDebugLevel >= 5) HLTInfo("CUDA Sync Done");
	return(0);
}

void AliHLTTPCCAGPUTracker::SetDebugLevel(const int dwLevel, std::ostream* const NewOutFile)
{
	//Set Debug Level and Debug output File if applicable
	fDebugLevel = dwLevel;
	if (NewOutFile) fOutFile = NewOutFile;
}

int AliHLTTPCCAGPUTracker::SetGPUTrackerOption(char* OptionName, int OptionValue)
{
	//Set a specific GPU Tracker Option
	{
		HLTError("Unknown Option: %s", OptionName);
		return(1);
	}
	//No Options used at the moment
	//return(0);
}

#ifdef HLTCA_STANDALONE
void AliHLTTPCCAGPUTracker::StandalonePerfTime(int iSlice, int i)
{
  //Run Performance Query for timer i of slice iSlice
  if (fDebugLevel >= 1)
  {
	  AliHLTTPCCAStandaloneFramework::StandaloneQueryTime( fSlaveTrackers[iSlice].PerfTimer(i));
  }
}
#else
void AliHLTTPCCAGPUTracker::StandalonePerfTime(int /*iSlice*/, int /*i*/) {}
#endif

void AliHLTTPCCAGPUTracker::DumpRowBlocks(AliHLTTPCCATracker* tracker, int iSlice, bool check)
{
	//Dump Rowblocks to File
	if (fDebugLevel >= 4)
	{
		*fOutFile << "RowBlock Tracklets" << std::endl;
	
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
				rowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].y << "/" << rowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].x << endl << "Phase 1: ";
			for (int j = 0;j < rowBlockPos[i].x;j++)
			{
				//Use Tracker Object to calculate Offset instead of fGpuTracker, since *fNTracklets of fGpuTracker points to GPU Mem!
				*fOutFile << rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] << ", ";
				if (check && rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] != k)
				{
					HLTError("Wrong starting Row Block %d, entry %d, is %d, should be %d", i, j, rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j], k);
				}
				k++;
				if (rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(0, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] == -1)
				{
					HLTError("Error, -1 Tracklet found");
				}
			}
			*fOutFile << endl << "Phase 2: ";
			for (int j = 0;j < rowBlockPos[tracker[iSlice].Param().NRows() / HLTCA_GPU_SCHED_ROW_STEP + 1 + i].x;j++)
			{
				*fOutFile << rowBlockTracklets[(tracker[iSlice].RowBlockTracklets(1, i) - tracker[iSlice].RowBlockTracklets(0, 0)) + j] << ", ";
			}
			*fOutFile << endl;
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

int AliHLTTPCCAGPUTracker::Reconstruct(AliHLTTPCCASliceOutput** pOutput, AliHLTTPCCAClusterData* pClusterData, int firstSlice, int sliceCountLocal)
{
	//Primary reconstruction function
	cudaStream_t* const cudaStreams = (cudaStream_t*) fpCudaStreams;

	if (sliceCountLocal == -1) sliceCountLocal = this->fSliceCount;

	if (sliceCountLocal * sizeof(AliHLTTPCCATracker) > HLTCA_GPU_TRACKER_CONSTANT_MEM)
	{
		HLTError("Insuffissant constant memory (Required %d, Available %d, Tracker %d, Param %d, SliceData %d)", sliceCountLocal * (int) sizeof(AliHLTTPCCATracker), (int) HLTCA_GPU_TRACKER_CONSTANT_MEM, (int) sizeof(AliHLTTPCCATracker), (int) sizeof(AliHLTTPCCAParam), (int) sizeof(AliHLTTPCCASliceData));
		return(1);
	}

	if (fDebugLevel >= 4)
	{
		for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
		{
			*fOutFile << endl << endl << "Slice: " << fSlaveTrackers[firstSlice + iSlice].Param().ISlice() << endl;
		}
	}

	memcpy(fGpuTracker, &fSlaveTrackers[firstSlice], sizeof(AliHLTTPCCATracker) * sliceCountLocal);

	if (fDebugLevel >= 2) HLTInfo("Running GPU Tracker (Slices %d to %d)", fSlaveTrackers[firstSlice].Param().ISlice(), fSlaveTrackers[firstSlice + sliceCountLocal].Param().ISlice());
	if (fDebugLevel >= 5) HLTInfo("Allocating GPU Tracker memory and initializing constants");
	
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		//Make this a GPU Tracker
		fGpuTracker[iSlice].SetGPUTracker();
		fGpuTracker[iSlice].SetGPUTrackerCommonMemory((char*) CommonMemory(fGPUMemory, iSlice));
		fGpuTracker[iSlice].pData()->SetGPUSliceDataMemory(SliceDataMemory(fGPUMemory, iSlice), RowMemory(fGPUMemory, iSlice));
		fGpuTracker[iSlice].pData()->SetPointers(&pClusterData[iSlice], false);

		//Set Pointers to GPU Memory
		char* tmpMem = (char*) GlobalMemory(fGPUMemory, iSlice);

		if (fDebugLevel >= 5) HLTInfo("Initialising GPU Hits Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerHitsMemory(tmpMem, pClusterData[iSlice].NumberOfClusters());
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fDebugLevel >= 5) HLTInfo("Initialising GPU Tracklet Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTrackletsMemory(tmpMem, HLTCA_GPU_MAX_TRACKLETS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets()*/);
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

		if (fDebugLevel >= 5) HLTInfo("Initialising GPU Track Memory");
		tmpMem = fGpuTracker[iSlice].SetGPUTrackerTracksMemory(tmpMem, HLTCA_GPU_MAX_TRACKS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets()*/, pClusterData[iSlice].NumberOfClusters());
		tmpMem = alignPointer(tmpMem, 1024 * 1024);

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
		fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount = HLTCA_GPU_BLOCK_COUNT * (iSlice + 1) / sliceCountLocal - HLTCA_GPU_BLOCK_COUNT * (iSlice) / sliceCountLocal;
		if (fDebugLevel >= 5) HLTInfo("Blocks for Slice %d: %d", iSlice, fGpuTracker[iSlice].GPUParametersConst()->fGPUFixedBlockCount);
		fGpuTracker[iSlice].GPUParametersConst()->fGPUiSlice = iSlice;
		fGpuTracker[iSlice].GPUParametersConst()->fGPUnSlices = sliceCountLocal;
		fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError = 0;
		fGpuTracker[iSlice].pData()->SetGPUTextureBase(fGpuTracker[0].Data().Memory());
	}

#ifdef HLTCA_GPU_TEXTURE_FETCH
		cudaChannelFormatDesc channelDescu2 = cudaCreateChannelDesc<ushort2>();
		size_t offset;
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefu2, fGpuTracker[0].Data().Memory(), &channelDescu2, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture (Offset %d)", (int) offset);
			return(1);
		}
		cudaChannelFormatDesc channelDescu = cudaCreateChannelDesc<unsigned short>();
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefu, fGpuTracker[0].Data().Memory(), &channelDescu, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture (Offset %d)", (int) offset);
			return(1);
		}
		cudaChannelFormatDesc channelDescs = cudaCreateChannelDesc<signed short>();
		if (CudaFailedMsg(cudaBindTexture(&offset, &gAliTexRefs, fGpuTracker[0].Data().Memory(), &channelDescs, sliceCountLocal * HLTCA_GPU_SLICE_DATA_MEMORY)) || offset)
		{
			HLTError("Error binding CUDA Texture (Offset %d)", (int) offset);
			return(1);
		}
#endif

	//Copy Tracker Object to GPU Memory
	if (fDebugLevel >= 5) HLTInfo("Copying Tracker objects to GPU");
#ifdef HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE
	if (CudaFailedMsg(cudaMalloc(&fGpuTracker[0].fStageAtSync, 100000000))) return(1);
	CudaFailedMsg(cudaMemset(fGpuTracker[0].fStageAtSync, 0, 100000000));
#endif
	CudaFailedMsg(cudaMemcpyToSymbolAsync(gAliHLTTPCCATracker, fGpuTracker, sizeof(AliHLTTPCCATracker) * sliceCountLocal, 0, cudaMemcpyHostToDevice, cudaStreams[0]));

	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		StandalonePerfTime(firstSlice + iSlice, 0);

		//Initialize GPU Slave Tracker
		if (fDebugLevel >= 5) HLTInfo("Creating Slice Data");
		fSlaveTrackers[firstSlice + iSlice].pData()->SetGPUSliceDataMemory(SliceDataMemory(fHostLockedMemory, iSlice), RowMemory(fHostLockedMemory, firstSlice + iSlice));
		fSlaveTrackers[firstSlice + iSlice].ReadEvent(&pClusterData[iSlice]);
		if (fSlaveTrackers[firstSlice + iSlice].Data().MemorySize() > HLTCA_GPU_SLICE_DATA_MEMORY)
		{
			HLTError("Insufficiant Slice Data Memory");
			return(1);
		}

		/*if (fSlaveTrackers[firstSlice + iSlice].CheckEmptySlice())
		{
			if (fDebugLevel >= 5) HLTInfo("Slice Empty, not running GPU Tracker");
			if (sliceCountLocal == 1)
				return(0);
		}*/

		//Initialize temporary memory where needed
		if (fDebugLevel >= 5) HLTInfo("Copying Slice Data to GPU and initializing temporary memory");		
		PreInitRowBlocks<<<30, 256, 0, cudaStreams[2]>>>(fGpuTracker[iSlice].RowBlockPos(), fGpuTracker[iSlice].RowBlockTracklets(), fGpuTracker[iSlice].Data().HitWeights(), fSlaveTrackers[firstSlice + iSlice].Data().NumberOfHitsPlusAlign());

		//Copy Data to GPU Global Memory
		CudaFailedMsg(cudaMemcpyAsync(fGpuTracker[iSlice].CommonMemory(), fSlaveTrackers[firstSlice + iSlice].CommonMemory(), fSlaveTrackers[firstSlice + iSlice].CommonMemorySize(), cudaMemcpyHostToDevice, cudaStreams[iSlice & 1]));
		CudaFailedMsg(cudaMemcpyAsync(fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyHostToDevice, cudaStreams[iSlice & 1]));
		CudaFailedMsg(cudaMemcpyAsync(fGpuTracker[iSlice].SliceDataRows(), fSlaveTrackers[firstSlice + iSlice].SliceDataRows(), (HLTCA_ROW_COUNT + 1) * sizeof(AliHLTTPCCARow), cudaMemcpyHostToDevice, cudaStreams[iSlice & 1]));

		if (fDebugLevel >= 4)
		{
			if (fDebugLevel >= 5) HLTInfo("Allocating Debug Output Memory");
			fSlaveTrackers[firstSlice + iSlice].TrackletMemory() = reinterpret_cast<char*> ( new uint4 [ fGpuTracker[iSlice].TrackletMemorySize()/sizeof( uint4 ) + 100] );
			fSlaveTrackers[firstSlice + iSlice].SetPointersTracklets( HLTCA_GPU_MAX_TRACKLETS );
			fSlaveTrackers[firstSlice + iSlice].HitMemory() = reinterpret_cast<char*> ( new uint4 [ fGpuTracker[iSlice].HitMemorySize()/sizeof( uint4 ) + 100] );
			fSlaveTrackers[firstSlice + iSlice].SetPointersHits( pClusterData[iSlice].NumberOfClusters() );
		}
		
		if (CUDASync("Initialization")) return(1);
		StandalonePerfTime(firstSlice + iSlice, 1);

		if (fDebugLevel >= 5) HLTInfo("Running GPU Neighbours Finder");
		AliHLTTPCCAProcess<AliHLTTPCCANeighboursFinder> <<<fSlaveTrackers[firstSlice + iSlice].Param().NRows(), 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);

		if (CUDASync("Neighbours finder")) return 1;

		StandalonePerfTime(firstSlice + iSlice, 2);

		if (fDebugLevel >= 4)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyDeviceToHost));
			fSlaveTrackers[firstSlice + iSlice].DumpLinks(*fOutFile);
		}

		if (fDebugLevel >= 5) HLTInfo("Running GPU Neighbours Cleaner");
		AliHLTTPCCAProcess<AliHLTTPCCANeighboursCleaner> <<<fSlaveTrackers[firstSlice + iSlice].Param().NRows()-2, 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);
		if (CUDASync("Neighbours Cleaner")) return 1;

		StandalonePerfTime(firstSlice + iSlice, 3);

		if (fDebugLevel >= 4)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Data().Memory(), fGpuTracker[iSlice].Data().Memory(), fSlaveTrackers[firstSlice + iSlice].Data().GpuMemorySize(), cudaMemcpyDeviceToHost));
			fSlaveTrackers[firstSlice + iSlice].DumpLinks(*fOutFile);
		}

		if (fDebugLevel >= 5) HLTInfo("Running GPU Start Hits Finder");
		AliHLTTPCCAProcess<AliHLTTPCCAStartHitsFinder> <<<fSlaveTrackers[firstSlice + iSlice].Param().NRows()-6, 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);
		if (CUDASync("Start Hits Finder")) return 1;

		StandalonePerfTime(firstSlice + iSlice, 4);

		if (fDebugLevel >= 5) HLTInfo("Running GPU Start Hits Sorter");
		AliHLTTPCCAProcess<AliHLTTPCCAStartHitsSorter> <<<30, 256, 0, cudaStreams[iSlice & 1]>>>(iSlice);
		if (CUDASync("Start Hits Sorter")) return 1;

		StandalonePerfTime(firstSlice + iSlice, 5);

		if (fDebugLevel >= 2)
		{
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugLevel >= 5) HLTInfo("Obtaining Number of Start Hits from GPU: %d", *fSlaveTrackers[firstSlice + iSlice].NTracklets());
			if (*fSlaveTrackers[firstSlice + iSlice].NTracklets() > HLTCA_GPU_MAX_TRACKLETS)
			{
				HLTError("HLTCA_GPU_MAX_TRACKLETS constant insuffisant");
				return(1);
			}
		}

		if (fDebugLevel >= 4)
		{
			*fOutFile << "Temporary ";
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].TrackletStartHits(), fGpuTracker[iSlice].TrackletTmpStartHits(), pClusterData[iSlice].NumberOfClusters() * sizeof(AliHLTTPCCAHitId), cudaMemcpyDeviceToHost));
			fSlaveTrackers[firstSlice + iSlice].DumpStartHits(*fOutFile);
			uint3* tmpMemory = (uint3*) malloc(sizeof(uint3) * fSlaveTrackers[firstSlice + iSlice].Param().NRows());
			CudaFailedMsg(cudaMemcpy(tmpMemory, fGpuTracker[iSlice].RowStartHitCountOffset(), fSlaveTrackers[firstSlice + iSlice].Param().NRows() * sizeof(uint3), cudaMemcpyDeviceToHost));
			*fOutFile << "Start Hits Sort Vector:" << std::endl;
			for (int i = 0;i < fSlaveTrackers[firstSlice + iSlice].Param().NRows();i++)
			{
				*fOutFile << "Row: " << i << ", Len: " << tmpMemory[i].x << ", Offset: " << tmpMemory[i].y << ", New Offset: " << tmpMemory[i].z << std::endl;
			}
			free(tmpMemory);

			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].HitMemory(), fGpuTracker[iSlice].HitMemory(), fSlaveTrackers[firstSlice + iSlice].HitMemorySize(), cudaMemcpyDeviceToHost));
			fSlaveTrackers[firstSlice + iSlice].DumpStartHits(*fOutFile);
		}

		StandalonePerfTime(firstSlice + iSlice, 6);
		
		fSlaveTrackers[firstSlice + iSlice].SetGPUTrackerTracksMemory((char*) TracksMemory(fHostLockedMemory, iSlice), HLTCA_GPU_MAX_TRACKS, pClusterData[iSlice].NumberOfClusters());
	}

	StandalonePerfTime(firstSlice, 7);
#ifdef HLTCA_GPU_PREFETCHDATA
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		if (fSlaveTrackers[firstSlice + iSlice].Data().GPUSharedDataReq() * sizeof(ushort_v) > ALIHLTTPCCATRACKLET_CONSTRUCTOR_TEMP_MEM / 4 * sizeof(uint4))
		{
			HLTError("Insufficiant GPU shared Memory, required: %d, available %d", fSlaveTrackers[firstSlice + iSlice].Data().GPUSharedDataReq() * sizeof(ushort_v), ALIHLTTPCCATRACKLET_CONSTRUCTOR_TEMP_MEM / 4 * sizeof(uint4));
			return(1);
		}
		if (fDebugLevel >= 1)
		{
			static int infoShown = 0;
			if (!infoShown)
			{
				HLTInfo("GPU Shared Memory Cache Size: %d", 2 * fSlaveTrackers[firstSlice + iSlice].Data().GPUSharedDataReq() * sizeof(ushort_v));
				infoShown = 1;
			}
		}
	}
#endif

	if (fDebugLevel >= 5) HLTInfo("Initialising Tracklet Constructor Scheduler");
	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
	{
		AliHLTTPCCATrackletConstructorInit<<<HLTCA_GPU_MAX_TRACKLETS /* *fSlaveTrackers[firstSlice + iSlice].NTracklets() */ / HLTCA_GPU_THREAD_COUNT + 1, HLTCA_GPU_THREAD_COUNT>>>(iSlice);
		if (CUDASync("Tracklet Initializer")) return 1;
		DumpRowBlocks(fSlaveTrackers, iSlice);
	}

	if (fDebugLevel >= 5) HLTInfo("Running GPU Tracklet Constructor");
	AliHLTTPCCATrackletConstructorNewGPU<<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT>>>();
	if (CUDASync("Tracklet Constructor (new)")) return 1;
	
	StandalonePerfTime(firstSlice, 8);

	if (fDebugLevel >= 4)
	{
		for (int iSlice = 0;iSlice < sliceCountLocal;iSlice++)
		{
			DumpRowBlocks(&fSlaveTrackers[firstSlice], iSlice, false);
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemory(), fGpuTracker[iSlice].CommonMemorySize(), cudaMemcpyDeviceToHost));
			if (fDebugLevel >= 5)
			{
				HLTInfo("Obtained %d tracklets", *fSlaveTrackers[firstSlice + iSlice].NTracklets());
			}
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].TrackletMemory(), fGpuTracker[iSlice].TrackletMemory(), fGpuTracker[iSlice].TrackletMemorySize(), cudaMemcpyDeviceToHost));
			CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].HitMemory(), fGpuTracker[iSlice].HitMemory(), fGpuTracker[iSlice].HitMemorySize(), cudaMemcpyDeviceToHost));
			fSlaveTrackers[firstSlice + iSlice].DumpTrackletHits(*fOutFile);
		}
	}

	for (int iSlice = 0;iSlice < sliceCountLocal;iSlice += HLTCA_GPU_TRACKLET_SELECTOR_SLICE_COUNT)
	{
		if (fDebugLevel >= 5) HLTInfo("Running HLT Tracklet selector (Slice %d to %d)", iSlice, iSlice + HLTCA_GPU_TRACKLET_SELECTOR_SLICE_COUNT);
		AliHLTTPCCAProcessMulti<AliHLTTPCCATrackletSelector><<<HLTCA_GPU_BLOCK_COUNT, HLTCA_GPU_THREAD_COUNT, 0, cudaStreams[iSlice]>>>(iSlice, CAMath::Min(HLTCA_GPU_TRACKLET_SELECTOR_SLICE_COUNT, sliceCountLocal - iSlice));
	}
	if (CUDASync("Tracklet Selector")) return 1;
	StandalonePerfTime(firstSlice, 9);

	CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + 0].CommonMemory(), fGpuTracker[0].CommonMemory(), fGpuTracker[0].CommonMemorySize(), cudaMemcpyDeviceToHost, cudaStreams[0]));
	for (int iSliceTmp = 0;iSliceTmp <= sliceCountLocal;iSliceTmp++)
	{
		if (iSliceTmp < sliceCountLocal)
		{
			int iSlice = iSliceTmp;
			if (fDebugLevel >= 5) HLTInfo("Transfering Tracks from GPU to Host");
			cudaStreamSynchronize(cudaStreams[iSlice]);
			CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + iSlice].Tracks(), fGpuTracker[iSlice].Tracks(), sizeof(AliHLTTPCCATrack) * *fSlaveTrackers[firstSlice + iSlice].NTracks(), cudaMemcpyDeviceToHost, cudaStreams[iSlice]));
			CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + iSlice].TrackHits(), fGpuTracker[iSlice].TrackHits(), sizeof(AliHLTTPCCAHitId) * *fSlaveTrackers[firstSlice + iSlice].NTrackHits(), cudaMemcpyDeviceToHost, cudaStreams[iSlice]));
			if (iSlice + 1 < sliceCountLocal)
				CudaFailedMsg(cudaMemcpyAsync(fSlaveTrackers[firstSlice + iSlice + 1].CommonMemory(), fGpuTracker[iSlice + 1].CommonMemory(), fGpuTracker[iSlice + 1].CommonMemorySize(), cudaMemcpyDeviceToHost, cudaStreams[iSlice + 1]));
		}

		if (iSliceTmp)
		{
			int iSlice = iSliceTmp - 1;
			cudaStreamSynchronize(cudaStreams[iSlice]);

			if (fDebugLevel >= 4)
			{
				CudaFailedMsg(cudaMemcpy(fSlaveTrackers[firstSlice + iSlice].Data().HitWeights(), fGpuTracker[iSlice].Data().HitWeights(), fSlaveTrackers[firstSlice + iSlice].Data().NumberOfHitsPlusAlign() * sizeof(int), cudaMemcpyDeviceToHost));
				fSlaveTrackers[firstSlice + iSlice].DumpHitWeights(*fOutFile);
				fSlaveTrackers[firstSlice + iSlice].DumpTrackHits(*fOutFile);
			}

			if (fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError)
			{
				HLTError("GPU Tracker returned Error Code %d", fSlaveTrackers[firstSlice + iSlice].GPUParameters()->fGPUError);
				return(1);
			}
			if (fDebugLevel >= 5) HLTInfo("Tracks Transfered: %d / %d", *fSlaveTrackers[firstSlice + iSlice].NTracks(), *fSlaveTrackers[firstSlice + iSlice].NTrackHits());

			fSlaveTrackers[firstSlice + iSlice].SetOutput(&pOutput[iSlice]);
			fSlaveTrackers[firstSlice + iSlice].WriteOutput();

			if (fDebugLevel >= 4)
			{
				delete[] fSlaveTrackers[firstSlice + iSlice].HitMemory();
				delete[] fSlaveTrackers[firstSlice + iSlice].TrackletMemory();
			}
		}
	}

	StandalonePerfTime(firstSlice, 10);

	if (fDebugLevel >= 5) HLTInfo("GPU Reconstruction finished");

#ifdef HLTCA_GPU_TRACKLET_CONSTRUCTOR_DO_PROFILE
	char* stageAtSync = (char*) malloc(100000000);
	CudaFailedMsg(cudaMemcpy(stageAtSync, fGpuTracker[0].fStageAtSync, 100 * 1000 * 1000, cudaMemcpyDeviceToHost));
	cudaFree(fGpuTracker[0].fStageAtSync);

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

	return(0);
}

int AliHLTTPCCAGPUTracker::InitializeSliceParam(int iSlice, AliHLTTPCCAParam &param)
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

int AliHLTTPCCAGPUTracker::ExitGPU()
{
	//Uninitialize CUDA
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
	return(0);
}
