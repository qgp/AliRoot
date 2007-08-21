
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

/*

  New release on 2007/08/17

The default raw data version (now fRAWversion ) is set to 3
in the constructer because version 3 raw data read and write
are fully debugged.

*/

//////////////////////////////////////////////////
//                                              //
//  TRD front end electronics parameters class  //
//  Contains all FEE (MCM, TRAP, PASA) related  //
//  parameters, constants, and mapping.         //
//                                              //
//////////////////////////////////////////////////

#include <TMath.h>

#include "AliLog.h"
#include "AliTRDfeeParam.h"
#include "AliTRDgeometry.h"
#include "AliTRDCommonParam.h"

ClassImp(AliTRDfeeParam)

AliTRDfeeParam *AliTRDfeeParam::fgInstance   = 0;
Bool_t          AliTRDfeeParam::fgTerminated = kFALSE;

//_____________________________________________________________________________
AliTRDfeeParam* AliTRDfeeParam::Instance()
{
  // Instance constructor
  
  if (fgTerminated != kFALSE) {
    return 0;
  }

  if (fgInstance == 0) {
    fgInstance = new AliTRDfeeParam();
  }  

  return fgInstance;
}

//_____________________________________________________________________________
void AliTRDfeeParam::Terminate()
{
  //
  // Terminate the class and release memory
  //
  
  fgTerminated = kTRUE;

  if (fgInstance != 0) {
    delete fgInstance;
    fgInstance = 0;
  }
}

//_____________________________________________________________________________
AliTRDfeeParam::AliTRDfeeParam()
  :TObject()
  //  ,fGeo(0)
  ,fCP(0)
  ,fTFr1(0)
  ,fTFr2(0)
  ,fTFc1(0)
  ,fTFc2(0)
  ,fRAWversion(3)
  ,fRAWstoreRaw(kTRUE)
{
  //
  // Default constructor
  //
  
  // PASA V.4
  if      (fgkTFnExp == 1) {
    fTFr1 = 1.1563;
    fTFr2 = 0.1299;
    fTFc1 = 0.0657;
    fTFc2 = 0.0000;
  }
  else if (fgkTFnExp == 2) {
    fTFr1 = 1.1563;
    fTFr2 = 0.1299;
    fTFc1 = 0.1141;
    fTFc2 = 0.6241;
  }

  //  fGeo = AliTRDgeometry::Instance();
  fCP  = AliTRDCommonParam::Instance();

}

//_____________________________________________________________________________
AliTRDfeeParam::AliTRDfeeParam(const AliTRDfeeParam &p)
  :TObject(p)
  //  ,fGeo(p.fGeo)
  ,fCP(p.fCP)
  ,fTFr1(p.fTFr1)
  ,fTFr2(p.fTFr2)
  ,fTFc1(p.fTFc1)
  ,fTFc2(p.fTFc2)
  ,fRAWversion(p.fRAWversion)
  ,fRAWstoreRaw(p.fRAWstoreRaw)
{
  //
  // AliTRDfeeParam copy constructor
  //
}

//_____________________________________________________________________________
AliTRDfeeParam::~AliTRDfeeParam()
{
  //
  // AliTRDfeeParam destructor
  //
}

//_____________________________________________________________________________
AliTRDfeeParam &AliTRDfeeParam::operator=(const AliTRDfeeParam &p)
{
  //
  // Assignment operator
  //

  if (this != &p) {
    ((AliTRDfeeParam &) p).Copy(*this);
  }
  return *this;
}

//_____________________________________________________________________________
void AliTRDfeeParam::Copy(TObject &p) const
{
  //
  // Copy function
  //

  //  ((AliTRDfeeParam &) p).fGeo     = fGeo;
  ((AliTRDfeeParam &) p).fCP          = fCP;
  ((AliTRDfeeParam &) p).fTFr1        = fTFr1;
  ((AliTRDfeeParam &) p).fTFr2        = fTFr2;
  ((AliTRDfeeParam &) p).fTFc1        = fTFc1;
  ((AliTRDfeeParam &) p).fTFc2        = fTFc2;
  ((AliTRDfeeParam &) p).fRAWversion  = fRAWversion;
  ((AliTRDfeeParam &) p).fRAWstoreRaw = fRAWstoreRaw;
  
  TObject::Copy(p);
}

//_____________________________________________________________________________
Int_t AliTRDfeeParam::GetPadRowFromMCM(Int_t irob, Int_t imcm) const
{
  //
  // return on which pad row this mcm sits
  //
  
  return fgkNmcmRobInRow*(irob/2) + imcm/fgkNmcmRobInCol;
}

//_____________________________________________________________________________
Int_t AliTRDfeeParam::GetPadColFromADC(Int_t irob, Int_t imcm, Int_t iadc) const
{
  //
  // Return which pad is connected to this adc channel.
  //
  // Return virtual pad number even if ADC is outside chamber
  // to keep compatibility of data processing at the edge MCM.
  // User has to check that this is in the chamber if it is essential.
  // Return -100 if iadc is invalid.
  //
  // Caution: ADC ordering in the online data is opposite to the pad column ordering.
  // And it is not one-by-one correspondence. Precise drawing can be found in:
  // http://wiki.kip.uni-heidelberg.de/ti/TRD/index.php/Image:ROB_MCM_numbering.pdf

  if (iadc < 0 || iadc > 19 ) return -100;
  Int_t mcmcol = imcm%fgkNmcmRobInCol + GetRobSide(irob)*fgkNmcmRobInCol;  // MCM column number on ROC [0..7]
  Int_t padcol = mcmcol*fgkNcolMcm + fgkNcolMcm + 1 - iadc;
  // if( padcol < 0 || padcol >= fgkNcol ) return -1;   // thisi s commented because of reson above KO
  return padcol;
}

//_____________________________________________________________________________
Int_t AliTRDfeeParam::GetMCMfromPad(Int_t irow, Int_t icol) const
{
  //
  // Return on which MCM this pad is directry connected.
  // Return -1 for error.
  //

  if ( irow < 0 || icol < 0 || irow > fgkNrowC1 || icol > fgkNcol ) return -1;

  return (icol%(fgkNcol/2))/fgkNcolMcm + fgkNmcmRobInCol*(irow%fgkNmcmRobInRow);
}

//_____________________________________________________________________________
Int_t AliTRDfeeParam::GetROBfromPad(Int_t irow, Int_t icol) const
{
  //
  // return on which rob this pad is
  //

  return (irow/fgkNmcmRobInRow)*2 + GetColSide(icol);
}

//_____________________________________________________________________________
Int_t AliTRDfeeParam::GetRobSide(Int_t irob) const
{
  //
  // return on which side this rob sits (A side = 0, B side = 1)
  //

  if ( irob < 0 || irob >= fgkNrobC1 ) return -1;
  return irob%2;
}

//_____________________________________________________________________________
Int_t AliTRDfeeParam::GetColSide(Int_t icol) const
{
  //
  // return on which side this column sits (A side = 0, B side = 1)
  //

  if ( icol < 0 || icol >= fgkNcol ) return -1;
  return icol/(fgkNcol/2);
}

//
//void AliTRDfeeParam::GetFilterParam( Float_t &r1, Float_t &r2, Float_t &c1, Float_t &c2, Float_t &ped ) const
//{
  //
  // Return current filter parameter
  //

  //  r1            = fR1;
  //r2            = fR2;
  //c1            = fC1;
  //c2            = fC2;
  //ped           = fPedestal;
//};


//_____________________________________________________________________________
Int_t    AliTRDfeeParam::GetRAWversion()
{
  // Return raw data version (major number only)

  return fRAWversion;
}

//_____________________________________________________________________________
void     AliTRDfeeParam::SetRAWversion( Int_t rawver )
{
  // Set raw data version (major number only)
  // Maximum available number is preset in fgkMaxRAWversion

  if( rawver >= 0 && rawver <= fgkMaxRAWversion ) {

    fRAWversion = rawver ;
  } else {
    AliError(Form("Raw version is out of range: %d",rawver));
  }
}

//_____________________________________________________________________________
Bool_t   AliTRDfeeParam::GetRAWstoreRaw()
{
  // Returns kTRUE if raw data itself is read instead of filtered data

  return fRAWstoreRaw;
}

//_____________________________________________________________________________
void     AliTRDfeeParam::SetRAWstoreRaw( Bool_t storeraw )
{
  // If kTRUE is set, raw data itself is read instead of filtered data

  fRAWstoreRaw = storeraw;
}
