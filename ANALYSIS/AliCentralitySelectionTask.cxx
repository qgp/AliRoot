/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

//*****************************************************
//   Class AliCentralitySelectionTask
//   Class to analyze determine centrality            
//   author: Alberica Toia
//*****************************************************

#include "AliCentralitySelectionTask.h"

#include <TTree.h>
#include <TList.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TProfile.h>
#include <TFile.h>
#include <TObjString.h>
#include <TString.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <iostream>

#include "AliAnalysisManager.h"
#include "AliHeader.h"
#include "AliVEvent.h"
#include "AliESD.h"
#include "AliESDEvent.h"
#include "AliESDHeader.h"
#include "AliESDInputHandler.h"
#include "AliESDZDC.h"
#include "AliESDFMD.h"
#include "AliESDTZERO.h"
#include "AliESDVZERO.h"
#include "AliESDtrackCuts.h"
#include "AliESDVertex.h"
#include "AliCentrality.h"
#include "AliOADBCentrality.h"
#include "AliOADBContainer.h"
#include "AliMultiplicity.h"
#include "AliAODHandler.h"
#include "AliAODHeader.h"
#include "AliAODEvent.h"
#include "AliAODVertex.h"
#include "AliAODVZERO.h"
#include "AliAODTracklets.h"
#include "AliAODMCHeader.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliAODMCParticle.h"
#include "AliMCParticle.h"
#include "AliStack.h"
#include "AliAnalysisTaskSE.h"
#include "AliGenEventHeader.h"
#include "AliGenHijingEventHeader.h"
#include "AliPhysicsSelectionTask.h"
#include "AliPhysicsSelection.h"
#include "AliBackgroundSelection.h"
#include "AliESDUtils.h"

ClassImp(AliCentralitySelectionTask)


//________________________________________________________________________
AliCentralitySelectionTask::AliCentralitySelectionTask():
AliAnalysisTaskSE(),
  fAnalysisInput("ESD"),
  fIsMCInput(kFALSE),
  fPass(0),
  fCurrentRun(-1),
  fUseScaling(0),
  fUseCleaning(0),
  fFillHistos(0),
  fV0MScaleFactor(0),
  fSPDScaleFactor(0),
  fTPCScaleFactor(0),
  fV0MScaleFactorMC(0),
  fV0MSPDOutlierPar0(0),  
  fV0MSPDOutlierPar1(0),  
  fV0MTPCOutlierPar0(0),  
  fV0MTPCOutlierPar1(0),  
  fV0MSPDSigmaOutlierPar0(0),  
  fV0MSPDSigmaOutlierPar1(0),  
  fV0MSPDSigmaOutlierPar2(0),  
  fV0MTPCSigmaOutlierPar0(0),  
  fV0MTPCSigmaOutlierPar1(0),  
  fV0MTPCSigmaOutlierPar2(0),    			   			   
  fV0MZDCOutlierPar0(0),	    
  fV0MZDCOutlierPar1(0),	    
  fV0MZDCEcalOutlierPar0(0),   
  fV0MZDCEcalOutlierPar1(0),   
  fTrackCuts(0),
  fZVCut(10),
  fOutliersCut(5),
  fQuality(999),
  fIsSelected(0),
  fMSL(0),
  fMSH(0),
  fMUL(0),
  fMLL(0),
  fEJE(0),
  fEGA(0),
  fPHS(0),
  fCVHN(0),
  fCVLN(0),
  fCVHNbit(0),
  fCVLNbit(0),
  fCCENT(0),
  fCSEMI(0),
  fCCENTbit(0),
  fCSEMIbit(0),
  fCentV0M(0),
  fCentFMD(0),
  fCentTRK(0),
  fCentTKL(0),
  fCentCL0(0),
  fCentCL1(0),
  fCentV0MvsFMD(0),
  fCentTKLvsV0M(0),
  fCentZEMvsZDC(0),
  fHtempV0M(0),
  fHtempFMD(0),
  fHtempTRK(0),
  fHtempTKL(0),
  fHtempCL0(0),
  fHtempCL1(0),
  fHtempV0MvsFMD(0),
  fHtempTKLvsV0M(0),
  fHtempZEMvsZDC(0),
  fOutputList(0),
  fHOutCentV0M     (0),
  fHOutCentV0M_CVHN(0),
  fHOutCentV0M_CVLN(0),
  fHOutCentV0M_CVHNinMB(0),
  fHOutCentV0M_CVLNinMB(0),
  fHOutCentV0M_CCENT(0),
  fHOutCentV0M_CSEMI(0),
  fHOutCentV0M_CCENTinMB(0),
  fHOutCentV0M_CSEMIinMB(0),
  fHOutCentV0M_MSL(0),
  fHOutCentV0M_MSH(0),
  fHOutCentV0M_MUL(0),
  fHOutCentV0M_MLL(0),
  fHOutCentV0M_EJE(0),
  fHOutCentV0M_EGA(0),
  fHOutCentV0M_PHS(0),
  fHOutCentV0M_MSLinMB(0),
  fHOutCentV0M_MSHinMB(0),
  fHOutCentV0M_MULinMB(0),
  fHOutCentV0M_MLLinMB(0),
  fHOutCentV0M_EJEinMB(0),
  fHOutCentV0M_EGAinMB(0),
  fHOutCentV0M_PHSinMB(0),
  fHOutCentFMD     (0),
  fHOutCentTRK     (0),
  fHOutCentTKL     (0),
  fHOutCentCL0     (0),
  fHOutCentCL1     (0),
  fHOutCentV0MvsFMD(0),
  fHOutCentTKLvsV0M(0),
  fHOutCentZEMvsZDC(0),
  fHOutCentV0MvsCentCL1(0),
  fHOutCentV0MvsCentTRK(0),
  fHOutCentTRKvsCentCL1(0),
  fHOutCentV0MvsCentZDC(0),
  fHOutMultV0M(0),
  fHOutMultV0O(0),
  fHOutMultFMD(0),
  fHOutMultTRK(0),
  fHOutMultTKL(0),
  fHOutMultCL0(0),
  fHOutMultCL1(0),
  fHOutMultV0MvsZDN(0),
  fHOutMultZEMvsZDN(0),
  fHOutMultV0MvsZDC(0),
  fHOutMultZEMvsZDC(0),
  fHOutMultZEMvsZDCw(0),
  fHOutMultV0MvsCL1(0),
  fHOutMultV0MvsTRK(0),
  fHOutMultTRKvsCL1(0),
  fHOutMultV0MvsV0O(0),
  fHOutMultV0OvsCL1(0),
  fHOutMultV0OvsTRK(0),
  fHOutCentV0Mqual1(0),
  fHOutCentTRKqual1(0),
  fHOutCentCL1qual1(0),
  fHOutMultV0MvsCL1qual1(0),
  fHOutMultV0MvsTRKqual1(0),
  fHOutMultTRKvsCL1qual1(0),
  fHOutCentV0Mqual2(0),
  fHOutCentTRKqual2(0),
  fHOutCentCL1qual2(0),
  fHOutMultV0MvsCL1qual2(0),
  fHOutMultV0MvsTRKqual2(0),
  fHOutMultTRKvsCL1qual2(0),
  fHOutQuality(0),
  fHOutVertex(0),
  fHOutVertexT0(0)
{   
  // Default constructor
  AliInfo("Centrality Selection enabled.");

  fUseScaling=kTRUE;
  fUseCleaning=kTRUE;
  fFillHistos=kFALSE;
  fBranchNames="ESD:AliESDRun.,AliESDHeader.,AliESDZDC.,AliESDFMD.,AliESDVZERO.,AliESDTZERO."
    ",SPDVertex.,TPCVertex.,PrimaryVertex.,AliMultiplicity.,Tracks ";
}   

//________________________________________________________________________
AliCentralitySelectionTask::AliCentralitySelectionTask(const char *name):
  AliAnalysisTaskSE(name),
  fAnalysisInput("ESD"),
  fIsMCInput(kFALSE),
  fPass(0),
  fCurrentRun(-1),
  fUseScaling(0),
  fUseCleaning(0),
  fFillHistos(0),
  fV0MScaleFactor(0),
  fSPDScaleFactor(0),
  fTPCScaleFactor(0),
  fV0MScaleFactorMC(0),
  fV0MSPDOutlierPar0(0),  
  fV0MSPDOutlierPar1(0),  
  fV0MTPCOutlierPar0(0),  
  fV0MTPCOutlierPar1(0),  
  fV0MSPDSigmaOutlierPar0(0),  
  fV0MSPDSigmaOutlierPar1(0),  
  fV0MSPDSigmaOutlierPar2(0),  
  fV0MTPCSigmaOutlierPar0(0),  
  fV0MTPCSigmaOutlierPar1(0),  
  fV0MTPCSigmaOutlierPar2(0),
  fV0MZDCOutlierPar0(0),	    
  fV0MZDCOutlierPar1(0),	    
  fV0MZDCEcalOutlierPar0(0),   
  fV0MZDCEcalOutlierPar1(0),   
  fTrackCuts(0),
  fZVCut(10),
  fOutliersCut(5),
  fQuality(999),
  fIsSelected(0),
  fMSL(0),
  fMSH(0),
  fMUL(0),
  fMLL(0),
  fEJE(0),
  fEGA(0),
  fPHS(0),
  fCVHN(0),
  fCVLN(0),
  fCVHNbit(0),
  fCVLNbit(0),
  fCCENT(0),
  fCSEMI(0),
  fCCENTbit(0),
  fCSEMIbit(0),
  fCentV0M(0),
  fCentFMD(0),
  fCentTRK(0),
  fCentTKL(0),
  fCentCL0(0),
  fCentCL1(0),
  fCentV0MvsFMD(0),
  fCentTKLvsV0M(0),
  fCentZEMvsZDC(0),
  fHtempV0M(0),
  fHtempFMD(0),
  fHtempTRK(0),
  fHtempTKL(0),
  fHtempCL0(0),
  fHtempCL1(0),
  fHtempV0MvsFMD(0),
  fHtempTKLvsV0M(0),
  fHtempZEMvsZDC(0),
  fOutputList(0),
  fHOutCentV0M     (0),
  fHOutCentV0M_CVHN(0),
  fHOutCentV0M_CVLN(0),
  fHOutCentV0M_CVHNinMB(0),
  fHOutCentV0M_CVLNinMB(0),
  fHOutCentV0M_CCENT(0),
  fHOutCentV0M_CSEMI(0),
  fHOutCentV0M_CCENTinMB(0),
  fHOutCentV0M_CSEMIinMB(0),
  fHOutCentV0M_MSL(0),
  fHOutCentV0M_MSH(0),
  fHOutCentV0M_MUL(0),
  fHOutCentV0M_MLL(0),
  fHOutCentV0M_EJE(0),
  fHOutCentV0M_EGA(0),
  fHOutCentV0M_PHS(0),
  fHOutCentV0M_MSLinMB(0),
  fHOutCentV0M_MSHinMB(0),
  fHOutCentV0M_MULinMB(0),
  fHOutCentV0M_MLLinMB(0),
  fHOutCentV0M_EJEinMB(0),
  fHOutCentV0M_EGAinMB(0),
  fHOutCentV0M_PHSinMB(0),
  fHOutCentFMD     (0),
  fHOutCentTRK     (0),
  fHOutCentTKL     (0),
  fHOutCentCL0     (0),
  fHOutCentCL1     (0),
  fHOutCentV0MvsFMD(0),
  fHOutCentTKLvsV0M(0),
  fHOutCentZEMvsZDC(0),
  fHOutCentV0MvsCentCL1(0),
  fHOutCentV0MvsCentTRK(0),
  fHOutCentTRKvsCentCL1(0),
  fHOutCentV0MvsCentZDC(0),
  fHOutMultV0M(0),
  fHOutMultV0O(0),
  fHOutMultFMD(0),
  fHOutMultTRK(0),
  fHOutMultTKL(0),
  fHOutMultCL0(0),
  fHOutMultCL1(0),
  fHOutMultV0MvsZDN(0),
  fHOutMultZEMvsZDN(0),
  fHOutMultV0MvsZDC(0),
  fHOutMultZEMvsZDC(0),
  fHOutMultZEMvsZDCw(0),
  fHOutMultV0MvsCL1(0),
  fHOutMultV0MvsTRK(0),
  fHOutMultTRKvsCL1(0),
  fHOutMultV0MvsV0O(0),
  fHOutMultV0OvsCL1(0),
  fHOutMultV0OvsTRK(0),
  fHOutCentV0Mqual1(0),
  fHOutCentTRKqual1(0),
  fHOutCentCL1qual1(0),
  fHOutMultV0MvsCL1qual1(0),
  fHOutMultV0MvsTRKqual1(0),
  fHOutMultTRKvsCL1qual1(0),
  fHOutCentV0Mqual2(0),
  fHOutCentTRKqual2(0),
  fHOutCentCL1qual2(0),
  fHOutMultV0MvsCL1qual2(0),
  fHOutMultV0MvsTRKqual2(0),
  fHOutMultTRKvsCL1qual2(0),
  fHOutQuality(0),
  fHOutVertex(0),
  fHOutVertexT0(0)
{
  // Default constructor
  AliInfo("Centrality Selection enabled.");
  DefineOutput(1, TList::Class());
  fUseScaling=kTRUE;
  fUseCleaning=kTRUE;
  fFillHistos=kFALSE;
  fBranchNames="ESD:AliESDRun.,AliESDHeader.,AliESDZDC.,AliESDFMD.,AliESDVZERO.,AliESDTZERO."
    ",SPDVertex.,TPCVertex.,PrimaryVertex.,AliMultiplicity.,Tracks ";
}

//________________________________________________________________________
AliCentralitySelectionTask& AliCentralitySelectionTask::operator=(const AliCentralitySelectionTask& c)
{
  // Assignment operator
  if (this!=&c) {
    AliAnalysisTaskSE::operator=(c);
  }
  return *this;
}

//________________________________________________________________________
AliCentralitySelectionTask::AliCentralitySelectionTask(const AliCentralitySelectionTask& ana):
  AliAnalysisTaskSE(ana),
  fAnalysisInput(ana.fDebug),
  fIsMCInput(ana.fIsMCInput),
  fPass(ana.fPass),
  fCurrentRun(ana.fCurrentRun),
  fUseScaling(ana.fUseScaling),
  fUseCleaning(ana.fUseCleaning),
  fFillHistos(ana.fFillHistos),
  fV0MScaleFactor(ana.fV0MScaleFactor),
  fSPDScaleFactor(ana.fSPDScaleFactor),
  fTPCScaleFactor(ana.fTPCScaleFactor),
  fV0MScaleFactorMC(ana.fV0MScaleFactorMC),
  fV0MSPDOutlierPar0(ana.fV0MSPDOutlierPar0),  
  fV0MSPDOutlierPar1(ana.fV0MSPDOutlierPar1),  
  fV0MTPCOutlierPar0(ana.fV0MTPCOutlierPar0),  
  fV0MTPCOutlierPar1(ana.fV0MTPCOutlierPar1),  
  fV0MSPDSigmaOutlierPar0(ana.fV0MSPDSigmaOutlierPar0),  
  fV0MSPDSigmaOutlierPar1(ana.fV0MSPDSigmaOutlierPar1),  
  fV0MSPDSigmaOutlierPar2(ana.fV0MSPDSigmaOutlierPar2),  
  fV0MTPCSigmaOutlierPar0(ana.fV0MTPCSigmaOutlierPar0),  
  fV0MTPCSigmaOutlierPar1(ana.fV0MTPCSigmaOutlierPar1),  
  fV0MTPCSigmaOutlierPar2(ana.fV0MTPCSigmaOutlierPar2), 
  fV0MZDCOutlierPar0(ana.fV0MZDCOutlierPar0),	    
  fV0MZDCOutlierPar1(ana.fV0MZDCOutlierPar1),	    
  fV0MZDCEcalOutlierPar0(ana.fV0MZDCEcalOutlierPar0),   
  fV0MZDCEcalOutlierPar1(ana.fV0MZDCEcalOutlierPar1),   
  fTrackCuts(ana.fTrackCuts),
  fZVCut(ana.fZVCut),
  fOutliersCut(ana.fOutliersCut),
  fQuality(ana.fQuality),
  fIsSelected(ana.fIsSelected),
  fMSL(ana.fMSL),
  fMSH(ana.fMSH),
  fMUL(ana.fMUL),
  fMLL(ana.fMLL),
  fEJE(ana.fEJE),
  fEGA(ana.fEGA),
  fPHS(ana.fPHS),
  fCVHN(ana.fCVHN),
  fCVLN(ana.fCVLN),
  fCVHNbit(ana.fCVHNbit),
  fCVLNbit(ana.fCVLNbit),
  fCCENT(ana.fCCENT),
  fCSEMI(ana.fCSEMI),
  fCCENTbit(ana.fCCENTbit),
  fCSEMIbit(ana.fCSEMIbit),
  fCentV0M(ana.fCentV0M),
  fCentFMD(ana.fCentFMD),
  fCentTRK(ana.fCentTRK),
  fCentTKL(ana.fCentTKL),
  fCentCL0(ana.fCentCL0),
  fCentCL1(ana.fCentCL1),
  fCentV0MvsFMD(ana.fCentV0MvsFMD),
  fCentTKLvsV0M(ana.fCentTKLvsV0M),
  fCentZEMvsZDC(ana.fCentZEMvsZDC),
  fHtempV0M(ana.fHtempV0M),
  fHtempFMD(ana.fHtempFMD),
  fHtempTRK(ana.fHtempTRK),
  fHtempTKL(ana.fHtempTKL),
  fHtempCL0(ana.fHtempCL0),
  fHtempCL1(ana.fHtempCL1),
  fHtempV0MvsFMD(ana.fHtempV0MvsFMD),
  fHtempTKLvsV0M(ana.fHtempTKLvsV0M),
  fHtempZEMvsZDC(ana.fHtempZEMvsZDC),
  fOutputList(ana.fOutputList),
  fHOutCentV0M     (ana.fHOutCentV0M     ),
  fHOutCentV0M_CVHN(ana.fHOutCentV0M_CVHN),
  fHOutCentV0M_CVLN(ana.fHOutCentV0M_CVLN),
  fHOutCentV0M_CVHNinMB(ana.fHOutCentV0M_CVHNinMB),
  fHOutCentV0M_CVLNinMB(ana.fHOutCentV0M_CVLNinMB),
  fHOutCentV0M_CCENT(ana.fHOutCentV0M_CCENT),
  fHOutCentV0M_CSEMI(ana.fHOutCentV0M_CSEMI),
  fHOutCentV0M_CCENTinMB(ana.fHOutCentV0M_CCENTinMB),
  fHOutCentV0M_CSEMIinMB(ana.fHOutCentV0M_CSEMIinMB),
  fHOutCentV0M_MSL(ana.fHOutCentV0M_MSL),
  fHOutCentV0M_MSH(ana.fHOutCentV0M_MSH),
  fHOutCentV0M_MUL(ana.fHOutCentV0M_MUL),
  fHOutCentV0M_MLL(ana.fHOutCentV0M_MLL),
  fHOutCentV0M_EJE(ana.fHOutCentV0M_EJE),
  fHOutCentV0M_EGA(ana.fHOutCentV0M_EGA),
  fHOutCentV0M_PHS(ana.fHOutCentV0M_PHS),
  fHOutCentV0M_MSLinMB(ana.fHOutCentV0M_MSLinMB),
  fHOutCentV0M_MSHinMB(ana.fHOutCentV0M_MSHinMB),
  fHOutCentV0M_MULinMB(ana.fHOutCentV0M_MULinMB),
  fHOutCentV0M_MLLinMB(ana.fHOutCentV0M_MLLinMB),
  fHOutCentV0M_EJEinMB(ana.fHOutCentV0M_EJEinMB),
  fHOutCentV0M_EGAinMB(ana.fHOutCentV0M_EGAinMB),
  fHOutCentV0M_PHSinMB(ana.fHOutCentV0M_PHSinMB),
  fHOutCentFMD     (ana.fHOutCentFMD     ),
  fHOutCentTRK     (ana.fHOutCentTRK     ),
  fHOutCentTKL     (ana.fHOutCentTKL     ),
  fHOutCentCL0     (ana.fHOutCentCL0     ),
  fHOutCentCL1     (ana.fHOutCentCL1     ),
  fHOutCentV0MvsFMD(ana.fHOutCentV0MvsFMD),
  fHOutCentTKLvsV0M(ana.fHOutCentTKLvsV0M),
  fHOutCentZEMvsZDC(ana.fHOutCentZEMvsZDC),
  fHOutCentV0MvsCentCL1(ana.fHOutCentV0MvsCentCL1),
  fHOutCentV0MvsCentTRK(ana.fHOutCentV0MvsCentTRK),
  fHOutCentTRKvsCentCL1(ana.fHOutCentTRKvsCentCL1),
  fHOutCentV0MvsCentZDC(ana.fHOutCentV0MvsCentZDC),
  fHOutMultV0M(ana.fHOutMultV0M),
  fHOutMultV0O(ana.fHOutMultV0O),
  fHOutMultFMD(ana.fHOutMultFMD),
  fHOutMultTRK(ana.fHOutMultTRK),
  fHOutMultTKL(ana.fHOutMultTKL),
  fHOutMultCL0(ana.fHOutMultCL0),
  fHOutMultCL1(ana.fHOutMultCL1),
  fHOutMultV0MvsZDN(ana.fHOutMultV0MvsZDN),
  fHOutMultZEMvsZDN(ana.fHOutMultZEMvsZDN),
  fHOutMultV0MvsZDC(ana.fHOutMultV0MvsZDC),
  fHOutMultZEMvsZDC(ana.fHOutMultZEMvsZDC),
  fHOutMultZEMvsZDCw(ana.fHOutMultZEMvsZDCw),
  fHOutMultV0MvsCL1(ana.fHOutMultV0MvsCL1),
  fHOutMultV0MvsTRK(ana.fHOutMultV0MvsTRK),
  fHOutMultTRKvsCL1(ana.fHOutMultTRKvsCL1),
  fHOutMultV0MvsV0O(ana.fHOutMultV0MvsV0O),
  fHOutMultV0OvsCL1(ana.fHOutMultV0OvsCL1),
  fHOutMultV0OvsTRK(ana.fHOutMultV0OvsTRK),
  fHOutCentV0Mqual1(ana.fHOutCentV0Mqual1),
  fHOutCentTRKqual1(ana.fHOutCentTRKqual1),
  fHOutCentCL1qual1(ana.fHOutCentCL1qual1),
  fHOutMultV0MvsCL1qual1(ana.fHOutMultV0MvsCL1qual1),
  fHOutMultV0MvsTRKqual1(ana.fHOutMultV0MvsTRKqual1),
  fHOutMultTRKvsCL1qual1(ana.fHOutMultTRKvsCL1qual1),
  fHOutCentV0Mqual2(ana.fHOutCentV0Mqual2),
  fHOutCentTRKqual2(ana.fHOutCentTRKqual2),
  fHOutCentCL1qual2(ana.fHOutCentCL1qual2),
  fHOutMultV0MvsCL1qual2(ana.fHOutMultV0MvsCL1qual2),
  fHOutMultV0MvsTRKqual2(ana.fHOutMultV0MvsTRKqual2),
  fHOutMultTRKvsCL1qual2(ana.fHOutMultTRKvsCL1qual2),
  fHOutQuality(ana.fHOutQuality),
  fHOutVertex(ana.fHOutVertex),
  fHOutVertexT0(ana.fHOutVertexT0)
{
  // Copy Constructor	

}

//________________________________________________________________________
AliCentralitySelectionTask::~AliCentralitySelectionTask()
{
  // Destructor  
  if (fOutputList && !AliAnalysisManager::GetAnalysisManager()->IsProofMode()) delete fOutputList;
  if (fTrackCuts) delete fTrackCuts;
}  

//________________________________________________________________________
void AliCentralitySelectionTask::UserCreateOutputObjects()
{  
  // Create the output containers
  if(fDebug>1) printf("AnalysisCentralitySelectionTask::UserCreateOutputObjects() \n");
  AliLog::SetClassDebugLevel("AliCentralitySelectionTask", AliLog::kInfo);

  if (fFillHistos) {    
    fOutputList = new TList();
    fOutputList->SetOwner();
    fHOutCentV0M     = new TH1F("fHOutCentV0M","fHOutCentV0M; Centrality V0",505,0,101);
    fHOutCentV0M_CVHN= new TH1F("fHOutCentV0M_CVHN","fHOutCentV0M_CVHN; Centrality V0",505,0,101);
    fHOutCentV0M_CVLN= new TH1F("fHOutCentV0M_CVLN","fHOutCentV0M_CVLN; Centrality V0",505,0,101);
    fHOutCentV0M_CVHNinMB= new TH1F("fHOutCentV0M_CVHNinMB","fHOutCentV0M_CVHN; Centrality V0",505,0,101);
    fHOutCentV0M_CVLNinMB= new TH1F("fHOutCentV0M_CVLNinMB","fHOutCentV0M_CVLN; Centrality V0",505,0,101);
    fHOutCentV0M_CCENT= new TH1F("fHOutCentV0M_CCENT","fHOutCentV0M_CCENT; Centrality V0",505,0,101);
    fHOutCentV0M_CSEMI= new TH1F("fHOutCentV0M_CSEMI","fHOutCentV0M_CSEMI; Centrality V0",505,0,101);
    fHOutCentV0M_CCENTinMB= new TH1F("fHOutCentV0M_CCENTinMB","fHOutCentV0M_CCENT; Centrality V0",505,0,101);
    fHOutCentV0M_CSEMIinMB= new TH1F("fHOutCentV0M_CSEMIinMB","fHOutCentV0M_CSEMI; Centrality V0",505,0,101);
  fHOutCentV0M_MSL= new TH1F("fHOutCentV0M_MSL","fHOutCentV0M_MSL; Centrality V0",505,0,101);
  fHOutCentV0M_MSH= new TH1F("fHOutCentV0M_MSH","fHOutCentV0M_MSH; Centrality V0",505,0,101);
  fHOutCentV0M_MUL= new TH1F("fHOutCentV0M_MUL","fHOutCentV0M_MUL; Centrality V0",505,0,101);
  fHOutCentV0M_MLL= new TH1F("fHOutCentV0M_MLL","fHOutCentV0M_MLL; Centrality V0",505,0,101);
  fHOutCentV0M_EJE= new TH1F("fHOutCentV0M_EJE","fHOutCentV0M_EJE; Centrality V0",505,0,101);
  fHOutCentV0M_EGA= new TH1F("fHOutCentV0M_EGA","fHOutCentV0M_EGA; Centrality V0",505,0,101);
  fHOutCentV0M_PHS= new TH1F("fHOutCentV0M_PHS","fHOutCentV0M_PHS; Centrality V0",505,0,101);
  fHOutCentV0M_MSLinMB= new TH1F("fHOutCentV0M_MSLinMB","fHOutCentV0M_MSLinMB; Centrality V0",505,0,101);
  fHOutCentV0M_MSHinMB= new TH1F("fHOutCentV0M_MSHinMB","fHOutCentV0M_MSHinMB; Centrality V0",505,0,101);
  fHOutCentV0M_MULinMB= new TH1F("fHOutCentV0M_MULinMB","fHOutCentV0M_MULinMB; Centrality V0",505,0,101);
  fHOutCentV0M_MLLinMB= new TH1F("fHOutCentV0M_MLLinMB","fHOutCentV0M_MLLinMB; Centrality V0",505,0,101);
  fHOutCentV0M_EJEinMB= new TH1F("fHOutCentV0M_EJEinMB","fHOutCentV0M_EJEinMB; Centrality V0",505,0,101);
  fHOutCentV0M_EGAinMB= new TH1F("fHOutCentV0M_EGAinMB","fHOutCentV0M_EGAinMB; Centrality V0",505,0,101);
  fHOutCentV0M_PHSinMB= new TH1F("fHOutCentV0M_PHSinMB","fHOutCentV0M_PHSinMB; Centrality V0",505,0,101);
  fHOutCentFMD     = new TH1F("fHOutCentFMD","fHOutCentFMD; Centrality FMD",505,0,101);
  fHOutCentTRK     = new TH1F("fHOutCentTRK","fHOutCentTRK; Centrality TPC",505,0,101);
  fHOutCentTKL     = new TH1F("fHOutCentTKL","fHOutCentTKL; Centrality tracklets",505,0,101);
  fHOutCentCL0     = new TH1F("fHOutCentCL0","fHOutCentCL0; Centrality SPD inner",505,0,101);
  fHOutCentCL1     = new TH1F("fHOutCentCL1","fHOutCentCL1; Centrality SPD outer",505,0,101);
  fHOutCentV0MvsFMD= new TH1F("fHOutCentV0MvsFMD","fHOutCentV0MvsFMD; Centrality V0 vs FMD",505,0,101);
  fHOutCentTKLvsV0M= new TH1F("fHOutCentTKLvsV0M","fHOutCentTKLvsV0M; Centrality tracklets vs V0",505,0,101);
  fHOutCentZEMvsZDC= new TH1F("fHOutCentZEMvsZDC","fHOutCentZEMvsZDC; Centrality ZEM vs ZDC",505,0,101);
  fHOutCentV0MvsCentCL1= new TH2F("fHOutCentV0MvsCentCL1","fHOutCentV0MvsCentCL1; Cent V0; Cent SPD",505,0,101,505,0,101);
  fHOutCentV0MvsCentTRK= new TH2F("fHOutCentV0MvsCentTRK","fHOutCentV0MvsCentTRK; Cent V0; Cent TPC",505,0,101,505,0,101);
  fHOutCentTRKvsCentCL1= new TH2F("fHOutCentTRKvsCentCL1","fHOutCentTRKvsCentCL1; Cent TPC; Cent SPD",505,0,101,505,0,101);
  fHOutCentV0MvsCentZDC= new TH2F("fHOutCentV0MvsCentZDC","fHOutCentV0MvsCentZDC; Cent V0; Cent ZDC",505,0,101,505,0,101);
  fHOutMultV0M = new TH1F("fHOutMultV0M","fHOutMultV0M; Multiplicity V0",25000,0,30000);
  fHOutMultV0O = new TH1F("fHOutMultV0O","fHOutMultV0O; Multiplicity V0",40000,0,40000);
  fHOutMultFMD = new TH1F("fHOutMultFMD","fHOutMultFMD; Multiplicity FMD",24000,0,24000);
  fHOutMultTRK = new TH1F("fHOutMultTRK","fHOutMultTRK; Multiplicity TPC",4000,0,4000);
  fHOutMultTKL = new TH1F("fHOutMultTKL","fHOutMultTKL; Multiplicity tracklets",5000,0,5000);
  fHOutMultCL0 = new TH1F("fHOutMultCL0","fHOutMultCL0; Multiplicity SPD inner",7000,0,7000);
  fHOutMultCL1 = new TH1F("fHOutMultCL1","fHOutMultCL1; Multiplicity SPD outer",7000,0,7000);
  fHOutMultV0MvsZDN = new TH2F("fHOutMultV0MvsZDN","fHOutMultV0MvsZDN; Multiplicity V0; Energy ZDC-N",500,0,30000,500,0,180000);
  fHOutMultZEMvsZDN = new TH2F("fHOutMultZEMvsZDN","fHOutMultZEMvsZDN; Energy ZEM; Energy ZDC-N",500,0,2500,500,0,180000);
  fHOutMultV0MvsZDC = new TH2F("fHOutMultV0MvsZDC","fHOutMultV0MvsZDC; Multiplicity V0; Energy ZDC",500,0,30000,500,0,200000);
  fHOutMultZEMvsZDC = new TH2F("fHOutMultZEMvsZDC","fHOutMultZEMvsZDC; Energy ZEM; Energy ZDC",500,0,2500,500,0,200000);
  fHOutMultZEMvsZDCw = new TH2F("fHOutMultZEMvsZDCw","fHOutMultZEMvsZDCw; Energy ZEM; Energy ZDC (weigthed with V0 percentile)",500,0,2500,500,0,200000);
  fHOutMultV0MvsCL1 = new TH2F("fHOutMultV0MvsCL1","fHOutMultV0MvsCL1; Multiplicity V0; Multiplicity SPD outer",2500,0,30000,700,0,7000);
  fHOutMultV0MvsTRK = new TH2F("fHOutMultV0MvsTRK","fHOutMultV0MvsTRK; Multiplicity V0; Multiplicity TPC",2500,0,30000,400,0,4000);
  fHOutMultTRKvsCL1 = new TH2F("fHOutMultTRKvsCL1","fHOutMultTRKvsCL1; Multiplicity TPC; Multiplicity SPD outer",400,0,4000,700,0,7000);
  fHOutMultV0MvsV0O = new TH2F("fHOutMultV0MvsV0O","fHOutMultV0MvsV0O; Multiplicity V0; Multiplicity V0 Online",500,0,30000,500,0,40000);
  fHOutMultV0OvsCL1 = new TH2F("fHOutMultV0OvsCL1","fHOutMultV0OvsCL1; Multiplicity V0; Multiplicity SPD outer",500,0,40000,700,0,7000);
  fHOutMultV0OvsTRK = new TH2F("fHOutMultV0OvsTRK","fHOutMultV0OvsTRK; Multiplicity V0; Multiplicity TPC",500,0,40000,400,0,4000);
  fHOutMultV0MvsV0O = new TH2F("fHOutMultV0MvsV0O","fHOutMultV0MvsV0O; Multiplicity V0; Multiplicity V0 Online",500,0,30000,500,0,30000);
  fHOutMultV0OvsCL1 = new TH2F("fHOutMultV0OvsCL1","fHOutMultV0OvsCL1; Multiplicity V0; Multiplicity SPD outer",2500,0,30000,700,0,7000);
  fHOutMultV0OvsTRK = new TH2F("fHOutMultV0OvsTRK","fHOutMultV0OvsTRK; Multiplicity V0; Multiplicity TPC",2500,0,30000,400,0,4000);
  
  fHOutCentV0Mqual1 = new TH1F("fHOutCentV0M_qual1","fHOutCentV0M_qual1; Centrality V0",505,0,101);
  fHOutCentTRKqual1 = new TH1F("fHOutCentTRK_qual1","fHOutCentTRK_qual1; Centrality TPC",505,0,101);
  fHOutCentCL1qual1 = new TH1F("fHOutCentCL1_qual1","fHOutCentCL1_qual1; Centrality SPD outer",505,0,101);
  fHOutMultV0MvsCL1qual1 = new TH2F("fHOutMultV0MvsCL1_qual1","fHOutMultV0MvsCL1_qual1; Multiplicity V0; Multiplicity SPD outer",2500,0,25000,700,0,7000);
  fHOutMultV0MvsTRKqual1 = new TH2F("fHOutMultV0MvsTRK_qual1","fHOutMultV0MvsTRK_qual1; Multiplicity V0; Multiplicity TPC",2500,0,25000,400,0,4000);
  fHOutMultTRKvsCL1qual1 = new TH2F("fHOutMultTRKvsCL1_qual1","fHOutMultTRKvsCL1_qual1; Multiplicity TPC; Multiplicity SPD outer",400,0,4000,700,0,7000);
  
  fHOutCentV0Mqual2 = new TH1F("fHOutCentV0M_qual2","fHOutCentV0M_qual2; Centrality V0",505,0,101);
  fHOutCentTRKqual2 = new TH1F("fHOutCentTRK_qual2","fHOutCentTRK_qual2; Centrality TPC",505,0,101);
  fHOutCentCL1qual2 = new TH1F("fHOutCentCL1_qual2","fHOutCentCL1_qual2; Centrality SPD outer",505,0,101);
  fHOutMultV0MvsCL1qual2 = new TH2F("fHOutMultV0MvsCL1_qual2","fHOutMultV0MvsCL1_qual2; Multiplicity V0; Multiplicity SPD outer",2500,0,25000,700,0,7000);
  fHOutMultV0MvsTRKqual2 = new TH2F("fHOutMultV0MvsTRK_qual2","fHOutMultV0MvsTRK_qual2; Multiplicity V0; Multiplicity TPC",2500,0,25000,400,0,4000);
  fHOutMultTRKvsCL1qual2 = new TH2F("fHOutMultTRKvsCL1_qual2","fHOutMultTRKvsCL1_qual2; Multiplicity TPC; Multiplicity SPD outer",400,0,4000,700,0,7000);
  
  fHOutQuality = new TH1F("fHOutQuality", "fHOutQuality", 100,-0.5,99.5);
  fHOutVertex  = new TH1F("fHOutVertex", "fHOutVertex", 100,-20,20);
  fHOutVertexT0  = new TH1F("fHOutVertexT0", "fHOutVertexT0", 100,-20,20);
  
  fOutputList->Add(  fHOutCentV0M     );
  fOutputList->Add(  fHOutCentV0M_CVHN);
  fOutputList->Add(  fHOutCentV0M_CVLN);
  fOutputList->Add(  fHOutCentV0M_CVHNinMB);
  fOutputList->Add(  fHOutCentV0M_CVLNinMB);
  fOutputList->Add(  fHOutCentV0M_CCENT);
  fOutputList->Add(  fHOutCentV0M_CSEMI);
  fOutputList->Add(  fHOutCentV0M_CCENTinMB);
  fOutputList->Add(  fHOutCentV0M_CSEMIinMB);
  fOutputList->Add(  fHOutCentV0M_MSL    );
  fOutputList->Add(  fHOutCentV0M_MSH    );
  fOutputList->Add(  fHOutCentV0M_MUL    );
  fOutputList->Add(  fHOutCentV0M_MLL    );
  fOutputList->Add(  fHOutCentV0M_EJE    );
  fOutputList->Add(  fHOutCentV0M_EGA    );
  fOutputList->Add(  fHOutCentV0M_PHS   );
  fOutputList->Add(  fHOutCentV0M_MSLinMB);
  fOutputList->Add(  fHOutCentV0M_MSHinMB);
  fOutputList->Add(  fHOutCentV0M_MULinMB);
  fOutputList->Add(  fHOutCentV0M_MLLinMB);
  fOutputList->Add(  fHOutCentV0M_EJEinMB);
  fOutputList->Add(  fHOutCentV0M_EGAinMB);
  fOutputList->Add(  fHOutCentV0M_PHSinMB);
  fOutputList->Add(  fHOutCentFMD     );
  fOutputList->Add(  fHOutCentTRK     );
  fOutputList->Add(  fHOutCentTKL     );
  fOutputList->Add(  fHOutCentCL0     );
  fOutputList->Add(  fHOutCentCL1     );
  fOutputList->Add(  fHOutCentV0MvsFMD);
  fOutputList->Add(  fHOutCentTKLvsV0M);
  fOutputList->Add(  fHOutCentZEMvsZDC);
  fOutputList->Add(  fHOutCentV0MvsCentCL1);
  fOutputList->Add(  fHOutCentV0MvsCentTRK);
  fOutputList->Add(  fHOutCentTRKvsCentCL1);
  fOutputList->Add(  fHOutCentV0MvsCentZDC);
  fOutputList->Add(  fHOutMultV0M); 
  fOutputList->Add(  fHOutMultV0O); 
  fOutputList->Add(  fHOutMultFMD); 
  fOutputList->Add(  fHOutMultTRK); 
  fOutputList->Add(  fHOutMultTKL); 
  fOutputList->Add(  fHOutMultCL0); 
  fOutputList->Add(  fHOutMultCL1); 
  fOutputList->Add(  fHOutMultV0MvsZDN);
  fOutputList->Add(  fHOutMultZEMvsZDN);
  fOutputList->Add(  fHOutMultV0MvsZDC);
  fOutputList->Add(  fHOutMultZEMvsZDC);
  fOutputList->Add(  fHOutMultZEMvsZDCw);
  fOutputList->Add(  fHOutMultV0MvsCL1);
  fOutputList->Add(  fHOutMultV0MvsTRK);
  fOutputList->Add(  fHOutMultTRKvsCL1);
  fOutputList->Add(  fHOutMultV0MvsV0O);
  fOutputList->Add(  fHOutMultV0OvsCL1);
  fOutputList->Add(  fHOutMultV0OvsTRK);
  fOutputList->Add(  fHOutCentV0Mqual1 );
  fOutputList->Add(  fHOutCentTRKqual1 );
  fOutputList->Add(  fHOutCentCL1qual1 );                   
  fOutputList->Add(  fHOutMultV0MvsCL1qual1);
  fOutputList->Add(  fHOutMultV0MvsTRKqual1);
  fOutputList->Add(  fHOutMultTRKvsCL1qual1);
  fOutputList->Add(  fHOutCentV0Mqual2 );
  fOutputList->Add(  fHOutCentTRKqual2 );
  fOutputList->Add(  fHOutCentCL1qual2 );
  fOutputList->Add(  fHOutMultV0MvsCL1qual2);
  fOutputList->Add(  fHOutMultV0MvsTRKqual2);
  fOutputList->Add(  fHOutMultTRKvsCL1qual2);
  fOutputList->Add(  fHOutQuality );
  fOutputList->Add(  fHOutVertex );
  fOutputList->Add(  fHOutVertexT0 );
  
  PostData(1, fOutputList); 
  }
  
  fTrackCuts = AliESDtrackCuts::GetStandardTPCOnlyTrackCuts();
  
  if (fPass==0) AliFatal("Which pass are you analyzing? You should set it via taskCentrality->SetPass(N)");
}

//________________________________________________________________________
void AliCentralitySelectionTask::UserExec(Option_t */*option*/)
{ 
  // Execute analysis for current event:
  if(fDebug>1) printf(" **** AliCentralitySelectionTask::UserExec() \n");

  Float_t  zncEnergy = 0.;          //  ZNC Energy
  Float_t  zpcEnergy = 0.;          //  ZPC Energy
  Float_t  znaEnergy = 0.;          //  ZNA Energy
  Float_t  zpaEnergy = 0.;          //  ZPA Energy
  Float_t  zem1Energy = 0.;         //  ZEM1 Energy
  Float_t  zem2Energy = 0.;         //  ZEM2 Energy
  Bool_t   zdcEnergyCal = kFALSE;   // if zdc is calibrated (in pass2)

  Int_t    nTracks = 0;             //  no. tracks
  Int_t    nTracklets = 0;          //  no. tracklets
  Int_t    nClusters[6] = {0};      //  no. clusters on 6 ITS layers
  Int_t    nChips[2];               //  no. chips on 2 SPD layers
  Float_t  spdCorr =0;              //  corrected spd2 multiplicity

  Float_t  multV0A  = 0;            //  multiplicity from V0 reco side A
  Float_t  multV0C  = 0;            //  multiplicity from V0 reco side C
  Short_t  multV0AOnline  = 0;      //  multiplicity from V0 reco side A
  Short_t  multV0COnline  = 0;      //  multiplicity from V0 reco side C
  Float_t  v0Corr = 0;               // corrected V0 multiplicity (used for MC)

  Float_t  multFMDA = 0;            //  multiplicity from FMD on detector A
  Float_t  multFMDC = 0;            //  multiplicity from FMD on detector C

  Float_t zvtx =0;                  // z-vertex SPD
  Int_t zvtxNcont =0;               // contributors to z-vertex SPD

  Float_t zvtxT0 =0;                // z-vertex T0


  AliCentrality *esdCent = 0;

  if(fAnalysisInput.CompareTo("ESD")==0){
  
    AliVEvent* event = InputEvent();
    AliESDEvent* esd = dynamic_cast<AliESDEvent*>(event);
    if (!esd) {
      AliError("No ESD Event");
      return;
    }
  
    LoadBranches();
    
    if (SetupRun(esd)<0) {
      AliError("Centrality File not available for this run");
      return;
    }
    
    esdCent = esd->GetCentrality();

    // ***** V0 info    
    AliESDVZERO* esdV0 = esd->GetVZEROData();
    multV0A=esdV0->GetMTotV0A();
    multV0C=esdV0->GetMTotV0C();
    v0Corr = multV0A+multV0C;

    multV0AOnline=esdV0->GetTriggerChargeA(); 
    multV0COnline=esdV0->GetTriggerChargeC(); 

    // ***** T0 info    
    const AliESDTZERO* esdT0 = esd->GetESDTZERO();
    if (!esdT0)
      {
	AliError("AliESDTZERO not available");
      }
    Int_t trig=esdT0->GetT0Trig();
    Bool_t kT0BB = kFALSE;    
    if(trig&1) kT0BB=kTRUE;
    zvtxT0=esdT0->GetT0zVertex();


    // ***** Trigger info    
    fIsSelected = ((esdV0->GetV0ADecision()==1) && (esdV0->GetV0CDecision()==1));
    TString trigStr(esd->GetFiredTriggerClasses());
    
    fCVHN=kFALSE; fCVLN=kFALSE; fCCENT=kFALSE; fCSEMI=kFALSE; 
    fMSL=kFALSE;  fMSH=kFALSE;  fMUL=kFALSE;   fMLL=kFALSE;
    fEJE=kFALSE;  fEGA=kFALSE;  fPHS=kFALSE;

    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CVHN")) && (fIsSelected)) 
      fCVHN=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CVLN")) && (fIsSelected))
      fCVLN=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CCENT")) && (fIsSelected)) 
      fCCENT=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CSEMI")) && (fIsSelected))
      fCSEMI=kTRUE;
    
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1MSL")) && (fIsSelected))
      fMSL=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1MSH")) && (fIsSelected))
      fMSH=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1MUL")) && (fIsSelected))
      fMUL=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1MLL")) && (fIsSelected))
      fMLL=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1EJE")) && (fIsSelected))
      fEJE=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1EGA")) && (fIsSelected))
      fEGA=kTRUE;
    if ( (trigStr.Contains("-B-")) &&  (trigStr.Contains("CPBI1PHS")) && (fIsSelected))
      fPHS=kTRUE;

    fCVHNbit=kFALSE;    fCVLNbit=kFALSE;       fCCENTbit=kFALSE;    fCSEMIbit=kFALSE; 
    if (esdV0->GetTriggerBits() & (1<<8)) 
      fCVHNbit=kTRUE;
    if (esdV0->GetTriggerBits() & (1<<6)) 
      fCVLNbit=kTRUE;
    
    if (kT0BB && fCVHNbit)
      fCCENTbit=kTRUE;
    if (kT0BB && fCVLNbit)
      fCSEMIbit=kTRUE;

    
    // ***** Vertex Info
    const AliESDVertex* vtxESD = esd->GetPrimaryVertexSPD();
    zvtx        = vtxESD->GetZ(); 
    zvtxNcont   = vtxESD->GetNContributors();

    // ***** CB info (tracklets, clusters, chips)
    //nTracks    = event->GetNumberOfTracks();     
    nTracks    = fTrackCuts ? (Short_t)fTrackCuts->GetReferenceMultiplicity(esd,kTRUE):-1;

    const AliMultiplicity *mult = esd->GetMultiplicity();

    nTracklets = mult->GetNumberOfTracklets();

    for(Int_t ilay=0; ilay<6; ilay++){
      nClusters[ilay] = mult->GetNumberOfITSClusters(ilay);
    }

    for(Int_t ilay=0; ilay<2; ilay++){
      nChips[ilay] = mult->GetNumberOfFiredChips(ilay);
    }

    spdCorr = AliESDUtils::GetCorrSPD2(nClusters[1],zvtx);    

    // ***** FMD info
    AliESDFMD *fmd = esd->GetFMDData();
    Float_t totalMultA = 0;
    Float_t totalMultC = 0;
    const Float_t fFMDLowCut = 0.4;
  
    for(UShort_t det=1;det<=3;det++) {
      Int_t nRings = (det==1 ? 1 : 2);
      for (UShort_t ir = 0; ir < nRings; ir++) {	  
 	Char_t   ring = (ir == 0 ? 'I' : 'O');
 	UShort_t nsec = (ir == 0 ? 20  : 40);
 	UShort_t nstr = (ir == 0 ? 512 : 256);
 	for(UShort_t sec =0; sec < nsec;  sec++)  {
 	  for(UShort_t strip = 0; strip < nstr; strip++) {
	    
 	    Float_t fmdMult = fmd->Multiplicity(det,ring,sec,strip);
 	    if(fmdMult == 0 || fmdMult == AliESDFMD::kInvalidMult) continue;
	    
 	    Float_t nParticles=0;
	    
 	    if(fmdMult > fFMDLowCut) {
 	      nParticles = 1.;
 	    }
	    
 	    if (det<3) totalMultA = totalMultA + nParticles;
 	    else totalMultC = totalMultC + nParticles;
	    
 	  }
 	}
      }
    }
    multFMDA = totalMultA;
    multFMDC = totalMultC;
  
    // ***** ZDC info
    AliESDZDC *esdZDC = esd->GetESDZDC();
    zdcEnergyCal = esdZDC->AliESDZDC::TestBit(AliESDZDC::kEnergyCalibratedSignal);
    if (zdcEnergyCal) {      
      zncEnergy = (Float_t) (esdZDC->GetZDCN1Energy());
      zpcEnergy = (Float_t) (esdZDC->GetZDCP1Energy());
      znaEnergy = (Float_t) (esdZDC->GetZDCN2Energy());
      zpaEnergy = (Float_t) (esdZDC->GetZDCP2Energy());
    } else {
      zncEnergy = (Float_t) (esdZDC->GetZDCN1Energy())/8.;
      zpcEnergy = (Float_t) (esdZDC->GetZDCP1Energy())/8.;
      znaEnergy = (Float_t) (esdZDC->GetZDCN2Energy())/8.;
      zpaEnergy = (Float_t) (esdZDC->GetZDCP2Energy())/8.;
    }
    zem1Energy = (Float_t) (esdZDC->GetZDCEMEnergy(0))/8.;
    zem2Energy = (Float_t) (esdZDC->GetZDCEMEnergy(1))/8.;
    
  }   

  else if(fAnalysisInput.CompareTo("AOD")==0){
    //AliAODEvent *aod =  dynamic_cast<AliAODEvent*> (InputEvent());
    // to be implemented
    printf("  AOD analysis not yet implemented!!!\n\n");
    return;
  } 

  // ***** Scaling for MC
  if (fIsMCInput) {
    fUseScaling=kFALSE;
    v0Corr  = Short_t((multV0A+multV0C)  * fV0MScaleFactorMC);
  }
  // ***** Scaling for Data 
  if (fUseScaling) {
    v0Corr  = Short_t(v0Corr /   fV0MScaleFactor);
    spdCorr = spdCorr / fSPDScaleFactor;
    nTracks = Int_t(nTracks /   fTPCScaleFactor);
  }

  // ***** Centrality Selection
  if(fHtempV0M) fCentV0M = fHtempV0M->GetBinContent(fHtempV0M->FindBin((v0Corr)));
  if(fHtempFMD) fCentFMD = fHtempFMD->GetBinContent(fHtempFMD->FindBin((multFMDA+multFMDC)));
  if(fHtempTRK) fCentTRK = fHtempTRK->GetBinContent(fHtempTRK->FindBin(nTracks));
  if(fHtempTKL) fCentTKL = fHtempTKL->GetBinContent(fHtempTKL->FindBin(nTracklets));
  if(fHtempCL0) fCentCL0 = fHtempCL0->GetBinContent(fHtempCL0->FindBin(nClusters[0]));
  if(fHtempCL1) fCentCL1 = fHtempCL1->GetBinContent(fHtempCL1->FindBin(spdCorr));

  if(fHtempV0MvsFMD) fCentV0MvsFMD = fHtempV0MvsFMD->GetBinContent(fHtempV0MvsFMD->FindBin((multV0A+multV0C)));
  if(fHtempTKLvsV0M) fCentTKLvsV0M = fHtempTKLvsV0M->GetBinContent(fHtempTKLvsV0M->FindBin(nTracklets));
  if(fHtempZEMvsZDC) fCentZEMvsZDC = fHtempZEMvsZDC->GetBinContent(fHtempZEMvsZDC->FindBin(zem1Energy+zem2Energy,zncEnergy+znaEnergy+zpcEnergy+zpaEnergy));


  // ***** Cleaning
  if (fUseCleaning) {
    fQuality=0;
    
    // ***** vertex
    if (TMath::Abs(zvtx)>fZVCut || zvtxNcont<1) fQuality += 1;   

    // ***** outliers, skip in case of MC input
    if (!fIsMCInput) {
      // **** V0 vs SPD
      if (IsOutlierV0MSPD(spdCorr, v0Corr, int(fCentV0M))) fQuality  += 2;
      // ***** V0 vs TPC
      if (IsOutlierV0MTPC(nTracks, v0Corr, int(fCentV0M))) fQuality  += 4;
      // ***** V0 vs ZDC
      if (IsOutlierV0MZDC((zncEnergy+znaEnergy+zpcEnergy+zpaEnergy), v0Corr) &&
	  (zdcEnergyCal==kFALSE) ) fQuality  += 8;
      if (IsOutlierV0MZDCECal((zncEnergy+znaEnergy+zpcEnergy+zpaEnergy), v0Corr) &&
	  (zdcEnergyCal==kTRUE) ) fQuality  += 8;
    }
  } else {
    fQuality = 0;
  }

      
  if (esdCent) {
    esdCent->SetQuality(fQuality);
    esdCent->SetCentralityV0M(fCentV0M);
    esdCent->SetCentralityFMD(fCentFMD);
    esdCent->SetCentralityTRK(fCentTRK);
    esdCent->SetCentralityTKL(fCentTKL);
    esdCent->SetCentralityCL0(fCentCL0);
    esdCent->SetCentralityCL1(fCentCL1);
    esdCent->SetCentralityV0MvsFMD(fCentV0MvsFMD);
    esdCent->SetCentralityTKLvsV0M(fCentTKLvsV0M);
    esdCent->SetCentralityZEMvsZDC(fCentZEMvsZDC);
  }

  // filling QA histograms
  if (fFillHistos) {    
    if (fCVHN)   fHOutCentV0M_CVHN->Fill(fCentV0M);
    if (fCVLN)   fHOutCentV0M_CVLN->Fill(fCentV0M);
    if (fCCENT)  fHOutCentV0M_CCENT->Fill(fCentV0M);
    if (fCSEMI)  fHOutCentV0M_CSEMI->Fill(fCentV0M);
    if (fMSL) fHOutCentV0M_MSL->Fill(fCentV0M);
    if (fMSH) fHOutCentV0M_MSH->Fill(fCentV0M);
    if (fMUL) fHOutCentV0M_MUL->Fill(fCentV0M);
    if (fMLL) fHOutCentV0M_MLL->Fill(fCentV0M);
    if (fEJE) fHOutCentV0M_EJE->Fill(fCentV0M);
    if (fEGA) fHOutCentV0M_EGA->Fill(fCentV0M);
    if (fPHS) fHOutCentV0M_PHS->Fill(fCentV0M);

    if (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & AliVEvent::kMB) { // fill the QA histograms only for MB events!
      fHOutQuality->Fill(fQuality);
      fHOutVertex->Fill(zvtx);
      fHOutVertexT0->Fill(zvtxT0);
      
      if (fQuality==0) {  
	fHOutCentV0M->Fill(fCentV0M);
	
	if (fCVHNbit)  fHOutCentV0M_CVHNinMB->Fill(fCentV0M);
	if (fCVLNbit)  fHOutCentV0M_CVLNinMB->Fill(fCentV0M);
	if (fCCENTbit) fHOutCentV0M_CCENTinMB->Fill(fCentV0M);
	if (fCSEMIbit) fHOutCentV0M_CSEMIinMB->Fill(fCentV0M);
	if (fMSL) fHOutCentV0M_MSLinMB->Fill(fCentV0M);
	if (fMSH) fHOutCentV0M_MSHinMB->Fill(fCentV0M);
	if (fMUL) fHOutCentV0M_MULinMB->Fill(fCentV0M);
	if (fMLL) fHOutCentV0M_MLLinMB->Fill(fCentV0M);
	if (fEJE) fHOutCentV0M_EJEinMB->Fill(fCentV0M);
	if (fEGA) fHOutCentV0M_EGAinMB->Fill(fCentV0M);
	if (fPHS) fHOutCentV0M_PHSinMB->Fill(fCentV0M);
       
	
	fHOutCentFMD->Fill(fCentFMD);
	fHOutCentTRK->Fill(fCentTRK);
	fHOutCentTKL->Fill(fCentTKL);
	fHOutCentCL0->Fill(fCentCL0);
	fHOutCentCL1->Fill(fCentCL1);
	fHOutCentV0MvsFMD->Fill(fCentV0MvsFMD);
	fHOutCentTKLvsV0M->Fill(fCentTKLvsV0M);
	fHOutCentZEMvsZDC->Fill(fCentZEMvsZDC);
	fHOutCentV0MvsCentCL1->Fill(fCentV0M,fCentCL1);
	fHOutCentV0MvsCentTRK->Fill(fCentV0M,fCentTRK);
	fHOutCentTRKvsCentCL1->Fill(fCentTRK,fCentCL1);
	fHOutCentV0MvsCentZDC->Fill(fCentV0M,fCentZEMvsZDC);
	fHOutMultV0M->Fill(multV0A+multV0C);
	fHOutMultV0O->Fill(multV0AOnline+multV0COnline);
	fHOutMultFMD->Fill(multFMDA+multFMDC);
	fHOutMultTRK->Fill(nTracks);
	fHOutMultTKL->Fill(nTracklets);
	fHOutMultCL0->Fill(nClusters[0]);
	fHOutMultCL1->Fill(spdCorr);
	fHOutMultV0MvsZDN->Fill(v0Corr,(zncEnergy+znaEnergy));
	fHOutMultZEMvsZDN->Fill((zem1Energy+zem2Energy),(zncEnergy+znaEnergy));
	fHOutMultV0MvsZDC->Fill(v0Corr,(zncEnergy+znaEnergy+zpcEnergy+zpaEnergy));
	fHOutMultZEMvsZDC->Fill((zem1Energy+zem2Energy),(zncEnergy+znaEnergy+zpcEnergy+zpaEnergy));
	fHOutMultZEMvsZDCw->Fill((zem1Energy+zem2Energy),(zncEnergy+znaEnergy+zpcEnergy+zpaEnergy),fCentV0M);
	fHOutMultV0MvsCL1->Fill(v0Corr,spdCorr);
	fHOutMultV0MvsTRK->Fill(v0Corr,nTracks);
	fHOutMultTRKvsCL1->Fill(nTracks,spdCorr);
	fHOutMultV0MvsV0O->Fill(v0Corr,(multV0AOnline+multV0COnline));
	fHOutMultV0OvsCL1->Fill((multV0AOnline+multV0COnline),spdCorr);
	fHOutMultV0OvsTRK->Fill((multV0AOnline+multV0COnline),nTracks);
      } else if (fQuality%2 == 0) {
	fHOutCentV0Mqual1->Fill(fCentV0M);
	fHOutCentTRKqual1->Fill(fCentTRK);
	fHOutCentCL1qual1->Fill(fCentCL1);
	fHOutMultV0MvsCL1qual1->Fill(v0Corr,spdCorr);
	fHOutMultV0MvsTRKqual1->Fill(v0Corr,nTracks);
	fHOutMultTRKvsCL1qual1->Fill(nTracks,spdCorr);
      } else {
	fHOutCentV0Mqual2->Fill(fCentV0M);
	fHOutCentTRKqual2->Fill(fCentTRK);
	fHOutCentCL1qual2->Fill(fCentCL1);
	fHOutMultV0MvsCL1qual2->Fill(v0Corr,spdCorr);
	fHOutMultV0MvsTRKqual2->Fill(v0Corr,nTracks);
	fHOutMultTRKvsCL1qual2->Fill(nTracks,spdCorr);
      }
    }
    PostData(1, fOutputList); 
  }
}
//________________________________________________________________________
void AliCentralitySelectionTask::Terminate(Option_t */*option*/)
{
  // Terminate analysis
}
//________________________________________________________________________
Int_t AliCentralitySelectionTask::SetupRun(AliESDEvent* const esd)
{
  // Setup files for run

  if (!esd)
    return -1;

  // check if something to be done
  if (fCurrentRun == esd->GetRunNumber())
    return 0;
  else
    fCurrentRun = esd->GetRunNumber();

  TString fileName =(Form("%s/COMMON/CENTRALITY/data/centrality.root", AliAnalysisManager::GetOADBPath()));
  AliInfo(Form("Setup Centrality Selection for run %d with file %s\n",fCurrentRun,fileName.Data()));

  AliOADBContainer *con = new AliOADBContainer("OADB");
  con->InitFromFile(fileName,"Centrality");

  AliOADBCentrality*  centOADB = 0;
  centOADB = (AliOADBCentrality*)(con->GetObject(fCurrentRun));
  if (!centOADB) {
    AliWarning(Form("Centrality OADB does not exist for run %d, using Default \n",fCurrentRun ));
    centOADB  = (AliOADBCentrality*)(con->GetDefaultObject("oadbDefault"));
  }

  // modes
  fUseScaling   = centOADB->UseScaling();
  fUseCleaning  = centOADB->UseCleaning();

  // cuts
  fZVCut        = centOADB->ZVCut();
  fOutliersCut  = centOADB->OutliersCut(); 

  // centrality histos
  fHtempV0M       = centOADB->V0hist(); 
  fHtempTRK       = centOADB->TPChist();
  fHtempCL1       = centOADB->SPDhist();
  fHtempZEMvsZDC  = centOADB->ZEMvsZDChist();
  
  TString path = gSystem->ExpandPathName(fileName.Data());
  if (!fHtempV0M) AliWarning(Form("Calibration for V0M does not exist in %s", path.Data()));
  if (!fHtempTRK) AliWarning(Form("Calibration for TRK does not exist in %s", path.Data()));
  if (!fHtempCL1) AliWarning(Form("Calibration for CL1 does not exist in %s", path.Data()));
  if (!fHtempZEMvsZDC) AliWarning(Form("Calibration for ZEMvsZDC does not exist in %s", path.Data()));

  // scale factors
  fV0MScaleFactor    = centOADB->V0MScaleFactor();
  fSPDScaleFactor    = centOADB->SPDScaleFactor();
  fTPCScaleFactor    = centOADB->TPCScaleFactor();
  fV0MScaleFactorMC  = centOADB->V0MScaleFactor();

  // outliers parameters
  fV0MSPDOutlierPar0 = centOADB->V0MSPDOutlierPar0();      
  fV0MSPDOutlierPar1 = centOADB->V0MSPDOutlierPar1();     
  fV0MTPCOutlierPar0 = centOADB->V0MTPCOutlierPar0();      
  fV0MTPCOutlierPar1 = centOADB->V0MTPCOutlierPar1();     
  			   			   
  fV0MSPDSigmaOutlierPar0 = centOADB->V0MSPDSigmaOutlierPar0(); 
  fV0MSPDSigmaOutlierPar1 = centOADB->V0MSPDSigmaOutlierPar1(); 
  fV0MSPDSigmaOutlierPar2 = centOADB->V0MSPDSigmaOutlierPar2();
  fV0MTPCSigmaOutlierPar0 = centOADB->V0MTPCSigmaOutlierPar0(); 
  fV0MTPCSigmaOutlierPar1 = centOADB->V0MTPCSigmaOutlierPar1(); 
  fV0MTPCSigmaOutlierPar2 = centOADB->V0MTPCSigmaOutlierPar2(); 
  			    
  fV0MZDCOutlierPar0 =      centOADB->V0MZDCOutlierPar0();      
  fV0MZDCOutlierPar1 =      centOADB->V0MZDCOutlierPar1();      
  fV0MZDCEcalOutlierPar0 =  centOADB->V0MZDCEcalOutlierPar0();  
  fV0MZDCEcalOutlierPar1 =  centOADB->V0MZDCEcalOutlierPar1();  



  return 0;
}



//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MSPD(Float_t spd, Float_t v0, Int_t cent) const
{
  // Clean outliers
  Float_t val = fV0MSPDOutlierPar0 +  fV0MSPDOutlierPar1 * v0;
  Float_t spdSigma = fV0MSPDSigmaOutlierPar0 + fV0MSPDSigmaOutlierPar1*cent + fV0MSPDSigmaOutlierPar2*cent*cent;
  if ( TMath::Abs(spd-val) > fOutliersCut*spdSigma ) 
    return kTRUE;
  else 
    return kFALSE;
}

//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MTPC(Int_t tracks, Float_t v0, Int_t cent) const
{
  // Clean outliers
  Float_t val = fV0MTPCOutlierPar0 +  fV0MTPCOutlierPar1 * v0;
  Float_t tpcSigma = fV0MTPCSigmaOutlierPar0 + fV0MTPCSigmaOutlierPar1*cent + fV0MTPCSigmaOutlierPar2*cent*cent;
  if ( TMath::Abs(tracks-val) > fOutliersCut*tpcSigma ) 
    return kTRUE;
  else 
    return kFALSE;
}

//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MZDC(Float_t zdc, Float_t v0) const
{
  // Clean outliers
  Float_t val = fV0MZDCOutlierPar0 + fV0MZDCOutlierPar1 * v0;
  if (zdc >  val) 
    return kTRUE;
  else 
  return kFALSE;
}

//________________________________________________________________________
Bool_t AliCentralitySelectionTask::IsOutlierV0MZDCECal(Float_t zdc, Float_t v0) const
{
  // Clean outliers
  Float_t val = fV0MZDCEcalOutlierPar0 + fV0MZDCEcalOutlierPar1 * v0;
  if (zdc >  val) 
    return kTRUE;
  else 
    return kFALSE;
}


