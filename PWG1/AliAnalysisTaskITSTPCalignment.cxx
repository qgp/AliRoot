////////////////////////////////////////////////////////////////////////////////
//  AliAnalysisTaskITSTPCalignment
//  Runs the relative ITS TPC alignment procedure and TPC vdrift calib
//  Origin: Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch
////////////////////////////////////////////////////////////////////////////////

#include <TChain.h>
#include <TTree.h>
#include <TH2.h>
#include <TProfile.h>
#include <TCanvas.h>
#include <TArrayI.h>
#include <TObjArray.h>
#include <TGraphErrors.h>
#include <AliMagF.h>
#include <AliAnalysisTask.h>
#include <AliMCEventHandler.h>
#include <AliAnalysisManager.h>
#include <AliESDEvent.h>
#include <AliESDfriend.h>
#include <AliMCEvent.h>
#include <AliStack.h>
#include <AliExternalTrackParam.h>
#include <AliESDtrack.h>
#include <AliESDfriendTrack.h>
#include <AliESDInputHandler.h>
#include "AliRelAlignerKalman.h"
#include "AliRelAlignerKalmanArray.h"
#include "AliAnalysisTaskITSTPCalignment.h"
#include "AliLog.h"

ClassImp(AliAnalysisTaskITSTPCalignment)

//________________________________________________________________________
AliAnalysisTaskITSTPCalignment::AliAnalysisTaskITSTPCalignment():
    AliAnalysisTask(),
    fESD(0),
    fESDfriend(0),
    fMC(0),
    fArrayITSglobal(0),
    fArrayITSsa(0),
    fDebugTree(0),
    fAligner(0),
    fList(0),
    fFillDebugTree(kFALSE),
    fDoQA(kTRUE),
    fT0(0),
    fTend(0),
    fSlotWidth(0),
    fMinPt(0.4),
    fMinPointsVol1(3),
    fMinPointsVol2(80),
    fRejectOutliers(kTRUE),
    fOutRejSigma(1.),
    fRejectOutliersSigma2Median(kFALSE),
    fOutRejSigma2Median(5.),
    fOutRejSigmaOnMerge(10.),
    fUseITSoutGlobalTrack(kFALSE),
    fUseITSoutITSSAtrack(kTRUE)
{
  //dummy ctor
  // Define input and output slots here
  // Input slot #0 works with a TChain
  //DefineInput(0, TChain::Class());
  //DefineOutput(0, TTree::Class());
  //DefineOutput(1, TList::Class());
  //DefineOutput(2, AliRelAlignerKalmanArray::Class());
}

//________________________________________________________________________
AliAnalysisTaskITSTPCalignment::AliAnalysisTaskITSTPCalignment(const char *name):
    AliAnalysisTask(name,name),
    fESD(0),
    fESDfriend(0),
    fMC(0),
    fArrayITSglobal(0),
    fArrayITSsa(0),
    fDebugTree(0),
    fAligner(0),
    fList(0),
    fFillDebugTree(kFALSE),
    fDoQA(kTRUE),
    fT0(0),
    fTend(0),
    fSlotWidth(0),
    fMinPt(0.4),
    fMinPointsVol1(3),
    fMinPointsVol2(80),
    fRejectOutliers(kTRUE),
    fOutRejSigma(1.),
    fRejectOutliersSigma2Median(kFALSE),
    fOutRejSigma2Median(5.),
    fOutRejSigmaOnMerge(10.),
    fUseITSoutGlobalTrack(kFALSE),
    fUseITSoutITSSAtrack(kTRUE)
{
  // Constructor

  // Define input and output slots here
  // Input slot #0 works with a TChain
  DefineInput(0, TChain::Class());
  DefineOutput(0, TTree::Class());
  DefineOutput(1, TList::Class());
  DefineOutput(2, AliRelAlignerKalmanArray::Class());
  DefineOutput(3, AliRelAlignerKalmanArray::Class());
}

//________________________________________________________________________
void AliAnalysisTaskITSTPCalignment::ConnectInputData(Option_t *)
{
  // Called once
  TTree* tree = dynamic_cast<TTree*> (GetInputData(0));
  if (!tree)
  {
    AliError("Could not read chain from input slot 0");
    return;
  }

  AliESDInputHandler *esdH = dynamic_cast<AliESDInputHandler*>
                             (AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler());
  if (!esdH)
  {
    AliError("Could not get ESDInputHandler");
    return;
  }
  else
    fESD = esdH->GetEvent();

  AliMCEventHandler* mcH = dynamic_cast<AliMCEventHandler*>
                           (AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler());
  if (!mcH) {
    AliWarning("Could not retrieve MC event handler");
  }
  else
    fMC = mcH->MCEvent();

  //esdH->SetFriendFileName("AliESDfriends.root");
  //fESDfriend = esdH->GetESDfriend();
  //attach the ESD friend
  tree->SetBranchStatus("ESDfriend*", 1);
  fESDfriend = (AliESDfriend*)fESD->FindListObject("AliESDfriend");
  if (!fESDfriend)
  {
    // works for both, we just want to avoid setting the branch adress twice
    // in case of the new ESD
    tree->SetBranchAddress("ESDfriend.",&fESDfriend);
  }
}

//________________________________________________________________________
Bool_t AliAnalysisTaskITSTPCalignment::Notify()
{
  //ON INPUT FILE/TREE CHANGE

  //fArray->Print();
  if (fFillDebugTree)
  {
    if (fAligner->GetNUpdates()>0) fDebugTree->Fill();
  }

  return kTRUE;
}

//________________________________________________________________________
void AliAnalysisTaskITSTPCalignment::CreateOutputObjects()
{
  // Create output objects
  // Called once
  fArrayITSglobal = new AliRelAlignerKalmanArray();
  fArrayITSglobal->SetName("outputArrayITSglobal");
  fArrayITSglobal->SetupArray(fT0,fTend,fSlotWidth);
  fArrayITSglobal->SetOutRejSigmaOnMerge(fOutRejSigmaOnMerge);
  //set up the template
  AliRelAlignerKalman* templ = fArrayITSglobal->GetAlignerTemplate();
  templ->SetRejectOutliers(fRejectOutliers);
  templ->SetOutRejSigma(fOutRejSigma);
  templ->SetRejectOutliersSigma2Median(fRejectOutliersSigma2Median);
  templ->SetOutRejSigma2Median(fOutRejSigma2Median);
  fArrayITSsa = new AliRelAlignerKalmanArray();
  fArrayITSsa->SetName("outputArrayITSsa");
  fArrayITSsa->SetupArray(fT0,fTend,fSlotWidth);
  fArrayITSsa->SetOutRejSigmaOnMerge(fOutRejSigmaOnMerge);
  //set up the template
  templ = fArrayITSsa->GetAlignerTemplate();
  templ->SetRejectOutliers(fRejectOutliers);
  templ->SetOutRejSigma(fOutRejSigma);
  templ->SetRejectOutliersSigma2Median(fRejectOutliersSigma2Median);
  templ->SetOutRejSigma2Median(fOutRejSigma2Median);

  fList = new TList();

  TH2F* pZYAResidualsHistBpos = new TH2F("fZYAResidualsHistBpos","z-r\\phi residuals side A (z>0), Bpos", 100, -4., 4., 100, -2.0, 2.0 );
  pZYAResidualsHistBpos->SetXTitle("\\deltaz [cm]");
  pZYAResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pZYCResidualsHistBpos = new TH2F("fZYCResidualsHistBpos","z-r\\phi residuals side C (z<0), Bpos", 100, -4., 4., 100, -2.0, 2.0 );
  pZYCResidualsHistBpos->SetXTitle("\\deltaz [cm]");
  pZYCResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLPAResidualsHistBpos = new TH2F("fLPAResidualsHistBpos", "sin(\\phi) tan(\\lambda) residuals side A (z>0), Bpos", 100, -.07, 0.07, 100, -0.07, 0.07 );
  pLPAResidualsHistBpos->SetXTitle("\\deltasin(\\phi)");
  pLPAResidualsHistBpos->SetYTitle("\\deltatan(\\lambda)");
  TH2F* pLPCResidualsHistBpos = new TH2F("fLPCResidualsHistBpos", "sin(\\phi) tan(\\lambda) residuals side C (z<0), Bpos", 100, -.07, 0.07, 100, -0.07, 0.07 );
  pLPCResidualsHistBpos->SetXTitle("\\deltasin(\\phi)");
  pLPCResidualsHistBpos->SetYTitle("\\deltatan(\\lambda)");
  TH2F* pPhiYAResidualsHistBpos = new TH2F("fPhiYAResidualsHistBpos","\\phi-\\deltar\\phi side A (z>0), Bpos",36,0.0,6.3,100,-2.0,2.0);
  pPhiYAResidualsHistBpos->SetXTitle("\\phi [rad]");
  pPhiYAResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPhiYCResidualsHistBpos = new TH2F("fPhiYCResidualsHistBpos","\\phi-\\deltar\\phi side C (z<0), Bpos",36,0.0,6.3,100,-2.0,2.0);
  pPhiYCResidualsHistBpos->SetXTitle("\\phi [rad]");
  pPhiYCResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPhiZAResidualsHistBpos = new TH2F("fPhiZAResidualsHistBpos","\\phi-\\deltaz side A (z>0), Bpos",36,0.0,6.3,100,-4.0,4.0);
  pPhiZAResidualsHistBpos->SetXTitle("\\phi [rad]");
  pPhiZAResidualsHistBpos->SetYTitle("\\deltaz [cm]");
  TH2F* pPhiZCResidualsHistBpos = new TH2F("fPhiZCResidualsHistBpos","\\phi-\\deltaz side C (z<0), Bpos",36,0.0,6.3,100,-4.0,4.0);
  pPhiZCResidualsHistBpos->SetXTitle("\\phi [rad]");
  pPhiZCResidualsHistBpos->SetYTitle("\\deltaz [cm]");
  TH2F* pPtYAResidualsHistBpos = new TH2F("fPtYAResidualsHistBpos","Pt-\\deltar\\phi side A (z>0), Bpos",20,.3,8.,80,-2.0,2.0);
  pPtYAResidualsHistBpos->SetXTitle("Pt [rad]");
  pPtYAResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPtYCResidualsHistBpos = new TH2F("fPtYCResidualsHistBpos","Pt-\\deltar\\phi side C (z<0), Bpos",20,.3,8.,80,-2.0,2.0);
  pPtYCResidualsHistBpos->SetXTitle("Pt [rad]");
  pPtYCResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPtZAResidualsHistBpos = new TH2F("fPtZAResidualsHistBpos","Pt-\\deltaz side A (z>0), Bpos",20,.3,8.,80,-4.0,4.0);
  pPtZAResidualsHistBpos->SetXTitle("Pt");
  pPtZAResidualsHistBpos->SetYTitle("\\deltaz [cm]");
  TH2F* pPtZCResidualsHistBpos = new TH2F("fPtZCResidualsHistBpos","Pt-\\deltaz side C (z<0), Bpos",20,.3,8.,80,-4.0,4.0);
  pPtZCResidualsHistBpos->SetXTitle("Pt");
  pPtZCResidualsHistBpos->SetYTitle("\\deltaz [cm]");
  TH2F* pLowPtYAResidualsHistBpos = new TH2F("fLowPtYAResidualsHistBpos","Pt-\\deltar\\phi side A (z>0), Bpos",100,0.0,1.5,80,-2.0,2.0);
  pLowPtYAResidualsHistBpos->SetXTitle("Pt [rad]");
  pLowPtYAResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLowPtYCResidualsHistBpos = new TH2F("fLowPtYCResidualsHistBpos","Pt-\\deltar\\phi side C (z<0), Bpos",100,0.0,1.5,80,-2.0,2.0);
  pLowPtYCResidualsHistBpos->SetXTitle("Pt [rad]");
  pLowPtYCResidualsHistBpos->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLowPtZAResidualsHistBpos = new TH2F("fLowPtZAResidualsHistBpos","Pt-\\deltaz side A (z>0), Bpos",100,0.0,1.5,80,-4.0,4.0);
  pLowPtZAResidualsHistBpos->SetXTitle("Pt");
  pLowPtZAResidualsHistBpos->SetYTitle("\\deltaz [cm]");
  TH2F* pLowPtZCResidualsHistBpos = new TH2F("fLowPtZCResidualsHistBpos","Pt-\\deltaz side C (z<0), Bpos",100,0.0,1.5,80,-4.0,4.0);
  pLowPtZCResidualsHistBpos->SetXTitle("Pt");
  pLowPtZCResidualsHistBpos->SetYTitle("\\deltaz [cm]");
  TList* listBpos = new TList();
  fList->Add(listBpos);
  listBpos->Add(pZYAResidualsHistBpos);
  listBpos->Add(pZYCResidualsHistBpos);
  listBpos->Add(pLPAResidualsHistBpos);
  listBpos->Add(pLPCResidualsHistBpos);
  listBpos->Add(pPhiYAResidualsHistBpos);
  listBpos->Add(pPhiYCResidualsHistBpos);
  listBpos->Add(pPhiZAResidualsHistBpos);
  listBpos->Add(pPhiZCResidualsHistBpos);
  listBpos->Add(pPtYAResidualsHistBpos);
  listBpos->Add(pPtYCResidualsHistBpos);
  listBpos->Add(pPtZAResidualsHistBpos);
  listBpos->Add(pPtZCResidualsHistBpos);
  listBpos->Add(pLowPtYAResidualsHistBpos);
  listBpos->Add(pLowPtYCResidualsHistBpos);
  listBpos->Add(pLowPtZAResidualsHistBpos);
  listBpos->Add(pLowPtZCResidualsHistBpos);

  TH2F* pZYAResidualsHistBneg = new TH2F("fZYAResidualsHistBneg","z-r\\phi residuals side A (z>0), Bneg", 100, -4., 4., 100, -2.0, 2.0 );
  pZYAResidualsHistBneg->SetXTitle("\\deltaz [cm]");
  pZYAResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pZYCResidualsHistBneg = new TH2F("fZYCResidualsHistBneg","z-r\\phi residuals side C (z<0), Bneg", 100, -4., 4., 100, -2.0, 2.0 );
  pZYCResidualsHistBneg->SetXTitle("\\deltaz [cm]");
  pZYCResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLPAResidualsHistBneg = new TH2F("fLPAResidualsHistBneg", "sin(\\phi) tan(\\lambda) residuals side A (z>0), Bneg", 100, -.07, 0.07, 100, -0.07, 0.07 );
  pLPAResidualsHistBneg->SetXTitle("\\deltasin(\\phi)");
  pLPAResidualsHistBneg->SetYTitle("\\deltatan(\\lambda)");
  TH2F* pLPCResidualsHistBneg = new TH2F("fLPCResidualsHistBneg", "sin(\\phi) tan(\\lambda) residuals side C (z<0), Bneg", 100, -.07, 0.07, 100, -0.07, 0.07 );
  pLPCResidualsHistBneg->SetXTitle("\\deltasin(\\phi)");
  pLPCResidualsHistBneg->SetYTitle("\\deltatan(\\lambda)");
  TH2F* pPhiYAResidualsHistBneg = new TH2F("fPhiYAResidualsHistBneg","\\phi-\\deltar\\phi side A (z>0), Bneg",36,0.0,6.3,100,-2.0,2.0);
  pPhiYAResidualsHistBneg->SetXTitle("\\phi [rad]");
  pPhiYAResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPhiYCResidualsHistBneg = new TH2F("fPhiYCResidualsHistBneg","\\phi-\\deltar\\phi side C (z<0), Bneg",36,0.0,6.3,100,-2.0,2.0);
  pPhiYCResidualsHistBneg->SetXTitle("\\phi [rad]");
  pPhiYCResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPhiZAResidualsHistBneg = new TH2F("fPhiZAResidualsHistBneg","\\phi-\\deltaz side A (z>0), Bneg",36,0.0,6.3,100,-4.0,4.0);
  pPhiZAResidualsHistBneg->SetXTitle("\\phi [rad]");
  pPhiZAResidualsHistBneg->SetYTitle("\\deltaz [cm]");
  TH2F* pPhiZCResidualsHistBneg = new TH2F("fPhiZCResidualsHistBneg","\\Pt-\\deltaz side C (z<0), Bneg",36,0.0,6.3,100,-4.0,4.0);
  pPhiZCResidualsHistBneg->SetXTitle("\\phi [rad]");
  pPhiZCResidualsHistBneg->SetYTitle("\\deltaz [cm]");
  TH2F* pPtYAResidualsHistBneg = new TH2F("fPtYAResidualsHistBneg","Pt-\\deltar\\phi side A (z>0), Bneg",20,.3,8.,80,-2.0,2.0);
  pPtYAResidualsHistBneg->SetXTitle("Pt [rad]");
  pPtYAResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPtYCResidualsHistBneg = new TH2F("fPtYCResidualsHistBneg","Pt-\\deltar\\phi side C (z<0), Bneg",20,.3,8.,80,-2.0,2.0);
  pPtYCResidualsHistBneg->SetXTitle("Pt [rad]");
  pPtYCResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPtZAResidualsHistBneg = new TH2F("fPtZAResidualsHistBneg","Pt-\\deltaz side A (z>0), Bneg",20,.3,8.,80,-4.0,4.0);
  pPtZAResidualsHistBneg->SetXTitle("Pt");
  pPtZAResidualsHistBneg->SetYTitle("\\deltaz [cm]");
  TH2F* pPtZCResidualsHistBneg = new TH2F("fPtZCResidualsHistBneg","Pt-\\deltaz side C (z<0), Bneg",20,.3,8.,80,-4.0,4.0);
  pPtZCResidualsHistBneg->SetXTitle("Pt");
  pPtZCResidualsHistBneg->SetYTitle("\\deltaz [cm]");
  TH2F* pLowPtYAResidualsHistBneg = new TH2F("fLowPtYAResidualsHistBneg","Pt-\\deltar\\phi side A (z>0), Bneg",100,0.0,1.5,80,-2.0,2.0);
  pLowPtYAResidualsHistBneg->SetXTitle("Pt [rad]");
  pLowPtYAResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLowPtYCResidualsHistBneg = new TH2F("fLowPtYCResidualsHistBneg","Pt-\\deltar\\phi side C (z<0), Bneg",100,0.0,1.5,80,-2.0,2.0);
  pLowPtYCResidualsHistBneg->SetXTitle("Pt [rad]");
  pLowPtYCResidualsHistBneg->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLowPtZAResidualsHistBneg = new TH2F("fLowPtZAResidualsHistBneg","Pt-\\deltaz side A (z>0), Bneg",100,0.0,1.5,80,-4.0,4.0);
  pLowPtZAResidualsHistBneg->SetXTitle("Pt");
  pLowPtZAResidualsHistBneg->SetYTitle("\\deltaz [cm]");
  TH2F* pLowPtZCResidualsHistBneg = new TH2F("fLowPtZCResidualsHistBneg","Pt-\\deltaz side C (z<0), Bneg",100,0.0,1.5,80,-4.0,4.0);
  pLowPtZCResidualsHistBneg->SetXTitle("Pt");
  pLowPtZCResidualsHistBneg->SetYTitle("\\deltaz [cm]");
  TList* listBneg = new TList();
  fList->Add(listBneg);
  listBneg->Add(pZYAResidualsHistBneg);
  listBneg->Add(pZYCResidualsHistBneg);
  listBneg->Add(pLPAResidualsHistBneg);
  listBneg->Add(pLPCResidualsHistBneg);
  listBneg->Add(pPhiYAResidualsHistBneg);
  listBneg->Add(pPhiYCResidualsHistBneg);
  listBneg->Add(pPhiZAResidualsHistBneg);
  listBneg->Add(pPhiZCResidualsHistBneg);
  listBneg->Add(pPtYAResidualsHistBneg);
  listBneg->Add(pPtYCResidualsHistBneg);
  listBneg->Add(pPtZAResidualsHistBneg);
  listBneg->Add(pPtZCResidualsHistBneg);
  listBneg->Add(pLowPtYAResidualsHistBneg);
  listBneg->Add(pLowPtYCResidualsHistBneg);
  listBneg->Add(pLowPtZAResidualsHistBneg);
  listBneg->Add(pLowPtZCResidualsHistBneg);

  TH2F* pZYAResidualsHistBnil = new TH2F("fZYAResidualsHistBnil","z-r\\phi residuals side A (z>0), Bnil", 100, -4., 4., 100, -2.0, 2.0 );
  pZYAResidualsHistBnil->SetXTitle("\\deltaz [cm]");
  pZYAResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pZYCResidualsHistBnil = new TH2F("fZYCResidualsHistBnil","z-r\\phi residuals side C (z<0), Bnil", 100, -4., 4., 100, -2.0, 2.0 );
  pZYCResidualsHistBnil->SetXTitle("\\deltaz [cm]");
  pZYCResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLPAResidualsHistBnil = new TH2F("fLPAResidualsHistBnil", "sin(\\phi) tan(\\lambda) residuals side A (z>0), Bnil", 100, -.07, 0.07, 100, -0.07, 0.07 );
  pLPAResidualsHistBnil->SetXTitle("\\deltasin(\\phi)");
  pLPAResidualsHistBnil->SetYTitle("\\deltatan(\\lambda)");
  TH2F* pLPCResidualsHistBnil = new TH2F("fLPCResidualsHistBnil", "sin(\\phi) tan(\\lambda) residuals side C (z<0), Bnil", 100, -.07, 0.07, 100, -0.07, 0.07 );
  pLPCResidualsHistBnil->SetXTitle("\\deltasin(\\phi)");
  pLPCResidualsHistBnil->SetYTitle("\\deltatan(\\lambda)");
  TH2F* pPhiYAResidualsHistBnil = new TH2F("fPhiYAResidualsHistBnil","\\phi-\\deltar\\phi side A (z>0), Bnil",36,0.0,6.3,100,-2.0,2.0);
  pPhiYAResidualsHistBnil->SetXTitle("\\phi [rad]");
  pPhiYAResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPhiYCResidualsHistBnil = new TH2F("fPhiYCResidualsHistBnil","\\phi-\\deltar\\phi side C (z<0), Bnil",36,0.0,6.3,100,-2.0,2.0);
  pPhiYCResidualsHistBnil->SetXTitle("\\phi [rad]");
  pPhiYCResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPhiZAResidualsHistBnil = new TH2F("fPhiZAResidualsHistBnil","\\phi-\\deltaz side A (z>0), Bnil",36,0.0,6.3,100,-4.0,4.0);
  pPhiZAResidualsHistBnil->SetXTitle("\\phi [rad]");
  pPhiZAResidualsHistBnil->SetYTitle("\\deltaz [cm]");
  TH2F* pPhiZCResidualsHistBnil = new TH2F("fPhiZCResidualsHistBnil","\\Pt-\\deltaz side C (z<0), Bnil",36,0.0,6.3,100,-4.0,4.0);
  pPhiZCResidualsHistBnil->SetXTitle("\\phi [rad]");
  pPhiZCResidualsHistBnil->SetYTitle("\\deltaz [cm]");
  TH2F* pPtYAResidualsHistBnil = new TH2F("fPtYAResidualsHistBnil","Pt-\\deltar\\phi side A (z>0), Bnil",20,.3,8.,80,-2.0,2.0);
  pPtYAResidualsHistBnil->SetXTitle("Pt [rad]");
  pPtYAResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPtYCResidualsHistBnil = new TH2F("fPtYCResidualsHistBnil","Pt-\\deltar\\phi side C (z<0), Bnil",20,.3,8.,80,-2.0,2.0);
  pPtYCResidualsHistBnil->SetXTitle("Pt [rad]");
  pPtYCResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pPtZAResidualsHistBnil = new TH2F("fPtZAResidualsHistBnil","Pt-\\deltaz side A (z>0), Bnil",20,.3,8.,80,-4.0,4.0);
  pPtZAResidualsHistBnil->SetXTitle("Pt");
  pPtZAResidualsHistBnil->SetYTitle("\\deltaz [cm]");
  TH2F* pPtZCResidualsHistBnil = new TH2F("fPtZCResidualsHistBnil","Pt-\\deltaz side C (z<0), Bnil",20,.3,8.,80,-4.0,4.0);
  pPtZCResidualsHistBnil->SetXTitle("Pt");
  pPtZCResidualsHistBnil->SetYTitle("\\deltaz [cm]");
  TH2F* pLowPtYAResidualsHistBnil = new TH2F("fLowPtYAResidualsHistBnil","Pt-\\deltar\\phi side A (z>0), Bnil",100,0.0,1.5,80,-2.0,2.0);
  pLowPtYAResidualsHistBnil->SetXTitle("Pt [rad]");
  pLowPtYAResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLowPtYCResidualsHistBnil = new TH2F("fLowPtYCResidualsHistBnil","Pt-\\deltar\\phi side C (z<0), Bnil",100,0.0,1.5,80,-2.0,2.0);
  pLowPtYCResidualsHistBnil->SetXTitle("Pt [rad]");
  pLowPtYCResidualsHistBnil->SetYTitle("\\deltar\\phi [cm]");
  TH2F* pLowPtZAResidualsHistBnil = new TH2F("fLowPtZAResidualsHistBnil","Pt-\\deltaz side A (z>0), Bnil",100,0.0,1.5,80,-4.0,4.0);
  pLowPtZAResidualsHistBnil->SetXTitle("Pt");
  pLowPtZAResidualsHistBnil->SetYTitle("\\deltaz [cm]");
  TH2F* pLowPtZCResidualsHistBnil = new TH2F("fLowPtZCResidualsHistBnil","Pt-\\deltaz side C (z<0), Bnil",100,0.0,1.5,80,-4.0,4.0);
  pLowPtZCResidualsHistBnil->SetXTitle("Pt");
  pLowPtZCResidualsHistBnil->SetYTitle("\\deltaz [cm]");
  TList* listBnil = new TList();
  fList->Add(listBnil);
  listBnil->Add(pZYAResidualsHistBnil);
  listBnil->Add(pZYCResidualsHistBnil);
  listBnil->Add(pLPAResidualsHistBnil);
  listBnil->Add(pLPCResidualsHistBnil);
  listBnil->Add(pPhiYAResidualsHistBnil);
  listBnil->Add(pPhiYCResidualsHistBnil);
  listBnil->Add(pPhiZAResidualsHistBnil);
  listBnil->Add(pPhiZCResidualsHistBnil);
  listBnil->Add(pPtYAResidualsHistBnil);
  listBnil->Add(pPtYCResidualsHistBnil);
  listBnil->Add(pPtZAResidualsHistBnil);
  listBnil->Add(pPtZCResidualsHistBnil);
  listBnil->Add(pLowPtYAResidualsHistBnil);
  listBnil->Add(pLowPtYCResidualsHistBnil);
  listBnil->Add(pLowPtZAResidualsHistBnil);
  listBnil->Add(pLowPtZCResidualsHistBnil);
  TH1F* pNmatchingEff=new TH1F("pNmatchingEff","matching efficiency",50,0.,1.);
  fList->Add(pNmatchingEff);

  fAligner = new AliRelAlignerKalman();
  fAligner->SetRejectOutliers(fRejectOutliers);
  fAligner->SetOutRejSigma(fOutRejSigma);
  fAligner->SetRejectOutliersSigma2Median(fRejectOutliersSigma2Median);
  fAligner->SetOutRejSigma2Median(fOutRejSigma2Median);
  fList->Add(fAligner);

  fDebugTree = new TTree("debugTree","tree with debug info");
  fDebugTree->Branch("aligner","AliRelAlignerKalman",&fAligner);
}

//________________________________________________________________________
void AliAnalysisTaskITSTPCalignment::Exec(Option_t *)
{
  // Main loop
  // Called for each event

  //check for ESD and friends
  if (!fESD)
  {
    AliError("no ESD");
    return;
  }

  if (!fESDfriend)
  {
    AliError("no ESD friend");
    return;
  }

  fESD->SetESDfriend(fESDfriend); //Attach the friend to the ESD

  //Update the parmeters
  AnalyzeESDevent();

  // Post output data.
  PostData(0, fDebugTree);
  PostData(1, fList);
  PostData(2, fArrayITSglobal);
  PostData(3, fArrayITSsa);
}

//________________________________________________________________________
void AliAnalysisTaskITSTPCalignment::AnalyzeESDevent()
{
  //analyze an ESD event with track matching
  Int_t ntracks = fESD->GetNumberOfTracks();
  if (ntracks==0) return;

  if (fUseITSoutGlobalTrack)
  {
    AliRelAlignerKalman* alignerGlobal = fArrayITSglobal->GetAligner(fESD);
    if (alignerGlobal)
      alignerGlobal->AddESDevent(fESD);
  }

  if (fUseITSoutITSSAtrack)
  { 
    //get the aligner for the event
    //do it with matching
    TObjArray arrayParamITS(ntracks);
    TObjArray arrayParamTPC(ntracks);

    Int_t n = FindMatchingTracks(arrayParamITS, arrayParamTPC, fESD);
    AliInfo(Form("n matched: %i\n",n));

    TH1F* nMatchingEff=dynamic_cast<TH1F*>(fList->At(3));
    Float_t ratio = (float)n/(float)ntracks;
    nMatchingEff->Fill(ratio);

    AliRelAlignerKalman* alignerSA = NULL;
    if (n>0) alignerSA = fArrayITSsa->GetAligner(fESD);

    for (Int_t i=0;i<n;i++)
    {
      AliExternalTrackParam* paramsITS=dynamic_cast<AliExternalTrackParam*>(arrayParamITS[i]);
      AliExternalTrackParam* paramsTPC=dynamic_cast<AliExternalTrackParam*>(arrayParamTPC[i]);

      //QA
      if (fDoQA) DoQA(paramsITS,paramsTPC);

      //debug
      if (fAligner->AddTrackParams(paramsITS, paramsTPC))
      {
        fAligner->SetRunNumber(fESD->GetRunNumber());
        fAligner->SetMagField(fESD->GetMagneticField());
        fAligner->SetTimeStamp(fESD->GetTimeStamp());
      }

      //alignment check
      if (alignerSA) alignerSA->AddTrackParams(paramsITS, paramsTPC);
    }
    arrayParamITS.Delete();
    arrayParamTPC.Delete();
  }
}

//________________________________________________________________________
void AliAnalysisTaskITSTPCalignment::Terminate(Option_t *)
{
  // Called once at the end of the query
}

//________________________________________________________________________
Int_t AliAnalysisTaskITSTPCalignment::FindMatchingTracks(TObjArray& arrITS, TObjArray& arrTPC, AliESDEvent* pESD)
{
  //find matching tracks and return tobjarrays with the params
  //fUniqueID of param object contains tracknumber in event.

  Int_t ntracks = pESD->GetNumberOfTracks();
  Double_t magfield = pESD->GetMagneticField();

  Int_t* matchedArr = new Int_t[ntracks]; //storage for index of ITS track for which a match was found
  for (Int_t i=0;i<ntracks;i++)
  {
    matchedArr[i]=-1;
  }

  Int_t iMatched=-1;
  for (Int_t i=0; i<ntracks; i++)
  {
    //get track1 and friend
    AliESDtrack* track1 = pESD->GetTrack(i);
    if (!track1) continue;

    if (track1->GetNcls(0) < fMinPointsVol1) continue;

    if (!( ( track1->IsOn(AliESDtrack::kITSrefit)) &&
           (!track1->IsOn(AliESDtrack::kTPCin)) )) continue;

    const AliESDfriendTrack* constfriendtrack1 = track1->GetFriendTrack();
    if (!constfriendtrack1) {AliInfo(Form("no friendTrack1\n"));continue;}
    AliESDfriendTrack friendtrack1(*constfriendtrack1);

    if (!friendtrack1.GetITSOut()) continue;
    AliExternalTrackParam params1(*(friendtrack1.GetITSOut()));

    Double_t bestd = 1000.; //best distance
    Bool_t newi = kTRUE; //whether we start with a new i
    for (Int_t j=0; j<ntracks; j++)
    {
      if (matchedArr[j]>0 && matchedArr[j]!=i) continue; //already matched, everything tried
      //get track2 and friend
      AliESDtrack* track2 = pESD->GetTrack(j);
      if (!track2) continue;
      if (track1==track2) continue;
      if (!(track2->IsOn(AliESDtrack::kTPCin))) continue; //must be TPC
      if (!(track2->IsOn(AliESDtrack::kITSout))) continue; //must have ITS

      //if (track2->GetNcls(0) != track1->GetNcls(0)) continue;
      //if (track2->GetITSClusterMap() != track1->GetITSClusterMap()) continue;
      if (track2->GetNcls(1) < fMinPointsVol2) continue; //min 80 clusters in TPC
      if (track2->GetTgl() > 1.) continue; //acceptance
      //cut crossing tracks
      if (!track2->GetInnerParam()) continue;
      if (!track2->GetOuterParam()) continue;
      if (track2->GetOuterParam()->GetZ()*track2->GetInnerParam()->GetZ()<0) continue;
      if (track2->GetInnerParam()->GetX()>90) continue;
      if (TMath::Abs(track2->GetInnerParam()->GetZ())<5.) continue; //too close to membrane?

      AliExternalTrackParam params2(*(track2->GetInnerParam()));

      //bring to same reference plane
      if (!params2.Rotate(params1.GetAlpha())) continue;
      if (!params2.PropagateTo(params1.GetX(), magfield)) continue;

      //pt cut, only for data with magfield
      if ((params2.Pt()<fMinPt)&&(TMath::Abs(magfield)>1.)) continue;
      //else 
      //if (fMC)
      // {
      //   AliStack* stack = fMC->Stack();
      //   Int_t label = track2->GetLabel();
      //   if (label<0) continue;
      //   TParticle* particle = stack->Particle(label);
      //   if (particle->Pt()<fMinPt) continue;
      // }

      const Double32_t*	p1 = params1.GetParameter();
      const Double32_t*	p2 = params2.GetParameter();

      //hard cuts
      Double_t dy = TMath::Abs(p2[0]-p1[0]);
      Double_t dz = TMath::Abs(p2[1]-p1[1]);
      Double_t dphi = TMath::Abs(p2[2]-p1[2]);
      Double_t dlam = TMath::Abs(p2[3]-p1[3]);
      if (dy > 2.0) continue;
      if (dz > 10.0) continue;
      if (dphi > 0.1 ) continue;
      if (dlam > 0.1 ) continue;

      //best match only
      Double_t d = TMath::Sqrt(dy*dy+dz*dz+dphi*dphi+dlam*dlam);
      if ( d >= bestd) continue;
      bestd = d;
      matchedArr[j]=i; //j-th track matches i-th (ITS) track
      if (newi) iMatched++; newi=kFALSE; //increment at most once per i
      params1.SetUniqueID(i); //store tracknummer
      params2.SetUniqueID(j);
      if (arrITS[iMatched] && arrTPC[iMatched])
      {
        *(arrITS[iMatched]) = params1;
        *(arrTPC[iMatched]) = params2;
      }
      else
      {
        arrITS[iMatched] = new AliExternalTrackParam(params1);
        arrTPC[iMatched] = new AliExternalTrackParam(params2);
      }//else
    }//for j
  }//for i
  return iMatched+1;
}

//________________________________________________________________________
void AliAnalysisTaskITSTPCalignment::DoQA(AliExternalTrackParam* paramsITS,
    AliExternalTrackParam* paramsTPC)
{
  //fill qa histograms in a given list, per field direction (5,-5,0)
  Float_t resy = paramsTPC->GetY() - paramsITS->GetY();
  Float_t resz = paramsTPC->GetZ() - paramsITS->GetZ();
  Float_t ressnp = paramsTPC->GetSnp() - paramsITS->GetSnp();
  Float_t restgl = paramsTPC->GetTgl() - paramsITS->GetTgl();

  TList* pList=NULL;
  Double_t field = fESD->GetMagneticField();
  if (field >= 1.)  pList = dynamic_cast<TList*>(fList->At(0));
  if (field <= -1.) pList = dynamic_cast<TList*>(fList->At(1));
  if (field < 1. && field > -1.) pList = dynamic_cast<TList*>(fList->At(2));
  if (!pList) return;

  TH2F* pZYAResidualsHist = dynamic_cast<TH2F*>(pList->At(0));
  TH2F* pZYCResidualsHist = dynamic_cast<TH2F*>(pList->At(1));
  TH2F* pLPAResidualsHist = dynamic_cast<TH2F*>(pList->At(2));
  TH2F* pLPCResidualsHist = dynamic_cast<TH2F*>(pList->At(3));
  TH2F* pPhiYAResidualsHist = dynamic_cast<TH2F*>(pList->At(4));
  TH2F* pPhiYCResidualsHist = dynamic_cast<TH2F*>(pList->At(5));
  TH2F* pPhiZAResidualsHist = dynamic_cast<TH2F*>(pList->At(6));
  TH2F* pPhiZCResidualsHist = dynamic_cast<TH2F*>(pList->At(7));
  TH2F* pPtYAResidualsHist = dynamic_cast<TH2F*>(pList->At(8));
  TH2F* pPtYCResidualsHist = dynamic_cast<TH2F*>(pList->At(9));
  TH2F* pPtZAResidualsHist = dynamic_cast<TH2F*>(pList->At(10));
  TH2F* pPtZCResidualsHist = dynamic_cast<TH2F*>(pList->At(11));
  TH2F* pLowPtYAResidualsHist = dynamic_cast<TH2F*>(pList->At(12));
  TH2F* pLowPtYCResidualsHist = dynamic_cast<TH2F*>(pList->At(13));
  TH2F* pLowPtZAResidualsHist = dynamic_cast<TH2F*>(pList->At(14));
  TH2F* pLowPtZCResidualsHist = dynamic_cast<TH2F*>(pList->At(15));

  if (paramsITS->GetZ() > 0.0)
  {
    pZYAResidualsHist->Fill(resz,resy);
    pLPAResidualsHist->Fill(restgl,ressnp);
    pPhiYAResidualsHist->Fill(paramsTPC->Phi(),resy);
    pPhiZAResidualsHist->Fill(paramsTPC->Phi(),resz);
    pPtYAResidualsHist->Fill(paramsTPC->Pt(),resy);
    pPtZAResidualsHist->Fill(paramsTPC->Pt(),resz);
    pLowPtYAResidualsHist->Fill(paramsTPC->Pt(),resy);
    pLowPtZAResidualsHist->Fill(paramsTPC->Pt(),resz);
  }
  else
  {
    pZYCResidualsHist->Fill(resz,resy);
    pLPCResidualsHist->Fill(restgl,ressnp);
    pPhiYCResidualsHist->Fill(paramsTPC->Phi(),resy);
    pPhiZCResidualsHist->Fill(paramsTPC->Phi(),resz);
    pPtYCResidualsHist->Fill(paramsTPC->Pt(),resy);
    pPtZCResidualsHist->Fill(paramsTPC->Pt(),resz);
    pLowPtYCResidualsHist->Fill(paramsTPC->Pt(),resy);
    pLowPtZCResidualsHist->Fill(paramsTPC->Pt(),resz);
  }
}

