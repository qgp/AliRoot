#ifndef ALIANALYSISTASKGAMMA_H
#define ALIANALYSISTASKGAMMA_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
 
#include "AliAnalysisTaskRL.h"
class AliAnaGamma;
class AliESD;
class TChain;
class TTree;
class TList;

class AliAnalysisTaskGamma : public AliAnalysisTaskRL
{
 public:
    AliAnalysisTaskGamma();
    AliAnalysisTaskGamma(const char* name);
    virtual ~AliAnalysisTaskGamma() ;// virtual dtor
 
    // Implementation of interface methods
    virtual void ConnectInputData(Option_t *option = "");
    virtual void CreateOutputObjects();
    virtual void Init();
    //virtual void LocalInit() {Init();}
    virtual void Exec(Option_t *option);
    virtual void Terminate(Option_t *option);
   
 private:

    AliAnaGamma* fAna; //  Pointer to the jet finder 
    TChain*       fChain;     //! chained files
    AliESD*       fESD;       //! ESD
    TTree*        fTreeG;     //  tree of prompt gamma 
    TList * fOutputContainer ; // Histogram container

    ClassDef(AliAnalysisTaskGamma, 0); // Analysis task for standard gamma correlation analysis
};
 
#endif //ALIANALYSISTASKGAMMA_H
