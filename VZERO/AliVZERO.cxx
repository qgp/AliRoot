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



///////////////////////////////////////////////////////////////////////////
//                                                                       //
//                          V-Zero   Detector                            //
//  This class contains the base procedures for the VZERO  detector      //
//  All comments should be sent to Brigitte CHEYNIS :                    //
//                                 b.cheynis@ipnl.in2p3.fr               //
//                                                                       //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <fstream.h>

#include "TMath.h"
#include "TTUBE.h"
#include "TNode.h"
#include "TGeometry.h"
#include "AliRun.h"
#include "AliVZERO.h"
#include "AliVZEROdigit.h"
#include "AliMC.h"
#include "AliVZEROhit.h"

ClassImp(AliVZERO)
 

//_____________________________________________________________________________
AliVZERO::AliVZERO(const char *name, const char *title)
       : AliDetector(name,title)
{
  //
  // Standard constructor for VZERO Detector
  //
  
  fIshunt       =  1;  // All hits are associated with primary particles  
   
  fHits         =  new TClonesArray("AliVZEROhit", 400);
  fDigits       =  new TClonesArray("AliVZEROdigit",400); 
   
  gAlice->AddHitList(fHits);

//  fDigits       =  new TClonesArray("AliVZEROdigit",400) ; 

  fThickness    =  3.1;   // total thickness of the V0R box
  fThickness1   =  1.0;   // thickness of the thickest cell (2.5 in version 0)
  
  fMaxStepQua   =  0.05; 
  fMaxStepAlu   =  0.01; 
  
  fMaxDestepQua =  -1.0;
  fMaxDestepAlu =  -1.0;
  
  SetMarkerColor(kRed);
}

//_____________________________________________________________________________
AliVZERO::~AliVZERO()
{
    if (fHits) {
        fHits->Delete();
        delete fHits;
    }
}

//_____________________________________________________________________________
void AliVZERO::BuildGeometry()
{
  //
  // Build simple ROOT TNode geometry for event display
  //
}
 
//_____________________________________________________________________________
void AliVZERO::CreateGeometry()
{
  //
  // Build simple ROOT TNode geometry for event display
  //
}
//_____________________________________________________________________________
void AliVZERO::CreateMaterials()
{
  //
  // Build simple ROOT TNode geometry for event display
  //
}



//_____________________________________________________________________________
Int_t AliVZERO::DistanceToPrimitive(Int_t px, Int_t py)
{
  //
  // Calculate the distance from the mouse to the VZERO on the screen
  // Dummy routine
  //
  
  return 9999;
}
 
//-------------------------------------------------------------------------
void AliVZERO::Init()
{
  //
  // Initialise the VZERO after it has been built
  //
}


//-------------------------------------------------------------------------

void AliVZERO::SetMaxStepQua(Float_t p1)
{
     fMaxStepQua = p1;
}

//___________________________________________
void AliVZERO::SetMaxStepAlu(Float_t p1)
{
    fMaxStepAlu = p1;
}

//___________________________________________
void AliVZERO::SetMaxDestepQua(Float_t p1)
{
    fMaxDestepQua = p1;
}

//___________________________________________
void AliVZERO::SetMaxDestepAlu(Float_t p1)
{
    fMaxDestepAlu = p1;
}
