// @(#) $Id$
// Original: AliHLTClustFinderNew.cxx,v 1.29 2005/06/14 10:55:21 cvetan Exp 

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Anders Vestbo, Constantin Loizides                    *
//* Developers:      Kenneth Aamodt <kenneth.aamodt@student.uib.no>        *
//*                  Kalliopi Kanaki                                       *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/** @file   AliHLTTPCClusterFinder.cxx
    @author Kenneth Aamodt, Kalliopi Kanaki
    @date   
    @brief  Cluster Finder for the TPC
*/

#include "AliHLTTPCDigitReader.h"
#include "AliHLTTPCRootTypes.h"
#include "AliHLTTPCLogging.h"
#include "AliHLTTPCClusterFinder.h"
#include "AliHLTTPCDigitData.h"
#include "AliHLTTPCTransform.h"
#include "AliHLTTPCSpacePointData.h"
#include "AliHLTTPCMemHandler.h"
#include "AliHLTTPCPad.h"
#include <sys/time.h>

#if __GNUC__ >= 3
using namespace std;
#endif

ClassImp(AliHLTTPCClusterFinder)

AliHLTTPCClusterFinder::AliHLTTPCClusterFinder()
  :
  fSpacePointData(NULL),
  fDigitReader(NULL),
  fPtr(NULL),
  fSize(0),
  fDeconvTime(kTRUE),
  fDeconvPad(kTRUE),
  fStdout(kFALSE),
  fCalcerr(kTRUE),
  fRawSP(kFALSE),
  fFirstRow(0),
  fLastRow(0),
  fCurrentRow(0),
  fCurrentSlice(0),
  fCurrentPatch(0),
  fMatch(1),
  fThreshold(10),
  fSignalThreshold(-1),
  fNSigmaThreshold(0),
  fNClusters(0),
  fMaxNClusters(0),
  fXYErr(0.2),
  fZErr(0.3),
  fOccupancyLimit(1.0),
  fUnsorted(0),
  fVectorInitialized(kFALSE),
  fRowPadVector(),
  fClusters(),
  fNumberOfPadsInRow(NULL),
  fNumberOfRows(0),
  fRowOfFirstCandidate(0)
{
  //constructor  
}

AliHLTTPCClusterFinder::~AliHLTTPCClusterFinder()
{
  //destructor
  if(fVectorInitialized){
    DeInitializePadArray();
  }
  if(fNumberOfPadsInRow){
    delete [] fNumberOfPadsInRow;
    fNumberOfPadsInRow=NULL;
  }
}
 
void AliHLTTPCClusterFinder::InitSlice(Int_t slice,Int_t patch,Int_t firstrow, Int_t lastrow,Int_t nmaxpoints)
{
  //init slice
  fNClusters = 0;
  fMaxNClusters = nmaxpoints;
  fCurrentSlice = slice;
  fCurrentPatch = patch;
  fFirstRow = firstrow;
  fLastRow = lastrow;
}

void AliHLTTPCClusterFinder::InitializePadArray(){
  // see header file for class documentation
  
  if(fCurrentPatch>5||fCurrentPatch<0){
    HLTFatal("Patch is not set");
    return;
  }

  HLTDebug("Patch number=%d",fCurrentPatch);

  fFirstRow = AliHLTTPCTransform::GetFirstRow(fCurrentPatch);
  fLastRow = AliHLTTPCTransform::GetLastRow(fCurrentPatch);

  fNumberOfRows=fLastRow-fFirstRow+1;
  fNumberOfPadsInRow= new UInt_t[fNumberOfRows];

  memset( fNumberOfPadsInRow, 0, sizeof(Int_t)*(fNumberOfRows));

  for(UInt_t i=0;i<fNumberOfRows;i++){
    fNumberOfPadsInRow[i]=AliHLTTPCTransform::GetNPads(i+fFirstRow);
    AliHLTTPCPadVector tmpRow;
    for(UInt_t j=0;j<fNumberOfPadsInRow[i];j++){
      AliHLTTPCPad *tmpPad = new AliHLTTPCPad(2);
      tmpPad->SetID(i,j);
      tmpRow.push_back(tmpPad);
    }
    fRowPadVector.push_back(tmpRow);
  }
  fVectorInitialized=kTRUE;
}

Int_t AliHLTTPCClusterFinder::DeInitializePadArray()
{
  // see header file for class documentation
  for(UInt_t i=0;i<fNumberOfRows;i++){
    for(UInt_t j=0;j<fNumberOfPadsInRow[i];j++){
      delete fRowPadVector[i][j];
      fRowPadVector[i][j]=NULL;
    }
    fRowPadVector[i].clear();
  }
  fRowPadVector.clear();
  return 1;
} 


void AliHLTTPCClusterFinder::InitSlice(Int_t slice,Int_t patch,Int_t nmaxpoints)
{
  //init slice
  fNClusters = 0;
  fMaxNClusters = nmaxpoints;
  fCurrentSlice = slice;
  fCurrentPatch = patch;
  fFirstRow=AliHLTTPCTransform::GetFirstRow(patch);
  fLastRow=AliHLTTPCTransform::GetLastRow(patch);
}

void AliHLTTPCClusterFinder::SetOutputArray(AliHLTTPCSpacePointData *pt)
{
  //set pointer to output
  fSpacePointData = pt;
}

void AliHLTTPCClusterFinder::Read(void* ptr,unsigned long size){
  //set input pointer
  fPtr = (UChar_t*)ptr;
  fSize = size;
}

void AliHLTTPCClusterFinder::ProcessDigits()
{
  int iResult=0;
  bool readValue = true;
  Int_t newRow = 0;    
  Int_t rowOffset = 0;
  UShort_t time=0,newTime=0;
  UInt_t pad=0,newPad=0;
  AliHLTTPCSignal_t charge=0;

  fNClusters = 0;

  // initialize block for reading packed data
  iResult=fDigitReader->InitBlock(fPtr,fSize,fFirstRow,fLastRow,fCurrentPatch,fCurrentSlice);
  if (iResult<0) return;

  readValue = fDigitReader->Next();

  // Matthias 08.11.2006 the following return would cause termination without writing the
  // ClusterData and thus would block the component. I just want to have the commented line
  // here for information
  //if (!readValue)return;

  pad = fDigitReader->GetPad();
  time = fDigitReader->GetTime();
  fCurrentRow = fDigitReader->GetRow();

  if ( fCurrentPatch >= 2 ) // Outer sector, patches 2, 3, 4, 5
    rowOffset = AliHLTTPCTransform::GetFirstRow( 2 );

  fCurrentRow += rowOffset;

  UInt_t lastpad = 123456789;
  const UInt_t kPadArraySize=5000;
  const UInt_t kClusterListSize=10000;
  AliClusterData *pad1[kPadArraySize]; //2 lists for internal memory=2pads
  AliClusterData *pad2[kPadArraySize]; //2 lists for internal memory=2pads
  AliClusterData clusterlist[kClusterListSize]; //Clusterlist

  AliClusterData **currentPt;  //List of pointers to the current pad
  AliClusterData **previousPt; //List of pointers to the previous pad
  currentPt = pad2;
  previousPt = pad1;
  UInt_t nprevious=0,ncurrent=0,ntotal=0;

  /* quick implementation of baseline calculation and zero suppression
     open a pad object for each pad and delete it after processing.
     later a list of pad objects with base line history can be used
     The whole thing only works if we really get unprocessed raw data, if
     the data is already zero suppressed, there might be gaps in the time
     bins.
   */
  Int_t gatingGridOffset=50;
  AliHLTTPCPad baseline(gatingGridOffset, AliHLTTPCTransform::GetNTimeBins());
  // just to make later conversion to a list of objects easier
  AliHLTTPCPad* pCurrentPad=NULL;
  if (fSignalThreshold>=0) {
    pCurrentPad=&baseline;
    baseline.SetThreshold(fSignalThreshold);
  }

  while ( readValue!=0 && iResult>=0){   // Reads through all digits in block
    iResult=0;

    if(pad != lastpad){
      //This is a new pad
      
      //Switch the lists:
      if(currentPt == pad2){
	currentPt = pad1;
	previousPt = pad2;
      }
      else {
	currentPt = pad2;
	previousPt = pad1;
      }
      nprevious = ncurrent;
      ncurrent = 0;
      if(pad != lastpad+1){
	//this happens if there is a pad with no signal.
	nprevious = ncurrent = 0;
      }
      lastpad = pad;
    }

    Bool_t newcluster = kTRUE;
    UInt_t seqcharge=0,seqaverage=0,seqerror=0;
    AliHLTTPCSignal_t lastcharge=0;
    UInt_t bLastWasFalling=0;
    Int_t newbin=-1;


    if(fDeconvTime){
      redo: //This is a goto.
      
      if(newbin > -1){
	//bin = newbin;
	newbin = -1;
      }
	  
      lastcharge=0;
      bLastWasFalling = 0;
    }

    while(iResult>=0){ //Loop over time bins of current pad
      // read all the values for one pad at once to calculate the base line
      if (pCurrentPad) {
	if (!pCurrentPad->IsStarted()) {
	  //HLTDebug("reading data for pad %d, padrow %d", fDigitReader->GetPad(), fDigitReader->GetRow()+rowOffset);
	  pCurrentPad->SetID(fDigitReader->GetRow()+rowOffset,fDigitReader->GetPad());
	  if ((pCurrentPad->StartEvent())>=0) {
	    do {
	      if ((fDigitReader->GetRow()+rowOffset)!=pCurrentPad->GetRowNumber()) break;
	      if (fDigitReader->GetPad()!=pCurrentPad->GetPadNumber()) break;
	      pCurrentPad->SetRawData(fDigitReader->GetTime(), fDigitReader->GetSignal());
	      //HLTDebug("set raw data to pad: bin %d charge %d", fDigitReader->GetTime(), fDigitReader->GetSignal());
	    } while ((readValue = fDigitReader->Next())!=0);
	  }
	  pCurrentPad->CalculateBaseLine(AliHLTTPCTransform::GetNTimeBins()/2);
	  if (pCurrentPad->Next(kTRUE/*do zero suppression*/)==0) {
	    //HLTDebug("no data available after zero suppression");
	    pCurrentPad->StopEvent();
	    pCurrentPad->ResetHistory();
	    break;
	  }
	  time=pCurrentPad->GetCurrentPosition();
	  if (time>pCurrentPad->GetSize()) {
	    HLTError("invalid time bin for pad");
	    break;
	  }
	}
      }
      if (pCurrentPad) {
	Float_t occupancy=pCurrentPad->GetOccupancy();
	//HLTDebug("pad %d occupancy level: %f", pCurrentPad->GetPadNumber(), occupancy);
	if ( occupancy < fOccupancyLimit ) {
	  charge = pCurrentPad->GetCorrectedData();
	} else {
	  charge = 0;
	  //HLTDebug("ignoring pad %d with occupancy level %f", pCurrentPad->GetPadNumber(), occupancy);
	}
      } else {
	charge = fDigitReader->GetSignal();
      }
      //HLTDebug("get next charge value: position %d charge %d", time, charge);


      // CHARGE DEBUG
      if (fDigitReader->GetRow() == 90){
/////	  LOG(AliHLTTPCLog::kFatal,"AliHLTTPCClusterFinder::Row","row90")  << "PAD=" <<  fDigitReader->GetPad() << "  TIME=" <<  fDigitReader->GetTime() 
	  //					   << "  SIGNAL=" <<  fDigitReader->GetSignal() << ENDLOG;

      }

      if(time >= AliHLTTPCTransform::GetNTimeBins()){
	HLTWarning("Pad %d: Timebin (%d) out of range (%d)", pad, time, AliHLTTPCTransform::GetNTimeBins());
	iResult=-ERANGE;
      }


      //Get the current ADC-value
      if(fDeconvTime){

	//Check if the last pixel in the sequence is smaller than this
	if(charge > lastcharge){
	  if(bLastWasFalling){
	    newbin = 1;
	    break;
	  }
	}
	else bLastWasFalling = 1; //last pixel was larger than this
	lastcharge = charge;
      }
	  
      //Sum the total charge of this sequence
      seqcharge += charge;
      seqaverage += time*charge;
      seqerror += time*time*charge;
      
      if (pCurrentPad) {
	
	if((pCurrentPad->Next(kTRUE/*do zero suppression*/))==0) {
	  pCurrentPad->StopEvent();
	  pCurrentPad->ResetHistory();
	  if(readValue) {
	    newPad = fDigitReader->GetPad();
	    newTime = fDigitReader->GetTime();
	    newRow = fDigitReader->GetRow() + rowOffset;
	  }
	  break;
	}

	newPad=pCurrentPad->GetPadNumber();
	newTime=pCurrentPad->GetCurrentPosition();
	newRow=pCurrentPad->GetRowNumber();
      } else {
      readValue = fDigitReader->Next();
      //Check where to stop:
      if(!readValue) break; //No more value

      newPad = fDigitReader->GetPad();
      newTime = fDigitReader->GetTime();
      newRow = fDigitReader->GetRow() + rowOffset;
      }

      if(newPad != pad)break; //new pad
      if(newTime != time+1) break; //end of sequence
      if(iResult<0) break;

      // pad = newpad;    is equal
      time = newTime;

    }//end loop over sequence

    //HLTDebug("ended time bin sequence loop: seqcharge=%d readValue=%d", seqcharge, readValue);
    //HLTDebug("pad=%d newpad=%d current row=%d newrow=%d", pad, newPad, fCurrentRow, newRow);
    if (seqcharge<=0) {
      // with active zero suppression zero values are possible
      continue;
    }

    //Calculate mean of sequence:
    Int_t seqmean=0;
    if(seqcharge)
      seqmean = seqaverage/seqcharge;
    else{
      LOG(AliHLTTPCLog::kFatal,"AliHLTTPCClusterFinder::ProcessRow","Data")
	<<"Error in data given to the cluster finder"<<ENDLOG;
      seqmean = 1;
      seqcharge = 1;
    }

    //Calculate mean in pad direction:
    Int_t padmean = seqcharge*pad;
    Int_t paderror = pad*padmean;

    //Compare with results on previous pad:
    for(UInt_t p=0; p<nprevious && p<kPadArraySize && ncurrent<kPadArraySize; p++){
      
      //dont merge sequences on the same pad twice
      if(previousPt[p]->fLastMergedPad==pad) continue;

      Int_t difference = seqmean - previousPt[p]->fMean;
      if(difference < -fMatch) break;

      if(difference <= fMatch){ //There is a match here!!
	AliClusterData *local = previousPt[p];
	
	if(fDeconvPad){
	  if(seqcharge > local->fLastCharge){
	    if(local->fChargeFalling){ //The previous pad was falling
	      break; //create a new cluster
	    }		    
	  }
	  else local->fChargeFalling = 1;
	  local->fLastCharge = seqcharge;
	}
	      
	//Don't create a new cluster, because we found a match
	newcluster = kFALSE;
	      
	//Update cluster on current pad with the matching one:
	local->fTotalCharge += seqcharge;
	local->fPad += padmean;
	local->fPad2 += paderror;
	local->fTime += seqaverage;
	local->fTime2 += seqerror;
	local->fMean = seqmean;
	local->fFlags++; //means we have more than one pad 
	local->fLastMergedPad = pad;

	currentPt[ncurrent] = local;
	ncurrent++;
	      
	break;
      } //Checking for match at previous pad
    } //Loop over results on previous pad.

    if(newcluster && ncurrent<kPadArraySize){
      //Start a new cluster. Add it to the clusterlist, and update
      //the list of pointers to clusters in current pad.
      //current pad will be previous pad on next pad.

      //Add to the clusterlist:
      AliClusterData *tmp = &clusterlist[ntotal];
      tmp->fTotalCharge = seqcharge;
      tmp->fPad = padmean;
      tmp->fPad2 = paderror;
      tmp->fTime = seqaverage;
      tmp->fTime2 = seqerror;
      tmp->fMean = seqmean;
      tmp->fFlags = 0;  //flags for single pad clusters
      tmp->fLastMergedPad = pad;

      if(fDeconvPad){
	tmp->fChargeFalling = 0;
	tmp->fLastCharge = seqcharge;
      }

      //Update list of pointers to previous pad:
      currentPt[ncurrent] = &clusterlist[ntotal];
      ntotal++;
      ncurrent++;
    }

    if(fDeconvTime)
      if(newbin >= 0) goto redo;
  
    // to prevent endless loop  
    if(time >= AliHLTTPCTransform::GetNTimeBins()){
      HLTWarning("Timebin (%d) out of range (%d)", time, AliHLTTPCTransform::GetNTimeBins());
      break;
    }


    if(!readValue) break; //No more value
    
    if (ntotal>=kClusterListSize || ncurrent>=kPadArraySize) {
      HLTWarning("pad array size exceeded ntotal=%d ncurrent=%d, skip rest of the data", ntotal, ncurrent);
      break;
    }

    if(fCurrentRow != newRow){
      WriteClusters(ntotal,clusterlist);

      lastpad = 123456789;

      currentPt = pad2;
      previousPt = pad1;
      nprevious=0;
      ncurrent=0;
      ntotal=0;
      
      fCurrentRow = newRow;
    }

    pad = newPad;
    time = newTime;

  } // END while(readValue)

  WriteClusters(ntotal,clusterlist);

  HLTInfo("ClusterFinder found %d clusters in slice %d patch %d", fNClusters, fCurrentSlice, fCurrentPatch);

} // ENDEND

void AliHLTTPCClusterFinder::WriteClusters(Int_t nclusters,AliClusterData *list)
{
  //write cluster to output pointer
  Int_t thisrow=-1,thissector=-1;
  UInt_t counter = fNClusters;
  
  for(int j=0; j<nclusters; j++)
    {



      if(!list[j].fFlags) continue; //discard single pad clusters
      if(list[j].fTotalCharge < fThreshold) continue; //noise cluster

      Float_t xyz[3];      
      Float_t fpad =(Float_t)list[j].fPad / list[j].fTotalCharge;
      Float_t fpad2=fXYErr*fXYErr; //fixed given error
      Float_t ftime =(Float_t)list[j].fTime / list[j].fTotalCharge;
      Float_t ftime2=fZErr*fZErr;  //fixed given error



      if(fUnsorted){
	fCurrentRow=list[j].fRow;
      }

   
      if(fCalcerr) { //calc the errors, otherwice take the fixed error 
	Int_t patch = AliHLTTPCTransform::GetPatch(fCurrentRow);
	UInt_t q2=list[j].fTotalCharge*list[j].fTotalCharge;
	Float_t sy2=list[j].fPad2 * list[j].fTotalCharge - list[j].fPad * list[j].fPad;
	sy2/=q2;
	if(sy2 < 0) {
	    LOG(AliHLTTPCLog::kError,"AliHLTTPCClusterFinder::WriteClusters","Cluster width")
	      <<"SigmaY2 negative "<<sy2<<" on row "<<fCurrentRow<<" "<<fpad<<" "<<ftime<<ENDLOG;
	    continue;
	} else {
	  if(!fRawSP){
	    fpad2 = (sy2 + 1./12)*AliHLTTPCTransform::GetPadPitchWidth(patch)*AliHLTTPCTransform::GetPadPitchWidth(patch);
	    if(sy2 != 0){
	      fpad2*=0.108; //constants are from offline studies
	      if(patch<2)
		fpad2*=2.07;
	    }
	  } else fpad2=sy2; //take the width not the error
	}
	Float_t sz2=list[j].fTime2*list[j].fTotalCharge - list[j].fTime*list[j].fTime;
	sz2/=q2;
	if(sz2 < 0){
	  LOG(AliHLTTPCLog::kError,"AliHLTTPCClusterFinder::WriteClusters","Cluster width")
	    <<"SigmaZ2 negative "<<sz2<<" on row "<<fCurrentRow<<" "<<fpad<<" "<<ftime<<ENDLOG;
	  continue;
	} else {
	  if(!fRawSP){
	    ftime2 = (sz2 + 1./12)*AliHLTTPCTransform::GetZWidth()*AliHLTTPCTransform::GetZWidth();
	    if(sz2 != 0) {
	      ftime2 *= 0.169; //constants are from offline studies
	      if(patch<2)
		ftime2 *= 1.77;
	    }
	  } else ftime2=sz2; //take the width, not the error
	}
      }
      if(fStdout==kTRUE)
	HLTInfo("WriteCluster: padrow %d pad %d +- %d time +- %d charge %d",fCurrentRow, fpad, fpad2, ftime, ftime2, list[j].fTotalCharge);
      
      if(!fRawSP){
	AliHLTTPCTransform::Slice2Sector(fCurrentSlice,fCurrentRow,thissector,thisrow);
	AliHLTTPCTransform::Raw2Local(xyz,thissector,thisrow,fpad,ftime);
	
	if(xyz[0]==0) LOG(AliHLTTPCLog::kError,"AliHLTTPCClustFinder","Cluster Finder")
	  <<AliHLTTPCLog::kDec<<"Zero cluster"<<ENDLOG;
	if(fNClusters >= fMaxNClusters)
	  {
	    LOG(AliHLTTPCLog::kError,"AliHLTTPCClustFinder::WriteClusters","Cluster Finder")
	      <<AliHLTTPCLog::kDec<<"Too many clusters "<<fNClusters<<ENDLOG;
	    return;
	  }  
	
	fSpacePointData[counter].fX = xyz[0];
	fSpacePointData[counter].fY = xyz[1];
	fSpacePointData[counter].fZ = xyz[2];
	
      } else {
	fSpacePointData[counter].fX = fCurrentRow;
	fSpacePointData[counter].fY = fpad;
	fSpacePointData[counter].fZ = ftime;
      }
      
      fSpacePointData[counter].fCharge = list[j].fTotalCharge;
      fSpacePointData[counter].fPadRow = fCurrentRow;
      fSpacePointData[counter].fSigmaY2 = fpad2;
      fSpacePointData[counter].fSigmaZ2  = ftime2;

      fSpacePointData[counter].fUsed = kFALSE;         // only used / set in AliHLTTPCDisplay
      fSpacePointData[counter].fTrackN = -1;           // only used / set in AliHLTTPCDisplay

      Int_t patch=fCurrentPatch;
      if(patch==-1) patch=0; //never store negative patch number
      fSpacePointData[counter].fID = counter
	+((fCurrentSlice&0x7f)<<25)+((patch&0x7)<<22);//Uli

#ifdef do_mc
      Int_t trackID[3];
      GetTrackID((Int_t)rint(fpad),(Int_t)rint(ftime),trackID);

      fSpacePointData[counter].fTrackID[0] = trackID[0];
      fSpacePointData[counter].fTrackID[1] = trackID[1];
      fSpacePointData[counter].fTrackID[2] = trackID[2];

#endif
      
      fNClusters++;
      counter++;
    }
}

// STILL TO FIX  ----------------------------------------------------------------------------

#ifdef do_mc
void AliHLTTPCClusterFinder::GetTrackID(Int_t pad,Int_t time,Int_t *trackID)
{
  //get mc id
  AliHLTTPCDigitRowData *rowPt = (AliHLTTPCDigitRowData*)fDigitRowData;
  
  trackID[0]=trackID[1]=trackID[2]=-2;
  for(Int_t i=fFirstRow; i<=fLastRow; i++){
    if(rowPt->fRow < (UInt_t)fCurrentRow){
      AliHLTTPCMemHandler::UpdateRowPointer(rowPt);
      continue;
    }
    AliHLTTPCDigitData *digPt = (AliHLTTPCDigitData*)rowPt->fDigitData;
    for(UInt_t j=0; j<rowPt->fNDigit; j++){
      Int_t cpad = digPt[j].fPad;
      Int_t ctime = digPt[j].fTime;
      if(cpad != pad) continue;
      if(ctime != time) continue;

      trackID[0] = digPt[j].fTrackID[0];
      trackID[1] = digPt[j].fTrackID[1];
      trackID[2] = digPt[j].fTrackID[2];
      
      break;
    }
    break;
  }
}
#endif

//----------------------------------Methods for the new unsorted way of reading the data --------------------------------

void AliHLTTPCClusterFinder::ReadDataUnsorted(void* ptr,unsigned long size)
{
  //set input pointer
  fPtr = (UChar_t*)ptr;
  fSize = size;

  if(!fVectorInitialized){
    InitializePadArray();
  }

  fDigitReader->InitBlock(fPtr,fSize,fFirstRow,fLastRow,fCurrentPatch,fCurrentSlice);
  
  while(fDigitReader->NextChannel()){
    UInt_t row=fDigitReader->GetRow();
    UInt_t pad=fDigitReader->GetPad();

    fRowPadVector[row][pad]->ClearCandidates();
    while(fDigitReader->NextBunch()){
      if(fDigitReader->GetBunchSize()>1){//to remove single timebin values, this will have to change at some point
	const UInt_t *bunchData= fDigitReader->GetSignals();
	UInt_t time = fDigitReader->GetTime();
	AliHLTTPCClusters candidate;
	for(Int_t i=0;i<fDigitReader->GetBunchSize();i++){
	  candidate.fTotalCharge+=bunchData[i];	
	  candidate.fTime += time*bunchData[i];
	  candidate.fTime2 += time*time*bunchData[i];
	  time++;
	}
	if(candidate.fTotalCharge>0){
	  candidate.fMean=candidate.fTime/candidate.fTotalCharge;
	  candidate.fPad=candidate.fTotalCharge*pad;
	  candidate.fPad2=candidate.fPad*pad;
	  candidate.fLastMergedPad=pad;
	  candidate.fRowNumber=row+fDigitReader->GetRowOffset();
	}
	fRowPadVector[row][pad]->AddClusterCandidate(candidate);
      }
    }
  }
}

Bool_t AliHLTTPCClusterFinder::ComparePads(AliHLTTPCPad *nextPad,AliHLTTPCClusters* cluster,Int_t nextPadToRead){
  //Checking if we have a match on the next pad
  for(UInt_t candidateNumber=0;candidateNumber<nextPad->fClusterCandidates.size();candidateNumber++){
    AliHLTTPCClusters *candidate =&nextPad->fClusterCandidates[candidateNumber]; 
    if(cluster->fMean-candidate->fMean==1 || candidate->fMean-cluster->fMean==1 || cluster->fMean-candidate->fMean==0){
      cluster->fMean=candidate->fMean;
      cluster->fTotalCharge+=candidate->fTotalCharge;
      cluster->fTime += candidate->fTime;
      cluster->fTime2 += candidate->fTime2;
      cluster->fPad+=candidate->fPad;
      cluster->fPad2=candidate->fPad2;
      cluster->fLastMergedPad=candidate->fPad;

      //setting the matched pad to used
      nextPad->fUsedClusterCandidates[candidateNumber]=1;
      nextPadToRead++;
      if(nextPadToRead<(Int_t)fNumberOfPadsInRow[fRowOfFirstCandidate]){
	nextPad=fRowPadVector[fRowOfFirstCandidate][nextPadToRead];
	ComparePads(nextPad,cluster,nextPadToRead);
      }
      else{
	return kFALSE;
      }
    }
    else{
      return kFALSE;
    }
  }
  return kFALSE;
}

void AliHLTTPCClusterFinder::FindClusters()
{
  // see header file for function documentation

  AliHLTTPCClusters* tmpCandidate=NULL;
  for(UInt_t row=0;row<fNumberOfRows;row++){
    fRowOfFirstCandidate=row;
    for(UInt_t pad=0;pad<fNumberOfPadsInRow[row]-1;pad++){
      AliHLTTPCPad *tmpPad=fRowPadVector[row][pad];
      for(size_t candidate=0;candidate<tmpPad->fClusterCandidates.size();candidate++){
	if(tmpPad->fUsedClusterCandidates[candidate]){
	  continue;
	}
	tmpCandidate=&tmpPad->fClusterCandidates[candidate];
	UInt_t tmpTotalCharge=tmpCandidate->fTotalCharge;
	ComparePads(fRowPadVector[row][pad+1],tmpCandidate,pad+1);
	if(tmpCandidate->fTotalCharge>tmpTotalCharge){
	  //we have a cluster
	  fClusters.push_back(*tmpCandidate);
	}
      }
      tmpPad->ClearCandidates();
    }
  }

  HLTInfo("Found %d clusters.",fClusters.size());

  //TODO:  Change so it stores AliHLTTPCSpacePointData directly, instead of this copying
  
  AliClusterData * clusterlist = new AliClusterData[fClusters.size()]; //Clusterlist
  for(unsigned int i=0;i<fClusters.size();i++){
    clusterlist[i].fTotalCharge = fClusters[i].fTotalCharge;
    clusterlist[i].fPad = fClusters[i].fPad;
    clusterlist[i].fPad2 = fClusters[i].fPad2;
    clusterlist[i].fTime = fClusters[i].fTime;
    clusterlist[i].fTime2 = fClusters[i].fTime2;
    clusterlist[i].fMean = fClusters[i].fMean;
    clusterlist[i].fFlags = fClusters[i].fFlags;
    clusterlist[i].fChargeFalling = fClusters[i].fChargeFalling;
    clusterlist[i].fLastCharge = fClusters[i].fLastCharge;
    clusterlist[i].fLastMergedPad = fClusters[i].fLastMergedPad;
    clusterlist[i].fRow = fClusters[i].fRowNumber;
  }
  //  PrintClusters();
  WriteClusters(fClusters.size(),clusterlist);
  delete [] clusterlist;
  fClusters.clear();
}

void AliHLTTPCClusterFinder::PrintClusters()
{
  // see header file for class documentation
  for(size_t i=0;i<fClusters.size();i++){
    HLTInfo("Cluster number: %d",i);
    HLTInfo("Row: %d \t Pad: %d",fClusters[i].fRowNumber,fClusters[i].fFirstPad);
    HLTInfo("Total Charge:   %d",fClusters[i].fTotalCharge);
    HLTInfo("fPad:           %d",fClusters[i].fPad);
    HLTInfo("PadError:       %d",fClusters[i].fPad2);
    HLTInfo("TimeMean:       %d",fClusters[i].fTime);
    HLTInfo("TimeError:      %d",fClusters[i].fTime2);
    HLTInfo("EndOfCluster:");
  }
}

void AliHLTTPCClusterFinder::WriteClusters(Int_t nclusters,AliHLTTPCClusters *list)//This is used when using the AliHLTTPCClusters class for cluster data
{
  //write cluster to output pointer
  Int_t thisrow,thissector;
  UInt_t counter = fNClusters;
  
  for(int j=0; j<nclusters; j++)
    {
      if(!list[j].fFlags) continue; //discard single pad clusters
      if(list[j].fTotalCharge < fThreshold) continue; //noise cluster

      Float_t xyz[3];      
      Float_t fpad =(Float_t)list[j].fPad / list[j].fTotalCharge;
      Float_t fpad2=fXYErr*fXYErr; //fixed given error
      Float_t ftime =(Float_t)list[j].fTime / list[j].fTotalCharge;
      Float_t ftime2=fZErr*fZErr;  //fixed given error


      if(fCalcerr) { //calc the errors, otherwice take the fixed error 
	Int_t patch = AliHLTTPCTransform::GetPatch(fCurrentRow);
	UInt_t q2=list[j].fTotalCharge*list[j].fTotalCharge;
	Float_t sy2=list[j].fPad2 * list[j].fTotalCharge - list[j].fPad * list[j].fPad;
	sy2/=q2;
	if(sy2 < 0) {
	    LOG(AliHLTTPCLog::kError,"AliHLTTPCClusterFinder::WriteClusters","Cluster width")
	      <<"SigmaY2 negative "<<sy2<<" on row "<<fCurrentRow<<" "<<fpad<<" "<<ftime<<ENDLOG;
	    continue;
	} else {
	  if(!fRawSP){
	    fpad2 = (sy2 + 1./12)*AliHLTTPCTransform::GetPadPitchWidth(patch)*AliHLTTPCTransform::GetPadPitchWidth(patch);
	    if(sy2 != 0){
	      fpad2*=0.108; //constants are from offline studies
	      if(patch<2)
		fpad2*=2.07;
	    }
	  } else fpad2=sy2; //take the width not the error
	}
	Float_t sz2=list[j].fTime2*list[j].fTotalCharge - list[j].fTime*list[j].fTime;
	sz2/=q2;
	if(sz2 < 0){
	  LOG(AliHLTTPCLog::kError,"AliHLTTPCClusterFinder::WriteClusters","Cluster width")
	    <<"SigmaZ2 negative "<<sz2<<" on row "<<fCurrentRow<<" "<<fpad<<" "<<ftime<<ENDLOG;
	  continue;
	} else {
	  if(!fRawSP){
	    ftime2 = (sz2 + 1./12)*AliHLTTPCTransform::GetZWidth()*AliHLTTPCTransform::GetZWidth();
	    if(sz2 != 0) {
	      ftime2 *= 0.169; //constants are from offline studies
	      if(patch<2)
		ftime2 *= 1.77;
	    }
	  } else ftime2=sz2; //take the width, not the error
	}
      }
      if(fStdout==kTRUE)
	HLTInfo("WriteCluster: padrow %d pad %d +- %d time +- %d charge %d",fCurrentRow, fpad, fpad2, ftime, ftime2, list[j].fTotalCharge);

      if(!fRawSP){
	AliHLTTPCTransform::Slice2Sector(fCurrentSlice,fCurrentRow,thissector,thisrow);
	AliHLTTPCTransform::Raw2Local(xyz,thissector,thisrow,fpad,ftime);
	
	if(xyz[0]==0) LOG(AliHLTTPCLog::kError,"AliHLTTPCClustFinder","Cluster Finder")
	  <<AliHLTTPCLog::kDec<<"Zero cluster"<<ENDLOG;
	if(fNClusters >= fMaxNClusters)
	  {
	    LOG(AliHLTTPCLog::kError,"AliHLTTPCClustFinder::WriteClusters","Cluster Finder")
	      <<AliHLTTPCLog::kDec<<"Too many clusters "<<fNClusters<<ENDLOG;
	    return;
	  }  
	
	fSpacePointData[counter].fX = xyz[0];
	fSpacePointData[counter].fY = xyz[1];
	fSpacePointData[counter].fZ = xyz[2];
	
      } else {
	fSpacePointData[counter].fX = fCurrentRow;
	fSpacePointData[counter].fY = fpad;
	fSpacePointData[counter].fZ = ftime;
      }
      
      fSpacePointData[counter].fCharge = list[j].fTotalCharge;
      fSpacePointData[counter].fPadRow = fCurrentRow;
      fSpacePointData[counter].fSigmaY2 = fpad2;
      fSpacePointData[counter].fSigmaZ2  = ftime2;

      fSpacePointData[counter].fUsed = kFALSE;         // only used / set in AliHLTTPCDisplay
      fSpacePointData[counter].fTrackN = -1;           // only used / set in AliHLTTPCDisplay

      Int_t patch=fCurrentPatch;
      if(patch==-1) patch=0; //never store negative patch number
      fSpacePointData[counter].fID = counter
	+((fCurrentSlice&0x7f)<<25)+((patch&0x7)<<22);//Uli

#ifdef do_mc
      Int_t trackID[3];
      GetTrackID((Int_t)rint(fpad),(Int_t)rint(ftime),trackID);

      fSpacePointData[counter].fTrackID[0] = trackID[0];
      fSpacePointData[counter].fTrackID[1] = trackID[1];
      fSpacePointData[counter].fTrackID[2] = trackID[2];

#endif
      
      fNClusters++;
      counter++;
    }
}
