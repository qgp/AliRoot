//to be used with aliroot for the moment.
//Macro to find prompt photon and do correlation analysis at the data level
//Example for PHOS and EMCAL analysis photon analysis
//Here both detectors used for detection of prompt photon 
//but should be done separately
Bool_t gIsAnalysisLoaded = kFALSE ; 

//______________________________________________________________________
Bool_t LoadLib( const char* pararchivename) 
{
  // Loads the AliRoot required libraries from a tar file 

  Bool_t rv = kTRUE ; 
 
  char cdir[1024] ; 
  sprintf(cdir, "%s", gSystem->WorkingDirectory() ) ; 
  
  // Setup par File
  if (pararchivename) {
   char parpar[80] ; 
   sprintf(parpar, "%s", pararchivename) ;
   if ( gSystem->AccessPathName(parpar) ) {
    gSystem->ChangeDirectory(gSystem->Getenv("ALICE_ROOT")) ;
    char processline[1024];
    sprintf(processline, ".! make %s", parpar) ; 
    cout << processline << endl ; 
    gROOT->ProcessLine(processline) ;
    gSystem->ChangeDirectory(cdir) ; 
    sprintf(processline, ".! mv /tmp/%s .", parpar) ;
    gROOT->ProcessLine(processline) ; 	
    sprintf(processline,".! tar xvzf %s",parpar);
    gROOT->ProcessLine(processline);
   }
   gSystem->ChangeDirectory(pararchivename);
   
    // check for BUILD.sh and execute
    if (!gSystem->AccessPathName("PROOF-INF/BUILD.sh")) {
      printf("*** Building PAR archive  %s  ***\n", pararchivename);

      if (gSystem->Exec("PROOF-INF/BUILD.sh")) {
	printf("Cannot Build the PAR Archive %s! - Abort!", pararchivename) );
        return kFALSE ;
      }
    }

    // check for SETUP.C and execute
    if (!gSystem->AccessPathName("PROOF-INF/SETUP.C")) {
      printf("*** Setup PAR archive  %s     ***\n", pararchivename);
      gROOT->Macro("PROOF-INF/SETUP.C");
    }    
  }


  if ( strstr(pararchivename, "AnalysisCheck") ) {
    gSystem->Load("libSpectrum.so");
  }
  
  printf("lib%s done\n", pararchivename);

  gSystem->ChangeDirectory(cdir);

  gIsAnalysisLoaded = kTRUE ; 
  return rv ; 
}

//______________________________________________________________________
void anaData(const Int_t kEvent=10)  
{

  // AliLog::SetGlobalDebugLevel(0);

  gSystem->AddIncludePath("-I\"$ALICE_ROOT/include\"");
  gSystem->Load("libANALYSIS.so");
  gSystem->Load("libANALYSISRL.so");

  if (! gIsAnalysisLoaded ) {
    //    LoadLib("ESD") ; 
    //   LoadLib("AOD") ;
    //   LoadLib("ANALYSIS") ; 
    LoadLib("PWG4Gamma") ; 
  }
  
  // create the analysis goodies object
  AliAnalysisGoodies * ag = new AliAnalysisGoodies() ; 
  
  // definition of analysis tasks
  //////////////////////////////////////////////////////////////////////////
  // first task :
  // Read the data, input: in this case ****ESD data****, 
  //                output: 3 TClonesArray of TParticles, 
  // 1) PHOS clusters, 2)EMCAL clusters, 3)Charged particles found in CTS
  //////////////////////////////////////////////////////////////////////////
  AliAnaGammaData * reader = new AliAnaGammaData("GammaData") ;
  //reader->SetCalorimeter("PHOS"); 
  ag->ConnectInput(reader, TChain::Class(), 0) ; 
  AliAnalysisDataContainer * outPHOSGamma  = ag->ConnectOuput(reader, TClonesArray::Class(), 0) ;
  AliAnalysisDataContainer * outEMCALGamma = ag->ConnectOuput(reader, TClonesArray::Class(), 1) ;
  AliAnalysisDataContainer * outCharged    = ag->ConnectOuput(reader, TClonesArray::Class(), 2) ;
  
  ////////////////////////////////////////////////////////////////////////////////
  //second task, find prompt/direct photon in calorimeter with pid and isolation cuts.
  // input: TClonesArrays of TParticles in calorimeter and in Central Tracking System
  // output: TParticle with prompt photon kinematics, 3 histograms
  ////////////////////////////////////////////////////////////////////////////////
  //Case 1: Calorimeter is PHOS
  ////////////////////////////////////////////////////////////////////////////////
  AliAnaGammaDirect * gammaDirectPhos = new AliAnaGammaDirect("GammaDirectPhos") ; 
  ag->ConnectInput(gammaDirectPhos, outPHOSGamma, 0) ; 
  ag->ConnectInput(gammaDirectPhos, outCharged,   1) ; 
  AliAnalysisDataContainer * outPHOSGammaDirect  = ag->ConnectOuput(gammaDirectPhos, TParticle::Class(), 0) ;
  ag->ConnectOuput(gammaDirectPhos, TList::Class(), 1) ;

  ////////////////////////////////////////////////////////////////////////////////
  //Case 2: Calorimeter is EMCAL
  ////////////////////////////////////////////////////////////////////////////////
  AliAnaGammaDirect * gammaDirectEmcal = new AliAnaGammaDirect("GammaDirectEmcal") ; 
  ag->ConnectInput(gammaDirectEmcal, outEMCALGamma, 0) ; 
  ag->ConnectInput(gammaDirectEmcal, outCharged,    1) ; 
  AliAnalysisDataContainer * outEMCALGammaDirect  = ag->ConnectOuput(gammaDirectEmcal, TParticle::Class(), 0) ;
  ag->ConnectOuput(gammaDirectEmcal, TList::Class(), 1) ;
  
  //////////////////////////////////////////////////////////////////////////////////
  //third task, correlate prompt photon with jet on the opposite side. 
  // Analysis options:
  // 1)kParton, only when reading Kinematics.root, NOT WITH THIS MACRO
  // 2)kHadron, gamma-hadron hadron correlation (no jet reconstruction)
  // 3)kJetFinder, correlate gamma and jet reconstructed with standard 
  //   jet reconstruction tools, to be implemented
  // 4)kJetLeadCone, gamma-jet correlation, jet reconstructed summing 
  //   particles in cone around leading particle
  // input: TParticle with prompt photon kinematics, TClonesArrays of TParticles in jet calorimeter and in 
  // Central Tracking System
  // output: histograms, amount depending on the analysis case.
  /////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  //Case 1: Prompt photon calorimeter is PHOS, jet calorimeter is EMCAL
  ////////////////////////////////////////////////////////////////////////////////
  AliAnaGammaCorr * corrPhos = new AliAnaGammaCorr("CorrPhos") ;
  corrPhos->SetAnalysisType(AliAnaGammaCorr::kHadron) ; // kJetFinder, kJetLeadCone, kHadron
  corrPhos->SetDataType(AliAnaGammaCorr::kData) ; // kData, kMC, kMCData
  ag->ConnectInput(corrPhos, outPHOSGammaDirect, 0) ; 
  ag->ConnectInput(corrPhos, outCharged, 1) ; 
  ag->ConnectInput(corrPhos, outEMCALGamma, 2) ; 
  AliAnalysisDataContainer * outPhos  = ag->ConnectOuput(corrPhos, TList::Class(), 0) ; 

  ////////////////////////////////////////////////////////////////////////////////
  //Case 2: Prompt photon calorimeter is EMCAL, !"jet calorimeter is PHOS"!
  ////////////////////////////////////////////////////////////////////////////////
  AliAnaGammaCorr * corrEmcal = new AliAnaGammaCorr("CorrEmcal") ; 
  corrEmcal->SetAnalysisType(AliAnaGammaCorr::kHadron) ; // kJetFinder, kJetLeadCone, kHadron
  corrEmcal->SetDataType(AliAnaGammaCorr::kData) ; // kData, kMC, kMCData
  ag->ConnectInput(corrEmcal, outEMCALGammaDirect, 0) ; 
  ag->ConnectInput(corrEmcal, outCharged, 1) ; 
  ag->ConnectInput(corrEmcal, outPHOSGamma, 2) ; 
  AliAnalysisDataContainer * outEmcal  =  ag->ConnectOuput(corrEmcal, TList::Class(), 0) ;
 
  //   //Scale Histograms with correspondin cross sections
  //   AliAnaScale * scalePhos = new AliAnaScale("ScaledcorrHadronPhos") ;
  //   ag->ConnectInput(scalePhos, outHadronPhos, 0) ;
  //   ag->ConnectOuput(scalePhos, TList::Class(), 0) ;
  
  //   AliAnaScale * scaleEmcal = new AliAnaScale("ScaledcorrHadronEmcal") ;
  //   ag->ConnectInput(scaleEmcal, outHadronEmcal, 0) ;
  //   ag->ConnectOuput(scaleEmcal, TList::Class(), 0) ;
  
  // get the data to analyze
  
  // definition of Tag cuts 
  const char * runCuts = 0x0 ; 
  const char * evtCuts = 0x0 ; 
  const char * lhcCuts = 0x0 ; 
  const char * detCuts = 0x0 ; 
  
  //"fEventTag.fNPHOSClustersMin == 1 && fEventTag.fNEMCALClustersMin == 1" ; 

  TString input = gSystem->Getenv("ANA_INPUT") ; 
  if ( input != "") {
    char argument[1024] ;  
    if ( input.Contains("tag?") ) {
      //create the ESD collection from the tag collection 
      input.ReplaceAll("tag?", "") ; 
      const char * collESD = "esdCollection.xml" ;
      ag->MakeEsdCollectionFromTagCollection(runCuts, lhcCuts, detCuts, evtCuts, input.Data(), collESD) ;
      sprintf(argument, "esd?%s", collESD) ; 
    } else if ( input.Contains("esd?") ) 
      sprintf(argument, "%s", input.Data()) ; 
    ag->Process(argument) ;

  } else {

    TChain* analysisChain = new TChain("esdTree") ;
    //   input = "alien:///alice/cern.ch/user/a/aliprod/prod2006_2/output_pp/105/411/AliESDs.root" ; 
    //   analysisChain->AddFile(input);
    input = "AliESDs.root" ; 
    const char * kInDir = gSystem->Getenv("OUTDIR") ; 
    if ( kInDir ) {
      if ( ! gSystem->cd(kInDir) ) {
	printf("%s does not exist\n", kInDir) ;
	return ;
      }     
      Int_t event, skipped=0 ; 
      char file[120] ;
      Double_t xsection = 0., ntrials = 0. ; 
      for (event = 0 ; event < kEvent ; event++) {
        sprintf(file, "%s/%d/AliESDs.root", kInDir,event) ; 
	TFile * fESD = 0 ; 
	if ( fESD = TFile::Open(file)) 
	  if ( fESD->Get("esdTree") ) { 
            printf("++++ Adding %s\n", file) ;
            analysisChain->AddFile(file);
	    Double_t * rv = ReadXsection(kInDir, event) ; 
	    xsection = xsection + rv[0] ;  
	    ntrials  = ntrials  + rv[1] ; 
	    cout << xsection << " " << ntrials << endl ; 
           
	  }
	  else { 
            printf("---- Skipping %s\n", file) ;
            skipped++ ;
	  }
      }
      printf("number of entries # %lld, skipped %d\n", analysisChain->GetEntries(), skipped*100) ; 	
    }
    else  
      analysisChain->AddFile(input);
    
    //scale->Set(xsection/ntrials) ; 
    ag->Process(analysisChain) ;
  }
  return ;
}

//______________________________________________________________________
void Merge(const char * xml, const char * sub, const char * out) 
{

  AliAnalysisGoodies * ag = new AliAnalysisGoodies() ; 
  ag->Merge(xml, sub, out) ;
}

Double_t * ReadXsection(const char * inDir, const Int_t event)
{
  // Read the PYTHIA statistics from the file pyxsec.root created by
  // the function WriteXsection():
  // integrated cross section (xsection) and
  // the  number of Pyevent() calls (ntrials)
  // and calculate the weight per one event xsection/ntrials
  // The spectrum calculated by a user should be
  // multiplied by this weight, something like this:
  // TH1F *userSpectrum ... // book and fill the spectrum
  // userSpectrum->Scale(weight)
  //
  // Yuri Kharlov 19 June 2007

  Double_t xsection;
  UInt_t    ntrials;

  char cfile[80] ; 
  sprintf(cfile, "%s/%d/pyxsec.root", inDir, event) ; 

  TFile *file = new TFile(cfile,"readonly");
  if ( ! file ) {
    AliFatal(Form("could not open %s", cfile)) ; 
    exit(1) ;
  }
  TTree *tree = file->Get("Xsection");
  tree->SetBranchAddress("xsection",&xsection);
  tree->SetBranchAddress("ntrials",&ntrials);
  tree->GetEntry(0);
  cout << "Cross section = "<<xsection<<" mb, N trials = "<<ntrials<<endl;
  Double_t rv[2] ; 
  rv[0] = xsection ; 
  rv[1] = ntrials ; 
  return rv ;
}
