
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
 * @file   AliHLTESDCaloClusterMaker.cxx
 * @author Oystein Djuvsland
 * @date 
 * @brief  ESD Calo Cluster maker for  HLT 
 */

// see header file for class documentation
// or
// refer to README to build package
// or 
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "AliESDEvent.h"
#include "AliHLTESDCaloClusterMaker.h"
#include "AliHLTCaloClusterDataStruct.h"
#include "AliHLTCaloClusterReader.h"
#include "AliESDCaloCluster.h"
#include <iostream>

ClassImp(AliHLTESDCaloClusterMaker);

AliHLTESDCaloClusterMaker::AliHLTESDCaloClusterMaker() : 
  fClusterReaderPtr(0)
{
  //See header file for documentation
  fClusterReaderPtr = new AliHLTCaloClusterReader();

}

AliHLTESDCaloClusterMaker::~AliHLTESDCaloClusterMaker()
{
  //See header file for documentation

}

Int_t 
AliHLTESDCaloClusterMaker::FillESD(AliESDEvent *esdPtr, const AliHLTCaloClusterHeaderStruct *caloClusterHeaderPtr)
{
  // See header file for documentation

  AliHLTCaloClusterDataStruct* caloClusterStructPtr = 0;

  fClusterReaderPtr->SetMemory(caloClusterHeaderPtr);

  Int_t nClusters = 0;

  while((caloClusterStructPtr = fClusterReaderPtr->NextCluster()) != 0)
    {
      AliESDCaloCluster esdCluster;

      esdCluster.SetID(caloClusterStructPtr->fID);
      esdCluster.SetClusterType(caloClusterStructPtr->fClusterType);
      esdCluster.SetPosition((Float_t*)(caloClusterStructPtr->fGlobalPos));
      esdCluster.SetE(caloClusterStructPtr->fEnergy);
      esdCluster.SetTOF(caloClusterStructPtr->fTOF);
      esdCluster.SetClusterDisp(caloClusterStructPtr->fDispersion);
      esdCluster.SetClusterChi2(caloClusterStructPtr->fFitQuality);
      const Float_t *pid = caloClusterStructPtr->fPID;
      esdCluster.SetPid(pid);
      esdCluster.SetM20(caloClusterStructPtr->fM20);
      esdCluster.SetM02(caloClusterStructPtr->fM02);
      esdCluster.SetNExMax(caloClusterStructPtr->fNExMax);
      esdCluster.SetEmcCpvDistance(caloClusterStructPtr->fEmcCpvDistance);
      esdCluster.SetDistanceToBadChannel(caloClusterStructPtr->fDistToBadChannel);
      esdCluster.SetNCells(caloClusterStructPtr->fNCells);
      TArrayI tracksMatched(caloClusterStructPtr->GetNTracksMatched(), caloClusterStructPtr->fTracksMatched);
      esdCluster.AddTracksMatched(tracksMatched);
      UShort_t *idArrayPtr = new UShort_t[caloClusterStructPtr->fNCells];
      Double32_t *ampFracArrayPtr = new Double32_t[caloClusterStructPtr->fNCells];
      for(UInt_t index = 0; index < caloClusterStructPtr->fNCells; index++)
	{
	    fClusterReaderPtr->GetCell(caloClusterStructPtr, idArrayPtr[index], ampFracArrayPtr[index], index);
//	    printf("EM: cellId: %d\n", idArrayPtr[index]);;
	}
      esdCluster.SetCellsAbsId(idArrayPtr);
      esdCluster.SetCellsAmplitudeFraction(ampFracArrayPtr);

      esdPtr->AddCaloCluster(&esdCluster);
      //printf("EM: Energy: %f\n", esdCluster.E());
      nClusters++;
    }
  return nClusters;
}
