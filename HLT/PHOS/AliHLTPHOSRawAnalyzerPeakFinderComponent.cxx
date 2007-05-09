/**************************************************************************
 * Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved.      *
 *                                                                        *
 * Author: Per Thomas Hille for the ALICE HLT Project.                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "AliHLTPHOSRawAnalyzerPeakFinderComponent.h"
#include "AliHLTPHOSRawAnalyzerPeakFinder.h"
#include <cstdlib>
#include "AliHLTPHOSCommonDefs.h"


AliHLTPHOSRawAnalyzerPeakFinderComponent gAliHLTPHOSRawAnalyzerPeakFinderComponent;

AliHLTPHOSRawAnalyzerPeakFinderComponent::AliHLTPHOSRawAnalyzerPeakFinderComponent():AliHLTPHOSRawAnalyzerComponent()
{
  fAnalyzerPtr = new AliHLTPHOSRawAnalyzerPeakFinder();

  if(LoadPFVector() == kFALSE)
    {
      //      cout << "Warning, could not load PF vectors" << endl;
    }
  else 
    {
      //    cout << "Loaded PF vectors" << endl;
    }
} 


AliHLTPHOSRawAnalyzerPeakFinderComponent::~AliHLTPHOSRawAnalyzerPeakFinderComponent()
{

}


AliHLTPHOSRawAnalyzerPeakFinderComponent::AliHLTPHOSRawAnalyzerPeakFinderComponent(const AliHLTPHOSRawAnalyzerPeakFinderComponent & ) : AliHLTPHOSRawAnalyzerComponent()
{

}


const char* 
AliHLTPHOSRawAnalyzerPeakFinderComponent::GetComponentID()
{
  return "PhosRawPeakFinder";
}

Bool_t 
AliHLTPHOSRawAnalyzerPeakFinderComponent::LoadPFVector()
{
  return LoadPFVector(PF_DEFAULT_STARTINDEX,  PF_DEFAULT_N_SAMPLES, DEFAULT_TAU, DEFAULT_FS );
}


Bool_t 
AliHLTPHOSRawAnalyzerPeakFinderComponent::LoadPFVector(int startIndex, int nSamples, int tau, int fs)
{
  char tmpPFPath[PF_MAX_PATH_LENGTH];
  Double_t tmpAVector[nSamples];
  Double_t tmpTVector[nSamples]; 
  sprintf(tmpPFPath,"%s%s/start%dN%dtau%dfs%d.txt", getenv("ALICE_ROOT"), PF_VECTOR_DIR, startIndex, nSamples, tau, fs);
  FILE *fp;
  fp = fopen(tmpPFPath, "r");
  
  if(fp != 0)
    {
      for(int i=0; i <  nSamples; i++)
	{
	  fscanf(fp, "%lf", &tmpAVector[i]);
	}

      fscanf(fp, "\n");

      for(int i=0; i < nSamples; i++)
	{
	  	  fscanf(fp, "%lf", &tmpTVector[i]);
	}
      fAnalyzerPtr->SetAVector(tmpAVector,  nSamples);
      fAnalyzerPtr->SetTVector(tmpTVector,  nSamples);
      fclose(fp);
      return kTRUE;
    }
  
  else
    {
      HLTFatal("ERROR: could not  open PF vector file");
      return kFALSE;
    }
}


AliHLTComponent*
AliHLTPHOSRawAnalyzerPeakFinderComponent::Spawn()
{
  return new AliHLTPHOSRawAnalyzerPeakFinderComponent;
}

