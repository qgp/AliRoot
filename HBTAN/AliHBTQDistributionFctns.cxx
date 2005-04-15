#include "AliHBTQDistributionFctns.h"
//__________________________________________________________________
////////////////////////////////////////////////////////////////////
//                                                                //
// class AliHBTQInvDistributionVsKtFctn                           //
// class AliHBTQOutDistributionVsKtFctn                           //
// class AliHBTQSideDistributionVsKtFctn                          //
// class AliHBTQLongDistributionVsKtFctn                          //
// class AliHBTQOutDistributionVsQInvFctn                         //
// class AliHBTQSideDistributionVsQInvFctn                        //
// class AliHBTQLongDistributionVsQInvFctn                        //
// class AliHBTPtDiffDistributionVsQInvFctn                       //
//                                                                //
// Classes for Q's monitoring Vs Kt and Vs Qinv                   //
//                                                                //
// Author:                                                        //
// Zbigniew Chajecki <chajecki@if.pw.edu.pl>                      //
//                                                                //
////////////////////////////////////////////////////////////////////

/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQInvDistributionVsKtFctn )

AliHBTQInvDistributionVsKtFctn::AliHBTQInvDistributionVsKtFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                               Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QInvDistributionVsKt","Q_{Inv} Distribution vs. K_{t}");
}
/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQOutDistributionVsKtFctn )

AliHBTQOutDistributionVsKtFctn::AliHBTQOutDistributionVsKtFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                               Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QOutDistributionVsKt","Q_{Out} Distribution vs. K_{t}");
}
/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQSideDistributionVsKtFctn )

AliHBTQSideDistributionVsKtFctn::AliHBTQSideDistributionVsKtFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                                 Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QSideDistributionVsKt","Q_{Side} Distribution vs. K_{t}");
}
/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQLongDistributionVsKtFctn )

AliHBTQLongDistributionVsKtFctn::AliHBTQLongDistributionVsKtFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                                 Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QLongDistributionVsKt","Q_{Long} Distribution vs. K_{t}");
}
/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQOutDistributionVsQInvFctn )

AliHBTQOutDistributionVsQInvFctn::AliHBTQOutDistributionVsQInvFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                                   Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QOutDistributionVsQInv","Q_{Out} Distribution vs. Q_{inv}");
}

/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQSideDistributionVsQInvFctn )

AliHBTQSideDistributionVsQInvFctn::AliHBTQSideDistributionVsQInvFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                                     Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QSideDistributionVsQInv","Q_{Side} Distribution vs. Q_{inv}");
}

/******************************************************************/
/******************************************************************/

ClassImp( AliHBTQLongDistributionVsQInvFctn )

AliHBTQLongDistributionVsQInvFctn::AliHBTQLongDistributionVsQInvFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                                     Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("QLongDistributionVsQInv","Q_{Long} Distribution vs. Q_{inv}");
}

/******************************************************************/
/******************************************************************/

ClassImp( AliHBTPtDiffDistributionVsQInvFctn )

AliHBTPtDiffDistributionVsQInvFctn::AliHBTPtDiffDistributionVsQInvFctn(Int_t nXbins, Double_t maxXval, Double_t minXval, 
                                                                     Int_t nYbins, Double_t maxYval, Double_t minYval):
 AliHBTOnePairFctn2D(nXbins,maxXval,minXval,nYbins,maxYval,minYval)
{
 //ctor
 Rename("PtDiffDistributionVsQInv","P_{t} Difference Distribution vs. Q_{inv}");
}
/******************************************************************/
/******************************************************************/

ClassImp(AliHBTRStarDistribution)


AliHBTRStarDistribution::AliHBTRStarDistribution(Int_t nXbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nXbins,maxXval,minXval)
{
//ctor
 Rename("RStarDistribution","R^{*} distribution");
}

/******************************************************************/
/******************************************************************/

ClassImp(AliHBTRDistribution)

AliHBTRDistribution::AliHBTRDistribution(Int_t nXbins, Double_t maxXval, Double_t minXval):
 AliHBTOnePairFctn1D(nXbins,maxXval,minXval)
{
//ctor
 Rename("RDistribution","R (distance between creation points) distribution ");
}

/******************************************************************/
/******************************************************************/

ClassImp(AliHBTTimeDiffDistribution)

AliHBTTimeDiffDistribution::AliHBTTimeDiffDistribution(Int_t nXbins, Double_t maxXval, Double_t minXval):
  AliHBTOnePairFctn1D(nXbins,maxXval,minXval)
{
//ctor
  Rename("TimeDiffDistribution","TimeDiff distribution");
}

/******************************************************************/
/******************************************************************/
/******************************************************************/
/******************************************************************/
