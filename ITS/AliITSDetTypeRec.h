#ifndef ALIITSDETTYPEREC_H
#define ALIITSDETTYPEREC_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*
$Id$ 
*/

////////////////////////////////////////////////////////////////////////
// This class contains all of the "external" information needed to do //
// detector specific reconstruction for the ITS.                      //
////////////////////////////////////////////////////////////////////////
#include <TObject.h>
#include <TObjArray.h>
#include <TClonesArray.h>
#include <TBits.h>
#include "AliITSDDLModuleMapSDD.h"
#include "AliITSFastOrCalibrationSPD.h"
#include "AliITSresponseSDD.h"
#include "AliITSgeom.h"
class TTree;
class TBranch;

//#include "AliITSLoader.h"
//#include "AliRunLoader.h"

class AliITSsegmentation;
class AliITSCalibration;
class AliITSCalibrationSSD;
class AliITSresponseSDD;
class AliITSClusterFinder;
class AliITSRecPoint;
class AliRawReader;
class AliITSGainSSDv2;
class AliITSBadChannelsSSDv2;
class AliITSNoiseSSDv2;

class AliITSDetTypeRec : public TObject {
  public:
    AliITSDetTypeRec(); // Default constructor
 
    virtual ~AliITSDetTypeRec(); // Proper Destructor

    virtual AliITSgeom* GetITSgeom() const { return fITSgeom; }
    virtual void SetITSgeom(AliITSgeom *geom) { fITSgeom = geom; }
    virtual void SetDefaults();
    virtual void SetDefaultClusterFindersV2(Bool_t rawdata=kFALSE);
    virtual void MakeBranch(TTree *tree,Option_t *opt);
    virtual void SetTreeAddressD(TTree* treeD);

    virtual void SetSegmentationModel(Int_t dettype, AliITSsegmentation *seg);
    virtual void SetCalibrationModel(Int_t iMod, AliITSCalibration *cal);
    virtual void SetSPDDeadModel(Int_t iMod, AliITSCalibration *cal);
    virtual void SetReconstructionModel(Int_t dettype, AliITSClusterFinder *rec);
    virtual Bool_t GetCalibration();
    virtual Bool_t GetCalibrationSPD(Bool_t cacheStatus);
    virtual Bool_t GetCalibrationSDD(Bool_t cacheStatus);
    virtual Bool_t GetCalibrationSSD(Bool_t cacheStatus);
    virtual AliITSFastOrCalibrationSPD* GetFastOrCalibrationSPD() const { return fSPDFastOr;}
    virtual AliITSsegmentation* GetSegmentationModel(Int_t dettype);
    virtual AliITSCalibration* GetCalibrationModel(Int_t iMod);
    virtual AliITSCalibration* GetSPDDeadModel(Int_t iMod);
    virtual AliITSClusterFinder* GetReconstructionModel(Int_t dettype);
    virtual AliITSDDLModuleMapSDD* GetDDLModuleMapSDD() const { return fDDLMapSDD;}
    virtual AliITSresponseSDD* GetResponseSDD() const { return fRespSDD;}
    virtual Bool_t IsHLTmodeC() const {return fIsHLTmodeC;}
    virtual void SetHLTmodeC(Bool_t ishltc){fIsHLTmodeC=ishltc;}
    virtual Float_t GetAverageGainSDD() const {
      if(fAveGainSDD>0.) return fAveGainSDD;
      else return 1.;
    }

    virtual void SetDigitClassName(Int_t i,const Char_t *digit) 
      {fDigClassName[i]=digit;}
    
    virtual void SetLoadOnlySPDCalib(Bool_t opt=kFALSE)
      {fLoadOnlySPDCalib=opt;}

    const Char_t* GetDigitClassName(Int_t i) const {return fDigClassName[i];}
    
    TObjArray* GetDigits() const {return fDigits;} 
    Int_t *Ndtype() {return fNdtype;}
    TClonesArray *DigitsAddress(Int_t id) const {return ((TClonesArray*)(*fDigits)[id]);}
    virtual void SelectVertexer(TString sel=" "){fSelectedVertexer = sel;}
    //
    virtual void ResetClusters(); 
    virtual void ResetClusters(Int_t branch);
    TBranch* MakeBranchInTree(TTree *tree, const char* name, const char *classname, void* address,Int_t size, Int_t splitlevel);

    TObjArray    *Ctype()  {return fCtype;}
    Int_t        *Nctype() {return fNctype;}
    TClonesArray *ClustersAddress(Int_t id) const {return ((TClonesArray*)(*fCtype)[id]);}
    virtual void ResetDigits();
    virtual void ResetDigits(Int_t branch);

    void MakeBranchR(TTree *treeR,Option_t *opt=" ");
    void SetTreeAddressR(TTree *treeR);
    void AddRecPoint(const AliITSRecPoint &p);
    void ResetRecPoints(){if(fRecPoints) fRecPoints->Clear();fNRecPoints = 0;};
    // Return pointer to rec points 
    TClonesArray  *RecPoints()   {return fRecPoints;}
    void MakeBranchRF(TTree *treeR){MakeBranchR(treeR,"Fast");}
    void DigitsToRecPoints(TTree *treeD,TTree *treeR,Int_t lastEntry,Option_t *det, Int_t optCluFind=0);
    void DigitsToRecPoints(AliRawReader* rawReader,TTree *treeR,Option_t *det="All");

    void   SetFastOrFiredMap(UInt_t chipKey){fFastOrFiredMap.SetBitNumber(chipKey);} 
    TBits  GetFastOrFiredMap() const {return fFastOrFiredMap;}
    void   ResetFastOrFiredMap(){fFastOrFiredMap.ResetAllBits();}

  private:
    // private methods
    AliITSDetTypeRec(const AliITSDetTypeRec& rec);
    AliITSDetTypeRec& operator=(const AliITSDetTypeRec &source);
 
    //conversion of the old SSD calibration objects tothe new ones
    void ReadOldSSDNoise(TObjArray *array, 
			 AliITSNoiseSSDv2 *noiseSSD);
    void ReadOldSSDBadChannels(TObjArray *array, 
			       AliITSBadChannelsSSDv2 *badChannelsSSD);
    void ReadOldSSDGain(TObjArray *array, 
			AliITSGainSSDv2 *gainSSD);

    //    virtual void SetLoader(AliITSLoader* loader) {fLoader=loader;}
    static const Int_t fgkNdettypes;          // number of det. types
    static const Int_t fgkDefaultNModulesSPD; // Total numbers of SPD modules by default
    static const Int_t fgkDefaultNModulesSDD; // Total numbers of SDD modules by default
    static const Int_t fgkDefaultNModulesSSD; // Total numbers of SSD modules by default
    Int_t *fNMod;     // numbers of modules from different types

    AliITSgeom   *fITSgeom;       //! ITS geometry

    TObjArray    *fReconstruction;//! [NDet]
    TObjArray    *fSegmentation;  //! [NDet]
    TObjArray    *fCalibration;   //! [NMod]
    AliITSCalibrationSSD* fSSDCalibration;  //! SSD calibration object
    TObjArray    *fSPDDead;       //! [fgkDefaultNModulesSPD]
    AliITSFastOrCalibrationSPD *fSPDFastOr; // Map of FastOr configured chips
    TObjArray    *fPreProcess;    //! [] e.g. Find Calibration values
    TObjArray    *fPostProcess;   //! [] e.g. find primary vertex
    TObjArray    *fDigits;        //! [NMod][NDigits]
    AliITSDDLModuleMapSDD *fDDLMapSDD; //! mapping DDL/module -> SDD module number
    AliITSresponseSDD *fRespSDD;  //! SDD response parameters 
    Float_t       fAveGainSDD;    //! Average gain of SDD good anodes
    Bool_t        fIsHLTmodeC;    //! flag for HLT mode C status (used by SDD)
    Int_t        *fNdtype;        //! detector types  
    const Char_t*       fDigClassName[3];     //! String with digit class name.

    TObjArray    *fCtype;      //! List of clusters
    Int_t        *fNctype;     //[fNDetTypes] Num. of clust. per type of det.

    TClonesArray *fRecPoints;  //! List of reconstructed points
    Int_t         fNRecPoints; // Number of rec points

    TString fSelectedVertexer; // Vertexer selected in CreateVertexer
    Bool_t fFirstcall;         //! flag
    Bool_t fLoadOnlySPDCalib;  //! flag for loading calibrations only fr SPD

    TBits fFastOrFiredMap;     // Map of FastOr fired chips

    ClassDef(AliITSDetTypeRec,16) // ITS Reconstruction structure
};

#endif
