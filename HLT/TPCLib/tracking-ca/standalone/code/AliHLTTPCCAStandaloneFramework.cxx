// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
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


#include "AliHLTTPCCAStandaloneFramework.h"
#include "AliHLTTPCCATrackParam.h"
#include "AliHLTTPCCAMerger.h"
#include "AliHLTTPCCAMergerOutput.h"
#include "AliHLTTPCCADataCompressor.h"
#include "AliHLTTPCCAMath.h"
#include "AliHLTTPCCAClusterData.h"
#include "TStopwatch.h"

//If not building GPU Code then build dummy functions to link against
#ifndef BUILD_GPU
AliHLTTPCCAGPUTracker::AliHLTTPCCAGPUTracker() : gpuTracker(), fDebugLevel(0) {}
AliHLTTPCCAGPUTracker::~AliHLTTPCCAGPUTracker() {}
int AliHLTTPCCAGPUTracker::InitGPU() {return(0);}
//template <class T> inline T* AliHLTTPCCAGPUTracker::alignPointer(T* ptr, int alignment) {return(NULL);}
//bool AliHLTTPCCAGPUTracker::CUDA_FAILED_MSG(cudaError_t error) {return(true);}
//int AliHLTTPCCAGPUTracker::CUDASync() {return(1);}
void AliHLTTPCCAGPUTracker::SetDebugLevel(int dwLevel, std::ostream *NewOutFile) {};
int SetGPUTrackerOption(char* OptionName, int OptionValue) {};
int AliHLTTPCCAGPUTracker::Reconstruct(AliHLTTPCCATracker* tracker) {return(1);}
int AliHLTTPCCAGPUTracker::ExitGPU() {return(0);}
#endif

AliHLTTPCCAStandaloneFramework &AliHLTTPCCAStandaloneFramework::Instance()
{
  // reference to static object
  static AliHLTTPCCAStandaloneFramework gAliHLTTPCCAStandaloneFramework;
  return gAliHLTTPCCAStandaloneFramework;
}

AliHLTTPCCAStandaloneFramework::AliHLTTPCCAStandaloneFramework()
    : fMerger(), fStatNEvents( 0 ), fUseGPUTracker(false), fGPUDebugLevel(0)
{
  //* constructor

  for ( int i = 0; i < 20; i++ ) {
    fLastTime[i] = 0;
    fStatTime[i] = 0;
  }
}

AliHLTTPCCAStandaloneFramework::AliHLTTPCCAStandaloneFramework( const AliHLTTPCCAStandaloneFramework& )
    : fMerger(), fStatNEvents( 0 )
{
  //* dummy
}

const AliHLTTPCCAStandaloneFramework &AliHLTTPCCAStandaloneFramework::operator=( const AliHLTTPCCAStandaloneFramework& ) const
{
  //* dummy
  return *this;
}

AliHLTTPCCAStandaloneFramework::~AliHLTTPCCAStandaloneFramework()
{
  //* destructor
}


void AliHLTTPCCAStandaloneFramework::StartDataReading( int guessForNumberOfClusters )
{
  //prepare for reading of the event

  int sliceGuess = 2 * guessForNumberOfClusters / fgkNSlices;

  for ( int i = 0; i < fgkNSlices; i++ ) {
    fClusterData[i].StartReading( i, sliceGuess );
  }
}

void AliHLTTPCCAStandaloneFramework::FinishDataReading()
{
  // finish reading of the event

  /*static int event_number = 0;
  char filename[256];

  sprintf(filename, "events/event.%d.dump", event_number);
  printf("Dumping event into file %s\n", filename);
  std::ofstream outfile(filename, std::ofstream::binary);
  if (outfile.fail())
  {
    printf("Error opening event dump file\n");
    exit(1);
  }
  WriteEvent(outfile);
  if (outfile.fail())
  {
    printf("Error writing event dump file\n");
    exit(1);
  }
  outfile.close();

  sprintf(filename, "events/settings.%d.dump", event_number);
  outfile.open(filename);
  WriteSettings(outfile);
  outfile.close();

  event_number++;*/
  
  /*std::ifstream infile(filename, std::ifstream::binary);
  ReadEvent(infile);
  infile.close();*/

  for ( int i = 0; i < fgkNSlices; i++ ) {
    fClusterData[i].FinishReading();
  }
}


//int
void AliHLTTPCCAStandaloneFramework::ProcessEvent()
{
  // perform the event reconstruction

  fStatNEvents++;

  TStopwatch timer0;
  TStopwatch timer1;

  if (!fUseGPUTracker || fGPUDebugLevel >= 3)
  {
#ifdef HLTCA_STANDALONE
#pragma omp parallel for
#endif
	for ( int iSlice = 0; iSlice < fgkNSlices; iSlice++ ) {
	  fSliceTrackers[iSlice].ReadEvent( &( fClusterData[iSlice] ) );
      fSliceTrackers[iSlice].Reconstruct();
	}
	if (fGPUDebugLevel >= 2) printf("\n");
  }

  if (fUseGPUTracker)
  {
	  for ( int iSlice = 0; iSlice < fgkNSlices; iSlice++ ) {
	    fSliceTrackers[iSlice].ReadEvent( &( fClusterData[iSlice] ) );
		if (fGPUTracker.Reconstruct(&fSliceTrackers[iSlice]))
		{
			printf("Error during GPU Reconstruction!!!\n");
			//return(1);
		}
	  }
	  if (fGPUDebugLevel >= 2) printf("\n");
  }

  timer1.Stop();
  TStopwatch timer2;

  fMerger.Clear();
  fMerger.SetSliceParam( fSliceTrackers[0].Param() );

  for ( int i = 0; i < fgkNSlices; i++ ) {
    fMerger.SetSliceData( i, fSliceTrackers[i].Output() );
  }

  fMerger.Reconstruct();

  timer2.Stop();
  timer0.Stop();

  fLastTime[0] = timer0.CpuTime();
  fLastTime[1] = timer1.CpuTime();
  fLastTime[2] = timer2.CpuTime();

  for ( int i = 0; i < 3; i++ ) fStatTime[i] += fLastTime[i];

  //return(0);
}


void AliHLTTPCCAStandaloneFramework::WriteSettings( std::ostream &out ) const
{
  //* write settings to the file
  out << NSlices() << std::endl;
  for ( int iSlice = 0; iSlice < NSlices(); iSlice++ ) {
    fSliceTrackers[iSlice].Param().WriteSettings( out );
  }
}

void AliHLTTPCCAStandaloneFramework::ReadSettings( std::istream &in )
{
  //* Read settings from the file
  int nSlices = 0;
  in >> nSlices;
  for ( int iSlice = 0; iSlice < nSlices; iSlice++ ) {
    AliHLTTPCCAParam param;
    param.ReadSettings ( in );
    fSliceTrackers[iSlice].Initialize( param );
  }
}

void AliHLTTPCCAStandaloneFramework::WriteEvent( std::ostream &out ) const
{
  // write event to the file
  for ( int iSlice = 0; iSlice < fgkNSlices; iSlice++ ) {
    fClusterData[iSlice].WriteEvent( out );
  }
}

void AliHLTTPCCAStandaloneFramework::ReadEvent( std::istream &in )
{
  //* Read event from file
  for ( int iSlice = 0; iSlice < fgkNSlices; iSlice++ ) {
    fClusterData[iSlice].ReadEvent( in );
  }
}

void AliHLTTPCCAStandaloneFramework::WriteTracks( std::ostream &out ) const
{
  //* Write tracks to file

  for ( int i = 0; i < 20; i++ ) out << fLastTime[i] << std::endl;
  //fMerger.Output()->Write( out );
}

void AliHLTTPCCAStandaloneFramework::ReadTracks( std::istream &in )
{
  //* Read tracks  from file

  for ( int i = 0; i < 20; i++ ) {
    in >> fLastTime[i];
    fStatTime[i] += fLastTime[i];
  }
  //fMerger.Output()->Read( in );
}

int AliHLTTPCCAStandaloneFramework::InitGPU()
{
	if (fUseGPUTracker) return(1);
	int retVal = fGPUTracker.InitGPU();
	fUseGPUTracker = retVal == 0;
	return(retVal);
}

int AliHLTTPCCAStandaloneFramework::ExitGPU()
{
	if (!fUseGPUTracker) return(1);
	return(fGPUTracker.ExitGPU());
}

void AliHLTTPCCAStandaloneFramework::SetGPUDebugLevel(int Level, std::ostream *OutFile, std::ostream *GPUOutFile)
{
	fGPUTracker.SetDebugLevel(Level, GPUOutFile);
	fGPUDebugLevel = Level;
	for (int i = 0;i < fgkNSlices;i++)
	{
		fSliceTrackers[i].SetGPUDebugLevel(Level, OutFile);
	}
}
