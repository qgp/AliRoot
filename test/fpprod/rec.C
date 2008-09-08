//--- Magnetic Field ---
enum Mag_t
{
  kNoField, k5kG, kFieldMax
};

const char * pprField[] = {
  "kNoField", "k5kG"
};
static Mag_t         mag      = k5kG;

//--- Functions ---
void ProcessEnvironmentVars();

void rec() {
  if (!strcmp(gSystem->GetBuildArch(),"win32gcc")) {
    gSystem->Load("libProof");
    gSystem->Load("libGui");
    gROOT->Macro("loadlibsrec.C");
    new AliRun("gAlice","The ALICE Off-line Simulation Framework");
  }
  ProcessEnvironmentVars();
  AliReconstruction reco;

  reco.SetWriteESDfriend();
  reco.SetWriteAlignmentData();
  reco.SetRecoParam("TPC",AliTPCRecoParam::GetLowFluxParam());
  reco.SetRecoParam("TRD",AliTRDrecoParam::GetLowFluxParam());
  reco.SetRecoParam("PHOS",AliPHOSRecoParam::GetDefaultParameters());
  reco.SetRecoParam("MUON",AliMUONRecoParam::GetLowFluxParam());
  //   reco.SetInput("raw.root");
  reco.SetRunReconstruction("ITS TPC TRD TOF HMPID PHOS EMCAL MUON VZERO T0 FMD PMD ZDC");
  reco.SetRunQA(":");
  reco.SetRunGlobalQA(kTRUE);
  reco.SetMeanVertexConstraint(kFALSE);

// **** The field map settings must be the same as in Config.C !
  AliMagWrapCheb* field = 0x0;
  if (mag == kNoField) {
    field = new AliMagWrapCheb("Maps","Maps", 2, 0., 10., AliMagWrapCheb::k2kG);
  } else if (mag == k5kG) {
    field = new AliMagWrapCheb("Maps","Maps", 2, 1., 10., AliMagWrapCheb::k5kG);
  }
  Bool_t uniform=kFALSE;
  AliTracker::SetFieldMap(field,uniform);  // tracking with the real map

  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}
void ProcessEnvironmentVars()
{
    // Field
    if (gSystem->Getenv("CONFIG_FIELD")) {
      for (Int_t iField = 0; iField < kFieldMax; iField++) {
	if (strcmp(gSystem->Getenv("CONFIG_FIELD"), pprField[iField])==0) {
	  mag = (Mag_t)iField;
	  cout<<"Field set to "<<pprField[iField]<<endl;
	}
      }
    }
}
