#ifndef TFLUKA
#define TFLUKA
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                                                           //
// FLUKA implementation of the AliMC Interface                               //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


#include "TVirtualMC.h"
#include "TMCProcess.h" 


class TFluka : public TVirtualMC {
  
 public:
  TFluka(const char *title);
  TFluka();
  virtual ~TFluka() {}
  
  //
  // methods for building/management of geometry
  // ------------------------------------------------
  //
  
  // functions from GCONS 
  virtual void  Gfmate(Int_t imat, char *name, Float_t &a, Float_t &z,  
		       Float_t &dens, Float_t &radl, Float_t &absl,
		       Float_t* ubuf, Int_t& nbuf)
    {printf(" not yet implemented !\n");}
  virtual void  Gfmate(Int_t imat, char *name, Double_t &a, Double_t &z,  
		       Double_t &dens, Double_t &radl, Double_t &absl,
		       Double_t* ubuf, Int_t& nbuf)
    {printf(" not yet implemented !\n");}
  
  // detector composition
  virtual void  Material(Int_t& kmat, const char* name, Double_t a, 
			 Double_t z, Double_t dens, Double_t radl, Double_t absl,
			 Float_t* buf, Int_t nwbuf)
    {printf(" not yet implemented !\n");}
  virtual void  Material(Int_t& kmat, const char* name, Double_t a, 
			 Double_t z, Double_t dens, Double_t radl, Double_t absl,
			 Double_t* buf, Int_t nwbuf)
    {printf(" not yet implemented !\n");}
  virtual void  Mixture(Int_t& kmat, const char *name, Float_t *a, 
			Float_t *z, Double_t dens, Int_t nlmat, Float_t *wmat)
    {printf(" not yet implemented !\n");}
  virtual void  Mixture(Int_t& kmat, const char *name, Double_t *a, 
			Double_t *z, Double_t dens, Int_t nlmat, Double_t *wmat)
    {printf(" not yet implemented !\n");}
  virtual void  Medium(Int_t& kmed, const char *name, Int_t nmat, 
		       Int_t isvol, Int_t ifield, Double_t fieldm, Double_t tmaxfd, 
		       Double_t stemax, Double_t deemax, Double_t epsil, 
		       Double_t stmin, Float_t* ubuf, Int_t nbuf)
    {printf(" not yet implemented !\n");}
  virtual void  Medium(Int_t& kmed, const char *name, Int_t nmat, 
		       Int_t isvol, Int_t ifield, Double_t fieldm, Double_t tmaxfd, 
		       Double_t stemax, Double_t deemax, Double_t epsil, 
		       Double_t stmin, Double_t* ubuf, Int_t nbuf)
    {printf(" not yet implemented !\n");}
  virtual void  Matrix(Int_t& krot, Double_t thetaX, Double_t phiX, 
		       Double_t thetaY, Double_t phiY, Double_t thetaZ, 
		       Double_t phiZ)
    {printf(" not yet implemented !\n");}
  virtual void  Gstpar(Int_t itmed, const char *param, Double_t parval)
    {printf(" not yet implemented !\n");}
  
  // functions from GGEOM 
  virtual Int_t  Gsvolu(const char *name, const char *shape, Int_t nmed,  
                          Float_t *upar, Int_t np)
    {printf(" not yet implemented !\n"); return -1;}
  virtual Int_t  Gsvolu(const char *name, const char *shape, Int_t nmed,  
			Double_t *upar, Int_t np)
    {printf(" not yet implemented !\n"); return -1;}
  virtual void  Gsdvn(const char *name, const char *mother, Int_t ndiv, 
		      Int_t iaxis)
    {printf(" not yet implemented !\n");}
  virtual void  Gsdvn2(const char *name, const char *mother, Int_t ndiv, 
		       Int_t iaxis, Double_t c0i, Int_t numed)
    {printf(" not yet implemented !\n");}
  virtual void  Gsdvt(const char *name, const char *mother, Double_t step, 
		      Int_t iaxis, Int_t numed, Int_t ndvmx)
    {printf(" not yet implemented !\n");}
  virtual void  Gsdvt2(const char *name, const char *mother, Double_t step, 
		       Int_t iaxis, Double_t c0, Int_t numed, Int_t ndvmx)
    {printf(" not yet implemented !\n");}
  virtual void  Gsord(const char *name, Int_t iax)
    {printf(" not yet implemented !\n");}
  virtual void  Gspos(const char *name, Int_t nr, const char *mother,  
		      Double_t x, Double_t y, Double_t z, Int_t irot, 
		      const char *konly="ONLY")
    {printf(" not yet implemented !\n");}
  virtual void  Gsposp(const char *name, Int_t nr, const char *mother,  
		       Double_t x, Double_t y, Double_t z, Int_t irot,
		       const char *konly, Float_t *upar, Int_t np)
    {printf(" not yet implemented !\n");}
  virtual void  Gsposp(const char *name, Int_t nr, const char *mother,  
		       Double_t x, Double_t y, Double_t z, Int_t irot,
		       const char *konly, Double_t *upar, Int_t np)
    {printf(" not yet implemented !\n");}
  virtual void  Gsbool(const char* onlyVolName, const char* manyVolName)
    {printf(" not yet implemented !\n");}
  
  virtual void  SetCerenkov(Int_t itmed, Int_t npckov, Float_t *ppckov,
			    Float_t *absco, Float_t *effic, Float_t *rindex)
    {printf(" not yet implemented !\n");}
  virtual void  SetCerenkov(Int_t itmed, Int_t npckov, Double_t *ppckov,
			    Double_t *absco, Double_t *effic, Double_t *rindex)
    {printf(" not yet implemented !\n");}
  
  
  // functions for drawing
  virtual void  DrawOneSpec(const char* name)
    {printf(" not yet implemented !\n");}
  virtual void  Gsatt(const char* name, const char* att, Int_t val)
    {printf(" not yet implemented !\n");}
  virtual void  Gdraw(const char*,Double_t theta = 30, Double_t phi = 30,
		      Double_t psi = 0, Double_t u0 = 10, Double_t v0 = 10,
		      Double_t ul = 0.01, Double_t vl = 0.01)
    {printf(" not yet implemented !\n");}
  
  // Euclid
  virtual void  WriteEuclid(const char*, const char*, Int_t, Int_t)
    {printf(" not yet implemented !\n");}
  
  // get methods
  virtual Int_t VolId(const Text_t* volName) const
    {printf(" not yet implemented !\n"); return -1;}
  virtual const char* VolName(Int_t id) const
    {printf(" not yet implemented !\n"); return "void";}
  virtual Int_t NofVolumes() const
    {printf(" not yet implemented !\n"); return -1;}
  virtual Int_t VolId2Mate(Int_t id) const
    {printf(" not yet implemented !\n"); return -1;}
  
  //
  // methods for physics management
  // ------------------------------------------------
  //
  
  // set methods
  virtual void     SetCut(const char* cutName, Double_t cutValue)
    {printf(" not yet implemented !\n");}
  virtual void     SetProcess(const char* flagName, Int_t flagValue)
    {printf(" not yet implemented !\n");}
  virtual Double_t Xsec(char*, Double_t, Int_t, Int_t)
    {printf(" not yet implemented !\n"); return -1.;}
  
  // particle table usage         
  virtual Int_t   IdFromPDG(Int_t id) const
    {printf(" not yet implemented !\n"); return -1;}  
  virtual Int_t   PDGFromId(Int_t pdg) const
    {printf(" not yet implemented !\n"); return -1;}  
  virtual void    DefineParticles()
    {printf(" not yet implemented !\n");}     
  
  //
  // methods for step management
  // ------------------------------------------------
  //
  
  // action methods
  virtual void StopTrack()
    {printf(" not yet implemented !\n");}
  virtual void StopEvent()
    {printf(" not yet implemented !\n");}   
  
  // set methods
  virtual void SetMaxStep(Double_t)
    {printf(" not yet implemented !\n");}
  virtual void SetMaxNStep(Int_t)
    {printf(" not yet implemented !\n");}
  virtual void SetUserDecay(Int_t)
    {printf(" not yet implemented !\n");}  
  
  // get methods
  // tracking volume(s) 
  virtual Int_t    CurrentVolID(Int_t& copyNo) const
    {printf(" not yet implemented !\n"); return -1;}
  virtual Int_t    CurrentVolOffID(Int_t off, Int_t& copyNo) const
    {printf(" not yet implemented !\n"); return -1;}
  virtual const char* CurrentVolName() const
    {printf(" not yet implemented !\n"); return "void";}
  virtual const char* CurrentVolOffName(Int_t off) const
    {printf(" not yet implemented !\n"); return "void";}
  virtual Int_t    CurrentMaterial(Float_t &a, Float_t &z, 
				   Float_t &dens, Float_t &radl, Float_t &absl) const
    {printf(" not yet implemented !\n"); return -1;}  
  virtual Int_t    CurrentEvent() const
    {printf(" not yet implemented !\n"); return -1;} 
  virtual void     Gmtod(Float_t* xm, Float_t* xd, Int_t iflag)
    {printf(" not yet implemented !\n");}
  virtual void     Gmtod(Double_t* xm, Double_t* xd, Int_t iflag)
    {printf(" not yet implemented !\n");}
  virtual void     Gdtom(Float_t* xd, Float_t* xm, Int_t iflag)
    {printf(" not yet implemented !\n");}
  virtual void     Gdtom(Double_t* xd, Double_t* xm, Int_t iflag)
    {printf(" not yet implemented !\n");}
  virtual Double_t MaxStep() const
    {printf(" not yet implemented !\n"); return -1.;}
  virtual Int_t    GetMaxNStep() const
    {printf(" not yet implemented !\n"); return -1;}
  virtual Int_t    GetMedium() const
    {printf(" not yet implemented !\n"); return -1;}
  
  // tracking particle 
  // dynamic properties
  virtual void     TrackPosition(TLorentzVector& position) const
    {printf(" not yet implemented !\n");}
  virtual void     TrackMomentum(TLorentzVector& momentum) const
    {printf(" not yet implemented !\n");}
  virtual Double_t TrackStep() const
    {printf(" not yet implemented !\n"); return -1.;}
  virtual Double_t TrackLength() const
    {printf(" not yet implemented !\n"); return -1.;} 
  virtual Double_t TrackTime() const
    {printf(" not yet implemented !\n"); return -1.;}
  virtual Double_t Edep() const
    {printf(" not yet implemented !\n"); return -1.;}
  // static properties
  virtual Int_t    TrackPid() const
    {printf(" not yet implemented !\n"); return -1;}
  virtual Double_t TrackCharge() const
    {printf(" not yet implemented !\n"); return -1.;}
  virtual Double_t TrackMass() const
    {printf(" not yet implemented !\n"); return -1.;}
  virtual Double_t Etot() const
    {printf(" not yet implemented !\n"); return -1.;}
  
  // track status
  virtual Bool_t   IsNewTrack() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackInside() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackEntering() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackExiting() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackOut() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackDisappeared() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackStop() const
    {printf(" not yet implemented !\n"); return 0;}
  virtual Bool_t   IsTrackAlive() const
    {printf(" not yet implemented !\n"); return 0;}
 
  // secondaries
  virtual Int_t    NSecondaries() const
    {printf(" not yet implemented !\n"); return -1;}
  virtual void     GetSecondary(Int_t isec, Int_t& particleId, 
				TLorentzVector& position, TLorentzVector& momentum)
    {printf(" not yet implemented !\n");}
  virtual TMCProcess ProdProcess(Int_t isec) const
    {printf(" not yet implemented !\n"); return kPNoProcess;} 
  virtual Int_t    StepProcesses(TArrayI &proc) const
    {printf(" not yet implemented !\n"); return -1;}
  
  //
  // other (then geometry/step/run management) methods
  // ----------------------------------------------
  //
  virtual TMCGeomType GetMCGeomType() const
    {printf(" not yet implemented !\n"); return kFluka;}
  
  //
  // Geant3 specific methods
  // !!! need to be transformed to common interface
  //
  virtual void Gdopt(const char*,const char*)
    {printf(" not yet implemented !\n");}
  virtual void SetClipBox(const char*,Double_t=-9999,Double_t=0, Double_t=-9999,
			  Double_t=0,Double_t=-9999,Double_t=0)
    {printf(" not yet implemented !\n");}
  virtual void DefaultRange()
    {printf(" not yet implemented !\n");}
  virtual void Gdhead(Int_t, const char*, Double_t=0)
    {printf(" not yet implemented !\n");}  
  virtual void Gdman(Double_t, Double_t, const char*)
    {printf(" not yet implemented !\n");}
  virtual void SetColors()
    {printf(" not yet implemented !\n");}
  virtual void Gtreve()
    {printf(" not yet implemented !\n");}
  virtual void GtreveRoot()
    {printf(" not yet implemented !\n");}
  virtual void Gckmat(Int_t, char*)
    {printf(" not yet implemented !\n");}
  virtual void InitLego()
    {printf(" not yet implemented !\n");}
  virtual void Gfpart(Int_t, char*, Int_t&, Float_t&, Float_t&, Float_t&)
    {printf(" not yet implemented !\n");}
  virtual void Gspart(Int_t, const char*, Int_t, Double_t, Double_t, Double_t)
    {printf(" not yet implemented !\n");}
  
  //
  // control methods
  // ------------------------------------------------
  //
  
  virtual void Init();
  //{printf(" not yet implemented !\n");}
  virtual void FinishGeometry()
    {printf("FinishGeometry not yet implemented !\n");}
  virtual void BuildPhysics()
    {printf("BuildPhysics not yet implemented !\n");}
  virtual void ProcessEvent()
    {printf("ProcessEvent not yet implemented !\n");}
  virtual void ProcessRun(Int_t nevent)
    {printf("ProcessRun not yet implemented !\n");}
  
 private:
  TFluka(const TFluka &mc){}
  TFluka & operator=(const TFluka &) {return (*this);}
  
  ClassDef(TFluka,1)  //C++ interface to Fluka montecarlo
};

#endif //TFLUKA

