#include "AliHMPIDPreprocessor.h" //header

#include <Riostream.h>
#include <AliCDBMetaData.h>
#include <AliDCSValue.h>      
#include <TObjString.h>       //Process()
#include <TF1.h>              //Process()
#include <TF2.h>              //Process()
#include <TGraph.h>           //Process()
#include <TMatrixF.h>         //Process()
#include <AliLog.h>           //Process() 

//.
// HMPID Preprocessor base class
//.
//.
//.
ClassImp(AliHMPIDPreprocessor)

char *AliHMPIDPreprocessor::fgP ="HMP_DET/HMP_MP%i/HMP_MP%i_GAS/HMP_MP%i_GAS_PMWC.actual.value";
char *AliHMPIDPreprocessor::fgT1="HMP_DET/HMP_MP%i/HMP_MP%i_LIQ_LOOP.actual.sensors.Rad%iIn_Temp";
char *AliHMPIDPreprocessor::fgT2="HMP_DET/HMP_MP%i/HMP_MP%i_LIQ_LOOP.actual.sensors.Rad%iOut_Temp";
char *AliHMPIDPreprocessor::fgHV="HMP_DET/HMP_MP%i/HMP_MP%i_PW/HMP_MP%i_SEC0/HMP_MP%i_SEC0_HV.actual.vMon";

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDPreprocessor::Initialize(Int_t run, UInt_t startTime,UInt_t endTime)
{
  AliPreprocessor::Initialize(run, startTime, endTime);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UInt_t AliHMPIDPreprocessor::Process(TMap* pMap)
{
// Process: 1. inlet and outlet C6F14 temperature, stores TObjArray of 21 TF1, where TF1 is Nmean=f(t), one per radiator
//          2. CH4 pressure and HV                 stores TObjArray of 7 TF1 where TF1 is thr=f(t), one per chamber
// Arguments: pDcsMap - map of structure "alias name" - TObjArray of AliDCSValue
// Assume that: HV is the same during the run for a given chamber, different chambers might have different HV
//              P=f(t), different for different chambers
  TObjArray arDaqSig(14); arDaqSig.SetOwner(kTRUE);
  for(Int_t iddl=0;iddl<14;iddl++){
    arDaqSig.AddAt(new TObjArray(24),iddl);
    for(Int_t row=1;row<=24;row++){
      ((TObjArray*)arDaqSig.At(iddl))->Add(new TMatrixF(1,10,0,47));
    }
  }
  TObjArray arSigCut(14); arSigCut.SetOwner(kTRUE);                                  //14  n. of sigmas for zero suppression
  Int_t ddl,r,d,a,hard;
  Float_t mean,sigma;
  Int_t sigmaCut;
  for(Int_t iddl=0;iddl<14;iddl++){
    TObject *sigCut= new TObject; 
    const char *name=GetFile(kDAQ,"pedestals",Form("DDL%i",iddl));                   //first analyse a set of pedestal files
    ifstream infile(name);
    //
    infile>>sigmaCut;
    sigCut->SetUniqueID(sigmaCut);arSigCut.AddAt(sigCut,iddl);
    TObjArray *pTmp = (TObjArray*)arDaqSig.At(iddl);
    TMatrixF *pM;
    while(!infile.eof()){
      infile>>dec>>ddl>>r>>d>>a>>mean>>sigma>>hex>>hard;
      pM = (TMatrixF*)pTmp->At(r-1);
      (*pM)(d,a) = sigma;
    }
    //
    infile.close();
    Printf("file %s read successfully",name);
  }

  if(!pMap)  return 0;                    //no DCS map provided 
  Double_t sor=0,eor=1500;

  TF2 idx("RidxC4F14","sqrt(1+0.554*(1239.84/x)^2/((1239.84/x)^2-5796)-0.0005*(y-20))",5.5 ,8.5 ,0  ,50);  //N=f(Ephot,T) [eV,grad C] DiMauro mail
  TF2 thr("RthrCH4"  ,"x+170745848*exp(-y*0.0162012)"                                                      ,2000,3000,900,1200); //thr=f(HV,P)  [V,mBar]
//  Double_t eMean=6.67786;                                                                                  //mean energy of photon defined  by transperancy window
  
//  TObjArray arTmean(21); arTmean.SetOwner(kTRUE);     //21 Tmean=f(time) one per radiator
//  TObjArray arPress(7);  arPress.SetOwner(kTRUE);     //7  Press=f(time) one pre chamber
  TObjArray arNmean(21); arNmean.SetOwner (kTRUE);     //21 Nmean=f(time) one per radiator
  TObjArray arQthre(7);  arQthre.SetOwner (kTRUE);     //7  Qthre=f(time) one pre chamber
  
//  AliDCSValue *pVal; Int_t cnt=0;
    
  for(Int_t iCh=0;iCh<7;iCh++){            
 //   TObjArray *pHV=(TObjArray*)pMap->GetValue(Form("HMP_DET/HMP_MP%i/HMP_MP%i_PW/HMP_MP%i_SEC0/HMP_MP%i_SEC0_HV.actual.vMon",iCh,iCh,iCh,iCh)); //HV
//    pVal=(AliDCSValue*)pHV->At(0); Float_t hv=pVal->GetFloat();//HV    

//    TObjArray *pP =(TObjArray*)pMap->GetValue(Form(fP,iCh,iCh,iCh)); //P
//    TGraph *pGrP; cnt=0;TIter nextp(pP); while((pVal=(AliDCSValue*)nextp())) pGrP->SetPoint(cnt++,pVal->GetTimeStamp(),pVal->GetFloat()); //P
                                                                         
//    TF1 *pFuP=new TF1(Form("P%i",iCh),"1005",sor,eor); 
//    pGrP->Fit(pFuP,"Q"); delete pGrP;
    TF1 *pFuQ=new TF1(Form("HMP_Qthre%i",iCh),"100",sor,eor);
    arQthre.AddAt(pFuQ,iCh);    
    for(Int_t iRad=0;iRad<3;iRad++){
//      TObjArray *pT1=(TObjArray*)pMap->GetValue(Form(fT1,iCh,iCh,iRad)); //Tin
//      TGraph *pGrT1=new TGraph; cnt=0; TIter next1(pT1); while((pVal=(AliDCSValue*)next1())) pGrT1->SetPoint(cnt++,pVal->GetTimeStamp(),pVal->GetFloat()); //inlet temperature
//      pGrT1->Fit(new TF1(Form("Tin%i%i",iCh,iRad),"[0]+[1]*x+[2]*sin([3]*x)",sor,eor),"Q");       //now fit the temp graph 
      
//      TObjArray *pT2=(TObjArray*)pMap->GetValue(Form(fT2,iCh,iCh,iRad));//Tout      
//      TGraph *pGrT2=new TGraph; cnt=0; TIter next2(pT2); while((pVal=(AliDCSValue*)next2())) pGrT2->SetPoint(cnt++,pVal->GetTimeStamp(),pVal->GetFloat()); //outlet temperature
//      TF1*pFuT1=new TF1(Form("Tout%i%i",iCh,iRad),"13",sor,eor)      
//      pGrT2->Fit(,"Q");       //now fit the temp graph 
      
//      arTmean.Add(pRadTempF);  
      TF1 *pFuN=new TF1(Form("HMP_Nmean%i-%i",iCh,iRad),"1.292",sor,eor); pFuN->SetLineColor(iRad+2); pFuN->SetLineWidth(1);
      arNmean.AddAt(pFuN,3*iCh+iRad);
//      delete pGrT1;  delete pGrT2;
    }//radiators loop
  }//chambers loop
  
  AliCDBMetaData metaData; metaData.SetBeamPeriod(0); metaData.SetResponsible("AliHMPIDPreprocessor"); metaData.SetComment("SIMULATED");
  
//  arQthre.Print();
//  Store("Calib", "Press" , &arPress , &metaData); 
//  Store("Calib", "Tmean" , &arTmean , &metaData);
  
  if(Store("Calib","Qthre"    ,&arQthre , &metaData,0,kTRUE) == kTRUE && 
     Store("Calib","Nmean"    ,&arNmean , &metaData,0,kTRUE) == kTRUE &&   
     Store("Calib","DaqSigCut",&arSigCut, &metaData,0,kTRUE) == kTRUE &&
     Store("Calib","DaqSig"   ,&arDaqSig, &metaData,0,kTRUE) == kTRUE    ) return 0; //all OK
  else
    return 1;
  
  //  AliInfo("End.");  

}//Process()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
