/* $Id$ */

#ifndef AliMultiplicityTask_H
#define AliMultiplicityTask_H

#include "AliAnalysisTask.h"

#include <TString.h>
#include "AliPWG0Helper.h"

class AliESDtrackCuts;
class AliMultiplicityCorrection;
class TNtuple;
class AliCorrection;
class TH1;
class TH1D;
class AliESDEvent;

class AliMultiplicityTask : public AliAnalysisTask {
  public:
    AliMultiplicityTask(const char* opt = "");
    virtual ~AliMultiplicityTask();

    virtual void   ConnectInputData(Option_t *);
    virtual void   CreateOutputObjects();
    virtual void   Exec(Option_t*);
    virtual void   Terminate(Option_t *);

    void SetTrackCuts(AliESDtrackCuts* cuts) { fEsdTrackCuts = cuts; }
    void SetPtSpectrum(TH1D* hist) { fPtSpectrum = hist; }

    void SetAnalysisMode(AliPWG0Helper::AnalysisMode mode) { fAnalysisMode = mode; }
    void SetTrigger(AliTriggerAnalysis::Trigger trigger) { fTrigger = trigger; }
    void SetDeltaPhiCut(Float_t cut) { fDeltaPhiCut = cut; }

    void SetReadMC(Bool_t flag = kTRUE) { fReadMC = flag; }
    void SetUseMCVertex(Bool_t flag = kTRUE) { fUseMCVertex = flag; }

 protected:
    AliESDEvent *fESD;    //! ESD object

    TString fOption;      // option string
    AliPWG0Helper::AnalysisMode fAnalysisMode; // detector that is used for analysis
    AliTriggerAnalysis::Trigger fTrigger;      // trigger that is used
    Float_t fDeltaPhiCut;                      // cut in delta phi (only SPD)

    Bool_t  fReadMC;       // if true reads MC data (to build correlation maps)
    Bool_t  fUseMCVertex;  // the MC vtx is used instead of the ESD vertex (for syst. check)

    AliMultiplicityCorrection* fMultiplicity; //! object containing the extracted data
    AliESDtrackCuts* fEsdTrackCuts;           // Object containing the parameters of the esd track cuts

    Bool_t fSystSkipParticles;     //! if true skips particles (systematic study)
    AliCorrection* fParticleCorrection[8]; //! correction from measured to generated particles for different particles for trigger, vertex sample in |eta| < 2; switch on with "particle-efficiency"
                                           // for each of the species (0..3): pi, k, p, other; for systematic study of pt cut off
                                           // 4..7 counts for the same species the decayed particles (in generated) and stopped (in measured)
    Int_t fSelectProcessType;        //! 0 = all (default), 1 = ND, 2 = SD, 3 = DD (for systematic study)
    TNtuple *fParticleSpecies;       //! per event: vtx_mc, (pi, k, p, rest (in |eta| < 1)) X (true, recon) + (nolabel,
                                     // doubleTracks, doublePrimaries) [doubleTracks + doublePrimaries are already part of
                                     // rec. particles!); enable with: particle-species
    TH1* fdNdpT;                     //! true pT spectrum (MC)

    TH1D* fPtSpectrum;               // function that modifies the pt spectrum (syst. study)

    TList* fOutput;                  //! list send on output slot 0
    
 private:
    AliMultiplicityTask(const AliMultiplicityTask&);
    AliMultiplicityTask& operator=(const AliMultiplicityTask&);

  ClassDef(AliMultiplicityTask, 1);
};

#endif
