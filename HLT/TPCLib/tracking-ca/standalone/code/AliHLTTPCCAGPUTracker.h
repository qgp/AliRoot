#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCATracker.h"

class AliHLTTPCCAGPUTracker
{
public:
	AliHLTTPCCAGPUTracker();
	~AliHLTTPCCAGPUTracker();

	int InitGPU();
	int Reconstruct(AliHLTTPCCATracker* tracker);
	int ExitGPU();

	void SetDebugLevel(int dwLevel);

private:
	AliHLTTPCCATracker gpuTracker;
	void* GPUMemory;

	int CUDASync();
	template <class T> T* alignPointer(T* ptr, int alignment);

	int DebugLevel;
	int GPUMemSize;
#ifdef HLTCA_GPUCODE
	bool CUDA_FAILED_MSG(cudaError_t error);
#endif
};