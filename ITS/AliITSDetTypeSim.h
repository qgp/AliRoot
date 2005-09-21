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

class AliITSdigit;
class AliITSmodule;
class AliITSpListItem;
class AliITSsimulation;
class AliITSsegmentation;
class AliITSresponse;
class AliLoader;
class AliITSgeom;

class AliITSDetTypeSim : public TObject {
 public:
  
    AliITSDetTypeSim();
    virtual ~AliITSDetTypeSim(); 
    AliITSDetTypeSim(const AliITSDetTypeSim &source);
    AliITSDetTypeSim& operator=(const AliITSDetTypeSim &source);
    AliITSgeom *GetITSgeom() const {return fGeom;}
    void SetITSgeom(AliITSgeom *geom){fGeom=geom;}
    
    virtual void SetSimulationModel(Int_t dettype,AliITSsimulation *sim);
    virtual AliITSsimulation* GetSimulationModel(Int_t dettype);        
    virtual AliITSsimulation* GetSimulationModelByModule(Int_t module);
        
    virtual void SetSegmentationModel(Int_t dettype,AliITSsegmentation *seg);
    virtual AliITSsegmentation* GetSegmentationModel(Int_t dettype);
    virtual AliITSsegmentation* GetSegmentationModelByModule(Int_t module);

    virtual void SetResponseModel(Int_t dettype,AliITSresponse *resp);
    virtual AliITSresponse* GetResponseModel(Int_t dettype);

    TObjArray* GetResponse() const {return fResponse;}
    TObjArray* GetSegmentation() const {return fSegmentation;}
    void ResetResponse();
    void ResetSegmentation();

    virtual void SetLoader(AliLoader* loader) {fLoader=loader;}
    AliLoader* GetLoader() const {return fLoader;}

    virtual void SetDefaults();
    virtual void SetDefaultSimulation();
    virtual void SetTreeAddressS(TTree* treeS, Char_t* name);
    virtual void SetTreeAddressD(TTree* treeD, Char_t* name);

    virtual void SetSDigits(TClonesArray* sdigits) {fSDigits=sdigits;}
    virtual void SetDigits(TObjArray* digits) {fDigits=digits;}
    TClonesArray* GetSDigits() const {return fSDigits;}
    TObjArray*    GetDigits() const {return fDigits;}
    Int_t* GetNDigitArray() const {return fNDigits;}
    TClonesArray *DigitsAddress(Int_t id) const { return ((TClonesArray*)(*fDigits)[id]);}

    virtual void ResetSDigits(){fNSDigits=0;if(fSDigits!=0) fSDigits->Clear();}
    virtual void ResetDigits();
    virtual void ResetDigits(Int_t branch);
    virtual void SDigitsToDigits(Option_t *opt,Char_t* name);

    virtual void AddSumDigit(AliITSpListItem &sdig);
    virtual void AddRealDigit(Int_t branch, Int_t *digits);
    virtual void AddSimDigit(Int_t branch, AliITSdigit *d);			    virtual void AddSimDigit(Int_t branch,Float_t phys,Int_t* digits,
		     Int_t* tracks,Int_t *hits,Float_t* trkcharges);

    virtual void SetDigitClassName(Int_t i, Char_t* name) {fDigClassName[i]=name;}
    Char_t* GetDigitClassName(Int_t i) const {return fDigClassName[i];}
    
 private:
    
    static const Int_t fgkNdettypes;   // number of different det. types
    AliITSgeom   *fGeom;         // pointer to ITS geom
    TObjArray    *fSimulation;   //! [NDet]
    TObjArray    *fSegmentation; //! [NDet]
    TObjArray    *fResponse;     //! [NMod]
    TObjArray    *fPreProcess;   //! [] e.g. Fill fHitModule with hits
    TObjArray    *fPostProcess;  //! [] e.g. Wright Raw data
    Int_t         fNSDigits;     //! number of SDigits
    TClonesArray *fSDigits;      //! [NMod][NSDigits]
    Int_t*        fNDigits;      //! [NDet] number of Digits for det.
    TObjArray     *fDigits;       //! [NMod][NDigits]
    TString       fHitClassName; //! String with Hit class name
    TString       fSDigClassName;//! String with SDigit class name.
    Char_t*       fDigClassName[3]; //! String with digit class name.
    AliLoader* fLoader;        // loader  
    
  ClassDef(AliITSDetTypeSim,1) // ITS Simulation structure
 
};

#endif
