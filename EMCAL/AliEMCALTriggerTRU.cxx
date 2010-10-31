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

/*




Author: R. GUERNANE LPSC Grenoble CNRS/IN2P3
*/


#include "AliEMCALTriggerTRU.h"
#include "AliEMCALTriggerPatch.h"
#include "AliEMCALTriggerTRUDCSConfig.h"
#include "AliLog.h"

#include <TClonesArray.h>
#include <TSystem.h>
#include <Riostream.h>
  
namespace
{
	const Int_t kTimeBins       = 16; // number of sampling bins of the FastOR signal
	const Int_t kTimeWindowSize =  4; // 
	const Int_t kNup            =  2; // 
	const Int_t kNdown          =  1; // 
}

ClassImp(AliEMCALTriggerTRU)

//________________
AliEMCALTriggerTRU::AliEMCALTriggerTRU() : AliEMCALTriggerBoard(),
fDCSConfig(0x0)
{
	//
	for (Int_t i=0;i<96;i++) for (Int_t j=0;j<256;j++) fADC[i][j] = 0;
}

//________________
AliEMCALTriggerTRU::AliEMCALTriggerTRU(AliEMCALTriggerTRUDCSConfig* dcsConf, const TVector2& rSize, Int_t mapType) : 
AliEMCALTriggerBoard(rSize),
fDCSConfig(dcsConf)
{
	//
	for (Int_t i=0;i<96;i++) for (Int_t j=0;j<256;j++) fADC[i][j] = 0;

	TVector2 size;
	
	if (dcsConf->GetL0SEL() & 0x0001) // 4-by-4
	{
		size.Set( 1. , 1. );
		SetSubRegionSize( size );
		
		size.Set( 2. , 2. );
		SetPatchSize( size );
	}	
	else                              // 2-by-2
	{
		size.Set( 1. , 1. );
		SetSubRegionSize( size );
		
		size.Set( 1. , 1. );
		SetPatchSize( size );	
	}	
	
	for (Int_t ietam=0;ietam<24;ietam++)
	{
		for (Int_t iphim=0;iphim<4;iphim++)
		{
			// idx: 0..95 since iphim: 0..11 ietam: 0..23
			Int_t idx = ( !mapType ) ? ( 3 - iphim ) + ietam * 4 : iphim + (23 - ietam) * 4;	
	
			// Build a matrix used to get TRU digit id (ADC channel) from (eta,phi)|SM
			fMap[ietam][iphim] = idx; // [0..11][0..3] namely [eta][phi] in SM
		}
	}
}

//________________
AliEMCALTriggerTRU::~AliEMCALTriggerTRU()
{
	//
}

//________________
void AliEMCALTriggerTRU::ShowFastOR(Int_t iTimeWindow, Int_t iChannel)
{
	//
	Int_t iChanF, iChanL;
	
	if (iChannel != -1) iChanF = iChanL = iChannel;
	else
	{
		iChanF =  0;
		iChanL = 96;
	}
	
	for (Int_t i=iChanF;i<iChanL+1;i++)
	{
		printf("\tChannel: %2d - ",i);
		for (Int_t j=0;j<60;j++) 
		{
			if (j == iTimeWindow)
				printf(" | %4d",fADC[i][j]);
			else if (j == iTimeWindow+kTimeWindowSize-1)
				printf(" %4d |",fADC[i][j]);
			else
				printf(" %4d",fADC[i][j]);
		}
		
		printf("\n");
	}
}

//________________
Int_t AliEMCALTriggerTRU::L0()
{
	// Mimick the TRU L0 'virtual' since not yet released algo
	
	// L0 issuing condition is: (2 up & 1 down) AND (time sum > thres)
	// fill a matrix to support sliding window
	// compute the time sum for all the FastOR of a given TRU
	// and then move the space window

	AliDebug(999,"=== Running TRU L0 algorithm ===");
	const Int_t xsize    = Int_t(fRegionSize->X());
	const Int_t ysize    = Int_t(fRegionSize->Y());
	const Int_t zsize    = kNup+kNdown;

	Int_t ***buffer = new Int_t**[xsize];
	for (Int_t x = 0; x < xsize; x++)
	{
		buffer[x] = new Int_t*[ysize];
		
		for (Int_t y = 0; y < ysize; y++)
		{
			buffer[x][y] = new Int_t[zsize];
		}
	}
	
	for (Int_t i = 0; i < fRegionSize->X(); i++) 
		for (Int_t j = 0; j < fRegionSize->Y(); j++) 
			for (Int_t k = 0; k < kNup + kNdown; k++) buffer[i][j][k] = 0;
		
	// Time sliding window algorithm
	for (Int_t i=0; i<=(kTimeBins-kTimeWindowSize); i++) 
	{
		AliDebug(999,Form("----------- Time window: %d\n",i));
		
		for (Int_t j=0; j<fRegionSize->X(); j++)
		{		
			for (Int_t k=0; k<fRegionSize->Y(); k++)
			{
				for (Int_t l=i; l<i+kTimeWindowSize; l++) 
				{
					// [eta][phi][time]
					fRegion[j][k] += fADC[fMap[j][k]][l];	
				}
				
				buffer[j][k][i%(kNup + kNdown)] = fRegion[j][k];
				
				if ( i > kNup + kNdown - 1 ) 
				{	
					for (Int_t v = 0; v < kNup + kNdown - 1; v++) buffer[j][k][v] =  buffer[j][k][v+1];
	
					buffer[j][k][kNup + kNdown - 1] = fRegion[j][k];
				}
				else
				{
					buffer[j][k][i] = fRegion[j][k];
				}
				
//				if (kTimeWindowSize > 4) fRegion[j][k] = fRegion[j][k] >> 1; // truncate time sum to fit 14b
			}
		}
		
		// Don't start to evaluate space sum if a peak can't be findable
		if (i < kNup + kNdown - 1) 
		{
			ZeroRegion();
			continue; 
		}
		
		Int_t **peaks = new Int_t*[xsize];
		for (Int_t x = 0; x < xsize; x++)
		{
			peaks[x] = new Int_t[ysize];
		}
		
		for (Int_t j=0; j<fRegionSize->X(); j++) for (Int_t k=0; k<fRegionSize->Y(); k++) peaks[j][k] = 0;
		
		Int_t nPeaks = 0;
		
		for (Int_t j=0; j<fRegionSize->X(); j++)
		{		
			for (Int_t k=0; k<fRegionSize->Y(); k++)
			{
				Int_t foundU = 0;
				
				Int_t foundD = 0;
		
				for (Int_t l=   1;l<kNup       ;l++) foundU = ( buffer[j][k][l]> buffer[j][k][l-1] && buffer[j][k][l-1] ) ? 1 : 0;
				
				for (Int_t l=kNup;l<kNup+kNdown;l++) foundD = ( buffer[j][k][l]<=buffer[j][k][l-1] && buffer[j][k][l  ] ) ? 1 : 0; 
		
				if ( foundU && foundD ) 
				{
					peaks[j][k] = 1;
					nPeaks++;
				}
			}
		}

		if (!nPeaks)
		{
			ZeroRegion();
			continue;
		}
		
		// Threshold 
		// FIXME: for now consider just one threshold for all patches, should consider one per patch?
		// ANSWE: both solutions will be implemented in the TRU
		// return the list of patches above threshold
		// Theshold checked out from OCDB
		
		SlidingWindow(kL0, fDCSConfig->GetGTHRL0(), i);
		
//		for(Int_t j=0; j<fRegionSize->X(); j++)
//			for (Int_t k=0; k<fRegionSize->Y(); k++) fRegion[j][k] = fRegion[j][k]>>2; // go to 12b before shipping to STU
		
		for (Int_t j=0; j<fPatches->GetEntriesFast(); j++)
		{
			AliEMCALTriggerPatch* p = (AliEMCALTriggerPatch*)fPatches->At( j );

			if ( AliDebugLevel() ) p->Print("");

			TVector2 v; p->Position(v);
			
			Int_t sizeX = (Int_t)(fPatchSize->X() * fSubRegionSize->X());
			
			Int_t sizeY = (Int_t)(fPatchSize->Y() * fSubRegionSize->Y());
			
			const Int_t psize =  sizeX * sizeY; // Number of FastOR in the patch
			
			Int_t foundPeak = 0;
			
			Int_t* idx = new Int_t[psize];
			
			for (Int_t xx=0;xx<sizeX;xx++) 
			{
				for (Int_t yy=0;yy<sizeY;yy++) 
				{   
					Int_t index = xx*sizeY+yy;
					
					idx[index] = fMap[int(v.X()*fSubRegionSize->X())+xx][int(v.Y()*fSubRegionSize->Y())+yy]; // Get current patch FastOR ADC channels 
					
					if (peaks[int(v.X() * fSubRegionSize->X()) + xx][int(v.Y() * fSubRegionSize->Y()) + yy]) 
					{
						foundPeak++;
						
						p->SetPeak(xx, yy, sizeX, sizeY);
					}
					
					if (AliDebugLevel() >= 999) ShowFastOR(i, idx[index]);
				}
			}
			
			if ( !foundPeak ) 
			{
				fPatches->RemoveAt( j );
				fPatches->Compress();
			}				
		}
		
		//Delete, avoid leak
		for (Int_t x = 0; x < xsize; x++)
		{
			delete [] peaks[x];
		}
		delete [] peaks;

		if ( !fPatches->GetEntriesFast() ) // No patch left
			ZeroRegion();
		else                             // Stop the algo when at least one patch is found ( thres & max )
		{
			for (Int_t xx = 0; xx < fRegionSize->X(); xx++) 
			{
				for (Int_t yy = 0; yy < fRegionSize->Y(); yy++) 
				{   
					// Prepare data to be sent to STU for further L1 processing
					// Sent time sum (rollback tuning) is the one before L0 is issued (maximum time sum)
					fRegion[xx][yy] = buffer[xx][yy][1];
				}
			}
			
			break;
		}		
	}
	
	//Delete, avoid leak
	for (Int_t x = 0; x < xsize; x++)
	{
		for (Int_t y = 0; y < ysize; y++)
		{
			delete [] buffer[x][y];
		}
		delete [] buffer[x];
	}
	delete [] buffer;
	
	return fPatches->GetEntriesFast();
}

//________________
void AliEMCALTriggerTRU::SetADC( Int_t channel, Int_t bin, Int_t sig )
{
  //Set ADC value
  if (channel > 95 || bin > 255) {
    AliError("TRU has 96 ADC channels and 256 bins only!");
  }
  else{ 
    fADC[channel][bin] = sig;
  }
}

//________________
void AliEMCALTriggerTRU::SaveRegionADC(Int_t iTRU, Int_t iEvent)
{
	// O for STU Hw
	//
	gSystem->Exec(Form("mkdir -p Event%d",iEvent));
	
	ofstream outfile(Form("Event%d/data_TRU%d.txt",iEvent,iTRU),ios_base::trunc);
	
	for (Int_t i=0;i<96;i++) 
	{
		Int_t ietam = 23 - i/4;
	
		Int_t iphim =  3 - i%4;
		
		outfile << fRegion[ietam][iphim] << endl;
	}

	outfile.close();
}

//________________
void AliEMCALTriggerTRU::Reset()
{
	//
	fPatches->Delete();
	
	ZeroRegion();
	
	for (Int_t i=0;i<96;i++) for (Int_t j=0;j<256;j++) fADC[i][j] = 0;
}

