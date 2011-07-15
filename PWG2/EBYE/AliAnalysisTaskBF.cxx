#include "TChain.h"
#include "TList.h"
#include "TCanvas.h"
#include "TLorentzVector.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"

#include "AliAnalysisTaskSE.h"
#include "AliAnalysisManager.h"

#include "AliESDVertex.h"
#include "AliESDEvent.h"
#include "AliESDInputHandler.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODInputHandler.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliESDtrackCuts.h"

#include "AliAnalysisTaskBF.h"
#include "AliBalance.h"


// Analysis task for the BF code
// Authors: Panos Cristakoglou@cern.ch

ClassImp(AliAnalysisTaskBF)

//________________________________________________________________________
AliAnalysisTaskBF::AliAnalysisTaskBF(const char *name) 
: AliAnalysisTaskSE(name), 
  fBalance(0),
  fList(0),
  fListBF(0),
  fHistEventStats(0),
  fHistTrackStats(0),
  fHistVx(0),
  fHistVy(0),
  fHistVz(0),
  fHistClus(0),
  fHistDCA(0),
  fHistChi2(0),
  fHistPt(0),
  fHistEta(0),
  fHistPhi(0),
  fHistV0M(0),
  fHistN(0),
  fESDtrackCuts(0),
  fCentralityEstimator("VOM"),
  fCentralityPercentileMin(0.), 
  fCentralityPercentileMax(5.),
  fUseOfflineTrigger(kFALSE),
  fVxMax(0.3),
  fVyMax(0.3),
  fVzMax(10.),
  fPtMin(0.3),
  fPtMax(1.5),
  fEtaMin(-0.8),
  fEtaMax(-0.8){
  // Constructor
  for(Int_t i = 0; i < NUMBER_OF_ANALYSES; i++){
    for (Int_t j = 0; j < 3; j++){
      fHistBF[i][j] = NULL;
    }
  }

  // Define input and output slots here
  // Input slot #0 works with a TChain
  DefineInput(0, TChain::Class());
  // Output slot #0 writes into a TH1 container
  DefineOutput(1, AliBalance::Class());
  DefineOutput(2, TList::Class());
  DefineOutput(3, TList::Class());
}


//________________________________________________________________________
void AliAnalysisTaskBF::UserCreateOutputObjects() {
  // Create histograms
  // Called once
  if(!fBalance) {
    fBalance = new AliBalance();
    fBalance->SetAnalysisLevel("ESD");
    fBalance->SetNumberOfBins(-1,9);
    fBalance->SetInterval(-1,0.,0.9);
  }

  //QA list
  fList = new TList();
  fList->SetName("listQA");
  fList->SetOwner();

  //Balance Function list
  fListBF = new TList();
  fListBF->SetName("listBF");
  fListBF->SetOwner();

  //Event stats.
  TString gCutName[4] = {"Total","Offline trigger",
                         "Vertex","Analyzed"};
  fHistEventStats = new TH1F("fHistEventStats",
                             "Event statistics;;N_{events}",
                             4,0.5,4.5);
  for(Int_t i = 1; i <= 4; i++)
    fHistEventStats->GetXaxis()->SetBinLabel(i,gCutName[i-1].Data());
  fList->Add(fHistEventStats);

  fHistTrackStats = new TH1F("fHistTrackStats","Event statistics;TriggerBit;N_{events}",130,0,130);
  fList->Add(fHistTrackStats);

  // Vertex distributions
  fHistVx = new TH1F("fHistVx","Primary vertex distribution - x coordinate;V_{x} (cm);Entries",100,-0.5,0.5);
  fList->Add(fHistVx);
  fHistVy = new TH1F("fHistVy","Primary vertex distribution - y coordinate;V_{y} (cm);Entries",100,-0.5,0.5);
  fList->Add(fHistVy);
  fHistVz = new TH1F("fHistVz","Primary vertex distribution - z coordinate;V_{z} (cm);Entries",100,-20.,20.);
  fList->Add(fHistVz);

  // QA histograms
  fHistClus = new TH2F("fHistClus","# Cluster (TPC vs. ITS)",10,0,10,200,0,200);
  fList->Add(fHistClus);
  fHistChi2 = new TH1F("fHistChi2","Chi2/NDF distribution",200,0,10);
  fList->Add(fHistChi2);
  fHistDCA  = new TH2F("fHistDCA","DCA (xy vs. z)",400,-5,5,400,-5,5); 
  fList->Add(fHistDCA);
  fHistPt   = new TH1F("fHistPt","p_{T} distribution",200,0,10);
  fList->Add(fHistPt);
  fHistEta  = new TH1F("fHistEta","#eta distribution",200,-2,2);
  fList->Add(fHistEta);
  fHistPhi  = new TH1F("fHistPhi","#phi distribution",200,-20,380);
  fList->Add(fHistPhi);
  fHistV0M  = new TH2F("fHistV0M","V0 Multiplicity C vs. A",500, 0, 20000, 500, 0, 20000);
  fList->Add(fHistV0M);


  if(fESDtrackCuts) fList->Add(fESDtrackCuts);


  //balance function histograms
  TString hname;
  for(Int_t i = 0; i < NUMBER_OF_ANALYSES; i++){
    for (Int_t j = 0; j < 3; j++){
      hname = "BF";
      hname+=i;
      hname+=j;
      fHistBF[i][j] = new TH1F(hname,hname,fBalance->GetNumberOfBins(i),fBalance->GetP2Start(i),fBalance->GetP2Stop(i));
      fListBF->Add(fHistBF[i][j]);
    }
  }
  fHistN = new TH1F("fN","fN",2,0,1);
  fListBF->Add(fHistN);

  // Post output data.
  //PostData(1, fBalance);
  PostData(2, fList);
  PostData(3, fListBF);
  
}

//________________________________________________________________________
void AliAnalysisTaskBF::UserExec(Option_t *) {
  // Main loop
  // Called for each event
  TString gAnalysisLevel = fBalance->GetAnalysisLevel();

  TObjArray *array = new TObjArray();
  
  //ESD analysis
  if(gAnalysisLevel == "ESD") {
    AliESDEvent* gESD = dynamic_cast<AliESDEvent*>(InputEvent()); // from TaskSE
    if (!gESD) {
      Printf("ERROR: gESD not available");
      return;
    }

    fHistEventStats->Fill(1); //all events
    Bool_t isSelected = kTRUE;
    if(fUseOfflineTrigger)
      isSelected = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
    if(isSelected) {
      fHistEventStats->Fill(2); //triggered events

      //Centrality stuff
      AliCentrality *centrality = gESD->GetCentrality();
      //Int_t nCentrality = 0;
      //nCentrality = centrality->GetCentralityClass5(fCentralityEstimator.Data());
      //cout<<nCentrality<<" "<<centrality->IsEventInCentralityClass(fCentralityPercentileMin,fCentralityPercentileMax,fCentralityEstimator.Data())<<endl;

      // take only events inside centrality class
      if(centrality->IsEventInCentralityClass(fCentralityPercentileMin,
					      fCentralityPercentileMax,
					      fCentralityEstimator.Data())){

	// centrality QA (V0M)
	fHistV0M->Fill(gESD->GetVZEROData()->GetMTotV0A(), gESD->GetVZEROData()->GetMTotV0C());
	
	const AliESDVertex *vertex = gESD->GetPrimaryVertex();
	if(vertex) {
	  if(vertex->GetNContributors() > 0) {
	    if(vertex->GetZRes() != 0) {
	      fHistEventStats->Fill(3); //events with a proper vertex
	      if(TMath::Abs(vertex->GetXv()) < fVxMax) {
		if(TMath::Abs(vertex->GetYv()) < fVyMax) {
		  if(TMath::Abs(vertex->GetZv()) < fVzMax) {
		    fHistEventStats->Fill(4); //analayzed events
		    fHistVx->Fill(vertex->GetXv());
		    fHistVy->Fill(vertex->GetYv());
		    fHistVz->Fill(vertex->GetZv());
		    
		    //Printf("There are %d tracks in this event", gESD->GetNumberOfTracks());
		    for (Int_t iTracks = 0; iTracks < gESD->GetNumberOfTracks(); iTracks++) {
		      AliESDtrack* track = dynamic_cast<AliESDtrack *>(gESD->GetTrack(iTracks));
		      if (!track) {
			Printf("ERROR: Could not receive track %d", iTracks);
			continue;
		      }	

		      // take only TPC only tracks (HOW IS THIS DONE IN ESDs???)
		      //if(!track->IsTPCOnly()) continue;

		      //ESD track cuts
		      if(fESDtrackCuts) 
			if(!fESDtrackCuts->AcceptTrack(track)) continue;

		      // fill QA histograms
		      Float_t b[2];
		      Float_t bCov[3];
		      track->GetImpactParameters(b,bCov);
		      if (bCov[0]<=0 || bCov[2]<=0) {
			AliDebug(1, "Estimated b resolution lower or equal zero!");
			bCov[0]=0; bCov[2]=0;
		      }

		      fHistClus->Fill(track->GetITSclusters(0),track->GetTPCclusters(0));
		      fHistDCA->Fill(b[1],b[0]);
		      fHistPt->Fill(track->Pt());
		      fHistEta->Fill(track->Eta());
		      fHistPhi->Fill(track->Phi()*TMath::RadToDeg());
		      
		      // fill BF array
		      array->Add(track);
		      
		  } //track loop
		  }//Vz cut
		}//Vy cut
	      }//Vx cut
	    }//proper vertex resolution
	  }//proper number of contributors
	}//vertex object valid
      }//centrality
    }//triggered event 
  }//ESD analysis
  

  //AOD analysis (vertex and track cuts also here!!!!)
  else if(gAnalysisLevel == "AOD") {
    AliAODEvent* gAOD = dynamic_cast<AliAODEvent*>(InputEvent()); // from TaskSE
    if(!gAOD) {
      Printf("ERROR: gAOD not available");
      return;
    }

    fHistEventStats->Fill(1); //all events
    Bool_t isSelected = kTRUE;
    if(fUseOfflineTrigger)
      isSelected = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected();
    if(isSelected) {
      fHistEventStats->Fill(2); //triggered events

		  
      //Centrality stuff (centrality in AOD header)
      AliAODHeader *aodHeader = gAOD->GetHeader();
      Float_t fCentrality     = aodHeader->GetCentralityP()->GetCentralityPercentile("V0M");
   
      // take only events inside centrality class
      if(fCentrality > fCentralityPercentileMin && fCentrality < fCentralityPercentileMax){

	// centrality QA (V0M)
	fHistV0M->Fill(gAOD->GetVZEROData()->GetMTotV0A(), gAOD->GetVZEROData()->GetMTotV0C());
      

	//cout<<gAOD->GetNumberOfVertices()<<endl;
	const AliAODVertex *vertex = gAOD->GetPrimaryVertex();
	//const AliAODVertex *vertex1 = gAOD->GetPrimaryVertexSPD();
	// vertex->Print();
	// vertex1->Print();

	if(vertex) {
	  Double32_t fCov[6];
	  Double32_t fPosition[3];
	  Double32_t fChi2 = vertex->GetChi2();
	  Int_t      nCont = vertex->GetNContributors();
	  vertex->GetXYZ(fPosition);
	  vertex->GetCovarianceMatrix(fCov);
	  
	  const AliESDVertex *esdVertex = new AliESDVertex(fPosition, fCov, fChi2, nCont);
	  
	  if(vertex->GetNContributors() > 0) {
	    //if(vertex->GetZRes() != 0) {
	      fHistEventStats->Fill(3); //events with a proper vertex
	      if(TMath::Abs(vertex->GetX()) < fVxMax) {
		if(TMath::Abs(vertex->GetY()) < fVyMax) {
		  if(TMath::Abs(vertex->GetZ()) < fVzMax) {
		    fHistEventStats->Fill(4); //analayzed events
		    fHistVx->Fill(vertex->GetX());
		    fHistVy->Fill(vertex->GetY());
		    fHistVz->Fill(vertex->GetZ());

		    //Printf("There are %d tracks in this event", gAOD->GetNumberOfTracks());
		    for (Int_t iTracks = 0; iTracks < gAOD->GetNumberOfTracks(); iTracks++) {
		      AliAODTrack* aodTrack = dynamic_cast<AliAODTrack *>(gAOD->GetTrack(iTracks));
		      if (!aodTrack) {
			Printf("ERROR: Could not receive track %d", iTracks);
			continue;
		      }

		      // AOD track cuts
		      
		      // For ESD Filter Information: ANALYSIS/macros/AddTaskESDfilter.C
		      // take only TPC only tracks 
		      fHistTrackStats->Fill(aodTrack->GetFilterMap());
		      if(!aodTrack->TestFilterBit(128)) continue;

		      Float_t p[3];
		      Float_t b[2];
		      Float_t bCov[3];
		      aodTrack->GetPosition(p);
		      //AliExternalTrackParam exParam;
		      AliESDtrack esdTrack( aodTrack );
		      esdTrack.RelateToVertex(esdVertex,gAOD->GetMagneticField(),100);
		      //esdTrack.RelateToVertexTPC(esdVertex,gAOD->GetMagneticField(),100,&exParam);
		      //AliExternalTrackParam* outer = (AliExternalTrackParam*)aodTrack->GetOuterParam();
		      //if(outer) outer->PropagateToDCA(vertex,gAOD->GetMagneticField(),1000,b,bCov);
		      //else continue;
      		      //cout<<"   ___  "<<b[0]<<" "<<b[1]<<endl;


		      //only for QA --> Remove?
		      esdTrack.GetImpactParameters(b,bCov);
		      if (bCov[0]<=0 || bCov[2]<=0) {
		      AliDebug(1, "Estimated b resolution lower or equal zero!");
		      	bCov[0]=0; bCov[2]=0;
		      }

		      Float_t pt  = aodTrack->Pt();
		      Float_t eta = aodTrack->Eta();
		      
		      // Kinematics cuts from ESD track cuts
		      if( pt < fPtMin || pt > fPtMax)      continue;
		      if( eta < fEtaMin || eta > fEtaMax)  continue;
		      		      
		      // fill QA histograms
		      fHistClus->Fill(aodTrack->GetITSNcls(),aodTrack->GetTPCNcls());
		      fHistDCA->Fill(b[1],b[0]);
		      //fHistDCA->Fill(p[1],p[0]);
		      fHistChi2->Fill(aodTrack->Chi2perNDF());
		      fHistPt->Fill(pt);
		      fHistEta->Fill(eta);
		      fHistPhi->Fill(aodTrack->Phi()*TMath::RadToDeg());
		      
		      // fill BF array
		      array->Add(aodTrack);

		    } //track loop
		  }//Vz cut
		}//Vy cut
	      }//Vx cut
	      //}//proper vertex resolution
	  }//proper number of contributors
	}//vertex object valid
      }//centrality
    }//triggered event 
  }//AOD analysis

  //MC analysis
  else if(gAnalysisLevel == "MC") {
    
    AliMCEvent*  mcEvent = MCEvent(); 
    if (!mcEvent) {
      Printf("ERROR: mcEvent not available");
      return;
    }
    
    Printf("There are %d tracks in this event", mcEvent->GetNumberOfPrimaries());
    for (Int_t iTracks = 0; iTracks < mcEvent->GetNumberOfPrimaries(); iTracks++) {
      AliMCParticle* track = dynamic_cast<AliMCParticle *>(mcEvent->GetTrack(iTracks));
      if (!track) {
	Printf("ERROR: Could not receive particle %d", iTracks);
	continue;
      }
      array->Add(track);
    } //track loop
  }//MC analysis
  
  fBalance->CalculateBalance(array);
  
  delete array;
  
}      

//________________________________________________________________________
void  AliAnalysisTaskBF::FinishTaskOutput(){
  //Printf("END BF");

  //fBalance = dynamic_cast<AliBalance*> (GetOutputData(1));
  if (!fBalance) {
    Printf("ERROR: fBalance not available");
    return;
  }
  
  for(Int_t a = 0; a < NUMBER_OF_ANALYSES; a++){
    for(Int_t iBin = 1; iBin <= fBalance->GetNumberOfBins(a); iBin++){

      //Printf("%d %d  ->   %f",a,iBin,fBalance->GetNnn(a,iBin-1));

	fHistBF[a][0]->SetBinContent(iBin,fBalance->GetNnn(a,iBin-1));
	fHistBF[a][1]->SetBinContent(iBin,fBalance->GetNpn(a,iBin-1));
	fHistBF[a][2]->SetBinContent(iBin,fBalance->GetNpp(a,iBin-1));
    }
  }
  fHistN->SetBinContent(1,fBalance->GetNn());
  fHistN->SetBinContent(2,fBalance->GetNp());  

}

//________________________________________________________________________
void AliAnalysisTaskBF::Terminate(Option_t *) {
  // Draw result to the screen
  // Called once at the end of the query

  // not implemented ...
  
}
