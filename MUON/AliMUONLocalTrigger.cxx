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

*/

#include "AliMUONLocalTrigger.h"

ClassImp(AliMUONLocalTrigger);
//----------------------------------------------------------------------
AliMUONLocalTrigger::AliMUONLocalTrigger()
{
// constructor
  fLoCircuit = 0;
  fLoStripX  = 0;
  fLoDev     = 0;
  fLoStripY  = 0;
  fLoLpt     = 0;
  fLoHpt     = 0;
  fLoApt     = 0;
}
//----------------------------------------------------------------------
AliMUONLocalTrigger::AliMUONLocalTrigger(Int_t *localtr)
{
// add a local trigger object 
  fLoCircuit = localtr[0];
  fLoStripX  = localtr[1];         
  fLoDev     = localtr[2];           
  fLoStripY  = localtr[3];           
  fLoLpt     = localtr[4];
  fLoHpt     = localtr[5];
  fLoApt     = localtr[6];
}

//----------------------------------------------------------------------
//--- methods which return member data related info
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoCircuit(){ 
// returns circuit number 
return fLoCircuit;
}
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoStripX(){  
// returns X strip in MT11 
return fLoStripX;
}
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoDev(){     
// returns deviation 
return fLoDev;
}
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoStripY(){  
// returns Y strip in MT11 
return fLoStripY;
}
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoLpt(){     
// returns Low pt  0 : nothing, 1 : Minus, 2 : Plus, 3 : Undef
return fLoLpt;
}
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoHpt(){     
// returns High pt 0 : nothing, 1 : Minus, 2 : Plus, 3 : Undef
return fLoHpt;
}
//----------------------------------------------------------------------
Int_t AliMUONLocalTrigger::LoApt(){     
// returns All pt  0 : nothing, 1 : Minus, 2 : Plus, 3 : Undef
return fLoApt;
}
