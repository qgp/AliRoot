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

//
// Class AliMUONGeometrySegmentation
// -------------------------------
// New class for the module segmentation 
// composed of the segmentations of detection elements.
// Applies transformations defined in geometry.
//
// Author:Ivana Hrivnacova, IPN Orsay

#include "AliSegmentation.h"
#include "AliLog.h"

#include "AliMUONGeometrySegmentation.h"
#include "AliMUONGeometryModule.h"
#include "AliMUONGeometryDetElement.h"
#include "AliMUONGeometryStore.h"


ClassImp(AliMUONGeometrySegmentation)


//______________________________________________________________________________
AliMUONGeometrySegmentation::AliMUONGeometrySegmentation(
                                  AliMUONGeometryModule* geometry) 
: TObject(),
  fCurrentDetElemId(0),
  fCurrentDetElement(0),
  fCurrentSegmentation(0),
  fGeometryModule(geometry),
  fDESegmentations(0)
  
{
// Normal constructor

  fDESegmentations 
    = new AliMUONGeometryStore(geometry->GetDEIndexing(), false);
}

//______________________________________________________________________________
AliMUONGeometrySegmentation::AliMUONGeometrySegmentation() 
: TObject(),
  fCurrentDetElemId(0),
  fCurrentDetElement(0),
  fCurrentSegmentation(0),
  fGeometryModule(0),
  fDESegmentations(0)
{
// Default Constructor
}

//______________________________________________________________________________
AliMUONGeometrySegmentation::AliMUONGeometrySegmentation(
                                  const AliMUONGeometrySegmentation& rhs) 
  : TObject(rhs)
{
// Copy constructor
  AliFatal("Copy constructor is not implemented.");
}

//______________________________________________________________________________
AliMUONGeometrySegmentation::~AliMUONGeometrySegmentation() {
// Destructor

  delete fDESegmentations;
} 

//
// operators
//

//______________________________________________________________________________
AliMUONGeometrySegmentation& 
AliMUONGeometrySegmentation::operator=(const AliMUONGeometrySegmentation& rhs)
{
// Copy operator 

  // check assignement to self
  if (this == &rhs) return *this;

  AliFatal("Assignment operator is not implemented.");
    
  return *this;  
}

//
// private methods
//

//______________________________________________________________________________
Bool_t AliMUONGeometrySegmentation::Notify(Int_t detElemId) const
{
// Updates current detection element and segmentation,
// and checks if they exist.
// Returns true if success.
// ---

  if (detElemId != fCurrentDetElemId) {

    // Find detection element and its segmentation
    AliMUONGeometryDetElement* detElement
      = fGeometryModule->GetDetElement(detElemId);
    if (!detElement) {
      AliError(Form("Detection element %d not defined", detElemId));
      return false;
    }     

    AliSegmentation* segmentation 
      = (AliSegmentation*) fDESegmentations->Get(detElemId);
    if (!segmentation) {
      AliError(Form("Segmentation for detection element %d not defined",
                     detElemId));
      return false;		     
    }
  
    fCurrentDetElemId = detElemId;
    fCurrentDetElement = detElement;
    fCurrentSegmentation = segmentation;
  }  
 
  return true;
} 	  

//
// public methods
//

//______________________________________________________________________________
void AliMUONGeometrySegmentation::Add(Int_t detElemId, 
                                      AliSegmentation* segmentation)
{
// Add detection element segmentation
// ---

  fDESegmentations->Add(detElemId, segmentation); 
}  

//______________________________________________________________________________
void AliMUONGeometrySegmentation::SetPadSize(Float_t p1, Float_t p2)
{
// Set pad size Dx*Dy to all detection element segmentations 
// ---

  for (Int_t i=0; i<fDESegmentations->GetNofEntries(); i++) {
     AliSegmentation* segmentation
       = (AliSegmentation*)fDESegmentations->GetEntry(i);
     segmentation->SetPadSize(p1, p2);
  }   
}

//______________________________________________________________________________
void AliMUONGeometrySegmentation::SetDAnod(Float_t d)
{
// Set anod pitch to all detection element segmentations
// ---

  for (Int_t i=0; i<fDESegmentations->GetNofEntries(); i++) {
     AliSegmentation* segmentation
       = (AliSegmentation*)fDESegmentations->GetEntry(i);
     segmentation->SetDAnod(d);
  }   
}

//______________________________________________________________________________
Float_t AliMUONGeometrySegmentation::GetAnod(Int_t detElemId, Float_t xhit) const
{
// Anod wire coordinate closest to xhit
// Returns for a hit position xhit the position of the nearest anode wire
// !!! xhit is understand a a distance, not as a position in the space
// CHECK
// ---

  if (!Notify(detElemId)) return 0;

  return fCurrentSegmentation->GetAnod(xhit);
}

//______________________________________________________________________________
void  AliMUONGeometrySegmentation::GetPadI(Int_t detElemId,
                                        Float_t xg, Float_t yg, Float_t zg, 
                                        Int_t& ix, Int_t& iy)
{					
//  Returns pad coordinates (ix,iy) for given real coordinates (x,y)
// ---

  if (!Notify(detElemId)) return;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xg, yg, zg, xl, yl, zl); 

  fCurrentSegmentation->GetPadI(xl, yl, zl, ix, iy);
}
		       
//______________________________________________________________________________
void  AliMUONGeometrySegmentation::GetPadC(Int_t detElemId,
                                        Int_t ix, Int_t iy, 
                                        Float_t& xg, Float_t& yg, Float_t& zg)
{					
// Transform from pad to real coordinates
// ---

  if (!Notify(detElemId)) return;

  Float_t xl, yl, zl;
  fCurrentSegmentation->GetPadC(ix, iy, xl , yl, zl);

  fGeometryModule->Local2Global(detElemId, xl, yl, zl, xg, yg, zg); 
}


//______________________________________________________________________________
void AliMUONGeometrySegmentation::Init(Int_t /*chamber*/)
{
// Initialize segmentation.
// Check that all detection elements have segmanetation set
// ---

  // Loop over detection elements
  AliMUONGeometryStore* detElements = fGeometryModule->GetDetElementStore();

  for (Int_t i=0; i<detElements->GetNofEntries(); i++) {

    // Get detction element Id
    Int_t detElemId = detElements->GetEntry(i)->GetUniqueID();

    // Check segmentation
    if (! fDESegmentations->Get(detElemId) )
      AliError(Form("Detection element %d has not a segmentation set.",
               detElements->GetEntry(i)->GetUniqueID())); 
  }	           
}
 
//______________________________________________________________________________
Float_t AliMUONGeometrySegmentation::Dpx(Int_t detElemId) const
{
// Get pad size in x
// ---

  if (!Notify(detElemId)) return 0.;
  
  return fCurrentSegmentation->Dpx();
}

//______________________________________________________________________________
Float_t AliMUONGeometrySegmentation::Dpy(Int_t detElemId) const
{
// Get pad size in y
// ---

  if (!Notify(detElemId)) return 0.;

  return fCurrentSegmentation->Dpy();
}
 
//______________________________________________________________________________
Float_t AliMUONGeometrySegmentation::Dpx(Int_t detElemId, Int_t isector) const
{
// Pad size in x by sector
// ---

  if (!Notify(detElemId)) return 0.;

  return fCurrentSegmentation->Dpx(isector);
} 

//______________________________________________________________________________
Float_t AliMUONGeometrySegmentation::Dpy(Int_t detElemId, Int_t isector) const
{
// Pad size in x, y by Sector 
// ---

  if (!Notify(detElemId)) return 0.;

  return fCurrentSegmentation->Dpy(isector);
}

//______________________________________________________________________________
Int_t AliMUONGeometrySegmentation::Npx(Int_t detElemId) const
{
// Maximum number of Pads in x
// ---

  if (!Notify(detElemId)) return 0;

  return fCurrentSegmentation->Npx();
}

//______________________________________________________________________________
Int_t AliMUONGeometrySegmentation::Npy(Int_t detElemId) const
{
// Maximum number of Pads in y
// ---

  if (!Notify(detElemId)) return 0;

  return fCurrentSegmentation->Npy();
}

//______________________________________________________________________________
void  AliMUONGeometrySegmentation::SetPad(Int_t detElemId, Int_t ix, Int_t iy)
{
// Set pad position.
// Sets virtual pad coordinates, needed for evaluating pad response 
// outside the tracking program.
// From AliMUONGeometrySegmentationV01.
// ---

  if (!Notify(detElemId)) return;

  fCurrentSegmentation->SetPad(ix, iy);
}

//______________________________________________________________________________
void  AliMUONGeometrySegmentation::SetHit(Int_t detElemId, 
                                        Float_t xghit, Float_t yghit, Float_t zghit)
{
// Set hit position
// Sets virtual hit position, needed for evaluating pad response 
// outside the tracking program 
// From AliMUONGeometrySegmentationV01.

  if (!Notify(detElemId)) return;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xghit, yghit, zghit, xl, yl, zl); 

  fCurrentSegmentation->SetHit(xl, yl, zl);
}
    
//______________________________________________________________________________
void  AliMUONGeometrySegmentation::FirstPad(Int_t detElemId,
                                        Float_t xghit, Float_t yghit, Float_t zghit, 
                                        Float_t dx, Float_t dy) 
{					 
// Iterate over pads - initialiser
// ---

  if (!Notify(detElemId)) return;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xghit, yghit, zghit, xl, yl, zl); 

  fCurrentSegmentation->FirstPad(xl, yl, zl, dx, dy);
}
 
//______________________________________________________________________________
void  AliMUONGeometrySegmentation::NextPad(Int_t detElemId)
{
// Iterate over pads - stepper
// ---

  if (!Notify(detElemId)) return;
  
  fCurrentSegmentation->NextPad();
}

//______________________________________________________________________________
Int_t AliMUONGeometrySegmentation::MorePads(Int_t detElemId)
{
// Iterate over pads - condition
// ---

  if (!Notify(detElemId)) return 0;
  
  return fCurrentSegmentation->MorePads();
}

//______________________________________________________________________________
Float_t AliMUONGeometrySegmentation::Distance2AndOffset(Int_t detElemId,
                                           Int_t ix, Int_t iy, 
				           Float_t xg, Float_t yg,  Float_t zg,
					   Int_t* dummy)
{					   
// Returns the square of the distance between 1 pad
// labelled by its channel numbers and a coordinate
// ---

  if (!Notify(detElemId)) return 0.;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xg, yg, zg, xl, yl, zl); 

  return fCurrentSegmentation->Distance2AndOffset(ix, iy, xl, yl, dummy);
}

//______________________________________________________________________________
void AliMUONGeometrySegmentation::GetNParallelAndOffset(Int_t detElemId,
                                            Int_t ix, Int_t iy,
				            Int_t* nparallel, Int_t* offset)
{					   
// Number of pads read in parallel and offset to add to x 
// (specific to LYON, but mandatory for display)
// CHECK
// ---

  if (!Notify(detElemId)) return;

  fCurrentSegmentation->GetNParallelAndOffset(ix, iy, nparallel, offset);  
}


//______________________________________________________________________________
void AliMUONGeometrySegmentation::Neighbours(Int_t detElemId,
                                           Int_t ix, Int_t iy, 
                                           Int_t* nlist, 
					   Int_t xlist[10], Int_t ylist[10])
{					  
// Get next neighbours 
// ---

  if (!Notify(detElemId)) return;

  fCurrentSegmentation->Neighbours(ix, iy, nlist, xlist, ylist);
}

//______________________________________________________________________________
Int_t  AliMUONGeometrySegmentation::Ix()
{
// Current pad cursor during disintegration
// x, y-coordinate
// ---

  return fCurrentSegmentation->Ix();
}

//______________________________________________________________________________
Int_t  AliMUONGeometrySegmentation::Iy()
{
// Current pad cursor during disintegration
// x, y-coordinate
// ---

  return fCurrentSegmentation->Iy();
}

//______________________________________________________________________________
Int_t  AliMUONGeometrySegmentation::DetElemId()
{
// Current pad cursor during disintegration
// x, y-coordinate
// ---

  return fCurrentDetElemId;
}

//______________________________________________________________________________
Int_t  AliMUONGeometrySegmentation::ISector()
{
// Current sector
// ---

  return fCurrentSegmentation->ISector();
}

//______________________________________________________________________________
Int_t AliMUONGeometrySegmentation::Sector(Int_t detElemId, Int_t ix, Int_t iy)
{
// Calculate sector from pad coordinates
// ---

  if (!Notify(detElemId)) return 0;

  return fCurrentSegmentation->Sector(ix, iy);
}

//______________________________________________________________________________
Int_t AliMUONGeometrySegmentation::Sector(Int_t detElemId,
                                        Float_t xg, Float_t yg, Float_t zg)
{
// Calculate sector from pad coordinates
// ---

  if (!Notify(detElemId)) return 0;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xg, yg, zg, xl, yl, zl); 

  return fCurrentSegmentation->Sector(xl, yl);
}

//______________________________________________________________________________
void  AliMUONGeometrySegmentation::IntegrationLimits(Int_t detElemId,
                                        Float_t& x1, Float_t& x2,
                                        Float_t& y1, Float_t& y2)
{					 	  
// Current integration limits 
// ---
 
  if (!Notify(detElemId)) return;

  return fCurrentSegmentation->IntegrationLimits(x1, x2, y1, y2);
}

//______________________________________________________________________________
Int_t AliMUONGeometrySegmentation::SigGenCond(Int_t detElemId,
                                        Float_t xg, Float_t yg, Float_t zg)
{
// Signal Generation Condition during Stepping
//  0: don't generate signal
//  1: generate signal 
//  Comments: 
//
//  Crossing of pad boundary and mid plane between neighbouring wires is checked.
//  To correctly simulate the dependence of the spatial resolution on the angle 
//  of incidence signal must be generated for constant steps on 
//  the projection of the trajectory along the anode wire.
//
//  Signal will be generated if particle crosses pad boundary or
//  boundary between two wires. 
// ---

  if (!Notify(detElemId)) return 0;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xg, yg, zg, xl, yl, zl); 

  return fCurrentSegmentation->SigGenCond(xl, yl, zl);
 }


//______________________________________________________________________________
void  AliMUONGeometrySegmentation::SigGenInit(Int_t detElemId,
                                       Float_t xg, Float_t yg, Float_t zg)
{
// Initialise signal generation at coord (x,y,z)
// Initialises pad and wire position during stepping.
// From AliMUONGeometrySegmentationV01
// ---

  if (!Notify(detElemId)) return;

  Float_t xl, yl, zl;
  fCurrentDetElement->Global2Local(xg, yg, zg, xl, yl, zl); 

  fCurrentSegmentation->SigGenInit(xl, yl, zl);
}		    
    
//______________________________________________________________________________
void AliMUONGeometrySegmentation::GiveTestPoints(Int_t /*detElemId*/,
                                       Int_t& /*n*/, 
				       Float_t* /*xg*/, Float_t* /*yg*/) const
{					      
// Test points for auto calibration
// Returns test point on the pad plane.
// Used during determination of the segmoid correction of the COG-method
// From AliMUONGeometrySegmentationV01
// ---

  // Requires change of interface
  // to convert points from local to global we need z coordinate
  AliError("Not implemented.");
}

//______________________________________________________________________________
void AliMUONGeometrySegmentation::Draw(Int_t detElemId, const char* opt) const
{
// Draw the segmentation zones.
// (Called from AliMUON::BuildGeometry)
// ---

  if (!Notify(detElemId)) return;

  fCurrentSegmentation->Draw(opt);
}

//______________________________________________________________________________
void AliMUONGeometrySegmentation::SetCorrFunc(Int_t detElemId, 
                                              Int_t isec, TF1* func)
{
// Set the correction function.
// From AliMUONGeometrySegmentationV01
// ---

  if (!Notify(detElemId)) return;

  fCurrentSegmentation->SetCorrFunc(isec, func);
}

//______________________________________________________________________________
TF1* AliMUONGeometrySegmentation::CorrFunc(Int_t detElemId, Int_t isec) const
{
// Get the correction Function.
// From AliMUONGeometrySegmentationV01
// ---

  if (!Notify(detElemId)) return 0;

  return  fCurrentSegmentation->CorrFunc(isec);
} 

