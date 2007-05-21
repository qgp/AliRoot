#ifndef ALIPMDCLUSTERINGV1_H
#define ALIPMDCLUSTERINGV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
//-----------------------------------------------------//
//                                                     //
//  Header File : PMDClusteringV1.h, Version 00        //
//                                                     //
//  Date   : September 26 2002                         //
//                                                     //
//  clustering code for alice pmd                      //
//                                                     //
//-----------------------------------------------------//
// -- Author   : S.C. Phatak
// -- Modified : B.K. Nandi, Ajay Dash
//               S. Chattopadhyay
//
#include "Rtypes.h"
#include "AliPMDClustering.h"

class TNtuple;
class TObjArray;
class AliPMDcluster;
class AliPMDcludata;
class AliPMDClusteringV1: public AliPMDClustering
{
 public:
  AliPMDClusteringV1();
  AliPMDClusteringV1(const AliPMDClusteringV1 &pmdclv1);
  AliPMDClusteringV1 &operator=(const AliPMDClusteringV1 &pmdclv1);
  virtual ~AliPMDClusteringV1();

  void     DoClust(Int_t idet, Int_t ismn, Double_t celladc[][96],
		   TObjArray *pmdcont);
  Int_t    CrClust(Double_t ave, Double_t cutoff, Int_t nmx1,
		   Int_t iord1[], Double_t edepcell[]);
  void     RefClust(Int_t incr, Double_t edepcell[]);
  void     GaussFit(Int_t ncell, Int_t nclust, Double_t &x,
		    Double_t &y, Double_t &z, Double_t &xc,
		    Double_t &yc, Double_t &zc, Double_t &rc);
  Double_t Distance(Double_t x1, Double_t y1,
		    Double_t x2, Double_t y2);
  void     SetEdepCut(Float_t decut);
  
 protected:
  
  TObjArray *fPMDclucont;    // carry cluster informations
  
  static const Double_t fgkSqroot3by2;  // fgkSqroot3by2 = sqrt(3.)/2.
  
  enum {
    kNMX    = 11424,     // no. of cells in a module
    kNDIMX  = 119,       // max no. of cells along x direction
    kNDIMY  = 96         // max no. of cells along axis at 60 deg with x axis
  };

  //Variables for association
  Int_t    fCellTrNo[kNDIMX][kNDIMY];  // id x-y value of cells
  Int_t    fInfocl[2][kNDIMX][kNDIMY]; // cellwise information on the 
                                       // cluster to which the cell
  Int_t    fInfcl[3][kNMX];            // cluster information [0][i]
                                       // -- cluster number
  Double_t fCoord[2][kNDIMX][kNDIMY];

  Float_t fCutoff; // Energy(ADC) cutoff per cell before clustering

  ClassDef(AliPMDClusteringV1,4) // Does clustering for PMD
};
#endif
