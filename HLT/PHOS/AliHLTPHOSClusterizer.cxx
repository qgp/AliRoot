// $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors: Oystein Djuvsland                                     *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          * 
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** 
 * @file   AliHLTPHOSClusterizer.cxx
 * @author Oystein Djuvsland
 * @date 
 * @brief  Clusterizer for PHOS HLT 
 */

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "AliHLTPHOSClusterizer.h"
//#include "AliHLTPHOSBase.h"
#include "AliHLTLogging.h"
#include "TMath.h"
#include "AliHLTPHOSRecPointContainerStruct.h"
#include "AliHLTPHOSRecPointDataStruct.h"
#include "AliHLTPHOSDigitDataStruct.h"
#include "AliHLTPHOSDigitContainerDataStruct.h"
#include "TClonesArray.h"
#include "AliPHOSDigit.h"
#ifndef HAVENOT__PHOSRECOPARAMEMC // set from configure if EMC functionality not available in AliPHOSRecoParam
#include "AliPHOSRecoParam.h"
#else
#include "AliPHOSRecoParamEmc.h"
#endif
#include <iostream>

using namespace std;

ClassImp(AliHLTPHOSClusterizer);

AliHLTPHOSClusterizer::AliHLTPHOSClusterizer():
  AliHLTLogging(),
  fRecPointDataPtr(0),
  fDigitDataPtr(0),
  fEmcClusteringThreshold(0),
  fEmcMinEnergyThreshold(0),
  fEmcTimeGate(0),
  fDigitsInCluster(0),
  fDigitContainerPtr(0),
  fMaxDigitIndexDiff(2*NZROWSMOD)
{
  //See header file for documentation
  fEmcClusteringThreshold = 0.2;
  fEmcMinEnergyThreshold = 0.03;
  fEmcTimeGate = 1.e-6 ;
}//end


AliHLTPHOSClusterizer::~AliHLTPHOSClusterizer()  
{
  //See header file for documentation
}

void 
AliHLTPHOSClusterizer::SetRecPointDataPtr(AliHLTPHOSRecPointDataStruct* recPointDataPtr)
{
  // See header file for documentation
  fRecPointDataPtr = recPointDataPtr;
}

Int_t 
AliHLTPHOSClusterizer::ClusterizeEvent(UInt_t availableSize, UInt_t& totSize)
{
  //see header file for documentation
  Int_t nRecPoints = 0;

  fAvailableSize = availableSize;

  //  UInt_t maxRecPointSize = sizeof(AliHLTPHOSRecPointDataStruct) + (sizeof(AliHLTPHOSDigitDataStruct) << 7); //Reasonable estimate... 

  //Clusterization starts
  for(UInt_t i = 0; i < fDigitContainerPtr->fNDigits; i++)
    { 
      fDigitsInCluster = 0;
      
      if(fDigitContainerPtr->fDigitDataStruct[i].fEnergy < fEmcClusteringThreshold)
	{
	  continue;
	}
      if(fAvailableSize < (sizeof(AliHLTPHOSRecPointDataStruct)))
	{
	  HLTError("Out of buffer, stopping clusterisation");
	  return -1; 
	}

      // First digit is placed at the fDigits member variable in the recpoint
      fDigitDataPtr = &(fRecPointDataPtr->fDigits);

      fRecPointDataPtr->fAmp = 0;
      fRecPointDataPtr->fModule = fDigitContainerPtr->fDigitDataStruct[i].fModule;

      // Assigning digit data to the digit pointer
      fRecPointDataPtr->fDigits = fDigitContainerPtr->fDigitDataStruct[i];

      fAvailableSize -= (sizeof(AliHLTPHOSRecPointDataStruct));

      // Incrementing the pointer to be ready for new entry
      fDigitDataPtr++;

      fRecPointDataPtr->fAmp += fDigitContainerPtr->fDigitDataStruct[i].fEnergy;
      fDigitContainerPtr->fDigitDataStruct[i].fEnergy = 0;
      fDigitsInCluster++;
      nRecPoints++;

      // Scanning for the neighbours
      if(ScanForNeighbourDigits(i, fRecPointDataPtr) < 0)
	{
	  return -1;
	}

      totSize += sizeof(AliHLTPHOSRecPointDataStruct) + (fDigitsInCluster-1)*sizeof(AliHLTPHOSDigitDataStruct);   
      HLTDebug("Initial available size: %d, used size: %d, remaining available size: %d, should be: %d", availableSize, totSize, fAvailableSize, availableSize-totSize);
      
      fRecPointDataPtr->fMultiplicity = fDigitsInCluster;     

      fRecPointDataPtr = reinterpret_cast<AliHLTPHOSRecPointDataStruct*>(fDigitDataPtr);
    }//end of clusterization

   return nRecPoints;
}

Int_t
AliHLTPHOSClusterizer::ScanForNeighbourDigits(Int_t index, AliHLTPHOSRecPointDataStruct* recPoint)
{
  //see header file for documentation
  Int_t max = TMath::Min((Int_t)fDigitContainerPtr->fNDigits, (Int_t)fMaxDigitIndexDiff+index);
  Int_t min = TMath::Max(0, (Int_t)(index - (Int_t)fMaxDigitIndexDiff));

  max = fDigitContainerPtr->fNDigits;
  min = 0;
  for(Int_t j = min; j < max; j++)
    {
      if(fDigitContainerPtr->fDigitDataStruct[j].fEnergy > fEmcMinEnergyThreshold)
	{
	  if(j != index)
	    {
	      if(AreNeighbours(&(fDigitContainerPtr->fDigitDataStruct[index]),
			       &(fDigitContainerPtr->fDigitDataStruct[j])))
		{
		  // Assigning value to digit ptr
		  if(fAvailableSize < sizeof(AliHLTPHOSDigitDataStruct))
		    {
		      HLTError("Out of buffer, stopping clusterisation");
		      return -1; 
		    }
		  fAvailableSize -= sizeof(AliHLTPHOSDigitDataStruct);

		  *fDigitDataPtr = fDigitContainerPtr->fDigitDataStruct[j];
		  // Incrementing digit pointer to be ready for new entry

		  fDigitDataPtr++;

		  recPoint->fAmp += fDigitContainerPtr->fDigitDataStruct[j].fEnergy;
		  fDigitContainerPtr->fDigitDataStruct[j].fEnergy = 0;	      
		  fDigitsInCluster++;
		  ScanForNeighbourDigits(j, recPoint);
		}
	    }
	}
    }
  return 0;
}

Int_t 
AliHLTPHOSClusterizer::AreNeighbours(AliHLTPHOSDigitDataStruct* digit1, 
					    AliHLTPHOSDigitDataStruct* digit2)
{
  //see header file for documentation
  if ( (digit1->fModule == digit2->fModule) /*&& (coord1[1]==coord2[1])*/ ) // inside the same PHOS module
    { 
      Int_t rowdiff = TMath::Abs( digit1->fZ - digit2->fZ );  
      Int_t coldiff = TMath::Abs( digit1->fX - digit2->fX ); 
      if (( coldiff <= 1   &&  rowdiff == 0 ) || ( coldiff == 0 &&  rowdiff <= 1 ))
	{
	  //	  cout << "Are neighbours: digit (E = "  << digit1->fEnergy << ") with x = " << digit1->fX << " and z = " << digit1->fZ << 
	    //	    " is neighbour with digit (E = " << digit2->fEnergy << ") with x = " << digit2->fX << " and z = " << digit2->fZ << endl;

	  if(TMath::Abs(digit1->fTime - digit2->fTime ) < fEmcTimeGate)
	    {
	      return 1; 
	    }
	}
    }
  return 0;
}
