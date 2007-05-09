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

///////////////////////////////////////////////////////////////////////////////
//  Reconstructed space point class for set:ITS   
//  Reconstructed points are expressed simultaneously in two different 
//  reference frames, both differing from the global system.
//  The first is referred to the sensor (see AliITSsegmentation for the
//  definition) and each point is represented by two coordinates: fXloc and
//  fZloc. This system in the code is referred to as "local"
//  The second is used for tracking (V2, SA and MI versions) and the X axis 
//  represents the radial coordinate (this system is, in the bending plane, 
//  a rotated system w.r.t. the global reference system). 
//  Each reaconstructed point is represented by two coordinates: fY and fZ, 
//  inherited from AliCluster. This system in the code is referred to as 
//  "trackingV2".
///////////////////////////////////////////////////////////////////////////////


#include "AliITSRecPoint.h"
#include "AliAlignObj.h"

ClassImp(AliITSRecPoint)

//_____________________________________________________________
AliITSRecPoint::AliITSRecPoint(): AliCluster(),
fXloc(0),
fZloc(0),
fdEdX(0),
fIndex(0),
fQ(0),
fLayer(0),
fNz(0),
fNy(0),
fChargeRatio(0),
fType(0),
fDeltaProb(0)
{
    // default constructor
}

//________________________________________________________________________
AliITSRecPoint::AliITSRecPoint(Int_t *lab,Float_t *hit, Int_t *info, Bool_t local):
AliCluster(AliAlignObj::LayerToVolUID((info[2]+AliAlignObj::kSPD1),lab[3]&0x3FF),hit,0,0,lab),
fXloc(0),
fZloc(0),
fdEdX(0),
fIndex(lab[3]),
fQ(hit[4]),
fLayer(info[2]),
fNz(info[1]),
fNy(info[0]),
fChargeRatio(0),
fType(0),
fDeltaProb(0)
{
  //standard constructor used in AliITSClusterFinderV2

  if (!local) { // Cluster V2
    Double_t txyz[3] = {GetX(), GetY(), GetZ()};
    Double_t lxyz[3] = {0, 0, 0};
    GetTracking2LocalMatrix()->LocalToMaster(txyz,lxyz);
    fXloc = lxyz[0]; fZloc = lxyz[2];
  }
  else {
    switch (fLayer) {
    case 0:
    case 1:
      fdEdX = 0;
      break;
    case 2:
    case 3:
      fdEdX=fQ*1e-6;
      break;
    case 4:
    case 5:
      fdEdX=fQ*2.16;
      break;
    default:
      AliError(Form("Wrong ITS layer %d (0 -> 5)",fLayer));
      break;
    }
    fXloc = hit[0];
    fZloc = hit[1];
    Double_t lxyz[3] = {fXloc, 0, fZloc};
    Double_t txyz[3] = {0, 0, 0};
    GetTracking2LocalMatrix()->MasterToLocal(lxyz,txyz);

    SetX(0.); SetY(txyz[1]); SetZ(txyz[2]);

  }

}

//_______________________________________________________________________
AliITSRecPoint::AliITSRecPoint(const AliITSRecPoint& pt):AliCluster(pt),
fXloc(pt.fXloc),
fZloc(pt.fZloc),
fdEdX(pt.fdEdX),
fIndex(pt.fIndex),
fQ(pt.fQ),
fLayer(pt.fLayer),
fNz(pt.fNz),
fNy(pt.fNy),
fChargeRatio(pt.fChargeRatio),
fType(pt.fType),
fDeltaProb(pt.fDeltaProb)
{
  //Copy constructor

}

//______________________________________________________________________
AliITSRecPoint& AliITSRecPoint::operator=(const AliITSRecPoint& source){
  // Assignment operator

  this->~AliITSRecPoint();
  new(this) AliITSRecPoint(source);
  return *this;

}

//----------------------------------------------------------------------
void AliITSRecPoint::Print(ostream *os){
    ////////////////////////////////////////////////////////////////////////
    // Standard output format for this class.
    ////////////////////////////////////////////////////////////////////////
#if defined __GNUC__
#if __GNUC__ > 2
    ios::fmtflags fmt;
#else
    Int_t fmt;
#endif
#else
#if defined __ICC || defined __ECC || defined __xlC__
    ios::fmtflags fmt;
#else
    Int_t fmt;
#endif
#endif
 
    fmt = os->setf(ios::fixed);  // set fixed floating point output
    *os << GetLabel(0) << " " << GetLabel(1) << " " << GetLabel(2) << " ";
    *os << fXloc << " " << fZloc << " " << fQ << " ";
    fmt = os->setf(ios::scientific); // set scientific for dEdX.
    *os << fdEdX << " ";
    fmt = os->setf(ios::fixed); // every fixed
    *os << GetSigmaY2() << " " << GetSigmaZ2();
    os->flags(fmt); // reset back to old formating.
    return;
}
//----------------------------------------------------------------------
void AliITSRecPoint::Read(istream *is){
////////////////////////////////////////////////////////////////////////
// Standard input format for this class.
////////////////////////////////////////////////////////////////////////
 
  Int_t lab[4];
  Float_t hit[5];
  lab[3] = 0; // ??
  *is >> lab[0] >> lab[1] >> lab[2] >> hit[0] >> hit[1] >> hit[4];
  *is >> fdEdX >> hit[2] >> hit[3];
  Int_t info[3] = {0,0,0};
  AliITSRecPoint rp(lab,hit,info,kTRUE);
  *this = rp;

  return;
}
//----------------------------------------------------------------------
ostream &operator<<(ostream &os,AliITSRecPoint &p){
////////////////////////////////////////////////////////////////////////
// Standard output streaming function.
////////////////////////////////////////////////////////////////////////
 
    p.Print(&os);
    return os;
}
//----------------------------------------------------------------------
istream &operator>>(istream &is,AliITSRecPoint &r){
////////////////////////////////////////////////////////////////////////
// Standard input streaming function.
////////////////////////////////////////////////////////////////////////
 
    r.Read(&is);
    return is;
}
//----------------------------------------------------------------------
