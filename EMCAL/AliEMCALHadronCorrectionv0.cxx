/**************************************************************************
 * Copyright(c) 1998-2002, ALICE Experiment at CERN, All rights reserved. *
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

Revision 1.3  2002/04/11 19:24:42  nilsen
fixed a complation warning about not brace-enclosing the sub-elements of
the static Double_t c[naxVariant][nPol].

Revision 1.2  2002/02/04 15:11:44  hristov
Use TMath::Abs instead of fabs (Alpha)

Revision 1.1  2002/01/17 23:52:43  morsch
First commit.

*/


#include "AliEMCALHadronCorrectionv0.h"

const Int_t maxVariant = 8;  // size eta grid
const Int_t nVec = 10;       // size momentum grid
const Int_t nPol = 4;        // number coefficients of polinom
static Double_t etaGrid[maxVariant]={ 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.67};
static Double_t pGrid[nVec]={ 0.2, 0.5, 1.0, 2.0, 3.0, 5.0,10.0,15.0,25.0,40.0};
// c[][] - first index for eta, second for momentum 
static Double_t c[maxVariant][nPol] ={
    {1.305705e-01, 3.725653e-01, -1.219962e-02, 1.806235e-04},
    {1.296153e-01, 3.827408e-01, -1.238640e-02, 1.822804e-04},
    {1.337690e-01, 3.797454e-01, -1.245227e-02, 1.848243e-04},
    {1.395796e-01, 3.623994e-01, -9.196803e-03, 1.243278e-04},
    {1.457184e-01, 3.753655e-01, -1.035324e-02, 1.473447e-04},
    {1.329164e-01, 4.219044e-01, -1.310515e-02, 1.948883e-04},
    {8.136581e-02, 4.646087e-01, -1.531917e-02, 2.274749e-04},
    {1.119836e-01, 4.262497e-01, -1.160125e-02, 1.628738e-04} };

ClassImp(AliEMCALHadronCorrectionv0)

AliEMCALHadronCorrectionv0* AliEMCALHadronCorrectionv0::fHadrCorr = 0;

AliEMCALHadronCorrectionv0::AliEMCALHadronCorrectionv0(const char *name,const char *title) 
                           :AliEMCALHadronCorrection(name, title)
{
  fHadrCorr = this;
}

AliEMCALHadronCorrectionv0*
AliEMCALHadronCorrectionv0::Instance()
{
  fHadrCorr = new AliEMCALHadronCorrectionv0();
  return fHadrCorr;
}

Double_t 
AliEMCALHadronCorrectionv0::GetEnergy(const Double_t pmom,const Double_t eta,const Int_t gid)
{
  Int_t iEta=0; // index 
  Double_t etaw = TMath::Abs(eta);
  if(etaw > etaGrid[maxVariant-1]) etaw = etaGrid[maxVariant-1];
  for(Int_t i=0; i<maxVariant; i++) if(eta>=etaGrid[i]) {iEta = i; break;}

  Double_t e[2], y, pw = pmom;
  if(pmom > pGrid[nVec-1]) pw = pGrid[nVec-1];
  for(Int_t i=0; i<2; i++){ // e for two eta value
    e[i] = c[iEta][0];
    y = 1.;
    for(Int_t j=1; j<nPol; j++){
      y *= pw;
      e[i] += c[iEta][j]*y;
    }
    if(i==0) iEta ++;
  }

  Double_t deta = etaGrid[iEta] - etaGrid[iEta-1];
  Double_t a = (e[1] - e[0])/deta; // slope 
  Double_t energy = e[0] + a*(eta-etaGrid[iEta-1]);
  return energy;
}
