// $Id$
// Category: event
//
// Author: I. Hrivnacova
//
// Class AliStackingAction
// -----------------------
// See the class description in the header file.

#include "AliStackingAction.h"
#include "AliGlobals.h"

#include "TG4TrackingAction.h"

#include <G4Track.hh>
#include <G4TrackStack.hh>
#include <G4StackedTrack.hh>
#include <G4StackManager.hh>
#include <G4NeutrinoE.hh>
#include <G4NeutrinoMu.hh>
#include <G4NeutrinoTau.hh>
#include <G4AntiNeutrinoE.hh>
#include <G4AntiNeutrinoMu.hh>
#include <G4AntiNeutrinoTau.hh>

//_____________________________________________________________________________
AliStackingAction::AliStackingAction()
  : AliVerbose("stackingAction",1),
    fStage(0), 
    fTrackingAction(0)
{
// 
}

//_____________________________________________________________________________
AliStackingAction::AliStackingAction(const AliStackingAction& right) 
  : AliVerbose("stackingAction") {
//
  AliGlobals::Exception("AliStackingAction is protected from copying.");
}

//_____________________________________________________________________________
AliStackingAction::~AliStackingAction() {
// 
}

// operators

//_____________________________________________________________________________
AliStackingAction& 
AliStackingAction::operator=(const AliStackingAction &right)
{
  // check assignement to self
  if (this == &right) return *this;
  
  AliGlobals::Exception("AliStackingAction is protected from assigning.");

  return *this;
}

// public methods

//_____________________________________________________________________________
G4ClassificationOfNewTrack 
AliStackingAction::ClassifyNewTrack(const G4Track* track)
{
// Classifies the new track.
// ---

  G4ClassificationOfNewTrack classification;
  if (fStage == 0) { 
    // move all primaries to PrimaryStack
    classification = fPostpone;

    // save primary particle info
    // (secondary particles are stored 
    //  by AlTrackingAction::PreUserTrackingAction() method)
    if (fTrackingAction->GetSavePrimaries())
      fTrackingAction->TrackToStack(track);
  }  
  else {
     // exclude neutrinos
    G4ParticleDefinition* particle = track->GetDefinition();
    if( particle == G4NeutrinoE::NeutrinoEDefinition() ||
        particle == G4NeutrinoMu::NeutrinoMuDefinition() ||
        particle == G4NeutrinoTau::NeutrinoTauDefinition() ||
        particle == G4AntiNeutrinoE::AntiNeutrinoEDefinition() ||
        particle == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition() ||
        particle == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()) {

        return fKill;	 
     }	

     G4int parentID = track->GetParentID();
     if (parentID ==0) { 
        return fUrgent; 
     }
     else { 
        return fWaiting; 
     }
  }
  return classification;
}

//_____________________________________________________________________________
void AliStackingAction::NewStage()
{
// Called by G4 kernel at the new stage of stacking.
// ---

  fStage++;
  
  if (VerboseLevel() > 1) {
    G4cout << "AliStackingAction::NewStage " << fStage 
           << " has been started." << G4endl;
  }

  if (stackManager->GetNUrgentTrack() == 0 &&
      stackManager->GetNPostponedTrack() != 0 ) {
      
      stackManager->TransferOneStackedTrack(fPostpone, fUrgent);
  }
}
    
//_____________________________________________________________________________
void AliStackingAction::PrepareNewEvent()
{
// Called by G4 kernel at the beginning of event.
// ---

  fStage = 0;
  fTrackingAction = TG4TrackingAction::Instance();
}


