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
//               Implementation of the ESD V0 vertex class
//
//     Origin: Iouri Belikov, IReS, Strasbourg, Jouri.Belikov@cern.ch
//-------------------------------------------------------------------------
#include <Riostream.h>
#include <TMath.h>
#include <TPDGCode.h>

#include "AliESDv0.h"

ClassImp(AliESDv0)

AliESDv0::AliESDv0() : TObject() {
  //--------------------------------------------------------------------
  // Default constructor  (K0s)
  //--------------------------------------------------------------------
  fPdgCode=kK0Short;
  fEffMass=0.497672;
  fChi2=1.e+33;
  fPos[0]=fPos[1]=fPos[2]=0.;
  fPosCov[0]=fPosCov[1]=fPosCov[2]=fPosCov[3]=fPosCov[4]=fPosCov[5]=0.;
  fNidx = -1;
  fNmom[0]=fNmom[1]=fNmom[2]=0.;
  fNmomCov[0]=fNmomCov[1]=fNmomCov[2]=fNmomCov[3]=fNmomCov[4]=fNmomCov[5]=0.;
  fPidx = -1;
  fPmom[0]=fPmom[1]=fPmom[2]=0.;
  fPmomCov[0]=fPmomCov[1]=fPmomCov[2]=fPmomCov[3]=fPmomCov[4]=fPmomCov[5]=0.;
}

Double_t AliESDv0::ChangeMassHypothesis(Int_t code) {
  //--------------------------------------------------------------------
  // This function changes the mass hypothesis for this V0
  // and returns the "kinematical quality" of this hypothesis 
  //--------------------------------------------------------------------
  Double_t nmass=0.13957, pmass=0.13957, mass=0.49767, ps=0.206;

  fPdgCode=code;

  switch (code) {
  case kLambda0:
    nmass=0.13957; pmass=0.93827; mass=1.1157; ps=0.101; break;
  case kLambda0Bar:
    pmass=0.13957; nmass=0.93827; mass=1.1157; ps=0.101; break;
  case kK0Short: 
    break;
  default:
    cerr<<"AliV0vertex::ChangeMassHypothesis: ";
    cerr<<"invalide PDG code ! Assuming K0s...\n";
    fPdgCode=kK0Short;
    break;
  }

  Double_t pxn=fNmom[0], pyn=fNmom[1], pzn=fNmom[2]; 
  Double_t pxp=fPmom[0], pyp=fPmom[1], pzp=fPmom[2];

  Double_t en=TMath::Sqrt(nmass*nmass + pxn*pxn + pyn*pyn + pzn*pzn);
  Double_t ep=TMath::Sqrt(pmass*pmass + pxp*pxp + pyp*pyp + pzp*pzp);
  Double_t pxl=pxn+pxp, pyl=pyn+pyp, pzl=pzn+pzp;
  Double_t pl=TMath::Sqrt(pxl*pxl + pyl*pyl + pzl*pzl);

  fEffMass=TMath::Sqrt((en+ep)*(en+ep)-pl*pl);

  Double_t beta=pl/(en+ep);
  Double_t pln=(pxn*pxl + pyn*pyl + pzn*pzl)/pl;
  Double_t plp=(pxp*pxl + pyp*pyl + pzp*pzl)/pl;

  Double_t pt2=pxp*pxp + pyp*pyp + pzp*pzp - plp*plp;

  Double_t a=(plp-pln)/(plp+pln);
  a -= (pmass*pmass-nmass*nmass)/(mass*mass);
  a = 0.25*beta*beta*mass*mass*a*a + pt2;

  return (a - ps*ps);
  
}

void AliESDv0::GetPxPyPz(Double_t &px, Double_t &py, Double_t &pz) const {
  //--------------------------------------------------------------------
  // This function returns V0's momentum (global)
  //--------------------------------------------------------------------
  px=fNmom[0]+fPmom[0]; 
  py=fNmom[1]+fPmom[1]; 
  pz=fNmom[2]+fPmom[2]; 
}

void AliESDv0::GetXYZ(Double_t &x, Double_t &y, Double_t &z) const {
  //--------------------------------------------------------------------
  // This function returns V0's position (global)
  //--------------------------------------------------------------------
  x=fPos[0]; 
  y=fPos[1]; 
  z=fPos[2]; 
}

Double_t AliESDv0::GetD(Double_t x0, Double_t y0, Double_t z0) const {
  //--------------------------------------------------------------------
  // This function returns V0's impact parameter
  //--------------------------------------------------------------------
  Double_t x=fPos[0],y=fPos[1],z=fPos[2];
  Double_t px=fNmom[0]+fPmom[0];
  Double_t py=fNmom[1]+fPmom[1];
  Double_t pz=fNmom[2]+fPmom[2];

  Double_t dx=(y0-y)*pz - (z0-z)*py; 
  Double_t dy=(x0-x)*pz - (z0-z)*px;
  Double_t dz=(x0-x)*py - (y0-y)*px;
  Double_t d=TMath::Sqrt((dx*dx+dy*dy+dz*dz)/(px*px+py*py+pz*pz));
  return d;
}
