#include "AliAnalysisEt.h"
#include "TMath.h"
#include "TList.h"
#include "TH1F.h"
#include "TH2F.h"
#include <iostream>
#include "AliAnalysisEtCuts.h"
#include "AliVEvent.h"

using namespace std;
ClassImp(AliAnalysisEt);


AliAnalysisEt::AliAnalysisEt() :
        fHistogramNameSuffix("")
        ,fPdgDB(0)
        ,fPiPlusCode(0)
        ,fPiMinusCode(0)
        ,fKPlusCode(0)
        ,fKMinusCode(0)
        ,fProtonCode(0)
        ,fAntiProtonCode(0)
        ,fLambdaCode(0)
        ,fAntiLambdaCode(0)
        ,fK0SCode(0)
        ,fOmegaCode(0)
        ,fAntiOmegaCode(0)
        ,fXi0Code(0)
        ,fAntiXi0Code(0)
        ,fXiCode(0)
        ,fAntiXiCode(0)
        ,fSigmaCode(0)
        ,fAntiSigmaCode(0)
        ,fK0LCode(0)
        ,fNeutronCode(0)
        ,fAntiNeutronCode(0)
        ,fEPlusCode(0)
        ,fEMinusCode(0)
        ,fPionMass(0)
        ,fSumEt(0)
        ,fSumEtAcc(0)
        ,fTotEt(0)
        ,fTotEtAcc(0)
        ,fTotNeutralEt(0)
        ,fTotNeutralEtAcc(0)
        ,fTotChargedEt(0)
        ,fTotChargedEtAcc(0)
        ,fMultiplicity(0)
        ,fChargedMultiplicity(0)
        ,fNeutralMultiplicity(0)
        ,fEtaCut(EtCommonCuts::kEtaCut)
        ,fEtaCutAcc(0)
        ,fPhiCutAccMin(0)
        ,fPhiCutAccMax(360.)
        ,fDetectorRadius(460.)
        ,fVertexXCut(0)
        ,fVertexYCut(0)
        ,fVertexZCut(0)
        ,fIPxyCut(0)
        ,fIPzCut(0)
        ,fClusterEnergyCut(EtCommonCuts::kClusterEnergyCut)
        ,fTrackPtCut(EtCommonCuts::kTrackPtCut)
        ,fSingleCellEnergyCut(0)
        ,fHistEt(0)
        ,fHistChargedEt(0)
        ,fHistNeutralEt(0)
        ,fHistEtAcc(0)
        ,fHistChargedEtAcc(0)
        ,fHistNeutralEtAcc(0)
        ,fHistMult(0)
        ,fHistChargedMult(0)
        ,fHistNeutralMult(0)
        ,fHistPhivsPtPos(0)
        ,fHistPhivsPtNeg(0)
        ,fHistBaryonEt(0)
        ,fHistAntiBaryonEt(0)
        ,fHistMesonEt(0)
        ,fHistBaryonEtAcc(0)
        ,fHistAntiBaryonEtAcc(0)
        ,fHistMesonEtAcc(0)
        ,fHistEtRecvsEtMC(0)
        ,fHistTMDeltaR(0)
{

}

AliAnalysisEt::~AliAnalysisEt()
{

}

void AliAnalysisEt::FillOutputList(TList *list)
{
    list->Add(fHistEt);
    list->Add(fHistChargedEt);
    list->Add(fHistNeutralEt);

    list->Add(fHistEtAcc);
    list->Add(fHistChargedEtAcc);
    list->Add(fHistNeutralEtAcc);

    list->Add(fHistMult);
    list->Add(fHistChargedMult);
    list->Add(fHistNeutralMult);

    list->Add(fHistPhivsPtPos);
    list->Add(fHistPhivsPtNeg);

    list->Add(fHistBaryonEt);
    list->Add(fHistAntiBaryonEt);
    list->Add(fHistMesonEt);

    list->Add(fHistBaryonEtAcc);
    list->Add(fHistAntiBaryonEtAcc);
    list->Add(fHistMesonEtAcc);

    list->Add(fHistEtRecvsEtMC);

    list->Add(fHistTMDeltaR);
}

void AliAnalysisEt::Init()
{

  if(!fPdgDB) fPdgDB = new TDatabasePDG();
  SetParticleCodes();
}

void AliAnalysisEt::CreateHistograms()
{
  // histogram binning for E_T, p_T and Multiplicity: defaults for p+p
  Int_t nbinsEt = 1000;
  Double_t minEt = 0.0001;
  Double_t maxEt = 100;
  Int_t nbinsPt = 200;
  Double_t minPt = 0;
  Double_t maxPt = 20;
  Int_t nbinsMult = 200;
  Double_t minMult = -0.5; // offset -0.5 to have integer bins centered around 0
  Double_t maxMult = nbinsMult + minMult; // 1 bin per integer value

    TString histname = "fHistEt" + fHistogramNameSuffix;
    fHistEt = new TH1F(histname.Data(), "Total E_{T} Distribution", nbinsEt, minEt, maxEt);
    fHistEt->GetXaxis()->SetTitle("E_{T} (GeV/c^{2})");
    fHistEt->GetYaxis()->SetTitle("dN/dE_{T} (c^{2}/GeV)");

    histname = "fHistChargedEt" + fHistogramNameSuffix;
    fHistChargedEt = new TH1F(histname.Data(), "Total Charged E_{T} Distribution", nbinsEt, minEt, maxEt);
    fHistChargedEt->GetXaxis()->SetTitle("E_{T} (GeV/c^{2})");
    fHistChargedEt->GetYaxis()->SetTitle("dN/dE_{T} (c^{2}/GeV)");

    histname = "fHistNeutralEt" + fHistogramNameSuffix;
    fHistNeutralEt = new TH1F(histname.Data(), "Total Neutral E_{T} Distribution", nbinsEt, minEt, maxEt);
    fHistNeutralEt->GetXaxis()->SetTitle("E_{T} (GeV/c^{2})");
    fHistNeutralEt->GetYaxis()->SetTitle("dN/dE_{T} (c^{2}/GeV)");

    histname = "fHistEtAcc" + fHistogramNameSuffix;
    fHistEtAcc = new TH1F(histname.Data(), "Total E_{T} Distribution in Acceptance", nbinsEt, minEt, maxEt);
    fHistEtAcc->GetXaxis()->SetTitle("E_{T} (GeV/c^{2})");
    fHistEtAcc->GetYaxis()->SetTitle("dN/dE_{T} (c^{2}/GeV)");

    histname = "fHistChargedEtAcc" + fHistogramNameSuffix;
    fHistChargedEtAcc = new TH1F(histname.Data(), "Total Charged E_{T} Distribution in Acceptance", nbinsEt, minEt, maxEt);
    fHistChargedEtAcc->GetXaxis()->SetTitle("E_{T} (GeV/c^{2})");
    fHistChargedEtAcc->GetYaxis()->SetTitle("dN/dE_{T} (c^{2}/GeV)");

    histname = "fHistNeutralEtAcc" + fHistogramNameSuffix;
    fHistNeutralEtAcc = new TH1F(histname.Data(), "Total Neutral E_{T} Distribution in Acceptance", nbinsEt, minEt, maxEt);
    fHistNeutralEtAcc->GetXaxis()->SetTitle("E_{T} (GeV/c^{2})");
    fHistNeutralEtAcc->GetYaxis()->SetTitle("dN/dE_{T} (c^{2}/GeV)");
    std::cout << histname << std::endl;
    histname = "fHistMult" + fHistogramNameSuffix;
    fHistMult = new TH1F(histname.Data(), "Total Multiplicity", nbinsMult, minMult, maxMult);
    fHistMult->GetXaxis()->SetTitle("N");
    fHistMult->GetYaxis()->SetTitle("Multiplicity");

    histname = "fHistChargedMult" + fHistogramNameSuffix;
    fHistChargedMult = new TH1F(histname.Data(), "Charged Multiplicity", nbinsMult, minMult, maxMult);
    fHistChargedMult->GetXaxis()->SetTitle("N");
    fHistChargedMult->GetYaxis()->SetTitle("Multiplicity");

    histname = "fHistNeutralMult" + fHistogramNameSuffix;
    fHistNeutralMult = new TH1F(histname.Data(), "Neutral Multiplicity", nbinsMult, minMult, maxMult);
    fHistNeutralMult->GetXaxis()->SetTitle("N");
    fHistNeutralMult->GetYaxis()->SetTitle("Multiplicity");

    histname = "fHistPhivsPtPos" + fHistogramNameSuffix;
    fHistPhivsPtPos = new TH2F(histname.Data(), "Phi vs pT of positively charged tracks hitting the calorimeter", 	200, 0, 2*TMath::Pi(), nbinsPt, minPt, maxPt);

    histname = "fHistPhivsPtNeg" + fHistogramNameSuffix;
    fHistPhivsPtNeg = new TH2F(histname.Data(), "Phi vs pT of negatively charged tracks hitting the calorimeter", 	200, 0, 2*TMath::Pi(), nbinsPt, minPt, maxPt);

    histname = "fHistBaryonEt" + fHistogramNameSuffix;
    fHistBaryonEt = new TH1F(histname.Data(), "E_{T} for baryons",  nbinsEt, minEt, maxEt);

    histname = "fHistAntiBaryonEt" + fHistogramNameSuffix;
    fHistAntiBaryonEt = new TH1F(histname.Data(), "E_{T} for anti baryons",  nbinsEt, minEt, maxEt);

    histname = "fHistMesonEt" + fHistogramNameSuffix;
    fHistMesonEt = new TH1F(histname.Data(), "E_{T} for mesons",  nbinsEt, minEt, maxEt);

    histname = "fHistBaryonEtAcc" + fHistogramNameSuffix;
    fHistBaryonEtAcc = new TH1F(histname.Data(), "E_{T} for baryons in calorimeter acceptance",  nbinsEt, minEt, maxEt);

    histname = "fHistAntiBaryonEtAcc" + fHistogramNameSuffix;
    fHistAntiBaryonEtAcc = new TH1F(histname.Data(), "E_{T} for anti baryons in calorimeter acceptance",  nbinsEt, minEt, maxEt);

    histname = "fHistMesonEtAcc" + fHistogramNameSuffix;
    fHistMesonEtAcc = new TH1F(histname.Data(), "E_{T} for mesons in calorimeter acceptance",  nbinsEt, minEt, maxEt);

    histname = "fHistEtRecvsEtMC" + fHistogramNameSuffix;
    fHistEtRecvsEtMC = new TH2F(histname.Data(), "Reconstructed E_{t} vs MC E_{t}", nbinsEt, minEt, maxEt, nbinsEt, minEt, maxEt);

    histname = "fHistTMDeltaR" + fHistogramNameSuffix;
    fHistTMDeltaR = new TH1F(histname.Data(), "#Delta R for calorimeter clusters", 200, 0, 50);

}

void AliAnalysisEt::FillHistograms()
{
    fHistEt->Fill(fTotEt);
    fHistChargedEt->Fill(fTotChargedEt);
    fHistNeutralEt->Fill(fTotNeutralEt);

    fHistEtAcc->Fill(fTotEtAcc);
    fHistChargedEtAcc->Fill(fTotChargedEtAcc);
    fHistNeutralEtAcc->Fill(fTotNeutralEtAcc);

    fHistMult->Fill(fMultiplicity);
    fHistChargedMult->Fill(fChargedMultiplicity);
    fHistNeutralMult->Fill(fNeutralMultiplicity);

    /* // DS commented out non-fills to prevent compilation warnings
    fHistPhivsPtPos;
    fHistPhivsPtNeg;

    fHistBaryonEt;
    fHistAntiBaryonEt;
    fHistMesonEt;

    fHistBaryonEtAcc;
    fHistAntiBaryonEtAcc;
    fHistMesonEtAcc;

    fHistTMDeltaR;
    */
}
Int_t AliAnalysisEt::AnalyseEvent(AliVEvent *event)
{
  //this line is basically here to eliminate a compiler warning that event is not used.  Making it a virtual function did not work with the plugin.
  cout<<"This event has "<<event->GetNumberOfTracks()<<" tracks"<<endl;
  return 0;
}

void AliAnalysisEt::ResetEventValues()
{
    fTotEt = 0;
    fTotEtAcc = 0;
    fTotNeutralEt = 0;
    fTotNeutralEtAcc = 0;
    fTotChargedEt  = 0;
    fTotChargedEtAcc = 0;
    fMultiplicity = 0;
    fChargedMultiplicity = 0;
    fNeutralMultiplicity = 0;

    if(!fPdgDB) fPdgDB = new TDatabasePDG();

    if(fPiPlusCode==0){
      SetParticleCodes();
    }
}

void AliAnalysisEt::SetParticleCodes()
{     
  fPionMass = fPdgDB->GetParticle("pi+")->Mass();
  fPiPlusCode = fPdgDB->GetParticle("pi+")->PdgCode();
  fPiMinusCode = fPdgDB->GetParticle("pi-")->PdgCode();
  fKPlusCode = fPdgDB->GetParticle("K+")->PdgCode();
  fKMinusCode = fPdgDB->GetParticle("K-")->PdgCode();
  fProtonCode = fPdgDB->GetParticle("proton")->PdgCode();
  fAntiProtonCode = fPdgDB->GetParticle("antiproton")->PdgCode();
  fLambdaCode = fPdgDB->GetParticle("Lambda0")->PdgCode();
  fAntiLambdaCode = fPdgDB->GetParticle("Lambda0_bar")->PdgCode();
  fK0SCode = fPdgDB->GetParticle("K_S0")->PdgCode();
  fOmegaCode = fPdgDB->GetParticle("Omega-")->PdgCode();
  fAntiOmegaCode = fPdgDB->GetParticle("Omega+")->PdgCode();
  fXi0Code = fPdgDB->GetParticle("Xi0")->PdgCode();
  fAntiXi0Code = fPdgDB->GetParticle("Xi0_bar")->PdgCode();
  fXiCode = fPdgDB->GetParticle("Xi-")->PdgCode();
  fAntiXiCode = fPdgDB->GetParticle("Xi-_bar")->PdgCode();
  fSigmaCode = fPdgDB->GetParticle("Sigma-")->PdgCode();
  fAntiSigmaCode = fPdgDB->GetParticle("Sigma+")->PdgCode();
  fK0LCode = fPdgDB->GetParticle("K_L0")->PdgCode();
  fNeutronCode = fPdgDB->GetParticle("neutron")->PdgCode();
  fAntiNeutronCode = fPdgDB->GetParticle("antineutron")->PdgCode();
  fEPlusCode = fPdgDB->GetParticle("e+")->PdgCode();
  fEMinusCode = fPdgDB->GetParticle("e-")->PdgCode();
  cout << "Resetting Codes: Pion " << fPiPlusCode
       << "," << fPiMinusCode 
       << " Kaon " << fKPlusCode 
       << "," << fKMinusCode << endl;
}

