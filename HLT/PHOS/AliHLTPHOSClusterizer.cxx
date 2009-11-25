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
#include "AliHLTPHOSDigitReader.h"
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
  fCurrentDigit(0),
  fStartDigit(0),
  fPreviousDigit(0),
  fEmcClusteringThreshold(0),
  fEmcMinEnergyThreshold(0),
  fEmcTimeGate(0),
  fDigitsInCluster(0),
  fDigitContainerPtr(0),
  fMaxDigitIndexDiff(2*NZROWSMOD),
  fAvailableSize(0),
  fDigitReader(0)
{
  //See header file for documentation
  fEmcClusteringThreshold = 0.2;
  fEmcMinEnergyThreshold = 0.03;
  fEmcTimeGate = 1.e-6 ;

  fDigitReader = new AliHLTPHOSDigitReader();
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
AliHLTPHOSClusterizer::ClusterizeEvent(AliHLTPHOSDigitHeaderStruct *digitHeader, UInt_t availableSize, UInt_t& totSize)
{
  //see header file for documentation
  Int_t nRecPoints = 0;

  fAvailableSize = availableSize;

  fDigitReader->SetDigitHeader(digitHeader);

  //  UInt_t maxRecPointSize = sizeof(AliHLTPHOSRecPointDataStruct) + (sizeof(AliHLTPHOSDigitDataStruct) << 7); //Reasonable estimate... 

  //  HLTError("Starting clusterisation");

  //Clusterization starts
  while((fCurrentDigit = fDigitReader->NextDigit()) != 0)
    { 
      //      HLTError("Digit with energy: %f", fCurrentDigit->fEnergy); 
      fDigitsInCluster = 0;
      
      if(fCurrentDigit->fEnergy < fEmcClusteringThreshold)
	{
	  continue;
	}

      //      HLTError("Have cluster candidate (x,z): %d, %d, with energy: %f", fCurrentDigit->fX, fCurrentDigit->fZ, fCurrentDigit->fEnergy); 

      if(fAvailableSize < (sizeof(AliHLTPHOSRecPointDataStruct)))
	{
	  HLTError("Out of buffer, stopping clusterisation");
	  return -1; 
	}

      // Save the starting digit
      fStartDigit = fCurrentDigit;
      fPreviousDigit = fStartDigit;
      // Get the offset of the digit starting the cluster relative to the start of the digit block
      fRecPointDataPtr->fStartDigitOffset = fDigitReader->GetCurrentDigitOffset();
      //      HLTError("Start digit offset: %d", fRecPointDataPtr->fStartDigitOffset);
      //      cout << "Start digit offset: " <<  fRecPointDataPtr->fStartDigitOffset << endl;
      fDigitReader->DropDigit();

      fRecPointDataPtr->fAmp = 0;
      fRecPointDataPtr->fModule = fCurrentDigit->fModule;

      fRecPointDataPtr->fAmp += fCurrentDigit->fEnergy;
      fDigitsInCluster++;

      nRecPoints++;

      // Scanning for the neighbours
      if(ScanForNeighbourDigits(fRecPointDataPtr, fCurrentDigit) < 0)
	{
	  return -1;
	}

      fPreviousDigit->fMemOffsetNext = 0;

      totSize += sizeof(AliHLTPHOSRecPointDataStruct) + (fDigitsInCluster-1)*sizeof(AliHLTPHOSDigitDataStruct);   
      HLTDebug("Initial available size: %d, used size: %d, remaining available size: %d, should be: %d", availableSize, totSize, fAvailableSize, availableSize-totSize);
      
      fRecPointDataPtr->fMultiplicity = fDigitsInCluster;     
      HLTDebug("Number of digits in cluster: %d", fDigitsInCluster);
      fRecPointDataPtr++;

      fDigitReader->Rewind(); // TODO: jump to the next instead of rewind
    }//end of clusterization

  return nRecPoints;
}

Int_t
AliHLTPHOSClusterizer::ScanForNeighbourDigits(AliHLTPHOSRecPointDataStruct* recPoint, AliHLTPHOSDigitDataStruct *digit)
{
  //see header file for documentation

  //Int_t max = TMath::Min((Int_t)fDigitContainerPtr->fNDigits, (Int_t)fMaxDigitIndexDiff+index);
  //  Int_t min = TMath::Max(0, (Int_t)(index - (Int_t)fMaxDigitIndexDiff));

  //  Int_t max = fDigitContainerPtr->fNDigits;
  //  Int_t min = 0;

  AliHLTPHOSDigitDataStruct *tmpDigit = 0;

  fDigitReader->Rewind();

  while((tmpDigit = fDigitReader->NextDigit()))
    {
      //HLTError("Checking digit (x,z): %d, %d, with energy: %f", tmpDigit->fX, tmpDigit->fZ, tmpDigit->fEnergy); 
      //      HLTError("Checking digit (x,z): %d, %d, with energy: %f for neighbourship of digit (x,z): %d, %d", tmpDigit->fX, tmpDigit->fZ, tmpDigit->fEnergy, digit->fX, digit->fZ); 
      //      if(tmpDigit->fEnergy > fEmcMinEnergyThreshold)
      if(tmpDigit->fEnergy > 0.2)
	{
	  //HLTError("Checking digit (x,z): %d, %d, with energy: %f for neighbourship of digit (x,z): %d, %d", tmpDigit->fX, tmpDigit->fZ, tmpDigit->fEnergy, digit->fX, digit->fZ); 

	  //	  if(AreNeighbours(fStartDigit, tmpDigit))
	  if(AreNeighbours(digit, tmpDigit))
	    {
	      //	      HLTError("Adding digit (x,z): %d, %d, with energy: %f", tmpDigit->fX, tmpDigit->fZ, tmpDigit->fEnergy);
	      fDigitReader->DropDigit();
	      fPreviousDigit->fMemOffsetNext = reinterpret_cast<Long_t>(tmpDigit) - reinterpret_cast<Long_t>(fPreviousDigit);
	      //	      cout << "Digit offset: " << fPreviousDigit->fMemOffsetNext << endl;
	      //   fPreviousDigit->fMemOffsetNext += tmpDigit->fMemOffsetNext;
	      recPoint->fAmp += tmpDigit->fEnergy;
	      fPreviousDigit = tmpDigit;
	      fDigitsInCluster++;
	      ScanForNeighbourDigits(recPoint, tmpDigit);
	      fDigitReader->SetCurrentDigit(tmpDigit);
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
      //      HLTError("coldiff: %d, rowdiff: %d, timediff: %f", coldiff, rowdiff, TMath::Abs(digit1->fTime - digit2->fTime ));
      if (( coldiff <= 1   &&  rowdiff == 0 ) || ( coldiff == 0 &&  rowdiff <= 1 ))
	{
	  //	  cout << "Are neighbours: digit (E = "  << digit1->fEnergy << ") with x = " << digit1->fX << " and z = " << digit1->fZ << 
	    //	    " is neighbour with digit (E = " << digit2->fEnergy << ") with x = " << digit2->fX << " and z = " << digit2->fZ << endl;

	  //	  if(TMath::Abs(digit1->fTime - digit2->fTime ) < fEmcTimeGate)
	    {
	      return 1; 
	    }
	}
    }
  return 0;
}
