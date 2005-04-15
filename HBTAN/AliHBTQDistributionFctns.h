#ifndef ALIHBTQOUTDISTRIBUTIONVSKTLFCTN_H
#define ALIHBTQOUTDISTRIBUTIONVSKTLFCTN_H

/////////////////////////////////////////////////////////////////////////////
// 
// class AliHBTQInvDistributionVsKtFctn;    //QInvLCMS   Distribution Vs   Kt
// class AliHBTQOutDistributionVsKtFctn;    //QOutLCMS   Distribution Vs   Kt
// class AliHBTQSideDistributionVsKtFctn;   //QSideLCMS  Distribution Vs   Kt
// class AliHBTQLongDistributionVsKtFctn;   //QLongLCMS  Distribution Vs   Kt

// class AliHBTQOutDistributionVsQInvFctn;    //QOutLCMS   Distribution Vs   QInv
// class AliHBTQSideDistributionVsQInvFctn;   //QSideLCMS  Distribution Vs   QInv
// class AliHBTQLongDistributionVsQInvFctn;   //QLongLCMS  Distribution Vs   QInv
// class AliHBTPtDiffDistributionVsQInvFctn;
//
// added by Zbigniew.Chajecki@cern.ch
// this classes create distribution functions of pair momentum 
//
/////////////////////////////////////////////////////////////////////////////

class AliHBTQInvDistributionVsKtFctn;    //QInvLCMS   Distribution Vs   Kt
class AliHBTQOutDistributionVsKtFctn;    //QOutLCMS   Distribution Vs   Kt
class AliHBTQSideDistributionVsKtFctn;   //QSideLCMS  Distribution Vs   Kt
class AliHBTQLongDistributionVsKtFctn;   //QLongLCMS  Distribution Vs   Kt

class AliHBTQOutDistributionVsQInvFctn;    //QOutLCMS   Distribution Vs   QInv
class AliHBTQSideDistributionVsQInvFctn;   //QSideLCMS  Distribution Vs   QInv
class AliHBTQLongDistributionVsQInvFctn;   //QLongLCMS  Distribution Vs   QInv
class AliHBTPtDiffDistributionVsQInvFctn;
class AliHBTRStarDistribution;
#include "AliHBTFunction.h"

/***********************************************************************/
/***********************************************************************/
class AliHBTQOutDistributionVsKtFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQOutDistributionVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   virtual ~AliHBTQOutDistributionVsKtFctn(){}
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQOutLCMS();
     x = partpair->GetKt();
    }
  protected:
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
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQSideLCMS();
     x = partpair->GetKt();
    }
  protected:
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
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQLongLCMS();
     x = partpair->GetKt();
    }
  protected:
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
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQInv();
     x = partpair->GetKt();
    }
  protected:
    ClassDef(AliHBTQInvDistributionVsKtFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTQOutDistributionVsQInvFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQOutDistributionVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   virtual ~AliHBTQOutDistributionVsQInvFctn(){}
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQOutLCMS();
     x = partpair->GetQInv();
    }
  protected:
    ClassDef(AliHBTQOutDistributionVsQInvFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTQSideDistributionVsQInvFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQSideDistributionVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 1.2, Double_t minXval = -0.1, 
                             Int_t nYbins = 500, Double_t maxYval = 1.2, Double_t minYval =-1.2);
   virtual ~AliHBTQSideDistributionVsQInvFctn(){}
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQSideLCMS();
     x = partpair->GetQInv();
    }
  protected:
    ClassDef(AliHBTQSideDistributionVsQInvFctn,1)
 };
/***********************************************************************/
/***********************************************************************/

class AliHBTQLongDistributionVsQInvFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTQLongDistributionVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 1.2, Double_t minXval = -0.1, 
                             Int_t nYbins = 500, Double_t maxYval = 1.2, Double_t minYval =-1.2);
   virtual ~AliHBTQLongDistributionVsQInvFctn(){}
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->GetQLongLCMS();
     x = partpair->GetQInv();
    }
  protected:
    ClassDef(AliHBTQLongDistributionVsQInvFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTPtDiffDistributionVsQInvFctn: public AliHBTOnePairFctn2D
 {
  public: 
   AliHBTPtDiffDistributionVsQInvFctn(Int_t nXbins = 800, Double_t maxXval = 4.0, Double_t minXval = 0., 
                             Int_t nYbins = 500, Double_t maxYval = 0.1, Double_t minYval =-0.1);
   virtual ~AliHBTPtDiffDistributionVsQInvFctn(){}
   TH1* GetResult(){return this->GetNumerator();}
   void GetValues(AliHBTPair* partpair, Double_t& x, Double_t& y) const
    {
     y = partpair->Particle1()->Pt() - partpair->Particle2()->Pt();
     x = partpair->GetQInv();
    }
  protected:
    ClassDef(AliHBTPtDiffDistributionVsQInvFctn,1)
 };
/***********************************************************************/
/***********************************************************************/

class AliHBTRStarDistribution: public AliHBTOnePairFctn1D
{
  public:
    AliHBTRStarDistribution(Int_t nXbins = 500, Double_t maxXval = 5e-11, Double_t minXval = 0.);
    virtual ~AliHBTRStarDistribution(){}
    TH1* GetResult(){return this->GetNumerator();}
  protected:
    Double_t GetValue(AliHBTPair* partpair) const
    {
      return partpair->GetRStar();
    }
   
  private:
   ClassDef(AliHBTRStarDistribution,1)
};

/***********************************************************************/
/***********************************************************************/

class AliHBTRDistribution: public AliHBTOnePairFctn1D
{
  public:
    AliHBTRDistribution(Int_t nXbins = 500, Double_t maxXval = 5e-11, Double_t minXval = 0.);
    virtual ~AliHBTRDistribution(){}
    TH1* GetResult(){return this->GetNumerator();}
  protected:
    Double_t GetValue(AliHBTPair* partpair) const
    {
      return partpair->GetR();
    }
   
  private:
   ClassDef(AliHBTRDistribution,1)
};

/***********************************************************************/
/***********************************************************************/

class AliHBTTimeDiffDistribution: public AliHBTOnePairFctn1D
{
 public:
    AliHBTTimeDiffDistribution(Int_t nXbins = 200, Double_t maxXval = 10., Double_t minXval = -10.);
    virtual ~AliHBTTimeDiffDistribution(){}
    TH1* GetResult(){return this->GetNumerator();}
  protected:
   Double_t GetValue(AliHBTPair* partpair) const
   {
//      Info("TimeDiff of first particle","%lf",partpair->Particle1()->T());
      return 1e13*(partpair->Particle1()->T()-partpair->Particle2()->T());
    }

  private:
   ClassDef(AliHBTTimeDiffDistribution,1)
};


#endif

