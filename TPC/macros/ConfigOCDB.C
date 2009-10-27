//
// Macro to Setup OCDB  
// This is just example macro
// Responsible: marian.ivanov@cern.ch
// To be used:
// 1. Before invocation of the calibration - in the calibration trains
// 2. To setup calibration viewer.
//  
// ConfigOCDB  - setup default and specific data storage
// SetupCustom - user sepcific configuration 
//             - Values in local cache of OCDB are overwritten


 
void SetupCustom(Int_t run);

void ConfigOCDB(Int_t crun=-1){
  // 
  printf("SETUP OCBD for TPC\n");
  //
  TGrid::Connect("alien://",0,0,"t");
  AliCDBManager::Instance()->SetDefaultStorage("alien://folder=/alice/data/2009/OCDB");
 
  Int_t run =crun;
  if (run<0) run =0;
  AliCDBManager::Instance()->SetRun(run);
  //SetupCustom(run);
  AliTPCcalibDB::Instance()->SetRun(run);
}


void SetupCustom(Int_t run){
  //
  //
  // Custom part - to be empty once we are happy with the calibration
  //
  //
  // Setup magnetic field
  //
  AliGRPObject *grp = AliTPCcalibDB::GetGRP(run);
  Float_t current = 0;
  Float_t bz      = 0;
  if (grp){
    current = grp->GetL3Current((AliGRPObject::Stats)0);
    bz = 5*current/30000.;
    printf("Run%d\tL3 current%f\tBz\t%f\n",run,current,bz);
  }
  else{
    printf("Run%d\tL3 current%f\tBz\t%f\n",run,current,bz);
  }
  AliMagF::BMap_t smag = AliMagF::k5kG;
  Double_t bzfac = bz/5;
  Double_t bzfacOrig=bzfac;
  if (TMath::Abs(bzfac)<0.01) {  // force default magnetic field if 0 field used
    bzfac=1;
    bz=5;
  }
  AliMagF* magF= new AliMagF("Maps","Maps", bzfac, 1., smag);
  TGeoGlobalMagField::Instance()->SetField(magF);  
  printf("\n\nSET EXB FIELD\t\n\n");
  AliTPCcalibDB::Instance()->SetExBField(magF);
  //
  //
  // import geometry
  //
  //
  TGeoManager::Import("/u/miranov/proof/geometry.root");
  AliGeomManager::LoadGeometry("/u/miranov/proof/geometry.root");

  AliTPCClusterParam * paramCl = AliTPCcalibDB::Instance()->GetClusterParam(); 
  AliTPCParam   * paramTPC = AliTPCcalibDB::Instance()->GetParameters();
  paramCl->SetInstance(paramCl);

  //
  // Setup reco param
  //
  AliTPCTransform *transform     = AliTPCcalibDB::Instance()->GetTransform() ;
  AliTPCRecoParam * tpcRecoParam = AliTPCRecoParam::GetCosmicTestParam(kTRUE);
  transform->SetCurrentRecoParam(tpcRecoParam);
  tpcRecoParam->SetUseRPHICorrection(kTRUE); 
  tpcRecoParam->SetUseTOFCorrection(kFALSE);
  //
  tpcRecoParam->SetUseDriftCorrectionTime(1);
  tpcRecoParam->SetUseDriftCorrectionGY(1);
  //
  tpcRecoParam->SetUseRadialCorrection(kFALSE);
  tpcRecoParam->SetUseQuadrantAlignment(kTRUE);
  //
  tpcRecoParam->SetUseSectorAlignment(kFALSE);
  tpcRecoParam->SetUseGainCorrectionTime(kFALSE);
  tpcRecoParam->SetUseFieldCorrection(kFALSE);
  tpcRecoParam->SetUseExBCorrection(kTRUE);
  if (TMath::Abs(bzfacOrig)<0.05){
    tpcRecoParam->SetUseExBCorrection(kFALSE);
  }
  //
  //
  //
  TFile fposcor("~/OCDB/calibUnlin.root");
  AliTPCPointCorrection *pcorr = fposcor.Get("correction");
  if (pcorr) pcorr->SetInstance(pcorr); 
  //
  //
  //
  printf("END of SETUP OCBD for TPC\n");
}

void DumpInfo(){
  AliTPCcalibDB::Instance()->GetParameters();
  AliDCSSensorArray *arrHV=AliTPCcalibDB::Instance()->GetVoltageSensors(run);
  //
  //
  Float_t vcorStart=AliTPCcalibDB::Instance()->GetVDriftCorrectionTime(arrHV->GetStartTime().GetSec(),run,0,1);
  Float_t vcorrEnd=AliTPCcalibDB::Instance()->GetVDriftCorrectionTime(arrHV->GetEndTime().GetSec(),run,0,1);
  Float_t tcorStart=AliTPCcalibDB::Instance()->GetTime0CorrectionTime(arrHV->GetStartTime().GetSec(),run,0,1);
  Float_t tcorrEnd=AliTPCcalibDB::Instance()->GetTime0CorrectionTime(arrHV->GetEndTime().GetSec(),run,0,1);


}



