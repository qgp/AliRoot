#include "AliHBTCorrelFctn.h"
//____________________________________________________________________________
//////////////////////////////////////////////////////////////////////////////
//
// class AliHBTQInvCorrelFctn
// class AliHBTQOutLCMSCorrelFctn
// class AliHBTQLongLCMSCorrelFctn
// class AliHBTQSideLCMSCorrelFctn
// class AliHBTInvMassCorrelFctn
// class AliHBTTwoKStarCorrelFctn
//
// Set of functions:
//   Q Invaraint Correlation Function
//   Invariant Mass Function
//
// more info: http://aliweb.cern.ch/people/skowron/analyzer/index.html
// Piotr.Skowronski@cern.ch
//
//////////////////////////////////////////////////////////////////////////////

#include <AliTrackPoints.h>

ClassImp(AliHBTQInvCorrelFctn)

//Corroleation function is created from dividing two histograms of QInvariant:
//  of particles from the same evnt
//by 
//  of particles from different events

AliHBTQInvCorrelFctn::AliHBTQInvCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qinvcf","Q_{inv} Correlation Function");
}
/*************************************************************************************/ 

TH1* AliHBTQInvCorrelFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 

ClassImp(AliHBTOutSideLongFctn)

AliHBTOutSideLongFctn::AliHBTOutSideLongFctn(Int_t nXbins, Double_t maxXval, Double_t minXval,
                                                   Int_t nYbins, Double_t maxYval, Double_t minYval,
                                                   Int_t nZbins, Double_t maxZval, Double_t minZval):
 AliHBTOnePairFctn3D(nXbins,maxXval,minXval,nYbins,maxYval,minYval,nZbins,maxZval,minZval)
{
//ctor
  fWriteNumAndDen = kTRUE;//change default behaviour
  Rename("qoslcf","Q_{out}-Q_{side}-Q_{long} Correlation Fctn");
}
/*************************************************************************************/ 

TH1* AliHBTOutSideLongFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}

void AliHBTOutSideLongFctn::GetValues(AliHBTPair* pair, Double_t& x, Double_t& y, Double_t& z) const
{ 
  //calculates values of that function
  //qout qside and qlong
  
  x=pair->GetQOutLCMS(); 
  y=pair->GetQSideLCMS(); 
  z=pair->GetQLongLCMS();
  if (fAbs)
   {
     x = TMath::Abs(x);
     y = TMath::Abs(y);
     z = TMath::Abs(z);
   }
} 

/*************************************************************************************/ 

ClassImp(AliHBTQOutLCMSCorrelFctn)
    
AliHBTQOutLCMSCorrelFctn::AliHBTQOutLCMSCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qoutcf","Q_{out} Correlation Function");
}
/*************************************************************************************/ 
    
TH1* AliHBTQOutLCMSCorrelFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 

ClassImp(AliHBTQLongLCMSCorrelFctn)
    
AliHBTQLongLCMSCorrelFctn::AliHBTQLongLCMSCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qlongcf","Q_{long} Correlation Function");
}
/*************************************************************************************/ 
    
TH1* AliHBTQLongLCMSCorrelFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 

ClassImp(AliHBTQSideLCMSCorrelFctn)
    
AliHBTQSideLCMSCorrelFctn::AliHBTQSideLCMSCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
 //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qsidecf","Q_{side} Correlation Function");
}
/*************************************************************************************/ 
    
TH1* AliHBTQSideLCMSCorrelFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}


/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 
ClassImp(AliHBTQtLCMSCorrelFctn)
    
AliHBTQtLCMSCorrelFctn::AliHBTQtLCMSCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("Qtcf","Q_{t}(LCMS) Correlation Function");
}
/*************************************************************************************/ 
    
TH1* AliHBTQtLCMSCorrelFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 
ClassImp(AliHBTQtCorrelFctn)
    
AliHBTQtCorrelFctn::AliHBTQtCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qtcf","Q_{t} Correlation Function");
}
/*************************************************************************************/ 
    
TH1* AliHBTQtCorrelFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 

ClassImp(AliHBTInvMassCorrelFctn)

AliHBTInvMassCorrelFctn::AliHBTInvMassCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
 //ctor 
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("InvMass CF","Invariant Mass Correlation Function");
}

TH1* AliHBTInvMassCorrelFctn::GetResult()
{
 //returns result
 return GetNumerator();
}
/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 

ClassImp(AliHBTTwoKStarCorrelFctn)

AliHBTTwoKStarCorrelFctn::AliHBTTwoKStarCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
 //ctor 
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("twokstarcf","2K^{*} Correlation Function");
}

/*************************************************************************************/ 

TH1* AliHBTTwoKStarCorrelFctn::GetResult()
{  
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}

/*************************************************************************************/ 
/*************************************************************************************/ 
/*************************************************************************************/ 
ClassImp(AliHBTAvSeparCorrelFctn)

AliHBTAvSeparCorrelFctn::AliHBTAvSeparCorrelFctn(Int_t nbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nbins,maxXval,minXval)
{
 //ctor 
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("avsepcf","Avarage separation Correlation Function");
}

/*************************************************************************************/ 

TH1* AliHBTAvSeparCorrelFctn::GetResult()
{  
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}

/*************************************************************************************/ 

ClassImp(AliHBTAvSeparVsQInvCorrelFctn)

AliHBTAvSeparVsQInvCorrelFctn::AliHBTAvSeparVsQInvCorrelFctn(Int_t nXbins, Double_t maxXval, Double_t minXval,
                                                             Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor 
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("avsepvsqinv","Avarage Separation VS Q_{inv} Correlation Function");
}


TH1* AliHBTAvSeparVsQInvCorrelFctn::GetResult()
{  
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
ClassImp(AliHBTITSSepVsQInvCorrelFctn)

AliHBTITSSepVsQInvCorrelFctn::AliHBTITSSepVsQInvCorrelFctn(Int_t layer,Int_t nXbins, Double_t maxXval, Double_t minXval,
                                                   Int_t nYbins, Double_t maxYval, Double_t minYval,
                                                   Int_t nZbins, Double_t maxZval, Double_t minZval):
 AliHBTOnePairFctn3D(nXbins,maxXval,minXval,nYbins,maxYval,minYval,nZbins,maxZval,minZval),
 fLayer(layer)
{
//ctor
  fWriteNumAndDen = kTRUE;//change default behaviour
  TString name("qinvvsitssep");
  TString title("2K^{*} Vs sep on Layer ");
  name+=fLayer;
  title+=fLayer;
  Rename(name,title);
}

/**************************************************************/

TH1* AliHBTITSSepVsQInvCorrelFctn::GetResult()
{  
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}

/**************************************************************/


void AliHBTITSSepVsQInvCorrelFctn::GetValues(AliHBTPair* pair, Double_t& x, Double_t& y, Double_t& z) const
{
//returns values of the function for a pair

  x = 10e5;
  y = -1;
  z = -1;  
  
  AliTrackPoints* tpts1 = pair->Particle1()->GetITSTrackPoints();
  if ( tpts1 == 0x0)
   {//it could be simulated pair
     Warning("GetValues","Track 1 does not have ITS Track Points. Pair NOT Passed.");
     return;
   }

  AliTrackPoints* tpts2 = pair->Particle2()->GetITSTrackPoints();
  if ( tpts2 == 0x0)
   {
     Warning("GetValues","Track 2 does not have ITS Track Points. Pair NOT Passed.");
     return;
   }
  Float_t  x1=0.0,y1=0.0,z1=0.0,x2=0.0,y2=0.0,z2=0.0;
  tpts1->PositionAt(fLayer,x1,y1,z1);
  tpts2->PositionAt(fLayer,x2,y2,z2);

//  Info("Pass","rphi %f z %f",fMin,fMax);
//  Info("Pass","P1: %f %f %f", x1,y1,z1);
//  Info("Pass","P2: %f %f %f", x2,y2,z2);
  x = 2.0*pair->GetQInv();
  z = TMath::Abs(z1-z2);
  y = TMath::Hypot(x1-x2,y1-y2);
  
  
}




/**************************************************************/
/**************************************************************/
/**************************************************************/
ClassImp(AliHBTITSSepVsTwoKStarCorrelFctn)

AliHBTITSSepVsTwoKStarCorrelFctn::AliHBTITSSepVsTwoKStarCorrelFctn(Int_t layer,Int_t nXbins, Double_t maxXval, Double_t minXval,
                                                   Int_t nYbins, Double_t maxYval, Double_t minYval,
                                                   Int_t nZbins, Double_t maxZval, Double_t minZval):
 AliHBTOnePairFctn3D(nXbins,maxXval,minXval,nYbins,maxYval,minYval,nZbins,maxZval,minZval),
 fLayer(layer)
{
//ctor
  fWriteNumAndDen = kTRUE;//change default behaviour
  TString name("qinvvsitssep");
  TString title("2K^{*} Vs sep on Layer ");
  name+=fLayer;
  title+=fLayer;
  Rename(name,title);
}

/**************************************************************/

TH1* AliHBTITSSepVsTwoKStarCorrelFctn::GetResult()
{  
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}

/**************************************************************/


void AliHBTITSSepVsTwoKStarCorrelFctn::GetValues(AliHBTPair* pair, Double_t& x, Double_t& y, Double_t& z) const
{
//returns values of the function for a pair

  x = 10e5;
  y = -1;
  z = -1;  
  
  AliTrackPoints* tpts1 = pair->Particle1()->GetITSTrackPoints();
  if ( tpts1 == 0x0)
   {//it could be simulated pair
     Warning("GetValues","Track 1 does not have ITS Track Points. Pair NOT Passed.");
     return;
   }

  AliTrackPoints* tpts2 = pair->Particle2()->GetITSTrackPoints();
  if ( tpts2 == 0x0)
   {
     Warning("GetValues","Track 2 does not have ITS Track Points. Pair NOT Passed.");
     return;
   }
  Float_t  x1=0.0,y1=0.0,z1=0.0,x2=0.0,y2=0.0,z2=0.0;
  tpts1->PositionAt(fLayer,x1,y1,z1);
  tpts2->PositionAt(fLayer,x2,y2,z2);

//  Info("Pass","rphi %f z %f",fMin,fMax);
//  Info("Pass","P1: %f %f %f", x1,y1,z1);
//  Info("Pass","P2: %f %f %f", x2,y2,z2);
  x = 2.0*pair->GetKStar();
  z = TMath::Abs(z1-z2);
  y = TMath::Hypot(x1-x2,y1-y2);
  
  
}





/**************************************************************/
/**************************************************************/
/**************************************************************/
ClassImp(AliHBTITSSepVsTwoKStarSideCorrelFctn)

AliHBTITSSepVsTwoKStarSideCorrelFctn::AliHBTITSSepVsTwoKStarSideCorrelFctn(Int_t layer,Int_t nXbins, Double_t maxXval, Double_t minXval,
                                                   Int_t nYbins, Double_t maxYval, Double_t minYval,
                                                   Int_t nZbins, Double_t maxZval, Double_t minZval):
 AliHBTOnePairFctn3D(nXbins,maxXval,minXval,nYbins,maxYval,minYval,nZbins,maxZval,minZval),
 fLayer(layer)
{
//ctor
  fWriteNumAndDen = kTRUE;//change default behaviour
  TString name("twpkstarsidevsitssep");
  TString title("2K^{*}_{side} Vs sep on Layer ");
  name+=fLayer;
  title+=fLayer;
  Rename(name,title);
}

/**************************************************************/

TH1* AliHBTITSSepVsTwoKStarSideCorrelFctn::GetResult()
{  
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}

/**************************************************************/


void AliHBTITSSepVsTwoKStarSideCorrelFctn::GetValues(AliHBTPair* pair, Double_t& x, Double_t& y, Double_t& z) const
{
//returns values of the function for a pair

  x = 10e5;
  y = -1;
  z = -1;  
  
  AliTrackPoints* tpts1 = pair->Particle1()->GetITSTrackPoints();
  if ( tpts1 == 0x0)
   {//it could be simulated pair
     Warning("GetValues","Track 1 does not have ITS Track Points. Pair NOT Passed.");
     return;
   }

  AliTrackPoints* tpts2 = pair->Particle2()->GetITSTrackPoints();
  if ( tpts2 == 0x0)
   {
     Warning("GetValues","Track 2 does not have ITS Track Points. Pair NOT Passed.");
     return;
   }
  Float_t  x1=0.0,y1=0.0,z1=0.0,x2=0.0,y2=0.0,z2=0.0;
  tpts1->PositionAt(fLayer,x1,y1,z1);
  tpts2->PositionAt(fLayer,x2,y2,z2);

//  Info("Pass","rphi %f z %f",fMin,fMax);
//  Info("Pass","P1: %f %f %f", x1,y1,z1);
//  Info("Pass","P2: %f %f %f", x2,y2,z2);

  x = 2.0*pair->GetKStarSide();
  z = TMath::Abs(z1-z2);
  y = TMath::Hypot(x1-x2,y1-y2);
  
  
}


/**************************************************************/
/**************************************************************/
/**************************************************************/


ClassImp(AliHBTQOutQSideFctn)


AliHBTQOutQSideFctn::AliHBTQOutQSideFctn(Int_t nxbins, Double_t maxXval, Double_t minXval,
                                         Int_t nybins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nxbins,maxXval,minXval,nybins,maxYval,minYval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qoutqsidecf","Q_{out} Q_{side} Correlation Function 2D");
}    
/**************************************************************/

TH1* AliHBTQOutQSideFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/**************************************************************/
/**************************************************************/

ClassImp(AliHBTQOutQLongFctn)

AliHBTQOutQLongFctn::AliHBTQOutQLongFctn(Int_t nxbins, Double_t maxXval, Double_t minXval,
                                                         Int_t nybins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nxbins,maxXval,minXval,nybins,maxYval,minYval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qoutqlongcf","Q_{out} Q_{long} Correlation Function 2D");
}    


/**************************************************************/

TH1* AliHBTQOutQLongFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/

ClassImp(AliHBTQSideQLongFctn)

/**************************************************************/
AliHBTQSideQLongFctn::AliHBTQSideQLongFctn(Int_t nxbins, Double_t maxXval, Double_t minXval,
                                                         Int_t nybins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nxbins,maxXval,minXval,nybins,maxYval,minYval)
{
  //ctor
 fWriteNumAndDen = kTRUE;//change default behaviour
 Rename("qsideqlongcf","Q_{side} Q_{long} Correlation Function 2D");
}    

TH1* AliHBTQSideQLongFctn::GetResult()
{
 //returns the scaled ratio
 delete fRatio;
 fRatio = GetRatio(Scale());
 return fRatio;
}



