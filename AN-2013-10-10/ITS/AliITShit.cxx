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

#include <Riostream.h>

#include <TLorentzVector.h>
#include <TParticle.h>

#include "AliRun.h"
#include "AliITS.h"
#include "AliITSgeom.h"
#include "AliITShit.h"
#include "AliMC.h"
#include "AliStack.h"

using std::ios;
ClassImp(AliITShit)
////////////////////////////////////////////////////////////////////////
// Version: 0
// Written by Rene Brun, Federico Carminati, and Roberto Barbera
//
// Version: 1
// Modified and documented by Bjorn S. Nilsen
// July 11 1999
//
// AliITShit is the hit class for the ITS. Hits are the information
// that comes from a Monte Carlo at each step as a particle mass through
// sensitive detector elements as particles are transported through a
// detector.
//
//Begin_Html
/*
<img src="picts/ITS/AliITShit_Class_Diagram.gif">
</pre>
<br clear=left>
<font size=+2 color=red>
<p>This show the relasionships between the ITS hit class and the rest of Aliroot.
</font>
<pre>
*/
//End_Html
////////////////////////////////////////////////////////////////////////
// Inline Member functions:
//
// AliITShit()
//     The default creator of the AliITShit class.
//
// ~AliITShit()
//     The default destructor of the AliITShit class.
//
// int GetTrack()
//     See AliHit for a full description. Returns the track number fTrack
// for this hit.
//
// SetTrack(int track)
//     See AliHit for a full description. Sets the track number fTrack
// for this hit.
//
// Int_t GetTrackStatus()
//     Returns the value of the track status flag fStatus. This flag
// indicates the track status at the time of creating this hit. It is
// made up of the following 8 status bits from highest order to lowest
// order bits
// 0           :  IsTrackAlive():    IsTrackStop():IsTrackDisappeared():
// IsTrackOut():IsTrackExiting():IsTrackEntering():IsTrackInside()     .
// See AliMC for a description of these functions. If the function is
// true then the bit is set to one, otherwise it is zero.
//
// Bool_t StatusInside()
//     Returns kTRUE if the particle producing this hit is still inside
// the present volume. Returns kFalse if this particle will be in another
// volume. {bit IsTrackInside is set or not}
//
// Bool_t StatusEntering()
//     Returns kTRUE if the particle producing this hit is has just enterd
// the present volume. Returns kFalse otherwise.  {bit IsTrackEntering is
// set or not}
//
// Bool_t StatusExiting()
//     Returns kTRUE if the particle producing this hit is will exit
// the present volume. Returns kFalse otherwise. {bit IsTrackExiting is set
// or not}
//
// Bool_t StatusOut()
//     Returns kTRUE if the particle producing this hit is goint exit the
// simulation. Returns kFalse otherwise. {bit IsTrackOut is set or not}
//
// Bool_t StatusDisappeared()
//     Returns kTRUE if the particle producing this hit is going to "disappear"
// for example it has interacted producing some other particles. Returns
//  kFalse otherwise. {bit IsTrackOut is set or not}
//
// Bool_t StatusStop()
//     Returns kTRUE if the particle producing this hit is has dropped below
// its energy cut off producing some other particles. Returns kFalse otherwise.
// {bit IsTrackOut is set or not}
//
// Bool_t StatuAlives()
//     Returns kTRUE if the particle producing this hit is going to continue
// to be transported. Returns kFalse otherwise. {bit IsTrackOut is set or not}
//
// Int_t GetLayer()
//     Returns the layer number, fLayer, for this hit.
//
// Int_t GetLadder()
//     Returns the ladder number, fLadder, for this hit.
//
// Int_t GetDetector()
//     Returns the detector number, fDet, for this hit.
//
// GetDetectorID(Int_t &layer, Int_t &ladder, Int_t &detector)
//     Returns the layer, ladder, and detector numbers, fLayer fLadder fDet,
// in one call.
//
// Float_t GetIonization()
//     Returns the energy lost, fDestep, by the particle creating this hit,
// in the units defined by the Monte Carlo.
//
// GetPositionG(Float_t &x, Float_t &y, Float_t &z)
//     Returns the global position, fX fY fZ, of this hit, in the units
// define by the Monte Carlo.
//
// GetPositionG(Double_t &x, Double_t &y, Double_t &z)
//     Returns the global position, fX fY fZ, of this hit, in the units
// define by the Monte Carlo.
//
// GetPositionG(Float_t &x, Float_t &y, Float_t &z, Float_t &tof)
//     Returns the global position and time of flight, fX fY fZ fTof, of
// this hit, in the units define by the Monte Carlo.
//
// GetPositionG(Double_t &x,Double_t &y,Double_t &z,Double_t &tof)
//     Returns the global position and time of flight, fX fY fZ fTof, of
// this hit, in the units define by the Monte Carlo.
//
// GetPositionL(Double_t &x,Double_t &y,Double_t &z)
//     Returns the local position, fX fY fZ, of this hit in the coordiante
// of this module, in the units define by the Monte Carlo.
//
// GetPositionG(Double_t &x,Double_t &y,Double_t &z,Double_t &tof)
//     Returns the local position and time of flight, fX fY fZ fTof, of
// this hit in the coordinates of this module, in the units define by the
//  Monte Carlo.
//
// Float_t GetXG()
//     Returns the global x position in the units defined by the Monte Carlo.
//
// Float_t GetYG()
//     Returns the global y position in the units defined by the Monte Carlo.
//
// Float_t GetYG()
//     Returns the global z position in the units defined by the Monte Carlo.
//
// Float_t GetTOF()
//     Returns the time of flight, fTof, of this hit, in the units defined
// by the Monte Carlo.
//
// GetMomentumG(Float_t &px, Float_t &py, Float_t &pz)
//     Returns the global momentum, fPx fPy fPz, of the particle that made
// this hit, in the units define by the Monte Carlo.
//
// GetMomentumG(Double_t &px,Double_t &py,Double_t &pz)
//     Returns the global momentum, fPx fPy fPz, of the particle that made
// this hit, in the units define by the Monte Carlo.
//
// GetMomentumL(Double_t &px,Double_t &py,Double_t &pz)
//     Returns the momentum, fPx fPy fPz in coordinate appropreate for this
// specific module, in the units define by the Monte Carlo.
//
// Float_t GetPXG()
//     Returns the global X momentum in the units defined by the Monte Carlo.
//
// Float_t GetPYG()
//     Returns the global Y momentum in the units defined by the Monte Carlo.
//
// Float_t GetPZG()
//     Returns the global Z momentum in the units defined by the Monte Carlo.
//
////////////////////////////////////////////////////////////////////////
//_____________________________________________________________________________
AliITShit::AliITShit():AliHit(),
fStatus(0), // Track Status
fModule(0),  // Module number 
fPx(0.0),     // PX of particle at the point of the hit
fPy(0.0),     // PY of particle at the point of the hit
fPz(0.0),     // PZ of particle at the point of the hit
fDestep(0.0), // Energy deposited in the current step
fTof(0.0),    // Time of flight at the point of the hit
fStatus0(0),// Track Status of Starting point
fx0(0.0),     // Starting point of this step
fy0(0.0),     // Starting point of this step
fz0(0.0),     // Starting point of this step
ft0(0.0)     // Starting point of this step
{
    // Default Constructor
    // Zero data member just to be safe.
    // Intputs:
    //    none.
    // Outputs:
    //    none.
    // Return:
    //    A default created AliITShit class.

}
//----------------------------------------------------------------------
AliITShit::AliITShit(Int_t shunt,Int_t track,Int_t *vol,Float_t edep,
                     Float_t tof,TLorentzVector &x,TLorentzVector &x0,
                     TLorentzVector &p) :
AliHit(shunt, track), // AliHit
fStatus(vol[3]), // Track Status
fModule(vol[0]),  // Module number 
fPx(p.Px()),     // PX of particle at the point of the hit
fPy(p.Py()),     // PY of particle at the point of the hit
fPz(p.Pz()),     // PZ of particle at the point of the hit
fDestep(edep), // Energy deposited in the current step
fTof(tof),    // Time of flight at the point of the hit
fStatus0(vol[4]),// Track Status of Starting point
fx0(x0.X()),     // Starting point of this step
fy0(x0.Y()),     // Starting point of this step
fz0(x0.Z()),     // Starting point of this step
ft0(x0.T())     // Starting point of this step
{
    // Create ITS hit
    //     The creator of the AliITShit class. The variables shunt and
    // track are passed to the creator of the AliHit class. See the AliHit
    // class for a full description. In the units of the Monte Carlo
    // Inputs:
    //    Int_t shunt   See AliHit
    //    Int_t track   Track number, see AliHit
    //    Int_t *vol     Array of integer hit data,
    //                     vol[0] module
    //                     vol[1] not used
    //                     vol[2] not used
    //                     vol[3] Set of status bits
    //                     vol[4] Set of status bits at start
    //    Float_t edep       The energy deposited GeV during the transport
    //                       of this step
    //    Float_t tof        The time of flight of this particle at this step
    //    TLorenzVector &x   The Global position of this step [cm]
    //    TLorenzVector &x0  The Global position of where this step 
    //                       started from [cm]
    //    TLorenzVector &p   The Global momentum of the particle at this
    //                       step [GeV/c]
    // Outputs:
    //    none.
    // Return:
    //    A default created AliITShit class.

    SetPosition(x);
}
//______________________________________________________________________
AliITShit::AliITShit(Int_t shunt, Int_t track, Int_t *vol, Float_t *hits):
    AliHit(shunt, track), // AliHit
fStatus(vol[3]), // Track Status
fModule(vol[0]),  // Module number 
fPx(hits[3]),     // PX of particle at the point of the hit
fPy(hits[4]),     // PY of particle at the point of the hit
fPz(hits[5]),     // PZ of particle at the point of the hit
fDestep(hits[6]), // Energy deposited in the current step
fTof(hits[7]),    // Time of flight at the point of the hit
fStatus0(vol[4]),// Track Status of Starting point
fx0(hits[8]),     // Starting point of this step
fy0(hits[9]),     // Starting point of this step
fz0(hits[10]),     // Starting point of this step
ft0(hits[11])     // Starting point of this step
{
    // Create ITS hit
    //     The creator of the AliITShit class. The variables shunt and
    // track are passed to the creator of the AliHit class. See the AliHit
    // class for a full description. the integer array *vol contains, in order,
    // fLayer = vol[0], fDet = vol[1], fLadder = vol[2], fStatus = vol[3].
    // The array *hits contains, in order, fX = hits[0], fY = hits[1],
    // fZ = hits[2], fPx = hits[3], fPy = hits[4], fPz = hits[5],
    // fDestep = hits[6], and fTof = hits[7]. In the units of the Monte Carlo
    // Intputs:
    //    Int_t shunt   See AliHit
    //    Int_t track   Track number, see AliHit
    //    Int_t *vol     Array of integer hit data,
    //                     vol[0] module number
    //                     vol[1] not used
    //                     vol[2] not used
    //                     vol[3] Set of status bits
    //                     vol[4] Set of status bits at start
    //    Float_t *hits   Array of hit information
    //                     hits[0] X global position of this hit
    //                     hits[1] Y global position of this hit
    //                     hits[2] Z global position of this hit
    //                     hits[3] Px global position of this hit
    //                     hits[4] Py global position of this hit
    //                     hits[5] Pz global position of this hit
    //                     hits[6] Energy deposited by this step
    //                     hits[7] Time of flight of this particle at this step
    //                     hits[8] X0 global position of start of step
    //                     hits[9] Y0 global position of start of step
    //                     hits[10] Z0 global position of start of step
    //                     hits[11] Time of flight of this particle before step
    // Outputs:
    //    none.
    // Return:
    //    A standard created AliITShit class.

    fX          = hits[0];  // Track X global position
    fY          = hits[1];  // Track Y global position
    fZ          = hits[2];  // Track Z global position
}
//______________________________________________________________________
AliITShit::AliITShit(const AliITShit &h):
AliHit(h), // AliHit
fStatus(h.fStatus), // Track Status
fModule(h.fModule),  // Module number 
fPx(h.fPx),     // PX of particle at the point of the hit
fPy(h.fPy),     // PY of particle at the point of the hit
fPz(h.fPz),     // PZ of particle at the point of the hit
fDestep(h.fDestep), // Energy deposited in the current step
fTof(h.fTof),    // Time of flight at the point of the hit
fStatus0(h.fStatus0),// Track Status of Starting point
fx0(h.fx0),     // Starting point of this step
fy0(h.fy0),     // Starting point of this step
fz0(h.fz0),     // Starting point of this step
ft0(h.ft0)     // Starting point of this step
{
    // The standard copy constructor
    // Inputs:
    //   AliITShit   &h the sourse of this copy
    // Outputs:
    //   none.
    // Return:
    //  A copy of the sourse hit h

    //Info("CopyConstructor","Coping hit");

    if(this == &h) return;
    return;
}
//______________________________________________________________________
AliITShit& AliITShit::operator=(const AliITShit &h){
    // The standard = operator
    // Inputs:
    //   AliITShit   &h the sourse of this copy
    // Outputs:
    //   none.
    // Return:
    //  A copy of the sourse hit h

    if(this == &h) return *this;
    this->fStatus  = h.fStatus;
    this->fModule  = h.fModule;
    this->fPx      = h.fPx;
    this->fPy      = h.fPy;
    this->fPz      = h.fPz;
    this->fDestep  = h.fDestep;
    this->fTof     = h.fTof;
    this->fStatus0 = h.fStatus0;
    this->fx0      = h.fx0;
    this->fy0      = h.fy0;
    this->fz0      = h.fz0;
    this->ft0      = h.ft0;
    return *this;
}
//______________________________________________________________________
void AliITShit::SetShunt(Int_t shunt){
    // Sets track flag based on shunt value. Code copied from
    // AliHit standar constructor.
    // Inputs:
    //   Int_t shunt    A flag to indecate what to do with track numbers
    // Outputs:
    //   none.
    // Return:
    //   none.
    Int_t primary,track,current,parent;
    TParticle *part;

    track = fTrack;
    if(shunt == 1) {
        primary = gAlice->GetMCApp()->GetPrimary(track);
        gAlice->GetMCApp()->Particle(primary)->SetBit(kKeepBit);
        fTrack=primary;
    }else if (shunt == 2) {
        // the "primary" particle associated to the hit is
        // the last track that has been flagged in the StepManager
        // used by PHOS to associate the hit with the decay gamma
        // rather than with the original pi0
        parent=track;
        while (1) {
            current=parent;
            part = gAlice->GetMCApp()->Particle(current);
            parent=part->GetFirstMother();
            if(parent<0 || part->TestBit(kKeepBit))
                break;
        }
        fTrack=current;
    }else {
        fTrack=track;
        gAlice->GetMCApp()->FlagTrack(fTrack);
    } // end if shunt
}
//______________________________________________________________________
void AliITShit::GetPositionL(Float_t &x,Float_t &y,Float_t &z,Float_t &tof){
    //     Returns the position and time of flight of this hit in the local
    // coordinates of this module, and in the units of the Monte Carlo.
    // Inputs:
    //   none.
    // Outputs:
    //   Float_t x   Global position of this hit [cm]
    //   Float_t y   Global position of this hit [cm]
    //   Float_t z   Global poistion of this hit [cm]
    //   Float_t tof Time of flight of particle at this hit
    // Return:
    //   none.
    AliITSgeom *gm = ((AliITS*)gAlice->GetDetector("ITS"))->GetITSgeom();
    Float_t g[3],l[3];

    g[0] = fX;
    g[1] = fY;
    g[2] = fZ;
    if(gm) {
        gm->GtoL(fModule,g,l);
        x = l[0];
        y = l[1];
        z = l[2];
    } else {
        Error("AliITShit","NULL pointer to the geometry! return smth else");
        // AliITSv7 - SDD case
        x=fX;
        y=fY;
        z=fZ;
    } // end if
    tof = fTof;
    return;
}
//______________________________________________________________________
void AliITShit::GetPositionL0(Double_t &x,Double_t &y,Double_t &z,
                              Double_t &tof){
    //     Returns the initial position and time of flight of this hit 
    // in the local coordinates of this module, and in the units of the 
    // Monte Carlo.
    // Inputs:
    //   none.
    // Outputs:
    //   Double_t x   Global position of this hit [cm]
    //   Double_t y   Global position of this hit [cm]
    //   Double_t z   Global poistion of this hit [cm]
    //   Double_t tof Time of flight of particle at this hit
    // Return:
    //   none.
    AliITSgeom *gm = ((AliITS*)gAlice->GetDetector("ITS"))->GetITSgeom();
    Float_t g[3],l[3];

    g[0] = fx0;
    g[1] = fy0;
    g[2] = fz0;
    if(gm) {
        gm->GtoL(fModule,g,l);
        x = l[0];
        y = l[1];
        z = l[2];
    } else {
        Error("AliITShit","NULL pointer to the geometry! return smth else");
        x=fx0;
        y=fy0;
        z=fz0;
    }
    tof = ft0;
    return;
}

//______________________________________________________________________
TParticle * AliITShit::GetParticle() const {
    //     Returns the pointer to the TParticle for the particle that created
    // this hit. From the TParticle all kinds of information about this 
    // particle can be found. See the TParticle class.
    // Inputs:
    //   none.
    // Outputs:
    //   none.
    // Return:
    //   The TParticle of the track that created this hit.

    return gAlice->GetMCApp()->Particle(GetTrack());
}
//----------------------------------------------------------------------
void AliITShit::GetDetectorID(Int_t &layer,Int_t &ladder,Int_t &det)const{
    // Returns the layer ladder and detector number lables for this
    // ITS module. The use of layer, ladder and detector number for
    // discribing the ITS is obsoleate.
    // Inputs:
    //   none.
    // Outputs:
    //   Int_t   &layer   Layer lable
    //   Int_t   &ladder  Ladder lable
    //   Int_t   &det     Detector lable
    // Return:
    //    none.
    AliITSgeom *gm = ((AliITS*)gAlice->GetDetector("ITS"))->GetITSgeom();

    gm->GetModuleId(fModule,layer,ladder,det);
    return;
}  
//----------------------------------------------------------------------
void AliITShit::Print(ostream *os) const {
    // Standard output format for this class.
    // Inputs:
    //   ostream *os   The output stream
    // Outputs:
    //   none.
    // Return:
    //   none.

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
 
    fmt = os->setf(ios::scientific);  // set scientific floating point output
    *os << fTrack << " " << fX << " " << fY << " " << fZ << " ";
    fmt = os->setf(ios::hex); // set hex for fStatus only.
    *os << fStatus << " ";
    fmt = os->setf(ios::dec); // every thing else decimel.
    *os << fModule << " ";
    *os << fPx << " " << fPy << " " << fPz << " ";
    *os << fDestep << " " << fTof;
    *os << " " << fx0 << " " << fy0 << " " << fz0;
//    *os << " " << endl;
    os->flags(fmt); // reset back to old formating.
    return;
}
//----------------------------------------------------------------------
void AliITShit::Read(istream *is) {
    // Standard input format for this class.
    // Inputs:
    //   istream *is  the input stream
    // Outputs:
    //   none.
    // Return:
    //   none.

    *is >> fTrack >> fX >> fY >> fZ;
    *is >> fStatus >> fModule >> fPx >> fPy >> fPz >> fDestep >> fTof;
    *is >> fx0 >> fy0 >> fz0;
    return;
}
//----------------------------------------------------------------------
ostream &operator<<(ostream &os,AliITShit &p){
    // Standard output streaming function.
    // Inputs:
    //   ostream os  The output stream
    //   AliITShit p The his to be printed out
    // Outputs:
    //   none.
    // Return:
    //   The input stream

    p.Print(&os);
    return os;
}
//----------------------------------------------------------------------
istream &operator>>(istream &is,AliITShit &r){
    // Standard input streaming function.
    // Inputs:
    //   istream is  The input stream
    //   AliITShit p The AliITShit class to be filled from this input stream
    // Outputs:
    //   none.
    // Return:
    //   The input stream

    r.Read(&is);
    return is;
}
//----------------------------------------------------------------------
