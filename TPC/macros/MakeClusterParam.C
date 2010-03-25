/*  
  Example macro to build AliTPCClusterParam
  postprocessing the output of the calibration using tracks
  In the future this macro will be part of the Preprocesor
  ..

*/
void StoreObject(AliTPCClusterParam *param);

void MakeClusterParam(const char *fnresolc="Output.root", const char *fnresolg="Output.root"){
  gSystem->Load("libTPCcalib.so");
  TFile fresolc(fnresolc);
  TFile fresolg(fnresolg);
  AliTPCcalibTracks *calibtracks = (AliTPCcalibTracks*)fresolc.Get("calibTracks");
  AliTPCcalibTracksGain *calibtracksGain = (AliTPCcalibTracksGain*)fresolg.Get("calibTracksGain");

  
  AliTPCClusterParam * param = AliTPCcalibDB::Instance()->GetClusterParam();
  AliTPCClusterParam::SetInstance(param);
  //
  // Make a resolution tree
  //
  calibtracks->MakeResPlotsQTree(200,"plots");  
  TFile fres("plots/resol.root");
  TTree *treeres = (TTree*)fres.Get("Resol");
  // Fit the resolution parameterization
  param->FitResol(treeres);
  param->FitRMS(treeres);
  TF1 f1z_z("f1z_z","AliTPCClusterParam::SGetError0Par(1,0,x,0)",0,250);
  //
  // angular effect calibration - usable only with the 
  // cosmic tracks 
  //
  calibtracksGain->UpdateClusterParam(&clParam);
  //
  //
  //
  TFile fclparam("TPCClusterParam.root","recreate");
  clParam->Write("Param");
  fclparam.Close();
  StoreObject(clParam);
  AliTPCClusterParam::SetInstance(&clParam);
}
 
void StoreObject(AliTPCClusterParam *param)
{ 
  //
  //
  //
  Int_t gkDummyRun = 0;
  char *gCDBpath   = "local://$ALICE_ROOT/OCDB";
  AliCDBMetaData *md1= new AliCDBMetaData(); 
  AliCDBId id1("TPC/Calib/ClusterParam", gkDummyRun, gkDummyRun); 
  AliCDBStorage* gStorLoc = 0;
  AliCDBManager *man = AliCDBManager::Instance();
  gStorLoc = man->GetStorage(gCDBpath);

  md1->SetObjectClassName("AliTPCClusterParam");
  md1->SetResponsible("Marian Ivanov");
  md1->SetBeamPeriod(1);
  md1->SetAliRootVersion("v5-08-Release"); //root version
  md1->SetComment("Calibration data using the MC cosmic");
  gStorLoc->Put(&param, id1, md1); 
}




/*





gSystem->Load("libANALYSIS");
gSystem->Load("libSTAT");
gSystem->Load("libTPCcalib");

TStatToolkit toolkit;
Double_t chi2;
TVectorD fitParam;
TMatrixD covMatrix;
Int_t npoints;

TString fstring="";
fstring+="(Zm/250)++";
fstring+="AngleM^2++";


TString *strRMS0_0 = toolkit.FitPlane(treeRes,"RMSm^2",fstring->Data(), "Dim==0&&Pad==0&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS0_0",strRMS0_0->Data());
TString *strRMS0_1 = toolkit.FitPlane(treeRes,"RMSm^2",fstring->Data(), "Dim==0&&Pad==1&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS0_1",strRMS0_1->Data());
TString *strRMS0_2 = toolkit.FitPlane(treeRes,"RMSm^2",fstring->Data(), "Dim==0&&Pad==2&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS0_2",strRMS0_2->Data());

TString *strRMS1_0 = toolkit.FitPlane(treeRes,"RMSm^2",fstring->Data(), "Dim==1&&Pad==0&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS1_0",strRMS1_0->Data());
TString *strRMS1_1 = toolkit.FitPlane(treeRes,"RMSm^2",fstring->Data(), "Dim==1&&Pad==1&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS1_1",strRMS1_1->Data());
TString *strRMS1_2 = toolkit.FitPlane(treeRes,"RMSm^2",fstring->Data(), "Dim==1&&Pad==2&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS1_2",strRMS1_2->Data());


TString fstringPar="";
fstringPar+="(Zm/250)++";
fstringPar+="AngleM^2++";
fstringPar+="(Zm/250)^2++";
fstringPar+="AngleM^2*(Zm/250)++";


TString *strRMS0_0Par = toolkit.FitPlane(treeRes,"RMSm^2",fstringPar->Data(), "Dim==0&&Pad==0&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS0_0Par",strRMS0_0Par->Data());
TString *strRMS0_1Par = toolkit.FitPlane(treeRes,"RMSm^2",fstringPar->Data(), "Dim==0&&Pad==1&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS0_1Par",strRMS0_1Par->Data());
TString *strRMS0_2Par = toolkit.FitPlane(treeRes,"RMSm^2",fstringPar->Data(), "Dim==0&&Pad==2&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS0_2Par",strRMS0_2Par->Data());

TString *strRMS1_0Par = toolkit.FitPlane(treeRes,"RMSm^2",fstringPar->Data(), "Dim==1&&Pad==0&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS1_0Par",strRMS1_0Par->Data());
TString *strRMS1_1Par = toolkit.FitPlane(treeRes,"RMSm^2",fstringPar->Data(), "Dim==1&&Pad==1&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS1_1Par",strRMS1_1Par->Data());
TString *strRMS1_2Par = toolkit.FitPlane(treeRes,"RMSm^2",fstringPar->Data(), "Dim==1&&Pad==2&&QMean<0", chi2,npoints,fitParam,covMatrix);
treeRes->SetAlias("RMS1_2Par",strRMS1_2Par->Data());



void DrawResult(){
treeRes
treeRes->Draw("RMSm/sqrt(RMS1_0Par):AngleM","Dim==1&&Pad==0&&QMean<0","*")
}



*/
