/*************************************************************************
* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
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
* provided "as is" without express or implied warranty.                  * **************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
//  Class to define the ALICE Trigger Scalers  
//
//  For each trigger class there are six scalers:
//
//    LOCB       L0 triggers before any vetos 
//    LOCA       L0 triggers after all vetos 
//    L1CB       L1 triggers before any vetos 
//    L1CA       L1 triggers after all vetos 
//    L2CB       L2 triggers before any vetos 
//    L2CA       L2 triggers after all vetos 
//
//////////////////////////////////////////////////////////////////////////////

#include <Riostream.h>

#include "AliLog.h"
#include "AliTriggerScalersESD.h"

ClassImp( AliTriggerScalersESD )

//_____________________________________________________________________________
AliTriggerScalersESD::AliTriggerScalersESD(): 
  TObject(),
  fClassIndex(0),
  fLOCB(0),     
  fLOCA(0),     
  fL1CB(0),     
  fL1CA(0),     
  fL2CB(0),     
  fL2CA(0)      
{
  // Default constructor
}

//_____________________________________________________________________________
AliTriggerScalersESD::AliTriggerScalersESD( UChar_t classIndex, ULong64_t LOCB, ULong64_t LOCA,        
                                      ULong64_t L1CB, ULong64_t L1CA, ULong64_t L2CB, ULong64_t L2CA ):   
  TObject(),
  fClassIndex( classIndex ),
  fLOCB(LOCB),     
  fLOCA(LOCA),     
  fL1CB(L1CB),     
  fL1CA(L1CA),     
  fL2CB(L2CB),     
  fL2CA(L2CA)      
{
  // Default constructor
}

//_____________________________________________________________________________
void AliTriggerScalersESD::Print( const Option_t* ) const
{
   // Print
  cout << "Trigger Scalers for Class: " << (Int_t)fClassIndex << endl;
  cout << "  LOCB: " << fLOCB << " LOCA: " << fLOCA; //<< endl;
  cout << "  L1CB: " << fL1CB << " L1CA: " << fL1CA; //<< endl;
  cout << "  L2CB: " << fL2CB << " L2CA: " << fL2CA << endl;
}
