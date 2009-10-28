void MyLoad(const char * name);

void loadlibs () 
{
  // Macro which loads the libraries needed for simulation and reconstruction
  // Possible usage: In a Root session (no AliRoot) one does
  // root [0] .x loadlibs.C
  // root [1] gAlice = new AliRun("gAlice","test")           
  // root [2] AliSimulation sim
  // root [3] sim.Run()
  // root [4] AliReconstruction rec
  // root [5] rec.Run()

  gSystem->Load("libPhysics");
  gSystem->Load("libMinuit");
  gSystem->Load("libProof");

  // Uncomment the following line for macosx
  // Waiting for a better solution
  //  gSystem->Load("libg2c_sh");
  gSystem->Load("libmicrocern");
  gSystem->Load("liblhapdf");
  gSystem->Load("libpythia6");

  gSystem->Load("libEG");
  gSystem->Load("libGeom");
  gSystem->Load("libVMC");
  gSystem->Load("libSTEERBase");

  AliSysInfo::AddStamp("Start");
  MyLoad("libEGPythia6");

  MyLoad("libNet");
  MyLoad("libTree");
  MyLoad("libGui");
 
  MyLoad("libESD");
  MyLoad("libCDB");
  MyLoad("libRAWDatabase");
  MyLoad("libRAWDatarec");
  MyLoad("libAOD");
  MyLoad("libSTEER");
  MyLoad("libRAWDatasim");
  MyLoad("libFASTSIM");
  MyLoad("libEVGEN");
  MyLoad("libAliPythia6");

  MyLoad("libhijing");
  MyLoad("libTHijing");// AliGenHijingEventHeader needed by libZDCsim.so

  MyLoad("libSTRUCT");
  MyLoad("libPHOSUtils");
  MyLoad("libPHOSbase");
  MyLoad("libPHOSsim");
  MyLoad("libPHOSrec");
  MyLoad("libMUONcore");
  MyLoad("libMUONmapping");
  MyLoad("libMUONgeometry");
  MyLoad("libMUONcalib");
  MyLoad("libMUONraw");
  MyLoad("libMUONtrigger");
  MyLoad("libMUONbase");
  MyLoad("libMUONsim");
  MyLoad("libMUONrec");
  MyLoad("libMUONevaluation");
  MyLoad("libFMDbase");
  MyLoad("libFMDsim");
  MyLoad("libFMDrec");
  MyLoad("libPMDbase");
  MyLoad("libPMDsim");
  MyLoad("libPMDrec");
  MyLoad("libHMPIDbase");
  MyLoad("libHMPIDsim");
  MyLoad("libHMPIDrec");
  MyLoad("libT0base");
  MyLoad("libT0sim");
  MyLoad("libT0rec");
  MyLoad("libZDCbase");
  MyLoad("libZDCsim");
  MyLoad("libZDCrec");
  MyLoad("libACORDEbase");
  MyLoad("libACORDErec");
  MyLoad("libACORDEsim");
  MyLoad("libVZERObase");
  MyLoad("libVZEROrec");
  MyLoad("libVZEROsim");
  MyLoad("libEMCALbase");
  MyLoad("libEMCALsim");
  MyLoad("libEMCALrec");

  // The following lines have to be commented on Darwin
  // for the moment due to cross dependencies
  MyLoad("libTPCbase");
  MyLoad("libTPCrec");
  MyLoad("libTPCsim");
  MyLoad("libTPCfast");
  MyLoad("libITSbase");
  MyLoad("libITSsim");
  MyLoad("libITSrec");
  MyLoad("libTRDbase");
  MyLoad("libTRDsim");
  MyLoad("libTRDrec");
  MyLoad("libTOFbase");
  MyLoad("libTOFrec");
  MyLoad("libTOFsim");

  MyLoad("libHLTbase");
  MyLoad("libHLTinterface");
  MyLoad("libHLTsim");
  MyLoad("libHLTrec");
}
void MyLoad(const char * name) {
  gSystem->Load(name);
  AliSysInfo::AddStamp(name) ;
}
