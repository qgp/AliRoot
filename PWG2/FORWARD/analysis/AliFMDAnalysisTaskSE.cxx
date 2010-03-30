#include "AliFMDAnalysisTaskSE.h"
#include "AliESDEvent.h"
#include "iostream"
#include "AliESDFMD.h"
#include "AliMCEventHandler.h"
#include "AliAnalysisManager.h"
#include "AliFMDAnaParameters.h"
#include "AliLog.h"

ClassImp(AliFMDAnalysisTaskSE)

//_____________________________________________________________________
AliFMDAnalysisTaskSE::AliFMDAnalysisTaskSE():
AliAnalysisTaskSE(),
  fListOfHistos(0),
  fSharing("Sharing",kFALSE),
  fDensity("Density",kFALSE),
  fBackground("BackgroundCorrected",kFALSE),
  fDndeta("dNdeta",kFALSE), 
  fParams(0)
{
  // Default constructor
}
//_____________________________________________________________________
AliFMDAnalysisTaskSE::AliFMDAnalysisTaskSE(const char* name):
  AliAnalysisTaskSE(name),
  fListOfHistos(0),
  fSharing("Sharing",kFALSE),
  fDensity("Density",kFALSE),
  fBackground("BackgroundCorrected",kFALSE),
  fDndeta("dNdeta",kFALSE), 
  fParams(0)
{
  SetParams(AliFMDAnaParameters::Instance());
  DefineOutput(1, TList::Class());
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSE::UserCreateOutputObjects()
{
// Create the output containers
//
  fListOfHistos = new TList();
  
  AliESDFMD* fmd = new AliESDFMD();
  AliESDVertex* vertex = new AliESDVertex();
  
  TList* densitylist = new TList();
  
  TList* bgcorlist = new TList();
  
  fSharing.SetFMDData(fmd);
  fSharing.SetVertex(vertex);
  fSharing.SetOutputList(fListOfHistos);

  fDensity.SetOutputList(densitylist);
  fDensity.SetInputESDFMD(fmd) ;
  fDensity.SetInputVertex(vertex);
  
  fBackground.SetInputList(densitylist);
  fBackground.SetOutputList(bgcorlist);
  fBackground.SetHitList(fListOfHistos);

  fDndeta.SetInputList(bgcorlist); 
  fDndeta.SetOutputList(fListOfHistos); 
  
  fSharing.CreateOutputObjects();
  fDensity.CreateOutputObjects();
  fBackground.CreateOutputObjects();
  fDndeta.CreateOutputObjects();
  
  
  
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSE::Init()
{
  std::cout<<"Init"<<std::endl;
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSE::UserExec(Option_t */*option*/)
{
  // Execute analysis for current event
  //
  //  AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
    PostData(1, fListOfHistos);
  AliESDEvent* fESD = (AliESDEvent*)InputEvent();
  fSharing.SetInputESD(fESD);
  
  fSharing.Exec("");
  if(fSharing.GetEventStatus()) {
    fDensity.Exec("");
    if(fDensity.GetEventStatus()) {
      fBackground.Exec("");  
      fDndeta.Exec("");
      
    }
  }
  else
    return;
  
  //fListOfHistos = fBackground.GetOutputList();
  
 
}
//_____________________________________________________________________
void AliFMDAnalysisTaskSE::Terminate(Option_t */*option*/)
{
  fBackground.Terminate("");
  fDndeta.Terminate("");

}

//_____________________________________________________________________
void AliFMDAnalysisTaskSE::Print(Option_t* option) const
{
  AliInfo(Form("FMD Single Event Analysis Task\n"
	       "Parameters set to %p", fParams));
  TString opt(option);
  opt.ToLower();
  if (opt.Contains("s")) { 
    fSharing.Print(option);     
    fDensity.Print(option);     
    fBackground.Print(option);  
    fDndeta.Print(option);      
  }
  if (opt.Contains("p") && fParams) 
    fParams->Print(option);      
}

//_____________________________________________________________________
//
// EOF
//
