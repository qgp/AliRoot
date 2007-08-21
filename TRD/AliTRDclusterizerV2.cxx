
/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
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

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// TRD cluster finder                                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TF1.h>
#include <TTree.h>
#include <TH1.h>
#include <TFile.h>

#include "AliRunLoader.h"
#include "AliLoader.h"
#include "AliRawReader.h"
#include "AliLog.h"
#include "AliAlignObj.h"

#include "AliTRDclusterizerV2.h"
#include "AliTRDgeometry.h"
#include "AliTRDdataArrayF.h"
#include "AliTRDdataArrayI.h"
#include "AliTRDdigitsManager.h"
#include "AliTRDpadPlane.h"
#include "AliTRDrawData.h"
#include "AliTRDcalibDB.h"
#include "AliTRDSimParam.h"
#include "AliTRDRecParam.h"
#include "AliTRDcluster.h"

#include "Cal/AliTRDCalROC.h"
#include "Cal/AliTRDCalDet.h"

#include "AliTRDSignalIndex.h"
#include "AliTRDRawStream.h"
#include "AliTRDRawStreamV2.h"

#include "AliTRDfeeParam.h"

ClassImp(AliTRDclusterizerV2)

//_____________________________________________________________________________
AliTRDclusterizerV2::AliTRDclusterizerV2()
  :AliTRDclusterizer()
  ,fDigitsManager(NULL)
  ,fGeometry(NULL)
  ,fAddLabels(kTRUE)
  ,fRawVersion(2)
  ,fIndexesOut(NULL)
  ,fIndexesMaxima(NULL)
{
  //
  // AliTRDclusterizerV2 default constructor
  //

  fRawVersion = AliTRDfeeParam::Instance()->GetRAWversion();
}

//_____________________________________________________________________________
AliTRDclusterizerV2::AliTRDclusterizerV2(const Text_t *name, const Text_t *title)
  :AliTRDclusterizer(name,title)
  ,fDigitsManager(new AliTRDdigitsManager())
  ,fGeometry(NULL)
  ,fAddLabels(kTRUE)
  ,fRawVersion(2)
  ,fIndexesOut(NULL)
  ,fIndexesMaxima(NULL)
{
  //
  // AliTRDclusterizerV2 constructor
  //

  fDigitsManager->CreateArrays();
  fGeometry = new AliTRDgeometry;

  fRawVersion = AliTRDfeeParam::Instance()->GetRAWversion();
}

//_____________________________________________________________________________
AliTRDclusterizerV2::AliTRDclusterizerV2(const AliTRDclusterizerV2 &c)
  :AliTRDclusterizer(c)
  ,fDigitsManager(NULL)
  ,fGeometry(NULL)
  ,fAddLabels(kTRUE)
  ,fRawVersion(2)
  ,fIndexesOut(NULL)
  ,fIndexesMaxima(NULL)
{
  //
  // AliTRDclusterizerV2 copy constructor
  //

}

//_____________________________________________________________________________
AliTRDclusterizerV2::~AliTRDclusterizerV2()
{
  //
  // AliTRDclusterizerV2 destructor
  //

  if (fDigitsManager) {
    delete fDigitsManager;
    fDigitsManager = NULL;
  }

  if (fGeometry)
    {
      delete fGeometry;
      fGeometry = NULL;
    }

  if (fIndexesOut)
    {
      delete fIndexesOut;
      fIndexesOut = NULL;
    }

  if (fIndexesMaxima)
    {
      delete fIndexesMaxima;
      fIndexesMaxima = NULL;
    }
}

//_____________________________________________________________________________
AliTRDclusterizerV2 &AliTRDclusterizerV2::operator=(const AliTRDclusterizerV2 &c)
{
  //
  // Assignment operator
  //

  if (this != &c) ((AliTRDclusterizerV2 &) c).Copy(*this);
  return *this;

}

//_____________________________________________________________________________
void AliTRDclusterizerV2::Copy(TObject &c) const
{
  //
  // Copy function
  //

  ((AliTRDclusterizerV2 &) c).fDigitsManager = NULL;
  ((AliTRDclusterizerV2 &) c).fGeometry = NULL;
  ((AliTRDclusterizerV2 &) c).fAddLabels = fAddLabels;
  ((AliTRDclusterizerV2 &) c).fRawVersion = fRawVersion;
  ((AliTRDclusterizerV2 &) c).fIndexesOut = NULL;
  ((AliTRDclusterizerV2 &) c).fIndexesMaxima = NULL;
  
  AliTRDclusterizer::Copy(c);

}

//_____________________________________________________________________________
void AliTRDclusterizerV2::ResetHelperIndexes(AliTRDSignalIndex *indexesIn)
{
  // 
  // Reset the helper indexes
  //
  if (fIndexesOut)
    {
      // carefull here - we assume that only row number may change - most probable
      if (indexesIn->GetNrow() <= fIndexesOut->GetNrow())
 	fIndexesOut->ResetContent();
      else
	fIndexesOut->ResetContentConditional(indexesIn->GetNrow(), indexesIn->GetNcol(), indexesIn->GetNtime());
    }
  else
    {
      fIndexesOut = new AliTRDSignalIndex(indexesIn->GetNrow(), indexesIn->GetNcol(), indexesIn->GetNtime());
    }
  
  if (fIndexesMaxima)
    {
      // carefull here - we assume that only row number may change - most probable
      if (indexesIn->GetNrow() <= fIndexesMaxima->GetNrow())
 	fIndexesMaxima->ResetContent();
      else
	fIndexesMaxima->ResetContentConditional(indexesIn->GetNrow(), indexesIn->GetNcol(), indexesIn->GetNtime());
    }
  else
    {
      fIndexesMaxima = new AliTRDSignalIndex(indexesIn->GetNrow(), indexesIn->GetNcol(), indexesIn->GetNtime());
    }
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::ReadDigits()
{
  //
  // Reads the digits arrays from the input aliroot file
  //

  if (!fRunLoader) {
    AliError("No run loader available");
    return kFALSE;
  }

  AliLoader* loader = fRunLoader->GetLoader("TRDLoader");
  if (!loader->TreeD()) {
    loader->LoadDigits();
  }

  // Read in the digit arrays
  return (fDigitsManager->ReadDigits(loader->TreeD()));

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::ReadDigits(TTree *digitsTree)
{
  //
  // Reads the digits arrays from the input tree
  //

  // Read in the digit arrays
  return (fDigitsManager->ReadDigits(digitsTree));

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::ReadDigits(AliRawReader *rawReader)
{
  //
  // Reads the digits arrays from the ddl file
  //

  AliTRDrawData raw;
  fDigitsManager = raw.Raw2Digits(rawReader);

  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::MakeClusters()
{
  //
  // Creates clusters from digits
  //

  //propagate info from the digits manager
  if (fAddLabels == kTRUE)
    fAddLabels = fDigitsManager->UsesDictionaries();
    
  Bool_t fReturn = kTRUE;
  for (Int_t i = 0; i < AliTRDgeometry::kNdet; i++)
    {
      AliTRDdataArrayI *digitsIn = fDigitsManager->GetDigits(i);      
      // This is to take care of switched off super modules
      if (digitsIn->GetNtime() == 0) {
	continue;
      }
      //AliInfo(Form("digitsIn->Expand() 0x%x", digitsIn));
      digitsIn->Expand();
      AliTRDSignalIndex* indexes = fDigitsManager->GetIndexes(i);
      if (indexes->IsAllocated() == kFALSE)
	{
	  fDigitsManager->BuildIndexes(i);
	}

      Bool_t fR = kFALSE;
      if (indexes->HasEntry())
	{
	  if (fAddLabels)
	    {
	      for (Int_t iDict = 0; iDict < AliTRDdigitsManager::kNDict; iDict++) 
		{
		  AliTRDdataArrayI *tracksIn = 0;
		  tracksIn = fDigitsManager->GetDictionary(i,iDict);
		  tracksIn->Expand();
		}
	    }
	  fR = MakeClusters(i);
	  fReturn = fR && fReturn;
	}

      if (fR == kFALSE)
	{
	  WriteClusters(i);
	  ResetRecPoints();
	}
      //digitsIn->Compress(1,0);
      // no compress just remove
      fDigitsManager->RemoveDigits(i);
      fDigitsManager->RemoveDictionaries(i);      
      fDigitsManager->ClearIndexes(i);
    }

  return fReturn;
}


//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::Raw2Clusters(AliRawReader *rawReader)
{
  //
  // Creates clusters from raw data
  //

  //AliDebug(2, "Read all");

  AliTRDdataArrayI *digits = 0;
  AliTRDdataArrayI *track0 = 0;
  AliTRDdataArrayI *track1 = 0;
  AliTRDdataArrayI *track2 = 0; 

  AliTRDSignalIndex *indexes = 0;
  // Create the digits manager
  if (!fDigitsManager)
    {
      fDigitsManager = new AliTRDdigitsManager();
      fDigitsManager->CreateArrays();
    }

  //AliTRDRawStream input(rawReader);
  AliTRDRawStreamV2 input(rawReader);
  input.SetRawVersion( fRawVersion );
  input.Init();

  AliInfo(Form("Stream version: %s", input.IsA()->GetName()));

  // Loop through the digits
  Int_t lastdet = -1;
  Int_t det    = 0;
  Int_t it = 0;
  while (input.Next()) 
    {

      det    = input.GetDet();

      if (det != lastdet) 
	{	
	  if (lastdet != -1)
	    {
	      digits = fDigitsManager->GetDigits(lastdet);
	      Bool_t iclusterBranch = kFALSE;
	      if (indexes->HasEntry())
		iclusterBranch = MakeClusters(lastdet);
	      if (iclusterBranch == kFALSE)
		{
		  WriteClusters(lastdet);
		  ResetRecPoints();
		}
	    }

	  if (digits)
	    {
	      fDigitsManager->RemoveDigits(lastdet);
	      fDigitsManager->RemoveDictionaries(lastdet);
	      fDigitsManager->ClearIndexes(lastdet);
	    }

	  lastdet = det;

	  // Add a container for the digits of this detector
	  digits = fDigitsManager->GetDigits(det);
	  track0 = fDigitsManager->GetDictionary(det,0);
	  track1 = fDigitsManager->GetDictionary(det,1);
	  track2 = fDigitsManager->GetDictionary(det,2);

	  // Allocate memory space for the digits buffer
	  if (digits->GetNtime() == 0) 
	    {
	      //AliDebug(5, Form("Alloc digits for det %d", det));
	      digits->Allocate(input.GetMaxRow(),input.GetMaxCol(), input.GetNumberOfTimeBins());
	      track0->Allocate(input.GetMaxRow(),input.GetMaxCol(), input.GetNumberOfTimeBins());
	      track1->Allocate(input.GetMaxRow(),input.GetMaxCol(), input.GetNumberOfTimeBins());
	      track2->Allocate(input.GetMaxRow(),input.GetMaxCol(), input.GetNumberOfTimeBins());
	    }
	  
	  indexes = fDigitsManager->GetIndexes(det);
	  indexes->SetSM(input.GetSM());
	  indexes->SetStack(input.GetStack());
	  indexes->SetLayer(input.GetLayer());
	  indexes->SetDetNumber(det);
	  if (indexes->IsAllocated() == kFALSE)
	    indexes->Allocate(input.GetMaxRow(), input.GetMaxCol(), input.GetNumberOfTimeBins());
	}
      
      for (it = 0; it < 3; it++)
	{
	  if ( input.GetTimeBin() + it < input.GetNumberOfTimeBins() )
	    {
	      if (input.GetSignals()[it] > 0)
		{
		  digits->SetDataUnchecked(input.GetRow(), input.GetCol(),
					   input.GetTimeBin() + it, input.GetSignals()[it]);

		  indexes->AddIndexTBin(input.GetRow(), input.GetCol(),
					input.GetTimeBin() + it);
		  track0->SetDataUnchecked(input.GetRow(), input.GetCol(),
					   input.GetTimeBin() + it, 0);
		  track1->SetDataUnchecked(input.GetRow(), input.GetCol(),
					   input.GetTimeBin() + it, 0);
		  track2->SetDataUnchecked(input.GetRow(), input.GetCol(),
					   input.GetTimeBin() + it, 0);
		}
	    }
	}
  }

  if (lastdet != -1)
    {
      Bool_t iclusterBranch = kFALSE;
      if (indexes->HasEntry())
	iclusterBranch = MakeClusters(lastdet);
      if (iclusterBranch == kFALSE)
	{
	  WriteClusters(lastdet);
	  ResetRecPoints();
	}
      //MakeClusters(lastdet);
      if (digits)
	{
	  fDigitsManager->RemoveDigits(lastdet);
	  fDigitsManager->RemoveDictionaries(lastdet);
	  fDigitsManager->ClearIndexes(lastdet);
	}
    }

  delete fDigitsManager;
  fDigitsManager = NULL;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::Raw2ClustersChamber(AliRawReader *rawReader)
{
  //
  // Creates clusters from raw data
  //

  //AliDebug(1, "Raw2ClustersChamber");

  // Create the digits manager
  // Create the digits manager
  if (!fDigitsManager)
    {
      fDigitsManager = new AliTRDdigitsManager();
      fDigitsManager->CreateArrays();
    }

  fDigitsManager->SetUseDictionaries(fAddLabels);

  //AliTRDRawStream input(rawReader);
  AliTRDRawStreamV2 input(rawReader);
  input.SetRawVersion( fRawVersion );
  input.Init();

  AliInfo(Form("Stream version: %s", input.IsA()->GetName()));
  
  Int_t det    = 0;
  while ((det = input.NextChamber(fDigitsManager)) >= 0)
    {
      Bool_t iclusterBranch = kFALSE;
      if (fDigitsManager->GetIndexes(det)->HasEntry())
	iclusterBranch = MakeClusters(det);
      if (iclusterBranch == kFALSE)
	{
	  WriteClusters(det);
	  ResetRecPoints();
	}
      fDigitsManager->RemoveDigits(det);
      fDigitsManager->RemoveDictionaries(det);      
      fDigitsManager->ClearIndexes(det);
    }

  delete fDigitsManager;
  fDigitsManager = NULL;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::MakeClusters(Int_t det)
{
  //
  // Generates the cluster.
  //

  // Get the digits
  //   digits should be expanded beforehand!
  //   digitsIn->Expand();
  AliTRDdataArrayI *digitsIn = fDigitsManager->GetDigits(det);      
  // This is to take care of switched off super modules
  if (digitsIn->GetNtime() == 0) 
    {
      //AliDebug(5, Form("digitsIn->GetNtime() == 0 [%d]", det));
      return kFALSE;
    }

  AliTRDSignalIndex *indexesIn = fDigitsManager->GetIndexes(det);
  if (indexesIn->IsAllocated() == kFALSE)
    {
      AliError("Indexes do not exist!");
      return kFALSE;      
    }
    
  AliTRDcalibDB  *calibration    = AliTRDcalibDB::Instance();
  if (!calibration) {
    AliFatal("No AliTRDcalibDB instance available\n");
    return kFALSE;  
  }
  
  AliTRDSimParam *simParam       = AliTRDSimParam::Instance();
  if (!simParam) {
    AliError("No AliTRDSimParam instance available\n");
    return kFALSE;  
  }
  
  AliTRDRecParam *recParam       = AliTRDRecParam::Instance();
  if (!recParam) {
    AliError("No AliTRDRecParam instance available\n");
    return kFALSE;  
  }

  // ADC thresholds
  //  Float_t ADCthreshold   = simParam->GetADCthreshold();
  Float_t ADCthreshold   = 0; // There is no ADC threshold anymore, and simParam should not be used ni clusterizer. KO

  // Threshold value for the maximum
  Float_t maxThresh      = recParam->GetClusMaxThresh();
  // Threshold value for the digit signal
  Float_t sigThresh      = recParam->GetClusSigThresh();

  // Detector wise calibration object for t0
  const AliTRDCalDet *calT0Det         = calibration->GetT0Det();
  // Detector wise calibration object for the gain factors
  const AliTRDCalDet *calGainFactorDet = calibration->GetGainFactorDet();

  // Iteration limit for unfolding procedure
  const Float_t kEpsilon = 0.01;             
  const Int_t   kNclus   = 3;  
  const Int_t   kNsig    = 5;

  Int_t    iUnfold       = 0;  
  Double_t ratioLeft     = 1.0;
  Double_t ratioRight    = 1.0;

  Double_t padSignal[kNsig];   
  Double_t clusterSignal[kNclus];
  Double_t clusterPads[kNclus];   

  Int_t icham = indexesIn->GetChamber();
  Int_t iplan = indexesIn->GetPlane();
  Int_t isect = indexesIn->GetSM();

  // Start clustering in the chamber

  Int_t    idet    = fGeometry->GetDetector(iplan,icham,isect);
  if (idet != det)
    {
      AliError("Strange Detector number Missmatch!");
      return kFALSE;
    }

  Int_t    ilayer  = AliGeomManager::kTRD1 + iplan;
  Int_t    imodule = icham + AliTRDgeometry::Ncham() * isect;
  UShort_t volid   = AliGeomManager::LayerToVolUID(ilayer,imodule); 

  //Int_t nRowMax = digitsIn->GetNrow();
  Int_t nColMax = digitsIn->GetNcol();
  Int_t nTimeTotal = digitsIn->GetNtime();

  AliTRDpadPlane *padPlane = fGeometry->GetPadPlane(iplan,icham);

  // Calibration object with pad wise values for t0
  AliTRDCalROC *calT0ROC              = calibration->GetT0ROC(idet);
  // Calibration object with pad wise values for the gain factors
  AliTRDCalROC *calGainFactorROC      = calibration->GetGainFactorROC(idet);
  // Calibration value for chamber wise t0
  Float_t       calT0DetValue         = calT0Det->GetValue(idet);
  // Calibration value for chamber wise gain factor
  Float_t       calGainFactorDetValue = calGainFactorDet->GetValue(idet);

  Int_t nClusters      = 0;

  // Apply the gain and the tail cancelation via digital filter
  AliTRDdataArrayF *digitsOut = new AliTRDdataArrayF(digitsIn->GetNrow()
						     ,digitsIn->GetNcol()
						     ,digitsIn->GetNtime()); 

//   AliInfo(Form("nrows %d cols %d time %d", 
// 	       digitsIn->GetNrow()
// 	       ,digitsIn->GetNcol()
// 	       ,digitsIn->GetNtime())); 

  ResetHelperIndexes(indexesIn);

  Transform(digitsIn
	    ,digitsOut
	    ,indexesIn
	    ,fIndexesOut
	    ,nTimeTotal
	    ,ADCthreshold
	    ,calGainFactorROC
	    ,calGainFactorDetValue);	
	
  Int_t row   = 0;
  Int_t col   = 0;
  Int_t time  = 0;
  Int_t iPad  = 0;
    
  fIndexesOut->ResetCounters();
  while (fIndexesOut->NextRCTbinIndex(row, col, time))
    {
      Float_t signalM = TMath::Abs(digitsOut->GetDataUnchecked(row,col,time));
	    
      // Look for the maximum
      if (signalM >= maxThresh) 
	{
		
	  if (col + 1 >= nColMax || col-1 < 0)
	    continue;

	  Float_t signalL = TMath::Abs(digitsOut->GetDataUnchecked(row,col+1  ,time));

	  Float_t signalR = TMath::Abs(digitsOut->GetDataUnchecked(row,col-1,time));

	  if ((TMath::Abs(signalL) <= signalM) && 
	      (TMath::Abs(signalR) <  signalM)) 
	    {
	      if ((TMath::Abs(signalL) >= sigThresh) ||
		  (TMath::Abs(signalR) >= sigThresh)) 
		{
		  // Maximum found, mark the position by a negative signal
		  digitsOut->SetDataUnchecked(row,col,time,-signalM);
		  fIndexesMaxima->AddIndexTBin(row,col,time);
		}
	    }	
	}	    
    }
	       
  // The index to the first cluster of a given ROC
  Int_t firstClusterROC = -1;
  // The number of cluster in a given ROC
  Int_t nClusterROC     =  0;

  // Now check the maxima and calculate the cluster position
  fIndexesMaxima->ResetCounters();
  while (fIndexesMaxima->NextRCTbinIndex(row, col, time)) 
    {
      // Maximum found ?             
      if (digitsOut->GetDataUnchecked(row,col,time) < 0.0) {

	for (iPad = 0; iPad < kNclus; iPad++) {
	  Int_t iPadCol = col - 1 + iPad;
	  clusterSignal[iPad] = 
	    TMath::Abs(digitsOut->GetDataUnchecked(row,iPadCol,time));
	}

	// Count the number of pads in the cluster
	Int_t nPadCount = 0;
	Int_t ii;
	// Look to the left
	ii = 0;
	while (TMath::Abs(digitsOut->GetDataUnchecked(row,col-ii  ,time)) >= sigThresh) {
	  nPadCount++;
	  ii++;
	  if (col-ii   <        0) break;
	}
	// Look to the right
	ii = 0;
	while (TMath::Abs(digitsOut->GetDataUnchecked(row,col+ii+1,time)) >= sigThresh) {
	  nPadCount++;
	  ii++;
	  if (col+ii+1 >= nColMax) break;
	}
	nClusters++;

	// Look for 5 pad cluster with minimum in the middle
	Bool_t fivePadCluster = kFALSE;
	if (col < (nColMax - 3)) {
	  if (digitsOut->GetDataUnchecked(row,col+2,time) < 0) {
	    fivePadCluster = kTRUE;
	  }
	  if ((fivePadCluster) && (col < (nColMax - 5))) {
	    if (digitsOut->GetDataUnchecked(row,col+4,time) >= sigThresh) {
	      fivePadCluster = kFALSE;
	    }
	  }
	  if ((fivePadCluster) && (col >             1)) {
	    if (digitsOut->GetDataUnchecked(row,col-2,time) >= sigThresh) {
	      fivePadCluster = kFALSE;
	    }
	  }
	}

	// 5 pad cluster
	// Modify the signal of the overlapping pad for the left part 
	// of the cluster which remains from a previous unfolding
	if (iUnfold) {
	  clusterSignal[0] *= ratioLeft;
	  iUnfold = 0;
	}

	// Unfold the 5 pad cluster
	if (fivePadCluster) {
	  for (iPad = 0; iPad < kNsig; iPad++) {
	    padSignal[iPad] = TMath::Abs(digitsOut->GetDataUnchecked(row
								     ,col-1+iPad
								     ,time));
	  }
	  // Unfold the two maxima and set the signal on 
	  // the overlapping pad to the ratio
	  ratioRight        = Unfold(kEpsilon,iplan,padSignal);
	  ratioLeft         = 1.0 - ratioRight; 
	  clusterSignal[2] *= ratioRight;
	  iUnfold = 1;
	}

	Double_t clusterCharge = clusterSignal[0]
	  + clusterSignal[1]
	  + clusterSignal[2];
                
	// The position of the cluster
	clusterPads[0] =  row + 0.5;
	// Take the shift of the additional time bins into account
	clusterPads[2] = time + 0.5;

	if (recParam->LUTOn()) {
	  // Calculate the position of the cluster by using the
	  // lookup table method
	  clusterPads[1] = recParam->LUTposition(iplan,clusterSignal[0]
						 ,clusterSignal[1]
						 ,clusterSignal[2]);
	}
	else {
	  // Calculate the position of the cluster by using the
	  // center of gravity method
	  for (Int_t i = 0; i < kNsig; i++) {
	    padSignal[i] = 0.0;
	  }
	  padSignal[2] = TMath::Abs(digitsOut->GetDataUnchecked(row,col  ,time)); // Central pad
	  padSignal[1] = TMath::Abs(digitsOut->GetDataUnchecked(row,col-1,time)); // Left    pad
	  padSignal[3] = TMath::Abs(digitsOut->GetDataUnchecked(row,col+1,time)); // Right   pad
	  if ((col >           2) && 
	      (TMath::Abs(digitsOut->GetDataUnchecked(row,col-2,time)) < padSignal[1])) {
	    padSignal[0] = TMath::Abs(digitsOut->GetDataUnchecked(row,col-2,time));
	  }
	  if ((col < nColMax - 3) &&
	      (TMath::Abs(digitsOut->GetDataUnchecked(row,col+2,time)) < padSignal[3])) {
	    padSignal[4] = TMath::Abs(digitsOut->GetDataUnchecked(row,col+2,time));
	  }		  
	  clusterPads[1] = GetCOG(padSignal);
	}

	Double_t q0 = clusterSignal[0];
	Double_t q1 = clusterSignal[1];
	Double_t q2 = clusterSignal[2];
	Double_t clusterSigmaY2 = (q1 * (q0 + q2) + 4.0 * q0 * q2)
	  / (clusterCharge*clusterCharge);

	//
	// Calculate the position and the error
	//		

	// Correct for t0 (sum of chamber and pad wise values !!!)
	Float_t  calT0ROCValue  = calT0ROC->GetValue(col,row);
	Char_t   clusterTimeBin = ((Char_t) TMath::Nint(time - (calT0DetValue + calT0ROCValue)));
	Double_t colSize        = padPlane->GetColSize(col);
	Double_t rowSize        = padPlane->GetRowSize(row);

	Float_t clusterPos[3];
	clusterPos[0] = padPlane->GetColPos(col) - (clusterPads[1] + 0.5) * colSize;
	clusterPos[1] = padPlane->GetRowPos(row) - 0.5                    * rowSize;
	clusterPos[2] = CalcXposFromTimebin(clusterPads[2],idet,col,row);
	Float_t clusterSig[2];
	clusterSig[0] = (clusterSigmaY2 + 1.0/12.0) * colSize*colSize;
	clusterSig[1] = rowSize * rowSize / 12.0;                                       
		
	// Store the amplitudes of the pads in the cluster for later analysis
	Short_t signals[7] = { 0, 0, 0, 0, 0, 0, 0 };
	for (Int_t jPad = col-3; jPad <= col+3; jPad++) {
	  if ((jPad <          0) || 
	      (jPad >= nColMax-1)) {
	    continue;
	  }
	  signals[jPad-col+3] = TMath::Nint(TMath::Abs(digitsOut->GetDataUnchecked(row,jPad,time)));
	}

	// Add the cluster to the output array
	// The track indices will be stored later 
	AliTRDcluster *cluster = new AliTRDcluster(idet
						   ,clusterCharge
						   ,clusterPos
						   ,clusterSig
						   ,0x0
						   ,((Char_t) nPadCount)
						   ,signals
						   ,((UChar_t) col)
						   ,clusterTimeBin
						   ,clusterPads[1]
						   ,volid);
	// Temporarily store the row, column and time bin of the center pad
	// Used to later on assign the track indices
	cluster->SetLabel( row,0);
	cluster->SetLabel( col,1);
	cluster->SetLabel(time,2);
	RecPoints()->Add(cluster);

	// Store the index of the first cluster in the current ROC
	if (firstClusterROC < 0) {
	  firstClusterROC = RecPoints()->GetEntriesFast() - 1;
	}
	// Count the number of cluster in the current ROC
	nClusterROC++;

      } // if: Maximum found ?

    }
  delete digitsOut;
  //delete fIndexesOut;
  //delete fIndexesMaxima;

  if (fAddLabels)
    AddLabels(idet, firstClusterROC, nClusterROC);

  // Write the cluster and reset the array
  WriteClusters(idet);
  ResetRecPoints();

  return kTRUE;

}

//_____________________________________________________________________________
Bool_t AliTRDclusterizerV2::AddLabels(Int_t idet, Int_t firstClusterROC, Int_t nClusterROC)
{
  //
  // Add the track indices to the found clusters
  //
  
  const Int_t   kNclus   = 3;  
  const Int_t   kNdict   = AliTRDdigitsManager::kNDict;
  const Int_t   kNtrack  = kNdict * kNclus;

  Int_t    iClusterROC   = 0;

  Int_t row   = 0;
  Int_t col   = 0;
  Int_t time  = 0;
  Int_t iPad  = 0;

  // Temporary array to collect the track indices
  Int_t *idxTracks = new Int_t[kNtrack*nClusterROC];

  // Loop through the dictionary arrays one-by-one
  // to keep memory consumption low
  AliTRDdataArrayI *tracksIn = 0;
  for (Int_t iDict = 0; iDict < kNdict; iDict++) {

    tracksIn = fDigitsManager->GetDictionary(idet,iDict);
    // tracksIn should be expanded beforehand!

    // Loop though the clusters found in this ROC
    for (iClusterROC = 0; iClusterROC < nClusterROC; iClusterROC++) {
 
      AliTRDcluster *cluster = (AliTRDcluster *)
	RecPoints()->UncheckedAt(firstClusterROC+iClusterROC);
      row  = cluster->GetLabel(0);
      col  = cluster->GetLabel(1);
      time = cluster->GetLabel(2);

      for (iPad = 0; iPad < kNclus; iPad++) {
	Int_t iPadCol = col - 1 + iPad;
	Int_t index   = tracksIn->GetDataUnchecked(row,iPadCol,time) - 1;
	idxTracks[3*iPad+iDict + iClusterROC*kNtrack] = index;     
      }

    }

    // Compress the arrays
    // no do not compress - we will delete them when we are done with the detector
    //tracksIn->Compress(1,0);

  }

  // Copy the track indices into the cluster
  // Loop though the clusters found in this ROC
  for (iClusterROC = 0; iClusterROC < nClusterROC; iClusterROC++) {
 
    AliTRDcluster *cluster = (AliTRDcluster *)
      RecPoints()->UncheckedAt(firstClusterROC+iClusterROC);
    cluster->SetLabel(-9999,0);
    cluster->SetLabel(-9999,1);
    cluster->SetLabel(-9999,2);
  
    cluster->AddTrackIndex(&idxTracks[iClusterROC*kNtrack]);

  }

  delete [] idxTracks;

  return kTRUE;
}

//_____________________________________________________________________________
Double_t AliTRDclusterizerV2::GetCOG(Double_t signal[5])
{
  //
  // Get COG position
  // Used for clusters with more than 3 pads - where LUT not applicable
  //

  Double_t sum = signal[0]
               + signal[1]
               + signal[2] 
               + signal[3]
               + signal[4];

  Double_t res = (0.0 * (-signal[0] + signal[4])
                      + (-signal[1] + signal[3])) / sum;

  return res;		  

}

//_____________________________________________________________________________
Double_t AliTRDclusterizerV2::Unfold(Double_t eps, Int_t plane, Double_t *padSignal)
{
  //
  // Method to unfold neighbouring maxima.
  // The charge ratio on the overlapping pad is calculated
  // until there is no more change within the range given by eps.
  // The resulting ratio is then returned to the calling method.
  //

  AliTRDcalibDB *calibration = AliTRDcalibDB::Instance();
  if (!calibration) {
    AliError("No AliTRDcalibDB instance available\n");
    return kFALSE;  
  }
  
  Int_t   irc                = 0;
  Int_t   itStep             = 0;                 // Count iteration steps

  Double_t ratio             = 0.5;               // Start value for ratio
  Double_t prevRatio         = 0.0;               // Store previous ratio

  Double_t newLeftSignal[3]  = { 0.0, 0.0, 0.0 }; // Array to store left cluster signal
  Double_t newRightSignal[3] = { 0.0, 0.0, 0.0 }; // Array to store right cluster signal
  Double_t newSignal[3]      = { 0.0, 0.0, 0.0 };

  // Start the iteration
  while ((TMath::Abs(prevRatio - ratio) > eps) && (itStep < 10)) {

    itStep++;
    prevRatio = ratio;

    // Cluster position according to charge ratio
    Double_t maxLeft  = (ratio*padSignal[2] - padSignal[0]) 
                      / (padSignal[0] + padSignal[1] + ratio*padSignal[2]);
    Double_t maxRight = (padSignal[4] - (1-ratio)*padSignal[2]) 
                      / ((1.0 - ratio)*padSignal[2] + padSignal[3] + padSignal[4]);

    // Set cluster charge ratio
    irc = calibration->PadResponse(1.0,maxLeft ,plane,newSignal);
    Double_t ampLeft  = padSignal[1] / newSignal[1];
    irc = calibration->PadResponse(1.0,maxRight,plane,newSignal);
    Double_t ampRight = padSignal[3] / newSignal[1];

    // Apply pad response to parameters
    irc = calibration->PadResponse(ampLeft ,maxLeft ,plane,newLeftSignal );
    irc = calibration->PadResponse(ampRight,maxRight,plane,newRightSignal);

    // Calculate new overlapping ratio
    ratio = TMath::Min((Double_t)1.0,newLeftSignal[2] / 
                                    (newLeftSignal[2] + newRightSignal[0]));

  }

  return ratio;

}

//_____________________________________________________________________________
void AliTRDclusterizerV2::Transform(AliTRDdataArrayI *digitsIn
				    , AliTRDdataArrayF *digitsOut
				    , AliTRDSignalIndex *indexesIn
				    , AliTRDSignalIndex *indexesOut
                                    , Int_t nTimeTotal
				    , Float_t ADCthreshold
				    , AliTRDCalROC *calGainFactorROC
				    , Float_t calGainFactorDetValue)
{
  //
  // Apply gain factor
  // Apply tail cancelation: Transform digitsIn to digitsOut
  //

  Int_t iRow  = 0;
  Int_t iCol  = 0;
  Int_t iTime = 0;

  AliTRDRecParam *recParam = AliTRDRecParam::Instance();
  if (!recParam) {
    AliError("No AliTRDRecParam instance available\n");
    return;
  }

  Double_t *inADC  = new Double_t[nTimeTotal];  // ADC data before tail cancellation
  Double_t *outADC = new Double_t[nTimeTotal];  // ADC data after tail cancellation
  indexesIn->ResetCounters();
  while (indexesIn->NextRCIndex(iRow, iCol))
    {
      Float_t  calGainFactorROCValue = calGainFactorROC->GetValue(iCol,iRow);
      Double_t gain                  = calGainFactorDetValue 
                                     * calGainFactorROCValue;

      for (iTime = 0; iTime < nTimeTotal; iTime++) 
	{	  
	  //
	  // Add gain
	  //
	  inADC[iTime]   = digitsIn->GetDataUnchecked(iRow,iCol,iTime);
	  inADC[iTime]  /= gain;
	  outADC[iTime]  = inADC[iTime];
	}

      // Apply the tail cancelation via the digital filter
      if (recParam->TCOn()) {
	DeConvExp(inADC,outADC,nTimeTotal,recParam->GetTCnexp());
      }

      indexesIn->ResetTbinCounter();
      while (indexesIn->NextTbinIndex(iTime))
	{
	  // Store the amplitude of the digit if above threshold
	  if (outADC[iTime] > ADCthreshold) 
	    {
	      digitsOut->SetDataUnchecked(iRow,iCol,iTime,outADC[iTime]);
	      //AliDebug(5, Form("add index %d", indexesIn->GetDetNumber()));
	      indexesOut->AddIndexTBin(iRow,iCol,iTime);
	    }	  
	} //while itime
    }//while irow icol
  
  delete [] inADC;
  delete [] outADC;

  //AliDebug(5, Form("Stop %d", indexesIn->GetDetNumber()));

  return;

}

//_____________________________________________________________________________
void AliTRDclusterizerV2::DeConvExp(Double_t *source, Double_t *target
				  , Int_t n, Int_t nexp) 
{
  //
  // Tail cancellation by deconvolution for PASA v4 TRF
  //

  Double_t rates[2];
  Double_t coefficients[2];

  // Initialization (coefficient = alpha, rates = lambda)
  Double_t R1 = 1.0;
  Double_t R2 = 1.0;
  Double_t C1 = 0.5;
  Double_t C2 = 0.5;

  if (nexp == 1) {   // 1 Exponentials
    R1 = 1.156;
    R2 = 0.130;
    C1 = 0.066;
    C2 = 0.000;
  }
  if (nexp == 2) {   // 2 Exponentials
    R1 = 1.156;
    R2 = 0.130;
    C1 = 0.114;
    C2 = 0.624;
  }

  coefficients[0] = C1;
  coefficients[1] = C2;

  Double_t Dt = 0.1;

  rates[0] = TMath::Exp(-Dt/(R1));
  rates[1] = TMath::Exp(-Dt/(R2));
  
  Int_t i = 0;
  Int_t k = 0;

  Double_t reminder[2];
  Double_t correction;
  Double_t result;

  // Attention: computation order is important
  correction = 0.0;
  for (k = 0; k < nexp; k++) {
    reminder[k] = 0.0;
  }
  for (i = 0; i < n; i++) {
    result    = (source[i] - correction);    // No rescaling
    target[i] = result;

    for (k = 0; k < nexp; k++) {
      reminder[k] = rates[k] * (reminder[k] + coefficients[k] * result);
    }
    correction = 0.0;
    for (k = 0; k < nexp; k++) {
      correction += reminder[k];
    }
  }

}
