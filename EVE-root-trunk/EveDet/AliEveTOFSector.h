// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/
#ifndef AliEveTOFSector_H
#define AliEveTOFSector_H

#include <TEveQuadSet.h>
#include <TEveElement.h>

#include <TEveRGBAPalette.h>
#include <TEveFrameBox.h>

#include <TGeoManager.h>
#include <TClonesArray.h>
#include <TTree.h>

#include <AliTOFGeometry.h>

  
class AliEveTOFSector : public TEveQuadSet
{
public:
  AliEveTOFSector(const Text_t* n="AliEveTOFSector", const Text_t* t=0);
  AliEveTOFSector(TGeoManager *localGeoManager, Int_t nSector);
    
  AliEveTOFSector(TGeoManager *localGeoManager, Int_t nSector,
                  TClonesArray *tofArray);
  AliEveTOFSector(TGeoManager *localGeoManager,
                  Int_t nSector, TTree *tofTree);
  virtual ~AliEveTOFSector();

  virtual void InitModule();
  virtual void SetTrans(); 

  static Bool_t    fgStaticInitDone;
  static void      InitStatics();

  void SetSectorID(Int_t id);
  void SetAutoTrans(Bool_t r){fAutoTrans=r;};
  void SetThreshold(Short_t t);
  void SetMaxVal(Int_t mv);
  Bool_t GetPlate(Int_t nPlate) const {return fPlateFlag[nPlate];};
  Short_t GetThreshold() const {return fThreshold;};
  Int_t GetMaxVal() const {return fMaxVal;};
  Bool_t GetAutoTrans() const {return fAutoTrans;};
  Int_t GetSectorID() const {return fSectorID;};
  virtual void DigitSelected(Int_t idx);
  ///////////////////////////////////////////

  void SetPlate(Int_t nPlate, Bool_t r);

  static TEveFrameBox    *fgTOFsectorFrameBox;
  static TEveRGBAPalette *fgTOFsectorPalette;
 
protected:
  AliTOFGeometry *fTOFgeometry;

  TClonesArray   *fTOFarray;
  TTree          *fTOFtree;

  Int_t fSector;
  //Int_t fPlate;
  //Int_t fStrip;

  Float_t  fDx;
  Float_t  fDy;
  Float_t  fDz;
  ///////////////////////////////

  Bool_t      fAutoTrans; 
  //Int_t       fMinTime;     
  //Int_t       fMaxTime;
  Short_t     fThreshold;
  Int_t       fMaxVal;
  Int_t       fSectorID;
  Bool_t     *fPlateFlag;

  //Bool_t      fPlateFlag0;
  //Bool_t      fPlateFlag1;
  //Bool_t      fPlateFlag2;
  //Bool_t      fPlateFlag3;
  //Bool_t      fPlateFlag4;

  //Color_t     fFrameColor;
  //Bool_t      fRnrFrame;

  TGeoManager *fGeoManager;

private:
  void LoadQuads();

  AliEveTOFSector(const AliEveTOFSector&);            // Not implemented
  AliEveTOFSector& operator=(const AliEveTOFSector&); // Not implemented

  ClassDef(AliEveTOFSector, 0); // Representation of a TOF sector.
};
#endif
