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

//-----------------------------------------------------------------
//           Implementation of the ESD class
//   This is the class to deal with during the phisical analysis of data
//
//      Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-----------------------------------------------------------------

#include "AliESD.h"

ClassImp(AliESD)

//_______________________________________________________________________
AliESD::AliESD():
  fEventNumber(0),
  fRunNumber(0),
  fTrigger(0),
  fRecoVersion(0),
  fMagneticField(0),
  fPrimaryVertex(),
  fTracks("AliESDtrack",15000),
  fMuonTracks("AliESDMuonTrack",30),
  fHLTConfMapTracks("AliESDHLTtrack",25000),
  fHLTHoughTracks("AliESDHLTtrack",15000),
  fV0s("AliESDv0",200),
  fCascades("AliESDcascade",20),
  fPHOSParticles(0), 
  fEMCALParticles(0), 
  fFirstPHOSParticle(-1), 
  fFirstEMCALParticle(-1)
{
}

void AliESD::Print(Option_t *) const {
 //
  // Print header information of the event
  //
  Info("Print","ESD run information");
  printf("Event # %d Run # %d Trigger %ld Magnetic field %f \n",
	 GetEventNumber(),
	 GetRunNumber(),
	 GetTrigger(),
	 GetMagneticField() );
  printf("Vertex: (%.4f +- %.4f, %.4f +- %.4f, %.4f +- %.4f) cm\n",
	 fPrimaryVertex.GetXv(), fPrimaryVertex.GetXRes(),
	 fPrimaryVertex.GetYv(), fPrimaryVertex.GetYRes(),
	 fPrimaryVertex.GetZv(), fPrimaryVertex.GetZRes());
  printf("Event from reconstruction version %d \n",fRecoVersion);
  printf("Number of tracks: \n");
  printf("                 charged   %d\n",GetNumberOfTracks()-GetNumberOfPHOSParticles()-GetNumberOfEMCALParticles());
  printf("                 phos      %d\n", GetNumberOfPHOSParticles());
  printf("                 emcal     %d\n", GetNumberOfEMCALParticles());
  printf("                 muon      %d\n", GetNumberOfMuonTracks());
  printf("                 hlt CF    %d\n", GetNumberOfHLTConfMapTracks());
  printf("                 hlt HT    %d\n", GetNumberOfHLTHoughTracks());
  printf("                 v0        %d\n", GetNumberOfV0s());
  printf("                 cascades  %d\n)", GetNumberOfCascades());
}
