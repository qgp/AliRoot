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
Revision 1.6.4.1  2002/06/06 14:23:57  hristov
Merged with v3-08-02

$Log$
Revision 1.9  2002/10/22 14:45:38  alibrary
Introducing Riostream.h

Revision 1.8  2002/10/14 14:57:00  hristov
Merging the VirtualMC branch to the main development branch (HEAD)

Revision 1.6.6.1  2002/06/10 17:51:15  hristov
Merged with v3-08-02

Revision 1.7  2002/04/24 22:02:31  nilsen
New SDigits and Digits routines, and related changes,  (including new
noise values).

*/

#include <Riostream.h>
#include <TMath.h>
#include <TString.h>
#include "AliITSetfSDD.h"

////////////////////////////////////////////////////////////////////////
// Version: 0
// Written by Piergiorgio Cerello
// November 23 1999
//
//_____________________________________________________________________________


ClassImp(AliITSetfSDD)

Int_t ppower(Int_t b, Int_t e) {
  Int_t power = 1;
  for(Int_t i=0; i<e; i++) power *= b;
  return power;
}

AliITSetfSDD::AliITSetfSDD(Double_t timestep, Int_t amplif)
{
  // sampling time in ns

  fTimeDelay = 53.5;
  if(amplif == 2) fTimeDelay = 35.5;
  fSamplingTime = timestep;

  fT0 = 0.;
  fDf = ppower(10,9)/(kMaxNofSamples*fSamplingTime);

  Int_t i,j;
  for(i=0; i<kMaxNofPoles; i++) {
    fZeroM[i] = 0.;
    fZeroR[i] = 0.;
    fZeroI[i] = 0.;
    fPoleM[i] = 0.;
    fPoleR[i] = 0.;
    fPoleI[i] = 0.;
  }
  // Alice

  // PASCAL amplif
  fA0 = 5.53269815e+11; 
  fPoleM[0] = 3.;
  fPoleR[0] = -8280000.; 
  fPoleI[0] = 0.; 

  if(amplif == 2) { // OLA amplif.
    fA0 = 24000.;
    fPoleM[0] = 1.;
    fPoleR[0] = -3000000.;
    fPoleI[0] = 4000000.;
    fPoleM[1] = 1.;
    fPoleR[1] = fPoleR[0];
    fPoleI[1] = -fPoleI[0]; 
  }

  if( amplif == 3 ) { // old PASCAL
    fA0 = 16500.; // AL: 16500.;  // TB: 24000.; // 26000.; // 24000.; // 18000.; 
    fPoleM[0] = 1.;
    fPoleR[0] = -4140000.; // AL: -4140000.; // TB: -3000000.; // -3750000.; // -3500000; // -3000000.; 
    fPoleI[0] = 0.; // AL: 0.; // TB: 4000000.; // 3750000.; // 3500000.; // 3000000.; 
    fPoleM[1] = 1.;
    fPoleR[1] = fPoleR[0];
    fPoleI[1] = -fPoleI[0]; 
  }

  //cout << "fA0: " << fA0 << endl;
  //cout << "fTimeDelay: " << fTimeDelay << endl;
  
  // Compute Transfer Function

  Double_t PI = acos(-1.);
  for(i=0; i<=kMaxNofSamples/2; i++) {
    Double_t frequency = fDf*i;
    Double_t VM = fA0;
    Double_t VA = 0.;
    for(Int_t k=0; k<kMaxNofPoles; k++) {
      if(fZeroM[k]) {
        Double_t VZR = -fZeroR[k];
        Double_t VZI = frequency - fZeroI[k];
        Double_t VZM = TMath::Sqrt(VZR*VZR+VZI*VZI);
        Double_t VZA = TMath::ATan2(VZI,VZR);
	//	cout << "VZM: " << VZM << ", VZA: " << VZA << endl;
	//	cout << "VZR: " << VZR << ", VZI: " << VZI << endl;
        for(j=1; j<= (Int_t) fZeroM[k]; j++) {
          VM *= VZM;
          VA += VZA;
          if(VA >= PI) VA -= (2.*PI);
          if(VA <= -PI) VA += (2.*PI);
	  //cout << "VM: " << VM << ", VA: " << VA << endl;
        }
      }

      if(fPoleM[k]) {
        Double_t VPR = -fPoleR[k];
        Double_t VPI = frequency - fPoleI[k];
	Double_t VPM = TMath::Sqrt(VPR*VPR+VPI*VPI);
	Double_t VPA = TMath::ATan2(VPI,VPR);
	//cout << "VPM: " << VPM << ", VPA: " << VPA << endl;
	//cout << "VPR: " << VPR << ", VPI: " << VPI << endl;
        for(j=1; j<= (Int_t) fPoleM[k]; j++) {
          VM /= VPM;
          VA -= VPA;
          if(VA >= PI) VA -= (2.*PI);
          if(VA <= -PI) VA += (2.*PI);
	  //cout << "VM: " << VM << ", VA: " << VA << endl;
        }
      }
      Double_t VR = VM*cos(VA);
      Double_t VI = VM*sin(VA);
      //cout << "VM: " << VM << ", VA: " << VA << endl;
      //cout << "VR: " << VR << ", VI: " << VI << endl;
      fTfR[i] = VR*ppower(10,9);
      fTfI[i] = VI*ppower(10,9);
      //cout << "fTfR[" << i << "] = " << fTfR[i] << endl;
      //cout << "fTfI[" << i << "] = " << fTfI[i] << endl;
      if(i) {
        fTfR[kMaxNofSamples-i] = fTfR[i];
        fTfI[kMaxNofSamples-i] = -fTfI[i];
      }
    }
  }
  
  // Compute Fourier Weights

  for(i=0; i<=kMaxNofSamples/2; i++) {
    fWR[i] = cos(-2.*PI*i/kMaxNofSamples);
    fWI[i] = sin(-2.*PI*i/kMaxNofSamples);
    if(i) {
      fWR[kMaxNofSamples-i] = fWR[i];
      fWI[kMaxNofSamples-i] = -fWI[i];
    }
  }

}

void AliITSetfSDD::PrintElectronics()
{
  cout << "Time Delay " << fTimeDelay << endl;
  cout << "Sampling Time " << fSamplingTime << endl;
  cout << "Number of Time Samples " << kMaxNofSamples << endl;
  cout << "fT0 " << fT0 << endl;
  cout << "fDf " << fDf << endl;
  cout << "fA0 " << fA0 << endl;

  cout << "Zero's and Pole's" << endl;
  cout << "fZeroM " << endl;
  Int_t i;
  for(i=0; i<kMaxNofPoles; i++) cout << fZeroM[i] << endl;
  cout << "fZero_R " << endl;
  for(i=0; i<kMaxNofPoles; i++) cout << fZeroR[i] << endl;
  cout << "fZeroI " << endl;
  for(i=0; i<kMaxNofPoles; i++) cout << fZeroI[i] << endl;
  cout << "fPoleM " << endl;
  for(i=0; i<kMaxNofPoles; i++) cout << fPoleM[i] << endl;
  cout << "fPoleR " << endl;
  for(i=0; i<kMaxNofPoles; i++) cout << fPoleR[i] << endl;
  cout << "fPoleI " << endl;
  for(i=0; i<kMaxNofPoles; i++) cout << fPoleI[i] << endl;

  cout << "Transfer function" << endl;
  cout << "Real Part" << endl;
  for(i=0; i<kMaxNofSamples; i++) cout << fTfR[i] << endl;
  cout << "Imaginary Part " << endl;
  for(i=0; i<kMaxNofSamples; i++) cout << fTfI[i] << endl;

  cout << "Fourier Weights" << endl;
  cout << "Real Part" << endl;
  for(i=0; i<kMaxNofSamples; i++) cout << fWR[i] << endl;
  cout << "Imaginary Part " << endl;
  for(i=0; i<kMaxNofSamples; i++) cout << fWI[i] << endl;
}








