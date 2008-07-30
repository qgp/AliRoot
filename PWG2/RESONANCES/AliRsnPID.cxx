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

//-------------------------------------------------------------------------
//                      Class AliRsnPID
//                     -------------------
//           Simple collection of reconstructed tracks
//           selected from an ESD event
//           to be used for analysis.
//           .........................................
//
// author: A. Pulvirenti             (email: alberto.pulvirenti@ct.infn.it)
//-------------------------------------------------------------------------

#include <TMath.h>
#include <TDirectory.h>

#include "AliLog.h"
#include "AliRsnMCInfo.h"
#include "AliRsnDaughter.h"
#include "AliRsnEvent.h"

#include "AliRsnPID.h"

ClassImp(AliRsnPID)

const Double_t AliRsnPID::fgkParticleMass[AliRsnPID::kSpecies + 1] =
{
  0.00051099892,   // electron
  0.105658369,     // muon
  0.13957018,      // pion
  0.493677,        // kaon
  0.93827203,      // proton
  0.0              // nothing
};

const char* AliRsnPID::fgkParticleNameLong[AliRsnPID::kSpecies + 1] =
{
  "electron",
  "muon",
  "pion",
  "kaon",
  "proton",
  "unknown"
};

const char* AliRsnPID::fgkParticleNameShort[AliRsnPID::kSpecies + 1] =
{
  "e",
  "mu",
  "pi",
  "K",
  "p",
  "unk"
};

const char* AliRsnPID::fgkParticleNameLatex[AliRsnPID::kSpecies + 1] =
{
  "e",
  "#mu",
  "#pi",
  "K",
  "p",
  "?"
};

const Int_t AliRsnPID::fgkParticlePDG[AliRsnPID::kSpecies + 1] =
{
    11,
    13,
    211,
    321,
    2212,
    0
};

//_____________________________________________________________________________
AliRsnPID::AliRsnPID() :
  TNamed("RsnPID", ""),
  fMaxPt(100.0),
  fMinProb(0.0)
{
//
// Constructor.
// Adds also the object in the default directory of the session.
//

    Int_t i; 
    for (i = 0; i < kSpecies; i++) fPrior[i] = 0.0;
    
    gDirectory->Append(this, kTRUE);
}

//_____________________________________________________________________________
AliRsnPID::EType AliRsnPID::InternalType(Int_t pdg)
//
// Return the internal enum value corresponding to the PDG
// code passed as argument, if possible.
// Otherwise, returns 'kUnknown' by default.
//
{
    EType value;
    Int_t absPDG = TMath::Abs(pdg);

	switch (absPDG) {
        case 11:
            value = kElectron;
            break;
        case 13:
            value = kMuon;
            break;
        case 211:
            value = kPion;
            break;
        case 321:
            value = kKaon;
            break;
        case 2212:
            value = kProton;
            break;
        default:
            value = kUnknown;
    }
    return value;
}


//_____________________________________________________________________________
Int_t AliRsnPID::PDGCode(EType type)
{
//
// Returns the PDG code of the particle type
// specified as argument (w.r. to the internal enum)
//

    if (type >= kElectron && type <= kUnknown) {
        return fgkParticlePDG[type];
    }
    else {
        return 0;
    }
}

//_____________________________________________________________________________
const char* AliRsnPID::ParticleName(EType type, Bool_t shortName)
{
//
// Returns the name of the particle type
// specified as argument (w.r. to the internal enum)
//

    if (type >= kElectron && type <= kUnknown) {
        return shortName ? fgkParticleNameShort[type] : fgkParticleNameLong[type];
    }
    else {
        return shortName ? "unk" : "unknown";
    }
}

//_____________________________________________________________________________
const char* AliRsnPID::ParticleNameLatex(EType type)
{
//
// Returns the name of the particle type
// specified as argument (w.r. to the internal enum)
//

    if (type >= kElectron && type <= kUnknown) {
        return fgkParticleNameLatex[type];
    }
    else {
        return "?";
    }
}

//_____________________________________________________________________________
Double_t AliRsnPID::ParticleMass(EType type)
{
//
// Returns the mass corresponding to the particle type
// specified as argument (w.r. to the internal enum)
//
    /*
    TDatabasePDG *db = TDatabasePDG::Instance();
    Int_t pdg = PDGCode(type);
    return db->GetParticle(pdg)->Mass();
    */
    if (type >= kElectron && type < kSpecies) return fgkParticleMass[type];
    return 0.0;
}

//_____________________________________________________________________________
Bool_t AliRsnPID::ComputeProbs(AliRsnDaughter *daughter)
{
//
// Uses the Bayesian combination of prior probabilities
// with the PID weights of the passed object to compute
// the overall PID probabilities for each particle type.
//
// Once this computation is done, the argument is assigned
// the PID corresponding to the largest probability,
// and its data members are updated accordingly.
// If the track Pt is larger than the cut defined (fMaxPt)
// or the probability is smaller than the cut defined (fMinProb),
// the track is considered unidentified.
//
// If the identification procedure encounters errors,
// the return value will be "FALSE", otherwise it is "TRUE".
//

    // reset all PID probabilities to 0.0
    Int_t i;
    for (i = 0; i < kSpecies; i++) daughter->SetPIDProb(i, 1.0 / (Double_t)kSpecies);

    // multiply weights and priors
    Double_t sum = 0.0, prob[kSpecies];
    for (i = 0; i < kSpecies; i++) {
        prob[i] = fPrior[i] * daughter->PID()[i];
        sum += prob[i];
    }
    if (sum <= (Double_t)0.) {
        AliError(Form("Sum of weights = %f <= 0", sum));
        return kFALSE;
    }

    // normalize
    for (i = 0; i < kSpecies; i++) {
        prob[i] /= sum;
        daughter->SetPIDProb(i, prob[i]);
    }
    
    return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliRsnPID::IdentifiedAs(AliRsnDaughter *d, EType type, Short_t charge)
{
//
// Tells if a particle has can be identified to be of a given tipe and charge.
// If the charge is zero, the check is done only on the PID type, otherwise
// both charge and PID type are required to match.
// If the track momentum is larger than the pt threshold passed to this object,
// or the maximum probability is smaller than the prob thrashold, the return value
// is kFALSE even when the type and charge are matched.
//

    EType dType = TrackType(d);
    if (dType != type) return kFALSE;
    if (charge == 0) {
        return kTRUE;
    }
    else if (charge > 0) {
        return (d->Charge() > 0);
    }
    else {
        return (d->Charge() < 0);
    }
}

//_____________________________________________________________________________
AliRsnPID::EType AliRsnPID::TrackType(AliRsnDaughter *d)
{
//
// Returns the track type according to the object settings
// and to the static settings in the AliRsnDaughter object.
//

    Double_t prob;
    EType type = d->PIDType(prob);
    
    if (d->Pt() > fMaxPt) return kUnknown;
    if (prob < fMinProb) return kUnknown;
    
    return type;
}

//_____________________________________________________________________________
Bool_t AliRsnPID::Process(AliRsnEvent *event)
{
//
// Performs identification for all tracks in a given event.
// Returns the logical AND of all PID operations.
//

    Bool_t check = kTRUE;
    if (!event) return check;
    if (!event->GetTracks()) return check;
    if (event->GetTracks()->IsEmpty()) return check;

    AliRsnDaughter *daughter = 0;
    TObjArrayIter iter(event->GetTracks());
    while ( (daughter = (AliRsnDaughter*)iter.Next()) ) {
        check = check && ComputeProbs(daughter);
    }
    event->FillPIDArrays();

    return check;
}


//_____________________________________________________________________________
void AliRsnPID::SetPriorProbability(EType type, Double_t p)
{
//
// Sets the prior probability for Realistic PID, for a
// given particle species.
//

    if (type >= kElectron && type < kSpecies) {
        fPrior[type] = p;
    }
}

//_____________________________________________________________________________
void AliRsnPID::DumpPriors()
{
//
// Print all prior probabilities
//

    Int_t i;
    Char_t msg[200];
    
    for (i = 0; i < kSpecies; i++) {
        sprintf(msg, "Prior probability for '%s' = %3.5f", fgkParticleNameLong[i], fPrior[i]);
        AliInfo(msg);
    }
}
