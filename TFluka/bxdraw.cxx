#include <Riostream.h>
#include "TFluka.h"
#include "Fdimpar.h"  //(DIMPAR) fluka include
#include "Ftrackr.h"  //(TRACKR) fluka common
#ifndef WIN32
# define bxdraw bxdraw_
#else
# define bxdraw BXDRAW
#endif
extern "C" {
void bxdraw(Int_t& icode, Int_t& mreg, Int_t& newreg,
            Double_t& xsco, Double_t& ysco, Double_t& zsco)
{
    TFluka* fluka = (TFluka*) gMC;
    
    fluka->SetIcode(icode);
    fluka->SetMreg(mreg);
    fluka->SetNewreg(newreg);
    fluka->SetXsco(xsco);
    fluka->SetYsco(ysco);
    fluka->SetZsco(zsco);
//
// Double step for boundary crossing
//
    fluka->SetTrackIsExiting();
    fluka->SetCaller(12);
    (TVirtualMCApplication::Instance())->Stepping(); 
    fluka->SetCaller(11);
    fluka->SetTrackIsEntering();
    (TVirtualMCApplication::Instance())->Stepping();
    fluka->SetCaller(1);
    fluka->SetTrackIsInside();
    (TVirtualMCApplication::Instance())->Stepping();
} // end of bxdraw
} // end of extern "C"

