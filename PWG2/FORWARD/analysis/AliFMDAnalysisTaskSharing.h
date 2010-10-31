#ifndef ALIFMDANALYSISTASKSHARING_H
#define ALIFMDANALYSISTASKSHARING_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               
 **/
 
#include "AliAnalysisTask.h"

#include "AliESDFMD.h"
//#include "TTree.h"
#include "AliESDEvent.h"
#include "AliFMDFloatMap.h"
class TChain;
class AliAODEvent;
class AliESDVertex;

/**
 * Do the sharing correction. 
 * @ingroup FMD_ana
 * 
 * 
 */
class AliFMDAnalysisTaskSharing : public AliAnalysisTask
{
 public:
    AliFMDAnalysisTaskSharing();
    AliFMDAnalysisTaskSharing(const char* name, Bool_t SE = kTRUE);
    virtual ~AliFMDAnalysisTaskSharing() {;}
 AliFMDAnalysisTaskSharing(const AliFMDAnalysisTaskSharing& o) : AliAnalysisTask(),
      fDebug(o.fDebug),
      fESD(o.fESD),
      // fOutputESD(),
      foutputESDFMD(o.foutputESDFMD),
      fSharedThis(o.fSharedThis),
      fSharedPrev(o.fSharedPrev),
      fDiagList(),
      fStandalone(o.fStandalone),
      fEsdVertex(o.fEsdVertex),
      fStatus(o.fStatus),
      fLastTrackByStrip(o.fLastTrackByStrip),
      fLastOrbit(o.fLastOrbit) {}
    AliFMDAnalysisTaskSharing& operator=(const AliFMDAnalysisTaskSharing&) { return *this; }
    
    // Implementation of interface methods
    virtual void ConnectInputData(Option_t *option = "");
    virtual void CreateOutputObjects();
    virtual void Init() {}
    virtual void LocalInit() {Init();}
    virtual void Exec(Option_t */*option*/);
    virtual void Terminate(Option_t* /* option*/);
    virtual void SetDebugLevel(Int_t level) {fDebug = level;}
    Float_t GetMultiplicityOfStrip(Float_t mult, Float_t eta, Float_t Eprev, Float_t Enext, UShort_t   det, Char_t  ring, UShort_t sec, UShort_t strip);
    // void GetVertex(Double_t* vertexXYZ) ;
    void SetFMDData(AliESDFMD* fmd) {foutputESDFMD = fmd;}
    void SetOutputList(TList* outlist) {fDiagList = outlist;}
    void SetVertex(AliESDVertex* vertex) {fEsdVertex = vertex;}
    void SetInputESD(AliESDEvent* esd) {fESD = esd;}
    Bool_t GetEventStatus() const {return fStatus;}
    Float_t GetVtxEfficiencyFromData() ;
    Float_t GetNSDVtxEfficiencyFromData() ;
    
 private:
    Float_t Eta2Theta(Float_t eta) const ;
    Double_t EtaFromStrip(UShort_t det, Char_t ring, UShort_t sector, UShort_t strip, Double_t zvtx);
    void ProcessPrimary();
    
    Int_t         fDebug;        //  Debug flag
    AliESDEvent*  fESD;          //! ESD
    // AliESDEvent   fOutputESD;
    AliESDFMD*    foutputESDFMD; // the output ESDFMD object
    // Float_t       fEnergy;       
    // Int_t         fNstrips;
    Bool_t        fSharedThis;        // was this strip shared?
    Bool_t        fSharedPrev;        // was the previous strip shared?
    TList*        fDiagList;          // list of diag histos
    Bool_t        fStandalone;        // do we run standalone or in SE task
    AliESDVertex* fEsdVertex;         // vtx info from the ESD
    Bool_t        fStatus;            // event status
    AliFMDFloatMap fLastTrackByStrip; // the last track to hit this strip
    UInt_t          fLastOrbit;
    
    ClassDef(AliFMDAnalysisTaskSharing, 0); // Analysis task for FMD analysis
};
 
#endif
// Local Variables:
//  mode: C++
// End Variables;
