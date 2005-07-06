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

//____________________________________________________________________
//                                                                          
// Forward Multiplicity Detector based on Silicon wafers. This class
// contains the base procedures for the Forward Multiplicity detector
// Detector consists of 3 sub-detectors FMD1, FMD2, and FMD3, each of
// which has 1 or 2 rings of silicon sensors. 
//                                                       
// This is the base class for all FMD manager classes. 
//                    
// The actual code is done by various separate classes.   Below is
// diagram showing the relationship between the various FMD classes
// that handles the simulation
//
//
//       +----------+   +----------+   
//       | AliFMDv1 |	| AliFMDv0 |   
//       +----------+   +----------+   
//            |              |                    +-----------------+
//       +----+--------------+                 +--| AliFMDDigitizer |
//       |                                     |  +-----------------+
//       |           +---------------------+   |
//       |        +- | AliFMDBaseDigitizer |<--+
//       V     1  |  +---------------------+   |
//  +--------+<>--+                            |  +------------------+
//  | AliFMD |                                 +--| AliFMDSDigitizer |    
//  +--------+<>--+                               +------------------+       
//	       1  | +-----------------+ 
//	          +-| AliFMDSimulator |
//	  	    +-----------------+
//                           ^              
//                           |
//             +-------------+-------------+
//             |                           |	      
//    +--------------------+   +-------------------+
//    | AliFMDGeoSimulator |   | AliFMDG3Simulator | 
//    +--------------------+   +---------+---------+
//      
//
// *  AliFMD 
//    This defines the interface for the various parts of AliROOT that
//    uses the FMD, like AliFMDSimulator, AliFMDDigitizer, 
//    AliFMDReconstructor, and so on. 
//
// *  AliFMDv0
//    This is a concrete implementation of the AliFMD interface. 
//    It is the responsibility of this class to create the FMD
//    geometry.
//
// *  AliFMDv1 
//    This is a concrete implementation of the AliFMD interface. 
//    It is the responsibility of this class to create the FMD
//    geometry, process hits in the FMD, and serve hits and digits to
//    the various clients. 
//  
// *  AliFMDSimulator
//    This is the base class for the FMD simulation tasks.   The
//    simulator tasks are responsible to implment the geoemtry, and
//    process hits. 
//                                                                          
// *  AliFMDGeoSimulator
//    This is a concrete implementation of the AliFMDSimulator that
//    uses the TGeo classes directly only. 
//
// *  AliFMDG3Simulator
//    This is a concrete implementation of the AliFMDSimulator that
//    uses the TVirtualMC interface with GEANT 3.21-like messages.
//

// These files are not in the same directory, so there's no reason to
// ask the preprocessor to search in the current directory for these
// files by including them with `#include "..."' 
#include <math.h>               // __CMATH__
#include <TClonesArray.h>	// ROOT_TClonesArray
#include <TGeometry.h>		// ROOT_TGeomtry
#include <TNode.h>		// ROOT_TNode
#include <TXTRU.h>		// ROOT_TXTRU
#include <TRotMatrix.h>		// ROOT_TRotMatrix
#include <TTUBE.h>		// ROOT_TTUBE
#include <TTree.h>		// ROOT_TTree
#include <TBrowser.h>		// ROOT_TBrowser
#include <TMath.h>		// ROOT_TMath
#include <TVirtualMC.h>		// ROOT_TVirtualMC

#include <AliRunDigitizer.h>	// ALIRUNDIGITIZER_H
#include <AliLoader.h>		// ALILOADER_H
#include <AliRun.h>		// ALIRUN_H
#include <AliMC.h>		// ALIMC_H
#include <AliLog.h>		// ALILOG_H
#include "AliFMD.h"		// ALIFMD_H
#include "AliFMDDigit.h"	// ALIFMDDIGIG_H
#include "AliFMDHit.h"		// ALIFMDHIT_H
#include "AliFMDGeometry.h"	// ALIFMDGEOMETRY_H
#include "AliFMDDetector.h"	// ALIFMDDETECTOR_H
#include "AliFMDRing.h"		// ALIFMDRING_H
#include "AliFMDDigitizer.h"	// ALIFMDDIGITIZER_H
#include "AliFMDSimulator.h"	// ALIFMDSIMULATOR_H
#include "AliFMDG3Simulator.h"	// ALIFMDG3SIMULATOR_H
#include "AliFMDGeoSimulator.h"	// ALIFMDGEOSIMULATOR_H
#include "AliFMDRawWriter.h"	// ALIFMDRAWWRITER_H

//____________________________________________________________________
ClassImp(AliFMD)
#if 0
  ; // This is to keep Emacs from indenting the next line 
#endif 

//____________________________________________________________________
AliFMD::AliFMD()
  : AliDetector(),
    fSDigits(0), 
    fNsdigits(0),
    fDetailed(kTRUE),
    fSimulator(0)
{
  //
  // Default constructor for class AliFMD
  //
  AliDebug(10, "\tDefault CTOR");
  fHits     = 0;
  fDigits   = 0;
  fIshunt   = 0;
}

//____________________________________________________________________
AliFMD::AliFMD(const AliFMD& other)
  : AliDetector(other),
    fSDigits(other.fSDigits), 
    fNsdigits(other.fNsdigits),
    fDetailed(other.fDetailed),
    fSimulator(other.fSimulator)
{
  // Copy constructor 
}

//____________________________________________________________________
AliFMD::AliFMD(const char *name, const char *title)
  : AliDetector (name, title),
    fSDigits(0),
    fNsdigits(0),
    fDetailed(kTRUE),
    fSimulator(0)
{
  //
  // Standard constructor for Forward Multiplicity Detector
  //
  AliDebug(10, "\tStandard CTOR");

  // Initialise Hit array
  HitsArray();
  gAlice->GetMCApp()->AddHitList(fHits);

  // (S)Digits for the detectors disk
  DigitsArray();
  SDigitsArray();
  
  // CHC: What is this?
  fIshunt = 0;
  SetMarkerColor(kRed);
  SetLineColor(kYellow);
}

//____________________________________________________________________
AliFMD::~AliFMD ()
{
  // Destructor for base class AliFMD
  if (fHits) {
    fHits->Delete();
    delete fHits;
    fHits = 0;
  }
  if (fDigits) {
    fDigits->Delete();
    delete fDigits;
    fDigits = 0;
  }
  if (fSDigits) {
    fSDigits->Delete();
    delete fSDigits;
    fSDigits = 0;
  }
}

//____________________________________________________________________
AliFMD&
AliFMD::operator=(const AliFMD& other)
{
  AliDetector::operator=(other);
  fSDigits		= other.fSDigits; 
  fNsdigits		= other.fNsdigits;
  fDetailed		= other.fDetailed;
  fSimulator            = other.fSimulator;
  
  return *this;
}

//====================================================================
//
// GEometry ANd Traking
//
//____________________________________________________________________
void 
AliFMD::CreateGeometry()
{
  //
  // Create the geometry of Forward Multiplicity Detector.  The actual
  // construction of the geometry is delegated to the class AliFMDRing
  // and AliFMDSubDetector and the relevant derived classes. 
  //
  // The flow of this member function is:
  //
  //   FOR rings fInner and fOuter DO  
  //     AliFMDRing::Init();
  //   END FOR
  // 
  //   Set up hybrud card support (leg) volume shapes  
  // 
  //   FOR rings fInner and fOuter DO  
  //     AliFMDRing::SetupGeometry();
  //   END FOR
  // 
  //   FOR subdetectors fFMD1, fFMD2, and fFMD3 DO 
  //     AliFMDSubDetector::SetupGeomtry();
  //   END FOR
  // 
  //   FOR subdetectors fFMD1, fFMD2, and fFMD3 DO 
  //     AliFMDSubDetector::Geomtry();
  //   END FOR
  //
  if (!fSimulator) {
    AliFatal("Simulator object not made yet!");
    return;
  }
  fSimulator->DefineGeometry();
}    

//____________________________________________________________________
void AliFMD::CreateMaterials() 
{
  // Register various materials and tracking mediums with the
  // backend.   
  // 
  AliDebug(10, "\tCreating materials");

  if (fSimulator) {
    AliFatal("Simulator object already instantised!");
    return;
  }
  AliFMDGeometry* geometry = AliFMDGeometry::Instance();
  geometry->Init();
  TVirtualMC* mc = TVirtualMC::GetMC();
  Bool_t geo = mc->IsRootGeometrySupported();
  if (geo)
    fSimulator = new AliFMDGeoSimulator(this, fDetailed);
  else 
    fSimulator = new AliFMDG3Simulator(this, fDetailed);
  
  fSimulator->DefineMaterials();
}

//____________________________________________________________________
void  
AliFMD::Init()
{
  //
  // Initialis the FMD after it has been built
  Int_t i;
  //
  if (fDebug) {
    cout << "\n" << ClassName() << ": " << flush;
    for (i = 0; i < 35; i++) cout << "*";
    cout << " FMD_INIT ";
    for (i = 0; i < 35; i++) cout << "*";
    cout << "\n" << ClassName() << ": " << flush;
    //
    // Here the FMD initialisation code (if any!)
    for (i = 0; i < 80; i++) cout << "*";
    cout << endl;
  }
  //
  //
}

//====================================================================
//
// Graphics and event display
//
//____________________________________________________________________
void 
AliFMD::BuildGeometry()
{
  //
  // Build simple ROOT TNode geometry for event display
  //
  // Build a simplified geometry of the FMD used for event display  
  // 
  // The actual building of the TNodes is done by
  // AliFMDSubDetector::SimpleGeometry. 
  AliDebug(10, "\tCreating a simplified geometry");

  AliFMDGeometry* fmd = AliFMDGeometry::Instance();
  
  static TXTRU*     innerShape = 0;
  static TXTRU*     outerShape = 0;
  static TObjArray* innerRot   = 0;
  static TObjArray* outerRot   = 0;

  if (!innerShape || !outerShape) {
    // Make the shapes for the modules 
    for (Int_t i = 0; i < 2; i++) {
      AliFMDRing* r = 0;
      switch (i) {
      case 0: r = fmd->GetRing('I'); break;
      case 1: r = fmd->GetRing('O'); break;
      }
      if (!r) {
	AliError(Form("no ring found for i=%d", i));
	return;
      }
      Double_t    siThick  = r->GetSiThickness();
      const Int_t nv       = r->GetNVerticies();
      Double_t    theta    = r->GetTheta();
      Int_t       nmod     = r->GetNModules();
      
      TXTRU* shape = new TXTRU(r->GetName(), r->GetTitle(), "void", nv, 2);
      for (Int_t j = 0; j < nv; j++) {
	TVector2* vv = r->GetVertex(nv - 1 - j);
	shape->DefineVertex(j, vv->X(), vv->Y());
      }
      shape->DefineSection(0, -siThick / 2, 1, 0, 0);
      shape->DefineSection(1, +siThick / 2, 1, 0, 0);
      shape->SetLineColor(GetLineColor());
      
      TObjArray* rots = new TObjArray(nmod);
      for (Int_t j = 0; j < nmod; j++) {
	Double_t th = (j + .5) * theta * 2;
	TString name(Form("FMD_ring_%c_rot_%02d", r->GetId(), j));
	TString title(Form("FMD Ring %c Rotation # %d", r->GetId(), j));
	TRotMatrix* rot = new TRotMatrix(name.Data(), title.Data(),
					 90, th, 90, fmod(90+th,360), 0, 0);
	rots->AddAt(rot, j);
      }
      
      switch (r->GetId()) {
      case 'i':
      case 'I': innerShape = shape; innerRot = rots; break;
      case 'o':
      case 'O': outerShape = shape; outerRot = rots; break;
      }
    }
  }
  
  TNode* top = gAlice->GetGeometry()->GetNode("alice");
  
  for (Int_t i = 1; i <= 3; i++) {
    AliFMDDetector* det = fmd->GetDetector(i);
    if (!det) {
      Warning("BuildGeometry", "FMD%d seems to be disabled", i);
      continue;
    }
    Double_t w  = 0;
    Double_t rh = det->GetRing('I')->GetHighR();
    Char_t   id = 'I';
    if (det->GetRing('O')) {
      w  = TMath::Abs(det->GetRingZ('O') - det->GetRingZ('I'));
      id = (TMath::Abs(det->GetRingZ('O')) 
	    > TMath::Abs(det->GetRingZ('I')) ? 'O' : 'I');
      rh = det->GetRing('O')->GetHighR();
    }
    w += (det->GetRing(id)->GetModuleSpacing() +
	  det->GetRing(id)->GetSiThickness());
    TShape* shape = new TTUBE(det->GetName(), det->GetTitle(), "void",
			      det->GetRing('I')->GetLowR(), rh, w / 2);
    Double_t z = (det->GetRingZ('I') - w / 2);
    if (z > 0) z += det->GetRing(id)->GetModuleSpacing();
    top->cd();
    TNode* node = new TNode(det->GetName(), det->GetTitle(), shape, 
			    0, 0, z, 0);
    fNodes->Add(node);
    
    for (Int_t j = 0; j < 2; j++) {
      AliFMDRing* r      = 0;
      TShape*     rshape = 0;
      TObjArray*  rots   = 0;
      switch (j) {
      case 0: 
	r = det->GetRing('I'); rshape = innerShape; rots = innerRot; break;
      case 1: 
	r = det->GetRing('O'); rshape = outerShape; rots = outerRot; break;
      }
      if (!r) continue;
      
      Double_t    siThick  = r->GetSiThickness();
      Int_t       nmod     = r->GetNModules();
      Double_t    modspace = r->GetModuleSpacing();
      Double_t    rz       = - (z - det->GetRingZ(r->GetId()));
      
      for (Int_t k = 0; k < nmod; k++) {
	node->cd();
	Double_t    offz    = (k % 2 == 1 ? modspace : 0);
	TRotMatrix* rot     = static_cast<TRotMatrix*>(rots->At(k));
	TString name(Form("%s%c_module_%02d", det->GetName(), r->GetId(),k));
	TString title(Form("%s%c Module %d", det->GetName(), r->GetId(),k));
	TNode* mnod = new TNode(name.Data(), title.Data(), rshape, 
				0, 0, rz - siThick / 2 
				+ TMath::Sign(offz,z), rot);
	mnod->SetLineColor(GetLineColor());
	fNodes->Add(mnod);
      } // for (Int_t k = 0 ; ...)
    } // for (Int_t j = 0 ; ...)
  } // for (Int_t i = 1 ; ...)
}

//____________________________________________________________________
void 
AliFMD::DrawDetector()
{
  //
  // Draw a shaded view of the Forward multiplicity detector
  //
  // DebugGuard guard("AliFMD::DrawDetector");
  AliDebug(10, "\tDraw detector");
  
#if 0
  //Set ALIC mother transparent
  gMC->Gsatt("ALIC","SEEN",0);
  //
  gMC->Gdopt("hide", "on");
  gMC->Gdopt("shad", "on");
  gMC->Gsatt("*", "fill", 7);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 1000, -1000, 1000, -1000, 1000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 40, 30, 0, 12, 12, .055, .055);
  gMC->Gdhead(1111, "Forward Multiplicity Detector");
  gMC->Gdman(16, 10, "MAN");
  gMC->Gdopt("hide", "off");
#endif
}

//____________________________________________________________________
Int_t 
AliFMD::DistanceToPrimitive(Int_t, Int_t)
{
  //
  // Calculate the distance from the mouse to the FMD on the screen
  // Dummy routine
  //
  return 9999;
}

//====================================================================
//
// Hit and Digit managment 
//
//____________________________________________________________________
void 
AliFMD::MakeBranch(Option_t * option)
{
  // Create Tree branches for the FMD.
  //
  // Options:
  //
  //    H          Make a branch of TClonesArray of AliFMDHit's
  //    D          Make a branch of TClonesArray of AliFMDDigit's
  //    S          Make a branch of TClonesArray of AliFMDSDigit's
  // 
  const Int_t kBufferSize = 16000;
  TString branchname(GetName());
  TString opt(option);
  
  if (opt.Contains("H", TString::kIgnoreCase)) {
    HitsArray();
    AliDetector::MakeBranch(option); 
  }
  if (opt.Contains("D", TString::kIgnoreCase)) { 
    DigitsArray();
    MakeBranchInTree(fLoader->TreeD(), branchname.Data(),
		     &fDigits, kBufferSize, 0);
  }
  if (opt.Contains("S", TString::kIgnoreCase)) { 
    SDigitsArray();
    MakeBranchInTree(fLoader->TreeS(), branchname.Data(),
		     &fSDigits, kBufferSize, 0);
  }
}

//____________________________________________________________________
void 
AliFMD::SetTreeAddress()
{
  // Set branch address for the Hits, Digits, and SDigits Tree.
  if (fLoader->TreeH()) HitsArray();
  AliDetector::SetTreeAddress();

  TTree *treeD = fLoader->TreeD();
  if (treeD) {
    DigitsArray();
    TBranch* branch = treeD->GetBranch ("FMD");
    if (branch) branch->SetAddress(&fDigits);
  }

  TTree *treeS = fLoader->TreeS();
  if (treeS) {
    SDigitsArray();
    TBranch* branch = treeS->GetBranch ("FMD");
    if (branch) branch->SetAddress(&fSDigits);
  }
}



//____________________________________________________________________
void 
AliFMD::SetHitsAddressBranch(TBranch *b)
{
  // Set the TClonesArray to read hits into. 
  b->SetAddress(&fHits);
}

//____________________________________________________________________
void 
AliFMD::AddHit(Int_t track, Int_t *vol, Float_t *hits) 
{
  // Add a hit to the hits tree 
  // 
  // The information of the two arrays are decoded as 
  // 
  // Parameters
  //    track	 	     Track #
  //    ivol[0]  [UShort_t ] Detector # 
  //    ivol[1]	 [Char_t   ] Ring ID 
  //    ivol[2]	 [UShort_t ] Sector #
  //    ivol[3]	 [UShort_t ] Strip # 
  //    hits[0]	 [Float_t  ] Track's X-coordinate at hit 
  //    hits[1]	 [Float_t  ] Track's Y-coordinate at hit
  //    hits[3]  [Float_t  ] Track's Z-coordinate at hit
  //    hits[4]  [Float_t  ] X-component of track's momentum 	       	 
  //    hits[5]	 [Float_t  ] Y-component of track's momentum	       	 
  //    hits[6]	 [Float_t  ] Z-component of track's momentum	       	
  //    hits[7]	 [Float_t  ] Energy deposited by track		       	
  //    hits[8]	 [Int_t    ] Track's particle Id # 
  //    hits[9]	 [Float_t  ] Time when the track hit
  // 
  // 
  AddHit(track, 
	 UShort_t(vol[0]),  // Detector # 
	 Char_t(vol[1]),    // Ring ID
	 UShort_t(vol[2]),  // Sector # 
	 UShort_t(vol[3]),  // Strip # 
	 hits[0],           // X
	 hits[1],           // Y
	 hits[2],           // Z
	 hits[3],           // Px
	 hits[4],           // Py
	 hits[5],           // Pz
	 hits[6],           // Energy loss 
	 Int_t(hits[7]),    // PDG 
	 hits[8]);          // Time
}

//____________________________________________________________________
void 
AliFMD::AddHit(Int_t    track, 
	       UShort_t detector, 
	       Char_t   ring, 
	       UShort_t sector, 
	       UShort_t strip, 
	       Float_t  x, 
	       Float_t  y, 
	       Float_t  z,
	       Float_t  px, 
	       Float_t  py, 
	       Float_t  pz,
	       Float_t  edep,
	       Int_t    pdg,
	       Float_t  t)
{
  //
  // Add a hit to the list
  //
  // Parameters:
  // 
  //    track	  Track #
  //    detector  Detector # (1, 2, or 3)                      
  //    ring	  Ring ID ('I' or 'O')
  //    sector	  Sector # (For inner/outer rings: 0-19/0-39)
  //    strip	  Strip # (For inner/outer rings: 0-511/0-255)
  //    x	  Track's X-coordinate at hit
  //    y	  Track's Y-coordinate at hit
  //    z	  Track's Z-coordinate at hit
  //    px	  X-component of track's momentum 
  //    py	  Y-component of track's momentum
  //    pz	  Z-component of track's momentum
  //    edep	  Energy deposited by track
  //    pdg	  Track's particle Id #
  //    t	  Time when the track hit 
  // 
  TClonesArray& a = *(HitsArray());
  // Search through the list of already registered hits, and see if we
  // find a hit with the same parameters.  If we do, then don't create
  // a new hit, but rather update the energy deposited in the hit.
  // This is done, so that a FLUKA based simulation will get the
  // number of hits right, not just the enerrgy deposition. 
  for (Int_t i = 0; i < fNhits; i++) {
    if (!a.At(i)) continue;
    AliFMDHit* hit = static_cast<AliFMDHit*>(a.At(i));
    if (hit->Detector() == detector 
	&& hit->Ring() == ring
	&& hit->Sector() == sector 
	&& hit->Strip() == strip
	&& hit->Track() == track) {
      Warning("AddHit", "already had a hit in FMD%d%c[%2d,%3d] for track # %d,"
	      " adding energy (%f) to that hit (%f) -> %f", 
	      detector, ring, sector, strip, track, edep, hit->Edep(),
	      hit->Edep() + edep);
      hit->SetEdep(hit->Edep() + edep);
      return;
    }
  }
  // If hit wasn't already registered, do so know. 
  new (a[fNhits]) AliFMDHit(fIshunt, track, detector, ring, sector, strip, 
			    x, y, z, px, py, pz, edep, pdg, t);
  fNhits++;
}

//____________________________________________________________________
void 
AliFMD::AddDigit(Int_t* digits)
{
  // Add a digit to the Digit tree 
  // 
  // Paramters 
  //
  //    digits[0]  [UShort_t] Detector #
  //    digits[1]  [Char_t]   Ring ID
  //    digits[2]  [UShort_t] Sector #
  //    digits[3]  [UShort_t] Strip #
  //    digits[4]  [UShort_t] ADC Count 
  //    digits[5]  [Short_t]  ADC Count, -1 if not used
  //    digits[6]  [Short_t]  ADC Count, -1 if not used 
  // 
  AddDigit(UShort_t(digits[0]),  // Detector #
	   Char_t(digits[1]),    // Ring ID
	   UShort_t(digits[2]),  // Sector #
	   UShort_t(digits[3]),  // Strip #
	   UShort_t(digits[4]),  // ADC Count1 
	   Short_t(digits[5]), 	 // ADC Count2 
	   Short_t(digits[6]));  // ADC Count3 
}

//____________________________________________________________________
void 
AliFMD::AddDigit(UShort_t detector, 
		 Char_t   ring, 
		 UShort_t sector, 
		 UShort_t strip, 
		 UShort_t count1, 
		 Short_t  count2,
		 Short_t  count3)
{
  // add a real digit - as coming from data
  // 
  // Parameters 
  //
  //    detector  Detector # (1, 2, or 3)                      
  //    ring	  Ring ID ('I' or 'O')
  //    sector	  Sector # (For inner/outer rings: 0-19/0-39)
  //    strip	  Strip # (For inner/outer rings: 0-511/0-255)
  //    count1    ADC count (a 10-bit word)
  //    count2    ADC count (a 10-bit word), or -1 if not used
  //    count3    ADC count (a 10-bit word), or -1 if not used
  TClonesArray& a = *(DigitsArray());
  
  new (a[fNdigits++]) 
    AliFMDDigit(detector, ring, sector, strip, count1, count2, count3);
}

//____________________________________________________________________
void 
AliFMD::AddSDigit(Int_t* digits)
{
  // Add a digit to the SDigit tree 
  // 
  // Paramters 
  //
  //    digits[0]  [UShort_t] Detector #
  //    digits[1]  [Char_t]   Ring ID
  //    digits[2]  [UShort_t] Sector #
  //    digits[3]  [UShort_t] Strip #
  //    digits[4]  [Float_t]  Total energy deposited 
  //    digits[5]  [UShort_t] ADC Count 
  //    digits[6]  [Short_t]  ADC Count, -1 if not used
  //    digits[7]  [Short_t]  ADC Count, -1 if not used 
  // 
  AddSDigit(UShort_t(digits[0]),  // Detector #
	    Char_t(digits[1]),    // Ring ID
	    UShort_t(digits[2]),  // Sector #
	    UShort_t(digits[3]),  // Strip #
	    Float_t(digits[4]),   // Edep
	    UShort_t(digits[5]),  // ADC Count1 
	    Short_t(digits[6]),   // ADC Count2 
	    Short_t(digits[7]));  // ADC Count3 
}

//____________________________________________________________________
void 
AliFMD::AddSDigit(UShort_t detector, 
		  Char_t   ring, 
		  UShort_t sector, 
		  UShort_t strip, 
		  Float_t  edep,
		  UShort_t count1, 
		  Short_t  count2,
		  Short_t  count3)
{
  // add a summable digit
  // 
  // Parameters 
  //
  //    detector  Detector # (1, 2, or 3)                      
  //    ring	  Ring ID ('I' or 'O')
  //    sector	  Sector # (For inner/outer rings: 0-19/0-39)
  //    strip	  Strip # (For inner/outer rings: 0-511/0-255)
  //    edep      Total energy deposited
  //    count1    ADC count (a 10-bit word)
  //    count2    ADC count (a 10-bit word), or -1 if not used
  //    count3    ADC count (a 10-bit word), or -1 if not used
  //
  TClonesArray& a = *(SDigitsArray());
  
  new (a[fNsdigits++]) 
    AliFMDSDigit(detector, ring, sector, strip, edep, count1, count2, count3);
}

//____________________________________________________________________
void 
AliFMD::ResetSDigits()
{
  //
  // Reset number of digits and the digits array for this detector
  //
  fNsdigits   = 0;
  if (fSDigits) fSDigits->Clear();
}


//____________________________________________________________________
TClonesArray*
AliFMD::HitsArray() 
{
  // Initialize hit array if not already, and return pointer to it. 
  if (!fHits) { 
    fHits = new TClonesArray("AliFMDHit", 1000);
    fNhits = 0;
  }
  return fHits;
}

//____________________________________________________________________
TClonesArray*
AliFMD::DigitsArray() 
{
  // Initialize digit array if not already, and return pointer to it. 
  if (!fDigits) { 
    fDigits = new TClonesArray("AliFMDDigit", 1000);
    fNdigits = 0;
  }
  return fDigits;
}

//____________________________________________________________________
TClonesArray*
AliFMD::SDigitsArray() 
{
  // Initialize digit array if not already, and return pointer to it. 
  if (!fSDigits) { 
    fSDigits = new TClonesArray("AliFMDSDigit", 1000);
    fNsdigits = 0;
  }
  return fSDigits;
}

//====================================================================
//
// Digitization 
//
//____________________________________________________________________
void 
AliFMD::Hits2Digits() 
{
  // Create AliFMDDigit's from AliFMDHit's.  This is done by making a
  // AliFMDDigitizer, and executing that code.
  // 
  AliRunDigitizer* manager = new AliRunDigitizer(1, 1);
  manager->SetInputStream(0, "galice.root");
  manager->SetOutputFile("H2Dfile");
  
  /* AliDigitizer* dig =*/ CreateDigitizer(manager);
  manager->Exec("");
  delete manager;
}

//____________________________________________________________________
void 
AliFMD::Hits2SDigits() 
{
  // Create AliFMDSDigit's from AliFMDHit's.  This is done by creating
  // an AliFMDSDigitizer object, and executing it. 
  // 
  AliFMDSDigitizer* digitizer = new AliFMDSDigitizer("galice.root");
  digitizer->Exec("");
  delete digitizer;
}

  
//____________________________________________________________________
AliDigitizer* 
AliFMD::CreateDigitizer(AliRunDigitizer* manager) const
{
  // Create a digitizer object 
  AliFMDDigitizer* digitizer = new AliFMDDigitizer(manager);
  return digitizer;
}

//====================================================================
//
// Raw data simulation 
//
//__________________________________________________________________
void 
AliFMD::Digits2Raw() 
{
  // Turn digits into raw data. 
  // 
  // This uses the class AliFMDRawWriter to do the job.   Please refer
  // to that class for more information. 
  AliFMDRawWriter writer(this);
  writer.Exec();
}


//====================================================================
//
// Utility 
//
//__________________________________________________________________
void 
AliFMD::Browse(TBrowser* b) 
{
  // Browse this object. 
  //
  AliDebug(30, "\tBrowsing the FMD");
  AliDetector::Browse(b);
  if (fSimulator) b->Add(fSimulator);
  b->Add(AliFMDGeometry::Instance());
}

//___________________________________________________________________
//
// EOF
//
