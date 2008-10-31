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

/* $Id: AliTRDtrackInfo.cxx 27496 2008-07-22 08:35:45Z cblume $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Reconstruction QA                                                     //
//                                                                        //
//  Authors:                                                              //
//    Alex Bercuci <A.Bercuci@gsi.de>                                     //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "AliTrackReference.h"
#include "AliExternalTrackParam.h"
#include "AliLog.h"

#include "AliTRDseedV1.h"
#include "AliTRDtrackV1.h"

#include <cstdio>
#include <cstdlib>

#include "AliTRDtrackInfo.h"

ClassImp(AliTRDtrackInfo)
ClassImp(AliTRDtrackInfo::AliMCinfo)
ClassImp(AliTRDtrackInfo::AliESDinfo)

//___________________________________________________
AliTRDtrackInfo::AliTRDtrackInfo():
  TObject()
  ,fNClusters(0)
  ,fTRDtrack(0x0)
  ,fOP(0x0)
  ,fMC(0x0)
  ,fESD()
{
  //
  // Default constructor
  //
}


//___________________________________________________
AliTRDtrackInfo::AliTRDtrackInfo(const AliTRDtrackInfo &trdInfo):
  TObject((const TObject&)trdInfo)  
  ,fNClusters(trdInfo.fNClusters)
  ,fTRDtrack(0x0)
  ,fOP(0x0)
  ,fMC(new AliMCinfo(*trdInfo.fMC))
  ,fESD(trdInfo.fESD)
{
  //
  // copy Entries
  //

  if(trdInfo.fOP) fOP = new AliExternalTrackParam(*trdInfo.fOP);

  if(trdInfo.fMC) fMC = new AliMCinfo(*trdInfo.fMC);

  if(trdInfo.fTRDtrack){ 
    fTRDtrack = new AliTRDtrackV1(*trdInfo.fTRDtrack);
    if(trdInfo.fTRDtrack->IsOwner()) fTRDtrack->SetOwner();
  }
}

//___________________________________________________
AliTRDtrackInfo::AliMCinfo::AliMCinfo()
  :fLabel(0)
  ,fPDG(0)
  ,fNTrackRefs(0)
{
  // Set 0-Pointers
  memset(fTrackRefs, 0, sizeof(AliTrackReference *) * 12);
}

//___________________________________________________
AliTRDtrackInfo::AliMCinfo::AliMCinfo(const AliMCinfo &mc)
  :fLabel(mc.fLabel)
  ,fPDG(mc.fPDG)
  ,fNTrackRefs(mc.fNTrackRefs)
{
  memset(fTrackRefs, 0, sizeof(AliTrackReference *) * 12);
  for(Int_t ien = 0; ien < 12; ien++){
    if(mc.fTrackRefs[ien])
      fTrackRefs[ien] = new AliTrackReference(*(mc.fTrackRefs[ien]));
  }
}

//___________________________________________________
AliTRDtrackInfo::AliESDinfo::AliESDinfo()
  :fId(-1)
  ,fStatus(0)
  ,fKinkIndex(0)
  ,fTPCncls(0)
  ,fTRDpidQuality(0)
  ,fTRDnSlices(0)
  ,fTRDslices(0x0)
{
  memset(fTRDr, 0, AliPID::kSPECIES*sizeof(Double32_t));
}

//___________________________________________________
AliTRDtrackInfo::AliESDinfo::AliESDinfo(const AliESDinfo &esd)
  :fId(esd.fId)
  ,fStatus(esd.fStatus)
  ,fKinkIndex(esd.fKinkIndex)
  ,fTPCncls(esd.fTPCncls)
  ,fTRDpidQuality(esd.fTRDpidQuality)
  ,fTRDnSlices(esd.fTRDnSlices)
  ,fTRDslices(0x0)
{
  memcpy(fTRDr, esd.fTRDr, AliPID::kSPECIES*sizeof(Double32_t));

  if(fTRDnSlices){
    fTRDslices = new Double32_t[fTRDnSlices];
    memcpy(fTRDslices, esd.fTRDslices, fTRDnSlices*sizeof(Double32_t));
  }
}


//___________________________________________________
AliTRDtrackInfo::~AliTRDtrackInfo()
{
  //
  // Destructor
  //

  if(fOP) delete fOP;
  if(fTRDtrack) delete fTRDtrack;
}

//___________________________________________________
AliTRDtrackInfo::AliMCinfo::~AliMCinfo()
{
  fNTrackRefs = 0;
  for(Int_t ien = 0; ien < 12; ien++){
    if(fTrackRefs[ien]) delete fTrackRefs[ien];
    fTrackRefs[ien] = 0x0;
  }
}

//___________________________________________________
AliTRDtrackInfo::AliESDinfo::~AliESDinfo()
{
  if(fTRDnSlices){
    delete [] fTRDslices;
    fTRDslices = 0x0;
    fTRDnSlices = 0;
  }
}


//___________________________________________________
AliTRDtrackInfo& AliTRDtrackInfo::operator=(const AliTRDtrackInfo &trdInfo)
{
  //
  // = Operator
  //

  fNClusters  = trdInfo.fNClusters;
  fESD = trdInfo.fESD;

  // copy Entries
  if(trdInfo.fOP){
    if(!fOP)
      fOP = new AliExternalTrackParam(*trdInfo.fOP);
    else
      new(fOP) AliExternalTrackParam(*trdInfo.fOP);
  }

  if(trdInfo.fMC){
    if(!fMC)
      fMC = new AliMCinfo(*trdInfo.fMC);
    else
      new(fMC) AliMCinfo(*trdInfo.fMC);
  }

  if(trdInfo.fTRDtrack){
    if(!fTRDtrack)
      fTRDtrack = new AliTRDtrackV1(*trdInfo.fTRDtrack);
    else
      new(fTRDtrack) AliTRDtrackV1(*trdInfo.fTRDtrack);
    if(trdInfo.fTRDtrack->IsOwner()) fTRDtrack->SetOwner();
  }

  return *this;
}

//___________________________________________________
AliTRDtrackInfo::AliMCinfo& AliTRDtrackInfo::AliMCinfo::operator=(const AliMCinfo &mc)
{
  fLabel      = mc.fLabel;
  fPDG        = mc.fPDG;
  fNTrackRefs = mc.fNTrackRefs;

  AliTrackReference **itr = &fTrackRefs[0];
  AliTrackReference* const *jtr = &mc.fTrackRefs[0];
  for(Int_t ien = 0; ien < 12; ien++, itr++, jtr++){
    if((*jtr)){
      if(!(*itr)) (*itr) = new AliTrackReference(*(*jtr));
      else new(&(*itr)) AliTrackReference(*(*jtr));
    } else (*itr) = 0x0;
  }
  return *this;
}

//___________________________________________________
AliTRDtrackInfo::AliESDinfo& AliTRDtrackInfo::AliESDinfo::operator=(const AliESDinfo &esd)
{
  fId          = esd.fId;
  fStatus      = esd.fStatus; 
  fTRDpidQuality = esd.fTRDpidQuality;
  fTRDnSlices  = esd.fTRDnSlices;
  fTRDslices   = 0x0;

  memcpy(fTRDr, esd.fTRDr, AliPID::kSPECIES*sizeof(Double32_t));

  if(fTRDnSlices){
    fTRDslices = new Double32_t[fTRDnSlices];
    memcpy(fTRDslices, esd.fTRDslices, fTRDnSlices*sizeof(Double32_t));
  }

  return *this;
}

//___________________________________________________
void AliTRDtrackInfo::Delete(const Option_t *)
{
  //
  // Delete
  //

  fNClusters  = 0;
  if(fOP) delete fOP; fOP = 0x0;
  if(fMC) delete fMC; fMC = 0x0;
  if(fTRDtrack) delete fTRDtrack; fTRDtrack = 0x0;
}

//___________________________________________________
void AliTRDtrackInfo::SetTRDtrack(const AliTRDtrackV1 *track)
{
  //
  // Set the TRD track
  //

  if(!fTRDtrack)
    fTRDtrack = new AliTRDtrackV1(*track);	
  else
    new(fTRDtrack)AliTRDtrackV1(*track);
  fTRDtrack->SetOwner();
  // Make a copy for the object in order to avoid ownership problems
}

//___________________________________________________
void AliTRDtrackInfo::AddTrackRef(const AliTrackReference *tref)
{
  //
  // Add track reference
  //

  SetMC();
  if(fMC->fNTrackRefs >= 12){ 
    SetCurved();
    return;
  }
  // Make a copy for the object in order to avoid ownership problems
  fMC->fTrackRefs[fMC->fNTrackRefs++] = new AliTrackReference(*tref);
}

//___________________________________________________
AliTrackReference * AliTRDtrackInfo::GetTrackRef(Int_t idx) const
{
  //
  // Returns a track reference
  //
  if(!fMC) return 0x0;
  return (idx>=0 && idx < 12) ? fMC->fTrackRefs[idx] : 0x0;
}

//___________________________________________________
Int_t AliTRDtrackInfo::GetNumberOfClusters() const
{
  //
  // Returns the number of clusters
  //

  Int_t n = 0;
  if(!fTRDtrack) return 0;
  if(fTRDtrack->GetNumberOfTracklets() == 0) return n;
  AliTRDseedV1 *tracklet = 0x0;
  for(Int_t ip=0; ip<6; ip++){
    if(!(tracklet = const_cast<AliTRDseedV1 *>(fTRDtrack->GetTracklet(ip)))) continue;
    n+=tracklet->GetN();
  }
  return n;
}


//___________________________________________________
void  AliTRDtrackInfo::SetOuterParam(const AliExternalTrackParam *op)
{
  //
  // Set outer track parameters
  //

  if(!op) return;
  if(fOP) new(fOP) AliExternalTrackParam(*op);
  else
    fOP = new AliExternalTrackParam(*op);
}

//___________________________________________________
Int_t AliTRDtrackInfo::GetNTracklets() const
{
  //
  // Return the number of tracklets
  //

  if(!fTRDtrack) return 0x0;
  return fTRDtrack->GetNumberOfTracklets();
}

//___________________________________________________
void AliTRDtrackInfo::SetSlices(Int_t n, Double32_t *s)
{
  if(fESD.fTRDnSlices != n){
    fESD.fTRDnSlices = 0;
    delete [] fESD.fTRDslices;
    fESD.fTRDslices = 0x0;
  }

  if(!fESD.fTRDnSlices){
    fESD.fTRDnSlices = n;
    fESD.fTRDslices = new Double32_t[n];
  }

  memcpy(fESD.fTRDslices, s, n*sizeof(Double32_t));
}

//___________________________________________________
Bool_t AliTRDtrackInfo::AliMCinfo::GetDirections(Float_t x0, Float_t &y0, Float_t &z0, Float_t &dydx, Float_t &dzdx) const
{
  // check for 2 track ref where the radial position has a distance less than 3.7mm and are close to the x0

  Int_t nFound = 0;
  AliTrackReference *tr[2] = {0x0, 0x0};
  AliTrackReference * const* jtr = &fTrackRefs[0];
  for(Int_t itr = 0; itr < fNTrackRefs; itr++, ++jtr){
    if(!(*jtr)) break;
/*
    if(fDebugLevel>=5) printf("\t\tref[%2d] x[%6.3f]\n", itr, (*jtr)->LocalX());*/
    if(TMath::Abs(x0 - (*jtr)->LocalX()) > 3.7) continue;
    tr[nFound++] = (*jtr);
    if(nFound == 2) break;
  } 
  if(nFound < 2){ 
    AliWarningGeneral("AliTRDtrackInfo::AliMCinfo::GetDirections()", Form("Missing track ref x0[%6.3f] nref[%d]\n", x0, nFound));
    return kFALSE;
  }
  Double_t dx = tr[1]->LocalX() - tr[0]->LocalX();
  if(dx <= 0. || TMath::Abs(dx-3.7)>1.E-3){
    AliWarningGeneral("AliTRDtrackInfo::AliMCinfo::GetDirections()", Form("Track ref with wrong radial distances refX0[%6.3f] refX1[%6.3f]", tr[0]->LocalX(), tr[1]->LocalX()));
    return kFALSE;
  }

  dydx = (tr[1]->LocalY() - tr[0]->LocalY()) / dx;
  dzdx = (tr[1]->Z() - tr[0]->Z()) / dx;
  Float_t dx0 = tr[1]->LocalX() - x0;
  y0   =  tr[1]->LocalY() - dydx*dx0;
  z0   =  tr[1]->Z() - dzdx*dx0;
  return kTRUE;
}

