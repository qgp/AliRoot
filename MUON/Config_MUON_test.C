void Config()
{
  //
  // Config file for MUJON test
  // Gines MARITNEZ, Subatech, mai 2003
  // 
  //=====================================================================
  // libraries required by geant321
  gSystem->Load("libgeant321.so");

  new TGeant3("C++ Interface to Geant3");

  //=======================================================================
  //  Create the output file    
  Text_t filename[100];
  sprintf(filename,"galice.root");
  cout << ">>> Output file is " << filename << endl;   
  cout<<">>> Config_MUON_test.C: Creating Run Loader ..."<<endl;
  AliRunLoader* rl = AliRunLoader::Open(
	filename, AliConfig::fgkDefaultEventFolderName, "recreate");
  if (rl == 0x0)
    {
      gAlice->Fatal("Config.C","Can not instatiate the Run Loader");
      return;
    }
  rl->SetCompressionLevel(2);
  rl->SetNumberOfEventsPerFile(3);
  gAlice->SetRunLoader(rl);

  //=======================================================================
  // MonteCarlo
  TGeant3 *geant3 = (TGeant3*)gMC;

  //=======================================================================
  // Set External decayer
  AliDecayer* decayer = new AliDecayerPythia();
  decayer->SetForceDecay(kAll);
  decayer->Init();
  gMC->SetExternalDecayer(decayer);

  //
  //=======================================================================
  // ******* GEANT STEERING parameters FOR ALICE SIMULATION *******
  geant3->SetTRIG(1); //Number of events to be processed 
  geant3->SetSWIT(4,10);
  geant3->SetDEBU(0,0,1);
  //geant3->SetSWIT(2,2);
  geant3->SetDCAY(1);
  geant3->SetPAIR(1);
  geant3->SetCOMP(1);
  geant3->SetPHOT(1);
  geant3->SetPFIS(0);
  geant3->SetDRAY(0);
  geant3->SetANNI(1);
  geant3->SetBREM(1);
  geant3->SetMUNU(1);
  geant3->SetCKOV(1);
  geant3->SetHADR(1); //Select pure GEANH (HADR 1) or GEANH/NUCRIN (HADR 3)
  geant3->SetLOSS(2);
  geant3->SetSTRA(0); // Simulation des collisions dans the thin layers
  geant3->SetMULS(1);
  geant3->SetRAYL(1);
  geant3->SetAUTO(1); //Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
  geant3->SetABAN(0); //Restore 3.16 behaviour for abandoned tracks
  geant3->SetOPTI(2); //Select optimisation level for GEANT geometry searches (0,1,2)
  geant3->SetERAN(5.e-7);

  Float_t cut    = 1.e-3; // 1MeV cut by default
  Float_t tofmax = 1.e10;
  //             GAM ELEC NHAD CHAD MUON EBREM MUHAB EDEL MUDEL MUPA TOFMAX
  geant3->SetCUTS(cut,cut, cut, cut, cut, cut,  cut,  cut, cut,  cut, tofmax);
  //
  //=======================================================================
  // ************* STEERING parameters FOR ALICE SIMULATION **************
  // --- Specify event type to be tracked through the ALICE setup
  // --- All positions are in cm, angles in degrees, and P and E in GeV
  // From AliMUONConstants class we get :
  //   Position Z (along beam) of the chambers (cm) (from AliMUONConstants class):  //      533.5,  546.5, 678.5, 693.5, 964.0, 986.0, 1251.5, 1278.5, 
  //     1416.5, 1443.5,  1610, 1625., 1710., 1725. 
  //   Internal Radius (cm)   
  //     36.4,  46.2,  66.0,   80.,   80., 100., 100.    
  //   Exterlnal Radius (cm)
  //     183., 245., 395.,  560.,  563., 850., 900.};  
  AliGenBox * gener = new AliGenBox(1);
  gener->SetMomentumRange(7.,7.1);
  gener->SetPhiRange(-180., 180.);       
  gener->SetThetaRange(2.000,4.001);
  gener->SetPart(13);           // Muons
  gener->SetOrigin(0.,0., 0.);  //vertex position
  gener->SetSigma(0.0, 0.0, 0.0);         //Sigma in (X,Y,Z) (cm) on IP position


  //=============================================================
  //Specify maximum magnetic field in Tesla (neg. ==> default field)
  gAlice->SetField(0,0) ;  //(-999,2);    

  //=================== Alice BODY parameters =============================
  AliBODY *BODY = new AliBODY("BODY","Alice envelop");


  //=================== MUON Subsystem ===========================
  AliMUONv1 *MUON  = new AliMUONv1("MUON","default");
  
}


Float_t EtaToTheta(Float_t arg){
  return (180./TMath::Pi())*2.*atan(exp(-arg));
}
