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

/* $Id:  */
/*
  $Log$
  Revision 1.5  2003/03/17 17:29:54  hristov
  Using Riostream.h instead of iostream.h

  Revision 1.4  2003/03/17 14:26:06  morsch
  Updates by M. Horner and J. Klay

  Revision 1.3  2003/02/05 17:24:25  morsch
  Modifiactions by M. Horner.

  Revision 1.2  2002/10/14 14:55:35  hristov
  Merging the VirtualMC branch to the main development branch (HEAD)

  Revision 1.1.2.1  2002/08/28 15:06:50  alibrary
  Updating to v3-09-01

  Revision 1.1  2002/08/21 10:29:29  schutz
  New classes (by Renan)

*/

//*-- Author: Renan Cabrera (Creighton U.)

#include "AliEMCALParton.h"
#include "Ecommon.h"
#include <Riostream.h>
ClassImp(AliEMCALParton)   
    
//____________________________________________________________________________
AliEMCALParton::AliEMCALParton()
{
  // Default constructor
//  fTrackEnergy = 0;
//  fTrackEta    = 0;
//  fTrackPhi    = 0;
//  fTrackPDG    = 0;
  fNTracks     = 0;
}

AliEMCALParton::AliEMCALParton(Float_t energy, Float_t phi, Float_t eta)
{
  // Constructor
  fEnergy = energy;
  fPhi    = phi;
  fEta    = eta;
//  fTrackEnergy = 0;
//  fTrackEta    = 0;
//  fTrackPhi    = 0;
//  fTrackPDG    = 0;
  fNTracks     = 0;
}

void AliEMCALParton::SetTrackList(Int_t NTracks, Float_t* Energy,  Float_t* Eta, Float_t* Phi, Int_t* PDG)
{

  fNTracks     = NTracks;
  for (Int_t i=0;i<NTracks;i++)
  {
    fTrackEnergy[i] = Energy[i] ;
    fTrackEta[i]    = Eta[i];
    fTrackPhi[i]    = Phi[i];
    fTrackPDG[i]    = PDG[i];
  }
}

void AliEMCALParton::GetTrackList(Float_t* Energy,  Float_t* Eta, Float_t* Phi, Int_t* PDG)
{
  for (Int_t i=0;i<fNTracks;i++)
  {
    Energy[i] = fTrackEnergy[i] ;
    Eta[i]    = fTrackEta[i];
    Phi[i]    = fTrackPhi[i];
    PDG[i]    = fTrackPDG[i];
  } 
}


//____________________________________________________________________________

AliEMCALParton::~AliEMCALParton()
{
  // Destructor
  
}
