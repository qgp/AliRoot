/**************************************************************************
 * Copyright(c) 1998-2000, ALICE Experiment at CERN, All rights reserved. *
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

#include <Riostream.h>
#include <TDirectory.h>
#include <TPDGCode.h>

#include "AliRun.h"
#include "AliRunDigitizer.h"
#include "AliRunLoader.h"
#include "AliLoader.h"

#include "AliMUONDigitizer.h"
#include "AliMUONConstants.h"
#include "AliMUONChamber.h"
#include "AliMUONHitMapA1.h"
#include "AliMUON.h"
#include "AliMUONChamber.h"
#include "AliMUONConstants.h"
#include "AliMUONDigit.h"
#include "AliMUONDigitizer.h"
#include "AliMUONHit.h"
#include "AliMUONHitMapA1.h"
#include "AliMUONPadHit.h"
#include "AliMUONTransientDigit.h"

/////////////////////////////////////////////////////////////////////////////////////
//
// AliMUONDigitizer should be base abstract class of all digitisers in the MUON 
// module. It implements the common functionality of looping over input streams
// filling the fTDList and writing the fTDList to the output stream. 
// Inheriting digitizers need to override certain methods to choose and initialize
// the correct input and output trees, apply the correct detector response if any
// and implement how the transient digits are generated from the input stream.
//
/////////////////////////////////////////////////////////////////////////////////////

ClassImp(AliMUONDigitizer)

//___________________________________________
AliMUONDigitizer::AliMUONDigitizer() : 
	AliDigitizer(),
	fHitMap(0),
	fTDList(0),
	fTDCounter(0),
	fMask(0),
	fSignal(0),
	fDebug(0)
{
// Default constructor.
// Initializes all pointers to NULL.

	runloader = NULL;
	gime = NULL;
	pMUON = NULL;
	muondata = NULL;
};

//___________________________________________
AliMUONDigitizer::AliMUONDigitizer(AliRunDigitizer* manager) : 
	AliDigitizer(manager),
	fHitMap(0),
	fTDList(0),
	fTDCounter(0),
	fMask(0),
	fSignal(0),
	fDebug(0)
{
// Constructor which should be used rather than the default constructor.
// Initializes all pointers to NULL.

	runloader = NULL;
	gime = NULL;
	pMUON = NULL;
	muondata = NULL;
};

//------------------------------------------------------------------------
Bool_t AliMUONDigitizer::Init()
{
// Does nothing. 
	return kTRUE;
}

//------------------------------------------------------------------------
void AliMUONDigitizer::Exec(Option_t* option)
{
// The main work loop starts here. 
// The digitization process is broken up into two steps: 
// 1) Loop over input streams and create transient digits from the input.
//    Done in GenerateTransientDigits()
// 2) Loop over the generated transient digits and write them to the output
//    stream. Done in CreateDigits()

	if (GetDebug() > 0) Info("Exec", "Running digitiser.");
	ParseOptions(option);

	if (fManager->GetNinputs() == 0)
	{
		Warning("Exec", "No inputs set, nothing to do.");
		return;
	};

	if (! FetchLoaders(fManager->GetInputFolderName(0), runloader, gime) ) return;
	if (! FetchGlobalPointers(runloader) ) return;

	InitArrays();
	
	if (GetDebug() > 1) Info("Exec", "Event Number is %d.", fManager->GetOutputEventNr());

	// Loop over files to merge and to digitize
	fSignal = kTRUE;
	for (Int_t inputFile = 0; inputFile < fManager->GetNinputs(); inputFile++)
	{
		fMask = fManager->GetMask(inputFile);
		if (GetDebug() > 1) 
			Info("Exec", "Digitising folder %d, with fMask = %d: %s", inputFile, fMask,
			     (const char*)fManager->GetInputFolderName(inputFile));

		if (inputFile != 0)
			// If this is the first file then we already have the loaders loaded.
			if (! FetchLoaders(fManager->GetInputFolderName(inputFile), runloader, gime) )
				continue;
		else
			// If this is not the first file then it is assumed to be background.
			fSignal = kFALSE;

		if (! InitInputData(gime) ) continue;
		GenerateTransientDigits();
		CleanupInputData(gime);
	};

	Bool_t ok = FetchLoaders(fManager->GetOutputFolderName(), runloader, gime);
	if (ok) ok = InitOutputData(gime);
	if (ok) CreateDigits();
	if (ok) CleanupOutputData(gime);

	CleanupArrays();
};

//--------------------------------------------------------------------------
void AliMUONDigitizer::AddOrUpdateTransientDigit(AliMUONTransientDigit* mTD)
{
// Checks to see if the transient digit exists in the corresponding fHitMap.
// If it does then the digit is updated otherwise it is added. 

	if (ExistTransientDigit(mTD)) 
	{
		UpdateTransientDigit(mTD);
		delete mTD;  // The new digit can be deleted because it was not added.
	}
	else 
		AddTransientDigit(mTD);
};

//------------------------------------------------------------------------
void AliMUONDigitizer::UpdateTransientDigit(AliMUONTransientDigit* mTD)
{
// Update the transient digit that is already in the fTDList by adding the new
// transient digits charges and track lists to the existing one.

	if (GetDebug() > 3)
		Info("UpdateTransientDigit", "Updating transient digit 0x%X", (void*)mTD);
	// Choosing the maping of the cathode plane of the chamber:
	Int_t iNchCpl= mTD->Chamber() + (mTD->Cathode()-1) * AliMUONConstants::NCh();
	AliMUONTransientDigit *pdigit = 
		static_cast<AliMUONTransientDigit*>( fHitMap[iNchCpl]->GetHit(mTD->PadX(),mTD->PadY()) );

	// update charge
	pdigit->AddSignal( mTD->Signal() );
	pdigit->AddPhysicsSignal( mTD->Physics() );  
	
	// update list of tracks
	Int_t ntracks = mTD->GetNTracks();
	if (ntracks > kMAXTRACKS)  // Truncate the number of tracks to kMAXTRACKS if we have to.
	{
		if (GetDebug() > 0)
		{
			Warning("UpdateTransientDigit", 
				"TransientDigit returned the number of tracks to be %d, which is bigger than kMAXTRACKS.",
				ntracks);
			Warning("UpdateTransientDigit", "Reseting the number of tracks to be %d.", kMAXTRACKS);
		}
		ntracks = kMAXTRACKS;
	};
	
	for (Int_t i = 0; i < ntracks; i++)
	{
		pdigit->UpdateTrackList( mTD->GetTrack(i), mTD->GetCharge(i) );
	};
};

//------------------------------------------------------------------------
void AliMUONDigitizer::AddTransientDigit(AliMUONTransientDigit* mTD)
{
// Adds the transient digit to the fTDList and sets the appropriate entry
// in the fHitMap arrays. 

	if (GetDebug() > 3)
		Info("AddTransientDigit", "Adding transient digit 0x%X", (void*)mTD);
	// Choosing the maping of the cathode plane of the chamber:
	Int_t iNchCpl= mTD->Chamber() + (mTD->Cathode()-1) * AliMUONConstants::NCh();
	fTDList->AddAtAndExpand(mTD, fTDCounter);
	fHitMap[iNchCpl]->SetHit( mTD->PadX(), mTD->PadY(), fTDCounter);
	fTDCounter++;
};

//------------------------------------------------------------------------
Bool_t AliMUONDigitizer::ExistTransientDigit(AliMUONTransientDigit* mTD)
{
// Checks if the transient digit already exists on the corresponding fHitMap.
// i.e. is there a transient digit on the same chamber, cathode and pad position 
// as mTD. If yes then kTRUE is returned else kFASLE is returned.

	// Choosing the maping of the cathode plane of the chamber:
	Int_t iNchCpl= mTD->Chamber() + (mTD->Cathode()-1) * AliMUONConstants::NCh();
	return( fHitMap[iNchCpl]->TestHit(mTD->PadX(), mTD->PadY()) );
};

//-----------------------------------------------------------------------
void AliMUONDigitizer::CreateDigits()
{
// Loops over the fTDList for each cathode, gets the correct signal for the 
// digit and adds the new digit to the output stream.

	if (GetDebug() > 1) Info("CreateDigits", "Creating digits...");
	for (Int_t icat = 0; icat < 2; icat++)
	{
		//
		// Filling Digit List
		Int_t nentries = fTDList->GetEntriesFast();
		for (Int_t nent = 0; nent < nentries; nent++)
		{
			AliMUONTransientDigit* td = (AliMUONTransientDigit*)fTDList->At(nent);
			if (td == NULL) continue; 
			
			// Must be the same cathode, otherwise we will fill a mixture
			// of digits from both cathodes.
			if (icat != td->Cathode() - 1) continue;
			
			if (GetDebug() > 2)
				Info("CreateDigits", "Creating digit from transient digit 0x%X", (void*)td);

			Int_t q = GetSignalFrom(td);
			if (q > 0) AddDigit(td, q);
		};
		FillOutputData();
	};
};

//------------------------------------------------------------------------
void AliMUONDigitizer::AddDigit(AliMUONTransientDigit* td, Int_t response_charge)
{
// Prepares the digits, track and charge arrays in preparation for a call to
// AddDigit(Int_t, Int_t[kMAXTRACKS], Int_t[kMAXTRACKS], Int_t[6])
// This method is called by CreateDigits() whenever a new digit needs to be added
// to the output stream trees.
// The response_charge value is used as the Signal of the new digit. 
// The OnWriteTransientDigit method is also called just before the adding the 
// digit to allow inheriting digitizers to be able to do some specific processing 
// at this point. 

	Int_t tracks[kMAXTRACKS];
	Int_t charges[kMAXTRACKS];
	Int_t digits[6];
      
	digits[0] = td->PadX();
	digits[1] = td->PadY();
	digits[2] = td->Cathode() - 1;
	digits[3] = response_charge;
	digits[4] = td->Physics();
	digits[5] = td->Hit();
	
	Int_t nptracks = td->GetNTracks();
	if (nptracks > kMAXTRACKS)
	{
		if (GetDebug() > 0)
		{
			Warning("AddDigit", 
				"TransientDigit returned the number of tracks to be %d, which is bigger than kMAXTRACKS.",
				nptracks);
			Warning("AddDigit", "Reseting the number of tracks to be %d.", kMAXTRACKS);
		}
		nptracks = kMAXTRACKS;
	};
	
	for (Int_t i = 0; i < nptracks; i++) 
	{
		tracks[i]   = td->GetTrack(i);
		charges[i]  = td->GetCharge(i);
	};

	// Sort list of tracks according to charge
	SortTracks(tracks,charges,nptracks);

	if (nptracks < kMAXTRACKS )
	{
		for (Int_t i = nptracks; i < kMAXTRACKS; i++)
		{
			tracks[i]  = -1;
			charges[i] = 0;
		};
	};

	if (GetDebug() > 3) Info("AddDigit", "Adding digit with charge %d.", response_charge);

	OnWriteTransientDigit(td);
	AddDigit(td->Chamber(), tracks, charges, digits);
};

//------------------------------------------------------------------------
void AliMUONDigitizer::OnCreateTransientDigit(AliMUONTransientDigit* /*digit*/, TObject* /*source_object*/)
{
	// Does nothing.
	//
	// This is derived by Digitisers that want to trace which digits were made from
	// which hits.
};

//------------------------------------------------------------------------
void AliMUONDigitizer::OnWriteTransientDigit(AliMUONTransientDigit* /*digit*/)
{
	// Does nothing.
	//
	// This is derived by Digitisers that want to trace which digits were made from
	// which hits.
};

//------------------------------------------------------------------------
Bool_t AliMUONDigitizer::FetchLoaders(const char* foldername, AliRunLoader*& runloader, AliMUONLoader*& muonloader)
{
// Fetches the run loader from the current folder, specified by 'foldername'. 
// The muon loader is then loaded from the fetched run loader. 
// kTRUE is returned if no error occurred otherwise kFALSE is returned. 

	if (GetDebug() > 2)
		Info("FetchLoaders", "Fetching run loader and muon loader from folder: %s", foldername);

	runloader = AliRunLoader::GetRunLoader(foldername);
	if (runloader == NULL)
	{
		Error("FetchLoaders", "RunLoader not found in folder: %s", foldername);
		return kFALSE; 
	}                                                                                       
	muonloader = (AliMUONLoader*) runloader->GetLoader("MUONLoader");
	if (muonloader == NULL) 
	{
		Error("FetchLoaders", "MUONLoader not found in folder: %s", foldername);
		return kFALSE; 
	}
	return kTRUE;
};

//------------------------------------------------------------------------
Bool_t AliMUONDigitizer::FetchGlobalPointers(AliRunLoader* runloader)
{
// Fetches the AliRun object into the global gAlice pointer from the specified
// run loader. The AliRun object is loaded into memory using the run loader if 
// not yet loaded. The MUON module object is then loaded from gAlice and 
// AliMUONData fetched from the MUON module. 
// kTRUE is returned if no error occurred otherwise kFALSE is returned. 

	if (GetDebug() > 2)
		Info("FetchGlobalPointers", "Fetching gAlice, MUON module and AliMUONData from runloader 0x%X.",
			(void*)runloader
		    );

	if (runloader->GetAliRun() == NULL) runloader->LoadgAlice();
	gAlice = runloader->GetAliRun();
	if (gAlice == NULL)
	{
		Error("FetchGlobalPointers", "Could not find the AliRun object in runloader 0x%X.", (void*)runloader);
		return kFALSE;
	};
	pMUON = (AliMUON*) gAlice->GetDetector("MUON");
	if (pMUON == NULL)
	{
		Error("FetchGlobalPointers", "Could not find the MUON module in runloader 0x%X.", (void*)runloader);
		return kFALSE;
	};
	muondata = pMUON->GetMUONData();
	if (muondata == NULL)
	{
		Error("FetchGlobalPointers", "Could not find AliMUONData object in runloader 0x%X.", (void*)runloader);
		return kFALSE;
	};
	return kTRUE;
}

//------------------------------------------------------------------------
void AliMUONDigitizer::ParseOptions(Option_t* options)
{
// Called by the Exec method. ParseOptions should parse the option string given to the Exec method.
// 
// The following options are defined:
// 	"debug" - Sets the debug level to 99, which will show all debug messages.
// 	"deb"   - Same as "debug", implemented for backward comparability.
//
// If an invalid option is specified it is simply ignored.

	TString optionString = options;
	if (optionString.Data() == "debug" || 
		optionString.Data() == "deb"   // maintained for compatability.
	   )
	{
		Info("ParseOptions", "Called with option \"debug\".");
		SetDebug(99);
	};
};

//------------------------------------------------------------------------
void AliMUONDigitizer::InitArrays()
{
// Creates a new fTDList object. 
// Also creates an array of 2 * chamber_number AliMUONHitMapA1 objects
// in the fHitMaps array. Each one is set to a chamber and cathode
// specific segmentation model. 
//
// Note: the fTDList and fHitMap arrays must be NULL before calling this method.

	if (GetDebug() > 1) Info("InitArrays", "Initialising internal arrays.");
	if (GetDebug() > 3) Info("InitArrays", "Creating transient digits list.");
	fTDList = new TObjArray;
	
	// Array of pointer of the AliMUONHitMapA1:
	//  two HitMaps per chamber, or one HitMap per cahtode plane
	fHitMap = new AliMUONHitMapA1* [2*AliMUONConstants::NCh()];

	// Loop over chambers for the definition AliMUONHitMap
	for (Int_t i = 0; i < AliMUONConstants::NCh(); i++) 
	{
		if (GetDebug() > 3) Info("InitArrays", "Creating hit map for chamber %d, cathode 1.", i+1);
		AliMUONChamber* chamber = &(pMUON->Chamber(i));
		AliSegmentation* c1Segmentation = chamber->SegmentationModel(1); // Cathode plane 1
		fHitMap[i] = new AliMUONHitMapA1(c1Segmentation, fTDList);
		if (GetDebug() > 3) Info("InitArrays", "Creating hit map for chamber %d, cathode 2.", i+1);
		AliSegmentation* c2Segmentation = chamber->SegmentationModel(2); // Cathode plane 2
		fHitMap[i+AliMUONConstants::NCh()] = new AliMUONHitMapA1(c2Segmentation, fTDList);
	};
};

//------------------------------------------------------------------------
void AliMUONDigitizer::CleanupArrays()
{
// The arrays fTDList and fHitMap are deleted and the pointers set to NULL.

	if (GetDebug() > 1) Info("CleanupArrays", "Deleting internal arrays.");
	for(Int_t i = 0; i < 2*AliMUONConstants::NCh(); i++)
	{
		if (GetDebug() > 3) Info("CleanupArrays", "Deleting hit map for chamber %d, cathode %d.", 
			i%AliMUONConstants::NCh()+1, i/AliMUONConstants::NCh()+1);
		delete fHitMap[i];
	};
	delete [] fHitMap;
	fHitMap = NULL;
	
	if (GetDebug() > 3) Info("CleanupArrays", "Deleting transient digits list.");
	fTDList->Delete();
	delete fTDList;
	fTDList = NULL;
};

//------------------------------------------------------------------------
void AliMUONDigitizer::SortTracks(Int_t *tracks, Int_t *charges, Int_t ntr)
{
//
// Sort the list of tracks contributing to a given digit
// Only the 3 most significant tracks are actually sorted
//

	if (ntr <= 1) return;

	//
	//  Loop over signals, only 3 times
	//

	Int_t qmax;
	Int_t jmax;
	Int_t idx[3] = {-2,-2,-2};
	Int_t jch[3] = {-2,-2,-2};
	Int_t jtr[3] = {-2,-2,-2};
	Int_t i, j, imax;

	if (ntr < 3) imax = ntr;
	else imax=3;
	
	for(i = 0; i < imax; i++)
	{
		qmax=0;
		jmax=0;

		for(j = 0; j < ntr; j++)
		{
			if (	(i == 1 && j == idx[i-1]) || 
				(i == 2 && (j == idx[i-1] || j == idx[i-2]))
			   ) 
				continue;

			if(charges[j] > qmax) 
			{
				qmax = charges[j];
				jmax = j;
			}       
		} 

		if(qmax > 0)
		{
			idx[i] = jmax;
			jch[i] = charges[jmax]; 
			jtr[i] = tracks[jmax]; 
		}

	} 

	for(i = 0; i < 3; i++)
	{
		if (jtr[i] == -2) 
		{
			charges[i] = 0;
			tracks[i] = 0;
		} 
		else 
		{
			charges[i] = jch[i];
			tracks[i] = jtr[i];
		}
	}
};
