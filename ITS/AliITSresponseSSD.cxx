/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                         *
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

#include <TMath.h>
#include <TString.h>

#include "AliITSresponseSSD.h"
#include "AliITSgeom.h"

ClassImp(AliITSresponseSSD)

//______________________________________________________________________
AliITSresponseSSD::AliITSresponseSSD(){
    // Default Constructor

    fDetPar = 0;
    fNPar   = 0;
    fNoiseP = 0;
    fNoiseN = 0;
    fSigmaP = 0;
    fSigmaN = 0;
    fDiffCoeff = 0;
    fADCpereV  = 0;
}
//______________________________________________________________________
AliITSresponseSSD::AliITSresponseSSD(const char *dataType){
    // constructor

    SetDiffCoeff();
    SetNoiseParam();
    SetDataType(dataType);
    SetSigmaSpread();
    SetParamOptions();
    SetNDetParam();   // Sets fNPar=6 by default.
    SetADCpereV();
    fDetPar = new Float_t[fNPar];
    if (fNPar==6) {
	fDetPar[0]=10.;
	fDetPar[1]=5.;
	fDetPar[2]=0.02;
	fDetPar[3]=0.02;
	fDetPar[4]=0.02;
	fDetPar[5]=0.03;
    } // end if
}
//______________________________________________________________________
AliITSresponseSSD::~AliITSresponseSSD(){
    // destructor

    delete [] fDetPar;
}
//______________________________________________________________________
AliITSresponseSSD& AliITSresponseSSD::operator=(const AliITSresponseSSD &src) {
    // = operator.

    if(&src == this) return *this;

    this->fNPar      = src.fNPar;
    for(Int_t i=0;i<this->fNPar;i++) this->fDetPar[i] = src.fDetPar[i];
    this->fNoiseP    = src.fNoiseP;
    this->fNoiseN    = src.fNoiseN;
    this->fSigmaP    = src.fSigmaP;
    this->fSigmaN    = src.fSigmaN;
    this->fDiffCoeff = src.fDiffCoeff;
    this->fADCpereV  = src.fADCpereV;
    this->fOption1   = src.fOption1;
    this->fOption2   = src.fOption2;
    this->fDataType  = src.fDataType;

    return *this;
}
//_________________________________________________________________________
AliITSresponseSSD::AliITSresponseSSD(const AliITSresponseSSD &src) :
    AliITSresponse(src) {
    // copy constructor

    *this = src;
}
//______________________________________________________________________
void AliITSresponseSSD::SetDetParam(Float_t  *par){
    // set det param
    Int_t i;

    for (i=0; i<fNPar; i++) {
	fDetPar[i]=par[i];
	//printf("\n CompressPar %d %d \n",i,fCPar[i]);    
    } // end for i
}
//______________________________________________________________________
void AliITSresponseSSD::GetDetParam(Float_t  *par){
    // get det param
    Int_t i;

    for (i=0; i<fNPar; i++) {
	par[i]=fDetPar[i];
    } // end for i
}
