#include <Riostream.h>
#ifndef WIN32
# define stuprf stuprf_
#else
# define stuprf STUPRF
#endif
//
// Fluka include
#include "Fdimpar.h"  //(DIMPAR) fluka include
// Fluka commons
#include "Fdblprc.h"  //(DBLPRC) fluka common
#include "Fevtflg.h"  //(EVTFLG) fluka common
#include "Fpaprop.h"  //(PAPROP) fluka common
#include "Fflkstk.h"  //(FLKSTK)  fluka common
#include "Ftrackr.h"  //(TRACKR) fluka common
#include "Fgenstk.h"  //(GENSTK)  fluka common

//Virtual MC
#include "TFluka.h"

#include "TVirtualMCStack.h"
#include "TVirtualMCApplication.h"
#include "TParticle.h"
#include "TVector3.h"

extern "C" {
    void stuprf(Int_t& /*ij*/, Int_t& /*mreg*/,
		Double_t& xx, Double_t& yy, Double_t& zz,
		Int_t& numsec, Int_t& npprmr)
{
//*----------------------------------------------------------------------*
//*                                                                      *
//*  SeT User PRoperties for Fluka particles                             *
//*                                                                      *
//*----------------------------------------------------------------------*

// FLKSTK.npflka  = stack pointer
// FLKSTK.louse   = user flag
// TRACKR.llouse = user defined flag for the current particle
  FLKSTK.louse[FLKSTK.npflka] = TRACKR.llouse;

// mkbmx1 = dimension for kwb real spare array in fluka stack in DIMPAR
// mkbmx2 = dimension for kwb int. spare array in fluka stack in DIMPAR
// FLKSTK.sparek  = spare real variables available for k.w.burn
// FLKSTK.ispark  = spare integer variables available for k.w.burn
// TRACKR.spausr = user defined spare variables for the current particle
// TRACKR.ispusr = user defined spare flags for the current particle
  Int_t ispr;
  for (ispr = 0; ispr <= mkbmx1 - 1; ispr++) {
    FLKSTK.sparek[FLKSTK.npflka][ispr] = TRACKR.spausr[ispr];
  }  
  for (ispr = 0; ispr <= mkbmx2 - 1; ispr++) {
    FLKSTK.ispark[FLKSTK.npflka][ispr] = TRACKR.ispusr[ispr];
  }  
 
// Get the pointer to the VMC
  TFluka* fluka =  (TFluka*) gMC;
  Int_t verbosityLevel = fluka->GetVerbosityLevel();
  Bool_t debug = (verbosityLevel>=3)?kTRUE:kFALSE;
  fluka->SetTrackIsNew(kTRUE);
//  TVirtualMC* fluka = TFluka::GetMC();
// Get the stack produced from the generator
  TVirtualMCStack* cppstack = fluka->GetStack();
  
// EVTFLG.ntrcks = track number
// Increment the track number and put it into the last flag
// was numsec -1
// clarify with Alberto
  if (numsec > npprmr) {
// Now call the PushTrack(...)
    Int_t done = 0;

    Int_t parent =  TRACKR.ispusr[mkbmx2-1];
    Int_t kpart  = GENSTK.kpart[numsec-1];
    if (kpart < -6) return;

    Int_t pdg = fluka->PDGFromId(kpart);

    
    Double_t px = GENSTK.plr[numsec-1] * GENSTK.cxr[numsec-1];
    Double_t py = GENSTK.plr[numsec-1] * GENSTK.cyr[numsec-1];
    Double_t pz = GENSTK.plr[numsec-1] * GENSTK.czr[numsec-1];
    Double_t e  = GENSTK.tki[numsec-1] + PAPROP.am[GENSTK.kpart[numsec-1]+6];

    Double_t vx = xx;
    Double_t vy = yy;
    Double_t vz = zz;
    
    Double_t tof  = TRACKR.atrack;
    Double_t polx = GENSTK.cxrpol[numsec-1];
    Double_t poly = GENSTK.cyrpol[numsec-1];
    Double_t polz = GENSTK.czrpol[numsec-1];
    

    TMCProcess mech = kPHadronic;
    
    if (EVTFLG.ldecay == 1) {
	mech = kPDecay;
	if (debug) cout << endl << "Decay" << endl;
	
    } else if (EVTFLG.ldltry == 1) {
	mech = kPDeltaRay;
	if (debug) cout << endl << "Delta Ray" << endl;
	
    } else if (EVTFLG.lpairp == 1) {
	mech = kPPair;
	if (debug) cout << endl << "Pair Production" << endl;
	
    } else if (EVTFLG.lbrmsp == 1) {
	mech = kPBrem;
	if (debug) cout << endl << "Bremsstrahlung" << endl;
	
    }
    

    Double_t weight = GENSTK.wei[numsec-1];
    Int_t is = 0;
    Int_t ntr;  
    // 
    // Save particle in VMC stack
    cppstack->PushTrack(done, parent, pdg,
		       px, py, pz, e,
		       vx, vy, vz, tof,
		       polx, poly, polz,
		       mech, ntr, weight, is);
    if (debug) cout << endl << " !!! stuprf: ntr=" << ntr << "pdg " << pdg << " parent=" << parent << "numsec " 
	 << numsec << "npprmr " << npprmr << endl;
//
//  Save current track number
    FLKSTK.ispark[FLKSTK.npflka][mkbmx2-1] = ntr;
    FLKSTK.ispark[FLKSTK.npflka][mkbmx2-2] = 0;
  } // end of if (numsec-1 > npprmr)
} // end of stuprf
} // end of extern "C"

