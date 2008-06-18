/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Indranil Das <indra.das@saha.ac.in>                                  *
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

///////////////////////////////////////////////
//Author : Indranil Das, SINP, INDIA
//         Sukalyan Chattopadhyay, SINP, INDIA
//         
//Email :  indra.das@saha.ac.in
//         sukalyan.chattopadhyay@saha.ac.in 
//
// This class implements a hit reconstruction algorithm for the dimuon
// high level trigger.
// The algorithm finds 3 pad clusters by looking for unique pads with a charge
// above a certain threshold. A centre of gravity type calculation is applied
// to the three pads forming the cluster to find the hit's X or Y coordinate
// along the non-bending and bending planes individually.
// The sepperate X and Y coordinates are then merged to give the full coordinate
// of the hit.
/////////////////////////////////////////////////

#include "AliHLTMUONHitReconstructor.h"
#include "AliHLTMUONRecHitsBlockStruct.h"
#include "AliHLTMUONUtils.h"
#include <cstring>
#include <strings.h>


const AliHLTInt32_t AliHLTMUONHitReconstructor::fgkDetectorId = 0xA00;
const AliHLTInt32_t AliHLTMUONHitReconstructor::fgkDDLOffSet = 12;
const AliHLTInt32_t AliHLTMUONHitReconstructor::fgkNofDDL = 8;
const AliHLTInt32_t AliHLTMUONHitReconstructor::fgkDDLHeaderSize = 8;
const AliHLTInt32_t AliHLTMUONHitReconstructor::fgkLutLine = 59648 + 1;


AliHLTMUONHitReconstructor::AliHLTMUONHitReconstructor() :
	AliHLTLogging(),
	fHLTMUONDecoder(),
	fkBlockHeaderSize(8),
	fkDspHeaderSize(8),
	fkBuspatchHeaderSize(4),
	fDCCut(0),
	fPadData(NULL),
	fLookUpTableData(NULL),
	fRecPoints(NULL),
	fRecPointsCount(NULL),
	fMaxRecPointsCount(0),
	fCentralCountB(0),
	fCentralCountNB(0),
	fDigitPerDDL(0),
	fCentralChargeB(NULL),
	fCentralChargeNB(NULL),
	fRecX(NULL),
	fRecY(NULL),
	fAvgChargeX(NULL),
	fAvgChargeY(NULL),
	fNofBChannel(NULL),
	fNofNBChannel(NULL),
	fNofFiredDetElem(0),
	fIdToEntry()
{
	/// Default constructor
	
	fkBlockHeaderSize    = 8;
	fkDspHeaderSize      = 8;
	fkBuspatchHeaderSize = 4;
	
	try
	{
		fPadData = new AliHLTMUONPad[fgkLutLine];
	}
	catch (const std::bad_alloc&)
	{
		HLTError("Dynamic memory allocation failed for fPadData in constructor.");
		throw;
	}
	
	fPadData[0].fDetElemId = 0;
	fPadData[0].fIX = 0 ;
	fPadData[0].fIY = 0 ;
	fPadData[0].fRealX = 0.0 ;
	fPadData[0].fRealY = 0.0 ;
	fPadData[0].fRealZ = 0.0 ;
	fPadData[0].fHalfPadSize = 0.0 ;
	fPadData[0].fPlane = -1 ;
	fPadData[0].fCharge = 0 ;
	
	bzero(fGetIdTotalData, 336*237*2*sizeof(int));
}


AliHLTMUONHitReconstructor::~AliHLTMUONHitReconstructor()
{
	/// Default destructor
	
	if (fPadData)
	{
		delete [] fPadData;
		fPadData = NULL;
	}
}


void AliHLTMUONHitReconstructor::SetLookUpTable(
		const AliHLTMUONHitRecoLutRow* lookupTable,
		const IdManuChannelToEntry* idToEntry
	)
{
	/// Sets the Lookup table (LUT) containing the position of each pad with
	/// electronic channel associated with it. Also the appropriate manu
	/// channel ID mapping to LUT row is also set.

	assert( lookupTable != NULL );
	assert( idToEntry != NULL );
	
	fIdToEntry = idToEntry;
	fLookUpTableData = lookupTable;
}


void AliHLTMUONHitReconstructor::TryRecover(bool value)
{
	/// Sets if the decoder should enable the error recovery logic.
	
	fHLTMUONDecoder.TryRecover(value);
	fHLTMUONDecoder.ExitOnError(not value);
	fHLTMUONDecoder.GetHandler().WarnOnly(value);
}


bool AliHLTMUONHitReconstructor::Run(
		const AliHLTUInt32_t* rawData,
		AliHLTUInt32_t rawDataSize,
		AliHLTMUONRecHitStruct* recHit,
		AliHLTUInt32_t& nofHit
	) 
{
  // main function called by HLTReconstructor to perform DHLT Hitreconstruction 

  fRecPoints = recHit;
  fMaxRecPointsCount = nofHit;
  fRecPointsCount = &nofHit;
  *fRecPointsCount = 0;
  fDigitPerDDL = 0;

  if (not DecodeDDL(rawData, rawDataSize)) {
    // Dont need to log any message again. Already done so in DecodeDDL.
    return false;
  }

  if (fDigitPerDDL == 1)
  {
    // There are no digits to process so stop here.
    return true;
  }

  if (not FindRecHits()) {
    HLTError("Failed to generate RecHits");
    return false;
  }

  return true;
}


bool AliHLTMUONHitReconstructor::DecodeDDL(const AliHLTUInt32_t* rawData,AliHLTUInt32_t rawDataSize)
{
  //function to decode Raw Data 

  AliHLTMUONRawDecoder& handler = reinterpret_cast<AliHLTMUONRawDecoder&>(fHLTMUONDecoder.GetHandler());
  UInt_t bufferSize = UInt_t(rawDataSize*sizeof(AliHLTUInt32_t));

  handler.SetDCCut(fDCCut);
  handler.SetPadData(fPadData);
  handler.SetLookUpTable(fLookUpTableData);
  handler.SetIdManuChannelToEntry(fIdToEntry);
  handler.SetNofFiredDetElemId(fNofFiredDetElem);
  handler.SetMaxFiredPerDetElem(fMaxFiredPerDetElem);
 
  if(!fHLTMUONDecoder.Decode(rawData,bufferSize))
  {
	if (TryRecover())
	{
		HLTWarning("There was a problem with the raw data."
			" Recovered as much data as possible."
			" Will continue processing next event."
		);
	}
	else
	{
		return false;
	}
  }

  fDigitPerDDL = handler.GetDataCount();
  fMaxFiredPerDetElem[fNofFiredDetElem-1] = handler.GetDataCount();
    
  if(fDigitPerDDL == 1){
    HLTInfo("An Empty DDL File found");
  }
    
  return true;
}


bool AliHLTMUONHitReconstructor::FindRecHits()
{
  // fuction that calls hit reconstruction detector element-wise.

  assert( fCentralChargeB == NULL );
  assert( fCentralChargeNB == NULL );
  assert( fRecX == NULL );
  assert( fRecY == NULL );
  assert( fAvgChargeX == NULL );
  assert( fAvgChargeY == NULL );
  assert( fNofBChannel == NULL );
  assert( fNofNBChannel == NULL );
  
  bool resultOk = false;

  for(int iDet=0; iDet<fNofFiredDetElem ; iDet++)
  {
    fCentralCountB = 0;
    fCentralCountNB = 0;

    try
    {
      fCentralChargeB = new int[fMaxFiredPerDetElem[iDet]];
      HLTDebug("Allocated fCentralChargeB with %d elements.", fMaxFiredPerDetElem[iDet]);
      fCentralChargeNB = new int[fMaxFiredPerDetElem[iDet]];
      HLTDebug("Allocated fCentralChargeNB with %d elements.", fMaxFiredPerDetElem[iDet]);
      resultOk = true;
    }
    catch(const std::bad_alloc&)
    {
      HLTError("Dynamic memory allocation failed for fCentralChargeNB and fCentralChargeB");
      resultOk = false;
    }

    // Continue processing, but check if everything is OK as we do, otherwise
    // do not execute the next steps.
    if (resultOk)
    {
      if(iDet>0)
        FindCentralHits(fMaxFiredPerDetElem[iDet-1],fMaxFiredPerDetElem[iDet]);
      else
        // minimum value is 1 because dataCount in ReadDDL starts from 1 instead of 0;
        FindCentralHits(1,fMaxFiredPerDetElem[iDet]);

      try
      {
        fRecY = new float[fCentralCountB];
        HLTDebug("Allocated fRecY with %d elements.", fCentralCountB);
        fRecX = new float[fCentralCountNB];
        HLTDebug("Allocated fRecX with %d elements.", fCentralCountNB);
        fAvgChargeY = new float[fCentralCountB];
        HLTDebug("Allocated fAvgChargeY with %d elements.", fCentralCountB);
        fAvgChargeX = new float[fCentralCountNB];
        HLTDebug("Allocated fAvgChargeX with %d elements.", fCentralCountNB);
        fNofBChannel = new int[fCentralCountB];
        HLTDebug("Allocated fNofBChannel with %d elements.", fCentralCountB);
        fNofNBChannel = new int[fCentralCountNB];
        HLTDebug("Allocated fNofNBChannel with %d elements.", fCentralCountNB);
        resultOk = true;
      }
      catch(const std::bad_alloc&){
        HLTError("Dynamic memory allocation failed for internal arrays.");
        resultOk = false;
      }
    }

    if (resultOk) RecXRecY();
    if (resultOk)
    {
      resultOk = MergeRecHits();
    }
    if (resultOk)
    {
      if(iDet==0)
        // minimum value in loop is 1 because dataCount in ReadDDL starts from 1 instead of 0;
        for(int i=1;i<fMaxFiredPerDetElem[iDet];i++)
          fGetIdTotalData[fPadData[i].fIX][fPadData[i].fIY][fPadData[i].fPlane] = 0;
      else
        for(int i=fMaxFiredPerDetElem[iDet-1];i<fMaxFiredPerDetElem[iDet];i++)
          fGetIdTotalData[fPadData[i].fIX][fPadData[i].fIY][fPadData[i].fPlane] = 0;
    }

    // Make sure to release any memory that was allocated.
    if (fCentralChargeB != NULL)
    {
      delete [] fCentralChargeB;
      HLTDebug("Released fCentralChargeB array.");
      fCentralChargeB = NULL;
    }
    if (fCentralChargeNB != NULL)
    {
      delete [] fCentralChargeNB;
      HLTDebug("Released fCentralChargeNB array.");
      fCentralChargeNB = NULL;
    }
    if (fRecX != NULL)
    {
      delete [] fRecX;
      HLTDebug("Released fRecX array.");
      fRecX = NULL;
    }
    if (fRecY != NULL)
    {
      delete [] fRecY;
      HLTDebug("Released fRecY array.");
      fRecY = NULL;
    }
    if (fAvgChargeX != NULL)
    {
      delete [] fAvgChargeX;
      HLTDebug("Released fAvgChargeX array.");
      fAvgChargeX = NULL;
    }
    if (fAvgChargeY != NULL)
    {
      delete [] fAvgChargeY;
      HLTDebug("Released fAvgChargeY array.");
      fAvgChargeY = NULL;
    }
    if (fNofBChannel != NULL)
    {
      delete [] fNofBChannel;
      HLTDebug("Released fNofBChannel array.");
      fNofBChannel = NULL;
    }
    if (fNofNBChannel != NULL)
    {
      delete [] fNofNBChannel;
      HLTDebug("Released fNofNBChannel array.");
      fNofNBChannel = NULL;
    }
  }

  Clear();  // clear internal arrays.

  return resultOk;
}


void AliHLTMUONHitReconstructor::FindCentralHits(int minPadId, int maxPadId)
{
  // to find central hit associated with each cluster

  assert( fCentralChargeB != NULL );
  assert( fCentralChargeNB != NULL );

  int b,nb;
  int idManuChannelCentral;
  bool hasFind;

  for(int iPad=minPadId;iPad<maxPadId;iPad++){

    fGetIdTotalData[fPadData[iPad].fIX]
      [fPadData[iPad].fIY]
      [fPadData[iPad].fPlane] = iPad ;
    
    if(fPadData[iPad].fPlane == 0 ){//&& fPadData[iPad].fIY > (0+1) && fPadData[iPad].fIY < (79 - 1)){
      //if(fPadData[iPad].fIY > 0){
      if(fCentralCountB>0){
	hasFind = false;
	for(b = 0;b<fCentralCountB;b++){
	  idManuChannelCentral = fCentralChargeB[b];
	  if(fPadData[iPad].fIX == fPadData[idManuChannelCentral].fIX
	     &&
	     (fPadData[iPad].fIY 
	      == fPadData[idManuChannelCentral].fIY + 1 
	      ||
	      fPadData[iPad].fIY 
	      == fPadData[idManuChannelCentral].fIY + 2 
	      ||
	      fPadData[iPad].fIY 
	      == fPadData[idManuChannelCentral].fIY - 2 
	      ||
	      fPadData[iPad].fIY 
	      == fPadData[idManuChannelCentral].fIY - 1)){
	    
	    hasFind = true;
	    if(fPadData[iPad].fCharge > fPadData[idManuChannelCentral].fCharge){
	      fCentralChargeB[b] = iPad;
	    }// if condn on pad charge
	  }// if condon on pad position
	}// for loop over b
	if(!hasFind){
	  fCentralChargeB[fCentralCountB] = iPad;
	  fCentralCountB++;
	}
      }
      else{
	fCentralChargeB[fCentralCountB] = iPad;
	fCentralCountB++;
      }// check the size of centralHitB
      for(b = 0;b<fCentralCountB;b++){
	idManuChannelCentral = fCentralChargeB[b];
      }
      //}// if cond on iY > 2 (to avoid edge value pb)
    }// B/Nb checking
    else{
      if(fCentralCountNB>0){
	hasFind = false;
	for(nb = 0;nb<fCentralCountNB;nb++){
	  idManuChannelCentral = fCentralChargeNB[nb];
	  if(fPadData[iPad].fIY == fPadData[idManuChannelCentral].fIY
	     &&
	     (fPadData[iPad].fIX 
	      == fPadData[idManuChannelCentral].fIX + 1 
	      ||
	      fPadData[iPad].fIX
	      == fPadData[idManuChannelCentral].fIX + 2
	      ||
	      fPadData[iPad].fIX
	      == fPadData[idManuChannelCentral].fIX - 2
	      ||
	      fPadData[iPad].fIX
	      == fPadData[idManuChannelCentral].fIX - 1)){
	    
	    hasFind = true;	  
	    if(fPadData[iPad].fCharge > fPadData[idManuChannelCentral].fCharge){
	      fCentralChargeNB[nb] = iPad;
	    }// if condn over to find higher charge
	  }// if condn over to find position
	}// for loop over presently all nb values
	if(!hasFind){
	  fCentralChargeNB[fCentralCountNB] = iPad;
	  fCentralCountNB++;
	}
      }// centralHitNB size test
      else{
	fCentralChargeNB[fCentralCountNB] = iPad;
	fCentralCountNB++;
      }// centralHitNB size test
      
    }// fill for bending and nonbending hit
  }// detElemId loop

}


void AliHLTMUONHitReconstructor::RecXRecY()
{
  // find reconstructed X and Y for each plane separately

  assert( fRecX != NULL );
  assert( fRecY != NULL );
  assert( fAvgChargeX != NULL );
  assert( fAvgChargeY != NULL );
  assert( fNofBChannel != NULL );
  assert( fNofNBChannel != NULL );

  int b,nb;
  int idCentral;
  int idLower = 0;
  int idUpper = 0;
  int idRight = 0;
  int idLeft = 0;
  
  for(b=0;b<fCentralCountB;b++){
    idCentral = fCentralChargeB[b];

    if(fPadData[idCentral].fIY==0)
      idLower = 0;
    else
      idLower = fGetIdTotalData[fPadData[idCentral].fIX][fPadData[idCentral].fIY-1][0];

    if(fPadData[idCentral].fIX==236)
      idUpper = 0;
    else
      idUpper = fGetIdTotalData[fPadData[idCentral].fIX][fPadData[idCentral].fIY+1][0];


    fRecY[b] = (fPadData[idCentral].fRealY*fPadData[idCentral].fCharge
	       +
		fPadData[idUpper].fRealY*fPadData[idUpper].fCharge
	       +
		fPadData[idLower].fRealY*fPadData[idLower].fCharge
		)/(fPadData[idCentral].fCharge + fPadData[idUpper].fCharge + fPadData[idLower].fCharge) ;
    
    fAvgChargeY[b] = (fPadData[idCentral].fCharge + fPadData[idUpper].fCharge + fPadData[idLower].fCharge)/3.0 ;
    
    fNofBChannel[b] = 0;
    if(fPadData[idLower].fCharge>0)
      fNofBChannel[b]++ ;
    if(fPadData[idCentral].fCharge>0)
      fNofBChannel[b]++ ;
    if(fPadData[idUpper].fCharge>0)
      fNofBChannel[b]++ ;

    HLTDebug("lower : %d, middle : %d, upper : %d, nofChannel : %d",fPadData[idLower].fCharge,
	    fPadData[idCentral].fCharge,fPadData[idUpper].fCharge,fNofBChannel[b]);

    HLTDebug("RecY[%d] : %f",b,fRecY[b]);
  }
      
  for(nb=0;nb<fCentralCountNB;nb++){
    idCentral = fCentralChargeNB[nb];

    if(fPadData[idCentral].fIX==0)
      idLeft = 0;
    else
      idLeft = fGetIdTotalData[fPadData[idCentral].fIX-1][fPadData[idCentral].fIY][1];
    
    if(fPadData[idCentral].fIX==335)
      idRight = 0 ;
    else
      idRight = fGetIdTotalData[fPadData[idCentral].fIX+1][fPadData[idCentral].fIY][1];

    fRecX[nb] = (fPadData[idCentral].fRealX*fPadData[idCentral].fCharge
		 +
		 fPadData[idRight].fRealX*fPadData[idRight].fCharge
		 +
		 fPadData[idLeft].fRealX*fPadData[idLeft].fCharge
		 )/(fPadData[idCentral].fCharge + fPadData[idRight].fCharge + fPadData[idLeft].fCharge);
    

    fAvgChargeX[nb] = (fPadData[idCentral].fCharge + fPadData[idRight].fCharge + fPadData[idLeft].fCharge)/3.0 ;
    
    
    fNofNBChannel[nb] = 0;
    if(fPadData[idLeft].fCharge>0)
      fNofNBChannel[nb]++ ;
    if(fPadData[idCentral].fCharge>0)
      fNofNBChannel[nb]++ ;
    if(fPadData[idRight].fCharge>0)
      fNofNBChannel[nb]++ ;

    HLTDebug("left : %d, middle : %d, right : %d, nofChannel : %d",fPadData[idLeft].fCharge,
	    fPadData[idCentral].fCharge,fPadData[idRight].fCharge,fNofNBChannel[nb]);

    HLTDebug("RecX[%d] : %f",nb,fRecX[nb]);

  }
}


bool AliHLTMUONHitReconstructor::MergeRecHits()
{
  // Merge reconstructed hits first over same plane then bending plane with non-bending plane

  assert( fRecX != NULL );
  assert( fRecY != NULL );
  assert( fAvgChargeX != NULL );
  assert( fAvgChargeY != NULL );
  assert( fNofBChannel != NULL );
  assert( fNofNBChannel != NULL );

  int idCentralB,idCentralNB ;
  float padCenterXB;
  float padCenterYNB;
  float diffX,diffY;
  float halfPadLengthX,halfPadLengthY;

  // MERGE Bending Plane hits, which are placed side by side
  for(int i=0;i<fCentralCountB-1;i++){
    if(fRecY[i] != 0.0){
      for(int j=i+1;j<fCentralCountB;j++){
		 
	if(fCentralChargeB[i]==fCentralChargeB[j]){
	  fRecY[j] = 0.0;
	  continue;
	}
	else if(
	   (
	    fPadData[fCentralChargeB[i]].fIY == fPadData[fCentralChargeB[j]].fIY
	    )
 	   &&
	   (
	    fPadData[fCentralChargeB[i]].fIX == fPadData[fCentralChargeB[j]].fIX + 1
	    ||
	    fPadData[fCentralChargeB[i]].fIX == fPadData[fCentralChargeB[j]].fIX - 1
	    )
 	   &&
 	   fRecY[j] != 0.0
	   &&
	   fRecY[i] != 0.0
	   ){

	  if(fAvgChargeY[i] > fAvgChargeY[j]){
	    fRecY[i] = (fRecY[i]*fAvgChargeY[i] + fRecY[j]*fAvgChargeY[j]
			)/(fAvgChargeY[i] + fAvgChargeY[j]);
	    fRecY[j] = 0.0;
	  }
	  else{
	    fRecY[j] = (fRecY[i]*fAvgChargeY[i] + fRecY[j]*fAvgChargeY[j]
			)/(fAvgChargeY[i] + fAvgChargeY[j]);
	    fRecY[i] = 0.0;

	  }// search for higher charge
	}//pad position
      }//j for loop
    }//if fRecY[i] != 0.0
  }// i for loop
  
  // MERGE Non Bending Plane hits, which are place side by side
  for(int i=0;i<fCentralCountNB-1;i++){
    if(fRecX[i] != 0.0){
      for(int j=i+1;j<fCentralCountNB;j++){

	if(fCentralChargeNB[i]==fCentralChargeNB[j]){
	  fRecX[j] = 0.0;
	  continue;
	}
	else if(
	   (
	    fPadData[fCentralChargeNB[i]].fIX == fPadData[fCentralChargeNB[j]].fIX
	    )
	   &&
	   (
	    fPadData[fCentralChargeNB[i]].fIY == fPadData[fCentralChargeNB[j]].fIY + 1
	    ||
	    fPadData[fCentralChargeNB[i]].fIY == fPadData[fCentralChargeNB[j]].fIY - 1
	    )
	   &&
	   fRecX[j] != 0.0
	   &&
	   fRecX[i] != 0.0
	   ){

	  if(fAvgChargeX[i] > fAvgChargeX[j]){
	    fRecX[i] = (fRecX[i]*fAvgChargeX[i] + fRecX[j]*fAvgChargeX[j]
		       )/(fAvgChargeX[i] + fAvgChargeX[j]);
	    fRecX[j] = 0.0;
	  }
	  else{
	    fRecX[j] = (fRecX[i]*fAvgChargeX[i] + fRecX[j]*fAvgChargeX[j]
		       )/(fAvgChargeX[i] + fAvgChargeX[j]);
	    fRecX[i] = 0.0;
	  }// search for higher charge
	}//pad position
      }//j for loop
    }//if fRecX[i] != 0.0
  }// i for loop

  // Merge bending Plane hits with Non Bending
  for(int b=0;b<fCentralCountB;b++){
    if(fRecY[b]!=0.0){
      idCentralB = fCentralChargeB[b];
      padCenterXB = fPadData[idCentralB].fRealX; 
      
      halfPadLengthX = fPadData[idCentralB].fIY ;

      for(int nb=0;nb<fCentralCountNB;nb++){
	if(fRecX[nb]!=0.0){
	  idCentralNB = fCentralChargeNB[nb];

	  padCenterYNB = fPadData[idCentralNB].fRealY;

	  halfPadLengthY = fPadData[idCentralNB].fHalfPadSize ;

	  if(fabsf(fRecX[nb]) > fabsf(padCenterXB))
	    diffX = fabsf(fRecX[nb]) -  fabsf(padCenterXB);
	  else
	    diffX = fabsf(padCenterXB) -  fabsf(fRecX[nb]);
	  
	  if(fabsf(padCenterYNB)>fabsf(fRecY[b]))
	    diffY = fabsf(padCenterYNB) - fabsf(fRecY[b]);
	  else
	    diffY =  fabsf(fRecY[b]) - fabsf(padCenterYNB);

	  if(diffX < halfPadLengthX && diffY < halfPadLengthY ){//&& fPadData[idCentralB].fIY != 0){

	    if(fNofBChannel[b]==3)
	      fRecY[b] += 0.025*sin(12.0*(fRecY[b] - fPadData[idCentralB].fRealY)) ;
	    
	    fRecX[nb] += 0.075*sin(9.5*(fRecX[nb] - fPadData[idCentralNB].fRealX)) ;
	    
	    // First check that we have not overflowed the buffer.
	    if((*fRecPointsCount) == fMaxRecPointsCount){
	      HLTError("Number of RecHit (i.e. %d) exceeds the max number of RecHit limit %d.",(*fRecPointsCount),fMaxRecPointsCount);
	      return false;
	    }

	    AliHLTUInt32_t idflags = AliHLTMUONUtils::PackRecHitFlags(
	         (fPadData[idCentralB].fDetElemId / 100) - 1,
	         fPadData[idCentralB].fDetElemId
	      );
	    fRecPoints[(*fRecPointsCount)].fFlags = idflags;
	    fRecPoints[(*fRecPointsCount)].fX = fRecX[nb];
	    fRecPoints[(*fRecPointsCount)].fY = fRecY[b];
	    fRecPoints[(*fRecPointsCount)].fZ = fPadData[idCentralB].fRealZ;
// 	    fRecPoints[(*fRecPointsCount)].fXCenter = fPadData[idCentralNB].fRealX;
// 	    fRecPoints[(*fRecPointsCount)].fYCenter = fPadData[idCentralB].fRealY;
// 	    fRecPoints[(*fRecPointsCount)].fNofBChannel = fNofBChannel[b];
// 	    fRecPoints[(*fRecPointsCount)].fNofNBChannel = fNofNBChannel[nb];
// 	    fRecPoints[(*fRecPointsCount)].fDetElemId = (AliHLTUInt32_t)fPadData[idCentralB].fDetElemId;
	    (*fRecPointsCount)++;
	  }//if lies wihtin 5.0 mm
	}// condn over fRecX ! = 0.0
      }// loop over NB side
    }// condn on fRecY[b] !=  0.0
  }// loop over B side;

  return true;
}


void AliHLTMUONHitReconstructor::Clear()
{
  // function to clear internal arrays.

  for(int iPad=1;iPad<fDigitPerDDL;iPad++){
    fGetIdTotalData[fPadData[iPad].fIX][fPadData[iPad].fIY][fPadData[iPad].fPlane] = 0;
    fPadData[iPad].fDetElemId = 0;
    fPadData[iPad].fIX = 0 ;
    fPadData[iPad].fIY = 0 ;
    fPadData[iPad].fRealX = 0.0 ;
    fPadData[iPad].fRealY = 0.0 ;
    fPadData[iPad].fRealZ = 0.0 ;
    fPadData[iPad].fHalfPadSize = -1 ;
    fPadData[iPad].fPlane = -1 ;
    fPadData[iPad].fCharge = 0 ;
  }  
  
  for(int i=0;i<13;i++)
    fMaxFiredPerDetElem[i] = 0;
}


AliHLTMUONHitReconstructor::AliHLTMUONRawDecoder::AliHLTMUONRawDecoder() :
	fBufferStart(NULL),
	fBusPatchId(0),
	fDCCut(0),
	fPadData(NULL),
	fLookUpTableData(NULL),
	fNofFiredDetElem(NULL),
	fMaxFiredPerDetElem(NULL),
	fIdToEntry(),
	fDataCount(1),
	fPrevDetElemId(0),
	fPadCharge(0),
	fCharge(0.0),
	fIdManuChannel(0x0),
	fLutEntry(0),
	fWarnOnly(false)
{
	// ctor
}


AliHLTMUONHitReconstructor::AliHLTMUONRawDecoder::~AliHLTMUONRawDecoder()
{
	// dtor
}


void AliHLTMUONHitReconstructor::AliHLTMUONRawDecoder::OnNewBuffer(const void* buffer, UInt_t /*bufferSize*/)
{
	/// Called for every new raw DDL data payload being processed.
	/// Just clears internal counters.
	/// \param buffer  The pointer to the raw data buffer.

	assert( buffer != NULL );
	fBufferStart = buffer;
	// dataCount starts from 1 because the 0-th element of fPadData is used as null value.
	fDataCount = 1;
	*fNofFiredDetElem = 0;
	fPrevDetElemId = 0 ;
};


void AliHLTMUONHitReconstructor::AliHLTMUONRawDecoder::OnError(ErrorCode code, const void* location)
{
	/// Called if there was an error detected in the raw DDL data.
	/// Logs an error message.
	/// \param code  The error code describing the problem.
	/// \param location  A pointer to the location in the raw data buffer
	///      where the problem was found.
	
	long bytepos = long(location) - long(fBufferStart) + sizeof(AliRawDataHeader);
	HLTError("There is a problem with decoding the raw data. %s (Error code: %d, at byte %d)",
		ErrorCodeToMessage(code), code, bytepos
	);
};


void AliHLTMUONHitReconstructor::AliHLTMUONRawDecoder::OnData(UInt_t dataWord, bool /*parityError*/)
{
  //function to arrange the decoded Raw Data

  fIdManuChannel = 0x0;
  fIdManuChannel = (fIdManuChannel|fBusPatchId)<<17;
  fIdManuChannel |= (dataWord >> 12) & 0x1FFFF;
  
  IdManuChannelToEntry& idToEntry = * const_cast<IdManuChannelToEntry*>(fIdToEntry);
  fLutEntry = idToEntry[fIdManuChannel];
  fPadCharge = int(((unsigned short)(dataWord & 0xFFF)) - fLookUpTableData[fLutEntry].fPed);
  
  fCharge = 0;	  
  if(fPadCharge > fDCCut && fPadCharge > 5.0*fLookUpTableData[fLutEntry].fSigma){  // (charge > 4) is due cut out the noise level  			
      
    fPadData[fDataCount].fDetElemId = fLookUpTableData[fLutEntry].fDetElemId;
    fPadData[fDataCount].fIX = fLookUpTableData[fLutEntry].fIX;
    fPadData[fDataCount].fIY = fLookUpTableData[fLutEntry].fIY;
    fPadData[fDataCount].fRealX = fLookUpTableData[fLutEntry].fRealX;
    fPadData[fDataCount].fRealY = fLookUpTableData[fLutEntry].fRealY;
    fPadData[fDataCount].fRealZ = fLookUpTableData[fLutEntry].fRealZ;
    fPadData[fDataCount].fHalfPadSize = fLookUpTableData[fLutEntry].fHalfPadSize;
    fPadData[fDataCount].fPlane = fLookUpTableData[fLutEntry].fPlane;
    
    if ( fPadCharge < fLookUpTableData[fLutEntry].fThres ) {
      fCharge = (fLookUpTableData[fLutEntry].fA0)*fPadCharge;
    }else{
      fCharge = (fLookUpTableData[fLutEntry].fA0)*(fLookUpTableData[fLutEntry].fThres) 
	+ (fLookUpTableData[fLutEntry].fA0)*(fPadCharge-fLookUpTableData[fLutEntry].fThres) 
	+ (fLookUpTableData[fLutEntry].fA1)*(fPadCharge-fLookUpTableData[fLutEntry].fThres)*(fPadCharge-fLookUpTableData[fLutEntry].fThres);
    }
    
    fPadData[fDataCount].fCharge = fCharge;
    
    if(fLookUpTableData[fLutEntry].fDetElemId != fPrevDetElemId){
      if((*fNofFiredDetElem)>0){
	fMaxFiredPerDetElem[(*fNofFiredDetElem)-1] = fDataCount;
      }
      (*fNofFiredDetElem)++;
      fPrevDetElemId =  fLookUpTableData[fLutEntry].fDetElemId ;
    }
    
//     HLTDebug("buspatch : %d, detele : %d, id : %d, manu : %d, channel : %d, iX : %d, iY: %d, (X,Y) : (%f, %f), charge : %d, padsize : %f, plane : %d",
// 	     fBusPatchId,fPadData[fDataCount].fDetElemId,
// 	     fIdManuChannel,((dataWord >> 18) & 0x7FF),((dataWord >> 12) & 0x3F),
// 	     fPadData[fDataCount].fIX,fPadData[fDataCount].fIY,
// 	     fPadData[fDataCount].fRealX,fPadData[fDataCount].fRealY,
// 	     fPadData[fDataCount].fCharge,fPadData[fDataCount].fHalfPadSize,fPadData[fDataCount].fPlane);
    
      fDataCount ++;
  }// if charge is more than DC Cut limit condition
  
}
