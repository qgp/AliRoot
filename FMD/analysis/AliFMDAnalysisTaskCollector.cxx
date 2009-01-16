 
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <TList.h>
#include <iostream>

#include "AliFMDAnalysisTaskCollector.h"
#include "AliAnalysisManager.h"
#include "AliESDFMD.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"
#include "AliMCEventHandler.h"
#include "AliStack.h"
#include "AliESDVertex.h"
#include "AliFMDAnaParameters.h"

ClassImp(AliFMDAnalysisTaskCollector)


AliFMDAnalysisTaskCollector::AliFMDAnalysisTaskCollector()
: fDebug(0),
  fChain(0x0),
  fESD(0x0),
  fAOD(0x0),
  fOutputList(0),
  fArray(0),
  fEdistHist(0),
  fZvtxDist(0)
{
  // Default constructor
  DefineInput (0, TChain::Class());
  DefineOutput(0, TList::Class());
}
//____________________________________________________________________
AliFMDAnalysisTaskCollector::AliFMDAnalysisTaskCollector(const char* name):
    AliAnalysisTask(name, "AnalysisTaskFMD"),
    fDebug(0),
    fChain(0x0),
    fESD(0x0),
    fAOD(0x0),
    fOutputList(0),
    fArray(0),
    fEdistHist(0),
    fZvtxDist(0)
{
  // Default constructor
  DefineInput (0, TChain::Class());
  DefineOutput(0, TList::Class());
}
//____________________________________________________________________
void AliFMDAnalysisTaskCollector::CreateOutputObjects()
{
  // Create the output container
  printf("AnalysisTaskFMD::CreateOutPutData() \n");
  
  fOutputList = new TList();//(TList*)GetOutputData(0);
  
  fArray     = new TObjArray();
  fArray->SetName("FMD");
  fArray->SetOwner();
  TH1F* hEdist = 0;
  for(Int_t det =1; det<=3;det++)
    {
      TObjArray* detArray = new TObjArray();
      detArray->SetName(Form("FMD%d",det));
      fArray->AddAtAndExpand(detArray,det);
      Int_t nRings = (det==1 ? 1 : 2);
      for(Int_t ring = 0;ring<nRings;ring++)
	{
	  Char_t ringChar = (ring == 0 ? 'I' : 'O');
	  hEdist = new TH1F(Form("FMD%d%c",det,ringChar),Form("FMD%d%c",det,ringChar),100,0,3);
	  hEdist->SetXTitle("#Delta E / E_{MIP}");
	  fOutputList->Add(hEdist);
	  detArray->AddAtAndExpand(hEdist,ring);
	} 
    }
  
  
  
  fZvtxDist  = new TH1F("ZvtxDist","Vertex distribution",100,-30,30);
  fZvtxDist->SetXTitle("z vertex");
  //fOutputList->Add(fArray);
  fOutputList->Add(fZvtxDist);
}
//____________________________________________________________________
void AliFMDAnalysisTaskCollector::Init()
{
  // Initialization
  printf("AnalysisTaskFMD::Init() \n");
  

}
//____________________________________________________________________
void AliFMDAnalysisTaskCollector::ConnectInputData(Option_t */*option*/)
{
  fChain = (TChain*)GetInputData(0);
  fESD = new AliESDEvent();
  fESD->ReadFromTree(fChain);
  
}
//____________________________________________________________________
void AliFMDAnalysisTaskCollector::Exec(Option_t */*option*/)
{
  AliESD* old = fESD->GetAliESDOld();
  if (old) {
    fESD->CopyFromOldESD();
  }
  
  Long64_t ientry = fChain->GetReadEntry();
  Double_t vertex[3];
  fESD->GetVertex()->GetXYZ(vertex);
  fZvtxDist->Fill(vertex[2]);
  AliESDFMD* fmd = fESD->GetFMDData();
  if (!fmd) return;
  
  for(UShort_t det=1;det<=3;det++) {
    TObjArray* detArray = (TObjArray*)fArray->At(det);
    Int_t nRings = (det==1 ? 1 : 2);
    for (UShort_t ir = 0; ir < nRings; ir++) {
      TH1F* Edist = (TH1F*)detArray->At(ir);
      Char_t   ring = (ir == 0 ? 'I' : 'O');
      UShort_t nsec = (ir == 0 ? 20  : 40);
      UShort_t nstr = (ir == 0 ? 512 : 256);
      for(UShort_t sec =0; sec < nsec;  sec++)  {
	for(UShort_t strip = 0; strip < nstr; strip++) {
	  Float_t mult = fmd->Multiplicity(det,ring,sec,strip);
	  if(mult == AliESDFMD::kInvalidMult) continue;
	  Edist->Fill(mult);
	  
	}
      }
    }
  }
  
  PostData(0, fOutputList); 
  
}
//____________________________________________________________________
void AliFMDAnalysisTaskCollector::Terminate(Option_t */*option*/)
{
  /*
  for(UShort_t det=1;det<=3;det++) {
    TObjArray* detArray = (TObjArray*)fArray->At(det);
    Int_t nRings = (det==1 ? 1 : 2);
    for (UShort_t ir = 0; ir < nRings; ir++) {
      TH1F* hEdist = (TH1F*)detArray->At(ir);
      hEdist->SetAxisRange(0.4,hEdist->GetXaxis()->GetXmax());
      Float_t max = hEdist->GetBinCenter(hEdist->GetMaximumBin());
      hEdist->Fit("landau","","",max-0.1,2*max);
    }
  }
  */
}

//____________________________________________________________________
//
// EOF
//
