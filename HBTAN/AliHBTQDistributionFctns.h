#ifndef ALIHBTQOUTDISTRIBUTIONVSKTLFCTN_H
#define ALIHBTQOUTDISTRIBUTIONVSKTLFCTN_H

// added by Zbigniew.Chajecki@cern.ch
// this classes create distribution functions of pair momentum 

class AliHBTQInvDistributionVsKtFctn;    //QInvCMSLC   Distribution Vs   Kt
class AliHBTQOutDistributionVsKtFctn;    //QOutCMSLC   Distribution Vs   Kt
class AliHBTQSideDistributionVsKtFctn;   //QSideCMSLC  Distribution Vs   Kt
class AliHBTQLongDistributionVsKtFctn;   //QLongCMSLC  Distribution Vs   Kt

#include "AliHBTFunction.h"

/***********************************************************************/
/***********************************************************************/
class AliHBTQOutDistributionVsKtFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQOutDistributionVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   virtual ~AliHBTQOutDistributionVsKtFctn(){}
   TH1* GetResult(){return GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQOutCMSLC();
     x = partpair->GetKt();
    }
  protected:
  private:
  public:
    ClassDef(AliHBTQOutDistributionVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTQSideDistributionVsKtFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQSideDistributionVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1.2, Double_t minXval = -0.1, 
                             Int_t nYbins = 500, Double_t maxYval = 1.2, Double_t minYval =-1.2);
   virtual ~AliHBTQSideDistributionVsKtFctn(){}
   TH1* GetResult(){return GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQSideCMSLC();
     x = partpair->GetKt();
    }
  protected:
  private:
  public:
    ClassDef(AliHBTQSideDistributionVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/

class AliHBTQLongDistributionVsKtFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQLongDistributionVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1.2, Double_t minXval = -0.1, 
                             Int_t nYbins = 500, Double_t maxYval = 1.2, Double_t minYval =-1.2);
   virtual ~AliHBTQLongDistributionVsKtFctn(){}
   TH1* GetResult(){return GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQLongCMSLC();
     x = partpair->GetKt();
    }
  protected:
  private:
  public:
    ClassDef(AliHBTQLongDistributionVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/

class AliHBTQInvDistributionVsKtFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQInvDistributionVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1.2, Double_t minXval = -0.1, 
                             Int_t nYbins = 500, Double_t maxYval = 1.2, Double_t minYval =-1.2);
   virtual ~AliHBTQInvDistributionVsKtFctn(){}
   TH1* GetResult(){return GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQInv();
     x = partpair->GetKt();
    }
  protected:
  private:
  public:
    ClassDef(AliHBTQInvDistributionVsKtFctn,1)
 };

/***********************************************************************/
/***********************************************************************/

#endif
