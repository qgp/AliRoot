/**************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
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

/*$Id$*/

/////////////////////////////////////////////////////////////
//
//   Test task
//
// /////////////////////////////////////////////////////////////

#include <TTree.h>
#include <TChain.h>

#include "AliLog.h"
#include "AliESDInputHandler.h"
#include "AliESDtrack.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include "AliAnalysisTaskFilter.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskFilterFriend.h"


ClassImp(AliAnalysisTaskFilterFriend)


//________________________________________________________________________
AliAnalysisTaskFilterFriend::AliAnalysisTaskFilterFriend():
AliAnalysisTaskFilter(),
fESDInput(0),
fESDfriendInput(0)
{
	// Constructor
	
	// Define input and output slots here
	// Input slot #0 works with a TChain
	DefineInput(0, TChain::Class());
	// Output slot #0 writes into a TTree
	//DefineOutput(0,TTree::Class());  
}

//________________________________________________________________________
AliAnalysisTaskFilterFriend::AliAnalysisTaskFilterFriend(const char* name):
AliAnalysisTaskFilter(name),
fESDInput(0),
fESDfriendInput(0)
{
	// Constructor
	
	// Define input and output slots here
	// Input slot #0 works with a TChain
	DefineInput(0, TChain::Class());
	// Output slot #0 writes into a TTree
	//DefineOutput(0,TTree::Class());  
}

//________________________________________________________________________
AliAnalysisTaskFilterFriend::~AliAnalysisTaskFilterFriend()
{

	// dtor

}  

//________________________________________________________________________
void AliAnalysisTaskFilterFriend::Init()
{
	// Initialization
	
	return;
}

//________________________________________________________________________
void AliAnalysisTaskFilterFriend::UserCreateOutputObjects()
{
	//
	// Create the output container
	//
	
	return;
}

//________________________________________________________________________
void AliAnalysisTaskFilterFriend::UserExec(Option_t */*option*/)
{

	AliInfo("Filling Friends");

	fESDInput = dynamic_cast<AliESDEvent*>(InputEvent()); // get the input ESD
	fESDfriendInput = (AliESDfriend*)(fESDInput->FindListObject("AliESDfriend"));
	if(!fESDInput) {
		printf("AliAnalysisTaskFilterFriend::Exec(): no ESD \n");
		return;
	} 
	if(!fESDfriendInput) {
		printf("AliAnalysisTaskFilterFriend::Exec(): no ESDfriend \n");
		return;
	} 
	// attach ESDfriend
	
	AliESDfriend* esdFriendOutput = (AliESDfriend*)ESDfriend();  
	AliInfo(Form("Number of ESD tracks in input = %d ",fESDInput->GetNumberOfTracks()));
	AliInfo(Form("Number of tracks in input friends = %d ",fESDfriendInput->GetNumberOfTracks()));
	AliInfo(Form("Number of tracks in output friendsNew before filtering = %d ",esdFriendOutput->GetNumberOfTracks()));
	
	AliESDfriendTrack* tNull = new AliESDfriendTrack();

	for (Int_t i = 0; i< fESDInput->GetNumberOfTracks(); i++){
		if (i%2 ==0){
			// keep friend
			AliInfo(Form("Keeping %d-th track",i));
			AliESDfriendTrack* tOld = (AliESDfriendTrack*)fESDfriendInput->GetTrack(i);
			AliDebug(2,Form("1P of the %d-th track = %f",i,tOld->Get1P()));
			AliInfo(Form("1P of the %d-th track = %f",i,tOld->Get1P()));
			AliInfo(Form("MaxITScluster %d-th track = %d",i,tOld->GetMaxITScluster()));
			//	tOld->Dump();
			AddFriendTrackAt(tOld,i);
		
		}
		else {
			//discard friend 
			AddFriendTrackAt(tNull,i);
		}
		
	} 
	AliInfo(Form("Number of tracks in output friendsNew after filtering = %d ",esdFriendOutput->GetNumberOfTracks()));
	AliInfo(Form("Number of tracks in output friendsNew after filtering with GetEntries() = %d ",esdFriendOutput->GetEntriesInTracks()));
	return;
}

//________________________________________________________________________
void AliAnalysisTaskFilterFriend::Terminate(Option_t */*option*/)
{
	// Terminate analysis
	//
	AliDebug(2,"AliAnalysisTaskFilterFriend: Terminate() \n");
	
	return;
}
//________________________________________________________________________
Bool_t AliAnalysisTaskFilterFriend::UserSelectESDfriendForCurrentEvent()
{
	// 
	// Selecting or discarding current event
	//

	
	fESDInput = dynamic_cast<AliESDEvent*>(InputEvent()); // get the input ESD
	if ((fESDInput->GetNumberOfTracks())%2 == 0) {
		AliInfo("Selecting event");
		return kTRUE;
	}
	AliInfo("Discarding event");	
	return kFALSE;
	/*
	return kTRUE;
	*/
}
