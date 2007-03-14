#ifndef AliAODVertex_H
#define AliAODVertex_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD vertex base class
//     Author: Markus Oldenburg, CERN
//-------------------------------------------------------------------------

#include <TNamed.h>
#include <TRef.h>
#include <TRefArray.h>
#include <TMath.h>

#include "AliAODRedCov.h"

class AliAODVertex : public TObject {

 public :

  enum AODVtx_t {kUndef=-1, kPrimary, kKink, kV0, kCascade, kMulti};

  AliAODVertex();
  AliAODVertex(const Double_t *position, 
	       const Double_t *covMatrix=0x0,
	       Double_t chi2 = -999.,
	       TObject *parent = 0x0,
	       Char_t vtype=kUndef);
    AliAODVertex(const Float_t *position, 
		 const Float_t *covMatrix=0x0,
		 Double_t chi2 = -999.,
		 TObject *parent = 0x0,
		 Char_t vtype=kUndef);
    AliAODVertex(const Double_t *position, 
		 Double_t chi2,
		 Char_t vtype=kUndef);
    AliAODVertex(const Float_t *position, 
		 Double_t chi2,
		 Char_t vtype=kUndef);

  virtual ~AliAODVertex();
  AliAODVertex(const AliAODVertex& vtx); 
  AliAODVertex& operator=(const AliAODVertex& vtx);

  void     SetX(Double_t x) { fPosition[0] = x; }
  void     SetY(Double_t y) { fPosition[1] = y; }
  void     SetZ(Double_t z) { fPosition[2] = z; }
  void     SetPosition(Double_t x, Double_t y, Double_t z) { fPosition[0] = x; fPosition[1] = y; fPosition[2] = z; }
  template <class T> void SetPosition(T *pos)
    { fPosition[0] = pos[0]; fPosition[1] = pos[1]; fPosition[2] = pos[2]; }

  void     SetChi2(Double_t chi2) { fChi2 = chi2; }

  void     SetParent(TObject *parent) { fParent = parent; }

  Double_t  GetX() const { return fPosition[0]; }
  Double_t  GetY() const { return fPosition[1]; }
  Double_t  GetZ() const { return fPosition[2]; }
  template <class T> void GetPosition(T *pos) const
    {pos[0]=fPosition[0]; pos[1]=fPosition[1]; pos[2]=fPosition[2];}

  template <class T> void SetCovMatrix(const T *covMatrix) {
    if(!fCovMatrix) fCovMatrix=new AliAODRedCov<3>();
    fCovMatrix->SetCovMatrix(covMatrix);}

  template <class T> Bool_t GetCovMatrix(T *covMatrix) const {
    if(!fCovMatrix) return kFALSE;
    fCovMatrix->GetCovMatrix(covMatrix); return kTRUE;}

  void RemoveCovMatrix() {delete fCovMatrix; fCovMatrix=NULL;}

  template <class T> void     GetSigmaXYZ(T *sigma) const;

  Double_t  GetChi2() const { return fChi2; }
  Double_t  GetChi2perNDF() const
    { return fChi2/(2.*fDaughters.GetEntriesFast()-3.); }

  Char_t    GetType() const { return fType; }
  void      SetType(AODVtx_t vtype) { fType=vtype; }

  TObject* GetParent() const   { return fParent.GetObject(); }
  Bool_t   HasParent(TObject *parent) const { return (fParent.GetObject() == parent) ? kTRUE : kFALSE; }

  void     AddDaughter(TObject *daughter) { fDaughters.Add(daughter);}
  void     RemoveDaughter(TObject *daughter) { fDaughters.Remove(daughter); }
  TObject* GetDaughter(Int_t i) { return fDaughters.At(i); }
  Bool_t   HasDaughter(TObject *daughter) const;
  Int_t    GetNDaughters() const { return fDaughters.GetEntriesFast(); }

  // covariance matrix elements after rotation by phi around z-axis 
  // and, then, by theta around new y-axis
  Double_t  RotatedCovMatrixXX(Double_t phi = 0., Double_t theta = 0.) const;
  Double_t  RotatedCovMatrixXY(Double_t phi = 0., Double_t theta = 0.) const;
  Double_t  RotatedCovMatrixYY(Double_t phi = 0.) const;
  Double_t  RotatedCovMatrixXZ(Double_t phi = 0., Double_t theta = 0.) const;
  Double_t  RotatedCovMatrixYZ(Double_t phi = 0., Double_t theta = 0.) const;
  Double_t  RotatedCovMatrixZZ(Double_t phi = 0., Double_t theta = 0.) const;

  template <class T, class P> void     PhiAndThetaToVertex(AliAODVertex *vtx, P &phi, T &theta) const;
  Double_t  DistanceToVertex(AliAODVertex *vtx) const;
  Double_t  ErrorDistanceToVertex(AliAODVertex *vtx) const;
  Double_t  DistanceXYToVertex(AliAODVertex *vtx) const;
  Double_t  ErrorDistanceXYToVertex(AliAODVertex *vtx) const;

  void     PrintIndices() const;
  void     Print(Option_t* option = "") const;

 private :

  Double32_t    fPosition[3]; // vertex position
  Double32_t    fChi2;        // chi2 of vertex fit
  AliAODRedCov<3> *fCovMatrix;   // vertex covariance matrix; values of and below the diagonal
  TRef          fParent;      // reference to the parent particle
  TRefArray     fDaughters;   // references to the daughter particles
  Char_t        fType;        // Vertex type

  ClassDef(AliAODVertex,1);
};

#endif
