// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#ifndef ALIEVE_VSDCreator_H
#define ALIEVE_VSDCreator_H

#include <TEveVSD.h>

class AliTPCParam;
class AliRunLoader;

#include <map>


class AliEveVSDCreator : public TEveVSD
{
private:
  AliEveVSDCreator(const AliEveVSDCreator&);            // Not implemented
  AliEveVSDCreator& operator=(const AliEveVSDCreator&); // Not implemented

protected:
  void               MakeItsDigitsInfo();
  TEveMCRecCrossRef* GetGeninfo(Int_t label);
  AliTPCParam*       GetTpcParam(const TEveException& eh);

  TString       fDataDir;    // Source data directory.
  Int_t         fEvent;      // Source event number.

  Float_t       fTPCHitRes;  // Resolution for storing TPC hits.
  Float_t       fTRDHitRes;  // Resolution for storing TRD hits.

  Int_t         fDebugLevel; // Internal debug level.

  AliRunLoader *fRunLoader;  // Internal run-loader.

  std::map<Int_t, TEveMCRecCrossRef*> fGenInfoMap; // Map label to MC-Rec cross-ref data structure.

public:
  AliEveVSDCreator(const Text_t* name="AliEveVSDCreator", const Text_t* title="");
  virtual ~AliEveVSDCreator() {}

  void CreateVSD(const Text_t* dataDir, Int_t event, const Text_t* vsdFile);

  void CreateTrees();

  // --------------------------------------------------------------
  // Conversion functions.

  void ConvertKinematics();
  void ConvertHits();
  void ConvertClusters();
  void ConvertTPCClusters();
  void ConvertITSClusters();
  void ConvertV0();
  void ConvertKinks();
  void ConvertRecTracks();
  void ConvertGenInfo();

  // --------------------------------------------------------------

  Int_t GetDebugLevel() const   { return fDebugLevel; }
  void  SetDebugLevel(Int_t dl) { fDebugLevel = dl; }

  ClassDef(AliEveVSDCreator, 0); // Create VSD file from ALICE data.
}; // endclass AliEveVSDCreator

#endif
