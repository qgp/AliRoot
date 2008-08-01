#ifndef ALIITSDETTYPESIM_H
#define ALIITSDETTYPESIM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*
$Id$ 
*/

/////////////////////////////////////////////////////////////////////////
// * This class contains all of the "external" information needed to do//
// * detector specific simulations for the ITS.                        //
/////////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include "AliITSCalibration.h"
#include "AliITSSimuParam.h"
#include "AliITSDDLModuleMapSDD.h"
#include "AliITSLoader.h"
#include "AliITSgeom.h"

class TTree;
class AliCDBMetaData;
class AliITSdigit;
class AliITSmodule;
class AliITSpListItem;
class AliITSsimulation;
class AliITSsegmentation;
class AliITSresponse;
class AliITSCalibrationSSD;
class AliITSGainSSDv2;
class AliITSBadChannelsSSDv2;
class AliITSNoiseSSDv2;

class AliITSDetTypeSim : public TObject {
 public:
  
    AliITSDetTypeSim();
    virtual ~AliITSDetTypeSim(); 
    AliITSDetTypeSim(const AliITSDetTypeSim &source);
    AliITSDetTypeSim& operator=(const AliITSDetTypeSim &source);
    AliITSgeom *GetITSgeom() const {
        if(fLoader)return ((AliITSLoader*)fLoader)->GetITSgeom();
	else return 0;}
    void SetITSgeom(AliITSgeom *geom);
    
    virtual void SetSimulationModel(Int_t dettype,AliITSsimulation *sim);
    virtual AliITSsimulation* GetSimulationModel(Int_t dettype);        
    virtual AliITSsimulation* GetSimulationModelByModule(Int_t module);
        
    virtual void SetSegmentationModel(Int_t dettype,AliITSsegmentation *seg);
    virtual AliITSsegmentation* GetSegmentationModel(Int_t dettype);
    virtual AliITSsegmentation* GetSegmentationModelByModule(Int_t module);

    virtual void SetCalibrationModel(Int_t iMod,AliITSCalibration *resp);
    virtual AliITSCalibration* GetCalibrationModel(Int_t iMod);
    virtual AliITSresponse* GetResponse(Int_t dettype) {
	return GetCalibrationModel(
	    GetITSgeom()->GetStartDet(dettype))->GetResponse();}

    virtual void SetSimuParam(AliITSSimuParam* spar){
      if(fSimuPar) delete fSimuPar;
      fSimuPar = new AliITSSimuParam(*spar);
    }
    virtual AliITSSimuParam* GetSimuParam() const {return fSimuPar;}

    virtual AliITSDDLModuleMapSDD* GetDDLModuleMapSDD()const { return fDDLMapSDD;}
    TObjArray* GetCalibrationArray() const {return fCalibration;}
    TObjArray* GetSegmentation() const {return fSegmentation;}
    void ResetCalibrationArray();
    void ResetSegmentation();

    virtual void SetLoader(AliITSLoader* loader);
    AliITSLoader* GetLoader() const {return fLoader;}

    virtual void SetDefaults();
    virtual void SetDefaultSimulation();
    virtual void SetRunNumber(Int_t rn=0){fRunNumber = rn;}
    virtual Int_t GetRunNumber() const {return fRunNumber;}
    virtual void SetTreeAddressS(TTree* treeS, Char_t* name);
    virtual void SetTreeAddressD(TTree* treeD, Char_t* name);

    virtual void SetDigits(TObjArray* digits) {fDigits=digits;}
    const TClonesArray* GetSDigits() const { return &fSDigits;}
    TObjArray*    GetDigits() const {return fDigits;}
    Int_t* GetNDigitArray() const {return fNDigits;}
    TClonesArray *DigitsAddress(Int_t id) const {
	return ((TClonesArray*)(*fDigits)[id]);}
    virtual void ResetSDigits(){fNSDigits=0; fSDigits.Clear();}
    virtual void ResetDigits();
    virtual void ResetDigits(Int_t branch);
    virtual void SDigitsToDigits(Option_t *opt,Char_t* name);

    virtual void AddSumDigit(AliITSpListItem &sdig);
    virtual void AddSimDigit(Int_t branch, AliITSdigit *d);
    virtual void AddSimDigit(Int_t branch,Float_t phys,Int_t* digits,
			     Int_t* tracks,Int_t *hits,Float_t* trkcharges,
			     Int_t sigexpanded=-1000);
    virtual void SetDigitClassName(Int_t i, Char_t* name) {
	fDigClassName[i]=name;}
    Char_t* GetDigitClassName(Int_t i) const {return fDigClassName[i];}
    void StoreCalibration(Int_t firstRun, Int_t lastRun, AliCDBMetaData &md);

 protected:
    virtual void CreateCalibrationArray(); 
    virtual Bool_t GetCalibration();
    
 private:
    void SetDefaultSegmentation(Int_t idet);  // creates def segm.

    //conversion of the old SSD calibration objects tothe new ones
    void ReadOldSSDNoise(TObjArray *array, 
			 AliITSNoiseSSDv2 *noiseSSD);
    void ReadOldSSDBadChannels(TObjArray *array, 
			       AliITSBadChannelsSSDv2 *badChannelsSSD);
    void ReadOldSSDGain(TObjArray *array, 
			AliITSGainSSDv2 *gainSSD);

    static const Int_t fgkNdettypes;          // number of different det. types
    static const Int_t fgkDefaultNModulesSPD; // Total numbers of SPD modules by default
    static const Int_t fgkDefaultNModulesSDD; // Total numbers of SDD modules by default
    static const Int_t fgkDefaultNModulesSSD; // Total numbers of SSD modules by default
    Int_t fNMod[3];                           // numbers of modules from different types

    TObjArray    *fSimulation;   //! [NDet]
    TObjArray    *fSegmentation; //! [NDet]
    TObjArray    *fCalibration;  //! [NMod]
    AliITSCalibrationSSD* fSSDCalibration;  //! SSD calibration object
    TObjArray    *fPreProcess;   //! [] e.g. Fill fHitModule with hits
    TObjArray    *fPostProcess;  //! [] e.g. Wright Raw data
    Int_t         fNSDigits;     //! number of SDigits
    TClonesArray  fSDigits;      //! Summable digits
    Int_t*        fNDigits;      //! [NDet] number of Digits for det.
    Int_t         fRunNumber;    //! run number (to access DB)
    TObjArray     *fDigits;      //! [NMod][NDigits]
    AliITSSimuParam *fSimuPar;   //! detector simulation parameters
    AliITSDDLModuleMapSDD *fDDLMapSDD; //! mapping DDL/module -> SDD module number
    TString       fHitClassName; //! String with Hit class name
    TString       fSDigClassName;//! String with SDigit class name.
    Char_t*       fDigClassName[3]; //! String with digit class name.
    AliITSLoader* fLoader;          //! loader  
    Bool_t        fFirstcall;       //! flag
    
    ClassDef(AliITSDetTypeSim,7) // ITS Simulation structure
 
};

#endif
