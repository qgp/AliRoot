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
$Log$
Revision 1.4  2002/10/14 14:55:35  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.2.6.2  2002/07/24 10:06:16  alibrary
Updating VirtualMC

Revision 1.3  2002/05/22 13:48:43  morsch
Pdg code added to track list.

*/

//*-- Author: Andreas Morsch (CERN)

#include "AliEMCALJet.h"
#include "Ecommon.h"

ClassImp(AliEMCALJet)

//____________________________________________________________________________
AliEMCALJet::AliEMCALJet()
{
// Default constructor
}

AliEMCALJet::AliEMCALJet(Float_t energy, Float_t phi, Float_t eta)
{
// Constructor
    fEnergy = energy;
    fPhi    = phi;
    fEta    = eta;
}

//____________________________________________________________________________
AliEMCALJet::~AliEMCALJet()
{
// Destructor
}


void AliEMCALJet::SetTrackList(Int_t n, Float_t* pt, Float_t* eta, Float_t* phi, Int_t* pdg)
{
//
// 
    fNt = n;
    for (Int_t i = 0; i < n; i++) {
	fPtT [i]  = pt [i];
	fEtaT[i]  = eta[i];
	fPhiT[i]  = phi[i];
	fPdgT[i]  = pdg[i];
    }
}



Int_t AliEMCALJet::TrackList(Float_t* pt, Float_t* eta, Float_t* phi, Int_t* pdg)
{
//
// 
    for (Int_t i = 0; i < fNt; i++) {
	pt [i] = fPtT [i];
	eta[i] = fEtaT[i];
	phi[i] = fPhiT[i];
	pdg[i] = fPdgT[i];
    }
    return fNt;
}







