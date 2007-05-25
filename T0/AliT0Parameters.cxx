/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id:  */

//____________________________________________________________________
//                                                                          
// T0 - T0. 
//
// This class is a singleton that handles various parameters of
// the T0 detectors.  
// Eventually, this class will use the Conditions DB to get the
// various parameters, which code can then request from here.
//                                                       
#include "AliLog.h"		  
#include "AliT0Parameters.h"	  
#include "AliT0CalibData.h"   
#include "AliT0LookUpValue.h"
#include <AliCDBManager.h>        
#include <AliCDBEntry.h>          
#include <AliCDBStorage.h>  
#include <TMath.h>
#include <TSystem.h>
#include <Riostream.h>
#include <TGeoManager.h>
#include <TGeoPhysicalNode.h>

AliT0CalibData* AliT0Parameters::fgCalibData = 0;
AliT0CalibData* AliT0Parameters::fgLookUp = 0;
AliT0CalibData* AliT0Parameters::fgSlewCorr =0;
//====================================================================
ClassImp(AliT0Parameters)
#if 0
  ; // This is here to keep Emacs for indenting the next line
#endif

//____________________________________________________________________
AliT0Parameters* AliT0Parameters::fgInstance = 0;
//____________________________________________________________________
AliT0Parameters* 
AliT0Parameters::Instance() 
{
  // Get static instance 
  if (!fgInstance) {
    fgInstance = new AliT0Parameters;
    fgInstance->Init();
  }
  return fgInstance;
}

//____________________________________________________________________
AliT0Parameters::AliT0Parameters()
  :fIsInit(kFALSE),
   fPh2Mip(0),fmV2Mip(0),
   fChannelWidth(0),fmV2Channel(0),
   fQTmin(0),fQTmax(0),
   fSlewingLED(),fSlewingRec(),
   fPMTeff(),
   fTimeDelayDA(0),fTimeDelayCFD(0),fTimeDelayTVD(0),fMeanT0(499),
   fCalibentry(), fLookUpentry(),fSlewCorr(),
   fLookUp(0), fNumberOfTRMs(0)

{
  // Default constructor 

  for (Int_t ipmt=0; ipmt<24; ipmt++)
    {
      SetSlewingLED(ipmt);
      SetSlewingRec(ipmt);
      SetWalk(ipmt);
      SetPh2Mip();      
      SetmV2Mip();      
      SetChannelWidth();
      SetmV2channel();
      SetQTmin();
      SetQTmax();
      SetPMTeff(ipmt);

   }
  SetTimeDelayTVD();
  SetZposition();
  SetNumberOfTRMs(2);
  
}

//__________________________________________________________________
void
AliT0Parameters::Init()
{
  // Initialize the parameters manager.  We need to get stuff from the
  // CDB here. 
 
   if (fIsInit) return;

  AliCDBManager *stor =AliCDBManager::Instance();
  //time equalizing
  AliCDBEntry* fCalibentry  = stor->Get("T0/Calib/TimeDelay");
  if (fCalibentry)
   fgCalibData  = (AliT0CalibData*)fCalibentry->GetObject();
  else {
    AliError(" ALARM !!!! No time delays in CDB "); 
    fIsInit = kFALSE;
    return;
  }
 //slewing correction
  AliCDBEntry* fSlewCorr  = stor->Get("T0/Calib/Slewing_Walk");
  if (fSlewCorr){
    fgSlewCorr  = (AliT0CalibData*)fSlewCorr->GetObject();
  }
  fLookUpentry  = stor->Get("T0/Calib/LookUp_Table");
  if (fLookUpentry){
    fgLookUp  = (AliT0CalibData*)fLookUpentry->GetObject();
  }
  else {
  const char * filename = gSystem->ExpandPathName("$ALICE_ROOT/T0/lookUpTable.txt");
  ifstream inFile(filename);
  fgLookUp->ReadAsciiLookup(filename);
  }

  fIsInit = kTRUE;
}


//__________________________________________________________________
Float_t
AliT0Parameters::GetTimeDelayDA(Int_t ipmt) 
{
  // return time delay for LED channel
  // 
  if (!fCalibentry) {
    fTimeDelayDA = 500;
    return  fTimeDelayDA;
  } 
  return fgCalibData ->GetTimeDelayDA(ipmt);
}
//__________________________________________________________________
Float_t
AliT0Parameters::GetTimeDelayCFD(Int_t ipmt) 
{
  // return time delay for CFD channel
   // 
  if (!fCalibentry) 
    {
      fTimeDelayCFD = 1000+ipmt*100;
      return fTimeDelayCFD;
    }
   
  return fgCalibData->GetTimeDelayCFD(ipmt);
}

//__________________________________________________________________
Int_t
AliT0Parameters::GetMeanT0() 
{
  // return mean of T0 distrubution with vertex=0
   // 
  if (!fCalibentry) 
    {
      return fMeanT0;
    }
   
  return fgCalibData->GetMeanT0();
}
//__________________________________________________________________

void 
AliT0Parameters::SetSlewingLED(Int_t ipmt)
{
  //  Set Slweing Correction for LED channel 
     Float_t mv[23] = {25, 30,40,60, 80,100,150,200,250,300,
		       400,500,600,800,1000,1500, 2000, 3000, 4000, 5500,
		       6000, 7000,8000};
      Float_t y[23] = {5044, 4719, 3835, 3224, 2847, 2691,2327, 2067, 1937, 1781,
		       1560, 1456 ,1339, 1163.5, 1027, 819, 650, 520, 370.5, 234,
		       156, 78, 0};
      
      TGraph* gr = new TGraph(23,mv,y);
      fSlewingLED.AddAtAndExpand(gr,ipmt);
  }
//__________________________________________________________________

Float_t AliT0Parameters::GetSlewingLED(Int_t ipmt, Float_t mv) const
{
  if (!fCalibentry) {
    return ((TGraph*)fSlewingLED.At(ipmt))->Eval(mv); 
  } 
  return fgCalibData->GetSlewingLED(ipmt, mv) ;
}


//__________________________________________________________________

TGraph *AliT0Parameters::GetSlew(Int_t ipmt) const
{
  if (!fCalibentry) {
    return  (TGraph*)fSlewingLED.At(ipmt); 
  } 
  return fgCalibData -> GetSlew(ipmt) ;
}

//__________________________________________________________________


void 
AliT0Parameters::SetSlewingRec(Int_t ipmt)
{
  //  Set Slweing Correction for LED channel 
      Float_t mv[23] = {25, 30, 40,60, 80,100,150,200,250,300,
			400,500,600,800,1000,1500, 2000, 3000, 4000, 5500,
			6000, 7000,8000};
      Float_t y[23] = {5044, 4719, 3835, 3224, 2847, 2691,2327, 2067, 1937, 1781, 
		       1560, 1456 ,1339, 1163.5, 1027, 819, 650, 520, 370.5, 234, 
		       156, 78, 0};
      Float_t y1[23], mv1[23];
      for (Int_t i=0; i<23; i++){
	y1[i] = y[22-i]; mv1[i] = mv[22-i];}

      TGraph* gr = new TGraph(23,y1,mv1);
      fSlewingRec.AddAtAndExpand(gr,ipmt);

}
//__________________________________________________________________

Float_t AliT0Parameters::GetSlewingRec(Int_t ipmt, Float_t mv) const
{
  if (!fCalibentry) {
    return ((TGraph*)fSlewingRec.At(ipmt))->Eval(mv); 
  } 
  return fgCalibData -> GetSlewingRec(ipmt, mv) ;
}

//__________________________________________________________________

TGraph *AliT0Parameters::GetSlewRec(Int_t ipmt) const
{
  if (!fCalibentry) {
    return  (TGraph*)fSlewingRec.At(ipmt); 
  } 
  return fgCalibData -> GetSlewRec(ipmt) ;
}


//________________________________________________________________
void AliT0Parameters::SetWalk(Int_t ipmt)
{

 Int_t mv, ps; 
  Int_t x[70000], y[70000], index[70000];
  Float_t time[10000],amplitude[10000];
  string buffer;
  Bool_t down=false;
  
  const char * filename = gSystem->ExpandPathName("$ALICE_ROOT/T0/data/CFD-Amp.txt");
  ifstream inFile(filename);
  //  if(!inFile) AliError(Form("Cannot open file %s !",filename));
  
  Int_t i=0;
  while(getline(inFile,buffer)){
    inFile >> ps >> mv;

    x[i]=ps; y[i]=mv;
    i++;
  }
  inFile.close();
 
  TMath::Sort(i, y, index,down);
  Int_t amp=0, iin=0, isum=0, sum=0;
  Int_t ind=0;
  for (Int_t ii=0; ii<i; ii++)
    {
      ind=index[ii];
      if(y[ind] == amp)
	{
	  sum +=x[ind];
	  iin++;
	}
      else
	{
	  if(iin>0)
	    time[isum] = Float_t (sum/(iin));
	  else
	    time[isum] =Float_t (x[ind]);
	  amplitude[isum] = Float_t (amp);
	  amp=y[ind];
	  //	  cout<<ii<<" "<<ind<<" "<<y[ind]<<" "<<x[ind]<<" iin "<<iin<<" mean "<<time[isum]<<" amp "<< amplitude[isum]<<" "<<isum<<endl;
	  iin=0;
	  isum++;
	  sum=0;
	}


    }

  inFile.close();

   TGraph* gr = new TGraph(isum, amplitude, time);
  fWalk.AddAtAndExpand(gr,ipmt);
  
  
}
//__________________________________________________________________

TGraph *AliT0Parameters::GetWalk(Int_t ipmt) const
{
  if (!fCalibentry) {
    return  (TGraph*)fWalk.At(ipmt); 
  } 
  return fgCalibData -> GetWalk(ipmt) ;
}

//__________________________________________________________________

Float_t AliT0Parameters::GetWalkVal(Int_t ipmt, Float_t mv) const
{
  if (!fCalibentry) {
    return ((TGraph*)fWalk.At(ipmt))->Eval(mv); 
  } 
  return fgCalibData -> GetWalkVal(ipmt, mv) ;
}


//__________________________________________________________________
void 
AliT0Parameters::SetPMTeff(Int_t ipmt)
{
  Float_t lambda[50];
  Float_t eff[50 ] = {0,        0,       0.23619,  0.202909, 0.177913, 
		    0.175667, 0.17856, 0.190769, 0.206667, 0.230286,
		    0.252276, 0.256267,0.26,     0.27125,  0.281818,
		    0.288118, 0.294057,0.296222, 0.301622, 0.290421, 
		    0.276615, 0.2666,  0.248,    0.23619,  0.227814, 
		    0.219818, 0.206667,0.194087, 0.184681, 0.167917, 
		    0.154367, 0.1364,  0.109412, 0.0834615,0.0725283, 
		    0.0642963,0.05861, 0.0465,   0.0413333,0.032069, 
		    0.0252203,0.02066, 0.016262, 0.012,    0.00590476,
		    0.003875, 0.00190, 0,        0,        0          } ;
  for (Int_t i=0; i<50; i++) lambda[i]=200+10*i; 

  TGraph* gr = new TGraph(50,lambda,eff);
  fPMTeff.AddAtAndExpand(gr,ipmt);
}
//________________________________________________________________

Int_t 
AliT0Parameters::GetChannel(Int_t trm,  Int_t tdc, Int_t chain, Int_t channel)
{

  
  AliT0LookUpKey * lookkey;  //= new AliT0LookUpKey();
  AliT0LookUpValue * lookvalue= new AliT0LookUpValue(trm,tdc,chain,channel);
    
   lookkey = (AliT0LookUpKey*) fgLookUp->GetMapLookup()->GetValue((TObject*)lookvalue);
  if (!lookkey ) {
    cout<<" no such address "<<endl; return -1;
  }
  

  //cout<<"AliT0Parameters:: key "<<lookkey->GetKey()<<endl;
  return lookkey->GetKey();
  

}
//__________________________________________________________________
Int_t
AliT0Parameters::GetNumberOfTRMs() 
{
  // return number of trms
  // 
  if (!fgLookUp) {
    fNumberOfTRMs = 2;
    return  fNumberOfTRMs;
  } 
  return  fgLookUp ->GetNumberOfTRMs();
}
//________________________________________________________________________________
Double_t AliT0Parameters::GetZPosition(const char* symname){
// Get the global z coordinate of the given T0 alignable volume
//
  Double_t *tr;
  
  cout<<symname<<endl;
  TGeoPNEntry *pne = gGeoManager->GetAlignableEntry(symname);
  if (!pne) return 0;
  

  TGeoPhysicalNode *pnode = pne->GetPhysicalNode();
  if(pnode){
          TGeoHMatrix* hm = pnode->GetMatrix();
           tr = hm->GetTranslation();
  }else{
          const char* path = pne->GetTitle();
          if(!gGeoManager->cd(path)){
                  AliErrorClass(Form("Volume path %s not valid!",path));
                  return 0;
          }
         tr = gGeoManager->GetCurrentMatrix()->GetTranslation();
  }
  return tr[2];

}

