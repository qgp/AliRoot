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
// Static function member which can be used in standalone cases
// especially as utils for AliTPCCosmicTrackfit
//
// detailed description can be found inside individual function
//
// grep "exitreport" in output log to check abnormal termination
//
//  Xianguo Lu 
//  lu@physi.uni-heidelberg.de
//  Xianguo.Lu@cern.ch
//

#ifndef ALITPCCOSMICUTILS_H
#define ALITPCCOSMICUTILS_H

class TVector3;

class AliESDfriend;

class AliTPCCosmicUtils
{
 public:
  enum{
    k0OB0=1,
    k0OB1=2,
    k0HWU=4,
    kTRDCO2=8,
    kAMU=16,
    kSCO=32
  };
  
  static Double_t GetMinPhi(const AliExternalTrackParam *params[]);
  static Int_t GetBField(const AliESDEvent *esd);
  static Int_t GetTrigger(const AliESDEvent *esd);
  static AliTPCseed * GetTPCseed(const AliESDtrack *esdtrack);
  static Bool_t GetESD(AliESDEvent *& esdevent, AliESDfriend *& esdfriend);
  //===========================

  static AliExternalTrackParam *MakeSeed(const AliTPCseed *tseed);

  static void SingleFit(AliExternalTrackParam * trackInOld, AliExternalTrackParam * trackOutOld, const AliTPCseed *tseed, const Bool_t kinward, const Int_t rowstartshift, const Int_t rowstep, const Double_t xmin, const Double_t xmax, Int_t &nfit, Int_t &nmiss, Double_t &pchi2, Double_t &lfit, TTreeSRedirector *debugstreamer=0x0);

  static void CombinedFit(AliExternalTrackParam *trackPars[],  const AliTPCseed *seeds[],  const Int_t rowstartshift, const Int_t rowstep, const Double_t xmin, const Double_t xmax, Int_t &nfit, Int_t &nmiss, Double_t &pchi2, Double_t &lfit, Double_t &vtxD, Double_t &vtxZ, TTreeSRedirector *debugstreamer=0x0);

  static void DrawTracks(AliESDtrack *esdtrks[], const TString tag, const TString outputformat="png");
  static void DrawSeeds(const AliTPCseed * seeds[], const TString tag, const TString outputformat="png");
  static void PrintTrackParam(const Int_t id, const AliExternalTrackParam * trackpar, const char *tag="");
  static Bool_t RotateSafe(AliExternalTrackParam *trackPar, const Double_t aa);
  static Double_t AngleInRange(Double_t phi);
  static Double_t Point2LineDist(const TVector3 p0, const TVector3 l1, const TVector3 l2, TVector3 *vtx = 0x0);

  static Int_t NRow(){ return 159;}           //number of pad rows
  static Int_t NclsMin(){ return 40;}         //minimum requirement of number of TPC cluster
  static Double_t Mass(){ return 0.105658;}   //muon mass

  //---------------------------

 private:

  static Int_t XMin(){ return 80;}            //minimum x (tracking system) to use in propagation
  static Int_t Niter(){ return 2;}            //number of iteration in SingleFit and CombinedFit, 1 is not enough, 3 is the same as 2

  static void IniCov(AliExternalTrackParam *trackPar, const Double_t ncl);
  static void SubCombined(AliExternalTrackParam *trackPar, const AliTPCseed *seeds[], const Int_t tk0, const Int_t tk1, const Int_t rowstartshift, const Int_t rowstep, const Double_t xmin, const Double_t xmax, const Double_t eloss, Int_t &nfit, Int_t &nmiss, Double_t &pchi2, Double_t &lfit, Double_t &vtxD, Double_t &vtxZ, TTreeSRedirector *debugstreamer=0x0);

  static void FitKernel(AliExternalTrackParam *trackPar, const AliTPCseed *tseed, const Int_t rowstart, const Int_t rowstop, const Int_t drow, const Double_t xmin, const Double_t xmax, const Double_t eloss, Int_t &ksite, Int_t &nfit, Int_t &nmiss, Double_t &pchi2, TVector3 &gposStart, TVector3 &gposStop, TTreeSRedirector *debugstreamer, const Bool_t kinicov);
};

#endif
