#ifndef ALIHBTQOUTVsQINVRESOLFCTN_H
#define ALIHBTQOUTVsQINVRESOLFCTN_H
//General Remark:
//CMSLC means
//Center of Mass System Longitudially Co-moving


//this class creates resolution function of Qout 
//(difference of simulated pair Qout and recontructed pair)
//in function of QInv
//it inherits from AliHBTTwoPairFctn2D
//  it needs two pairs to compare
//  and is two dimentional: numerator and denominator are TH2D

class AliHBTQOutResolVsQInvFctn;  //QOutCMSLC  Res   Vs   QInvCMSLC 
class AliHBTQSideResolVsQInvFctn; //QSideCMSLC Res   Vs   QInvCMSLC 
class AliHBTQLongResolVsQInvFctn; //QLongCMSLC Res   Vs   QInvCMSLC 
class AliHBTQInvResolVsQInvFctn;  //QInvCMSLC Res   Vs   QInvCMSLC 

class AliHBTPairThetaResolVsQInvFctn;//PairTheta Res   Vs   QInvCMSLC
class AliHBTPairPhiResolVsQInvFctn;  //PairPhi   Res   Vs   QInvCMSLC

class AliHBTQInvResolVsKtFctn;    //QInvCMSLC  Res   Vs   Kt
class AliHBTQOutResolVsKtFctn;    //QOutCMSLC  Res   Vs   Kt
class AliHBTQSideResolVsKtFctn;   //QSideCMSLC Res   Vs   Kt
class AliHBTQLongResolVsKtFctn;   //QLongCMSLC Res   Vs   Kt

class AliHBTPairThetaResolVsKtFctn;   //PairTheta Res   Vs   Kt
class AliHBTPairPhiResolVsKtFctn;   //PairPhi Res   Vs   Kt


class AliHBTQOutResolVsQOutFctn;  //QOutCMSLC  Res   Vs   QOut
class AliHBTQSideResolVsQSideFctn;//QSideCMSLC Res   Vs   QSide
class AliHBTQLongResolVsQLongFctn;//QLongCMSLC Res   Vs   QLong


 
#include "AliHBTFunction.h"
/***********************************************************************/
/***********************************************************************/
class AliHBTQInvResolVsQInvFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQInvResolVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   
   virtual ~AliHBTQInvResolVsQInvFctn(){}
   
   TH1* GetResult(){return fNumerator;}  
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQInv() - trackpair->GetQInv();
     x = partpair->GetQInv();
    }
  protected:
  private: 
  public:
    ClassDef(AliHBTQInvResolVsQInvFctn,1)
 };


/***********************************************************************/
/***********************************************************************/
class AliHBTQOutResolVsQInvFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQOutResolVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   
   virtual ~AliHBTQOutResolVsQInvFctn(){}
   
   TH1* GetResult(){return fNumerator;}  
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQOutCMSLC() - trackpair->GetQOutCMSLC();
     x = partpair->GetQInv();
    }
  protected:
  private: 
  public:
    ClassDef(AliHBTQOutResolVsQInvFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTQSideResolVsQInvFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQSideResolVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .05, Double_t minYval =-0.05);
   virtual ~AliHBTQSideResolVsQInvFctn(){}

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair,  Double_t& x, Double_t& y)
    {
     y = partpair->GetQSideCMSLC() - trackpair->GetQSideCMSLC();
     x = partpair->GetQInv();
    }
   TH1* GetResult(){return fNumerator;} 
  protected:
  private:
  public:
    ClassDef(AliHBTQSideResolVsQInvFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTQLongResolVsQInvFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQLongResolVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .05, Double_t minYval =-0.05);
   virtual ~AliHBTQLongResolVsQInvFctn(){}

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQLongCMSLC() - trackpair->GetQLongCMSLC();
     x = partpair->GetQInv();
    }
   TH1* GetResult(){return fNumerator;} 
  protected:
  private:
  public:
    ClassDef(AliHBTQLongResolVsQInvFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTQInvResolVsKtFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQInvResolVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .05, Double_t minYval =-0.05);
   virtual ~AliHBTQInvResolVsKtFctn(){};

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQInv() - trackpair->GetQInv();
     x = partpair->GetKt();
    }
   TH1* GetResult(){return fNumerator;} 
  protected:
  private:
  public:
    ClassDef(AliHBTQInvResolVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTQOutResolVsKtFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQOutResolVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   virtual ~AliHBTQOutResolVsKtFctn(){}
   TH1* GetResult(){return GetNumerator();}
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQOutCMSLC() - trackpair->GetQOutCMSLC();
     x = partpair->GetKt();
    }
  protected:
  private:
  public:
    ClassDef(AliHBTQOutResolVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTQSideResolVsKtFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQSideResolVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                            Int_t nYbins = 500, Double_t maxYval = .05, Double_t minYval =-0.05);
   virtual ~AliHBTQSideResolVsKtFctn(){}
   TH1* GetResult(){return GetNumerator();}
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQSideCMSLC() - trackpair->GetQSideCMSLC();
     x = partpair->GetKt();
    }
  protected:
  private:
  public:
    ClassDef(AliHBTQSideResolVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTQLongResolVsKtFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQLongResolVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 1., Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .05, Double_t minYval =-0.05);
   virtual ~AliHBTQLongResolVsKtFctn(){}

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     y = partpair->GetQLongCMSLC() - trackpair->GetQLongCMSLC();
     x = partpair->GetKt();
    }
   TH1* GetResult(){return fNumerator;}
  protected:
  private:
  public:
    ClassDef(AliHBTQLongResolVsKtFctn,1)
 };
/***********************************************************************/
/***********************************************************************/
class AliHBTQOutResolVsQOutFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQOutResolVsQOutFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = -0.2, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   virtual ~AliHBTQOutResolVsQOutFctn(){}

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     x = partpair->GetQOutCMSLC();
     y = x - trackpair->GetQOutCMSLC();
    }
   TH1* GetResult(){return fNumerator;}  
  protected:
  private:
  public:
    ClassDef(AliHBTQOutResolVsQOutFctn,1)
 };

/***********************************************************************/
/***********************************************************************/

class AliHBTQSideResolVsQSideFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQSideResolVsQSideFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = -0.2, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   virtual ~AliHBTQSideResolVsQSideFctn(){}

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     x = partpair->GetQSideCMSLC();
     y = x - trackpair->GetQSideCMSLC();
    }
   TH1* GetResult(){return fNumerator;}  
  protected:
  private:
  public:
    ClassDef(AliHBTQSideResolVsQSideFctn,1)
 };


/***********************************************************************/
/***********************************************************************/

class AliHBTQLongResolVsQLongFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTQLongResolVsQLongFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = -0.2,
                             Int_t nYbins = 500, Double_t maxYval = .05, Double_t minYval =-0.05);
   virtual ~AliHBTQLongResolVsQLongFctn(){}

   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     x = partpair->GetQLongCMSLC();
     y = x - trackpair->GetQLongCMSLC();
    }
   TH1* GetResult(){return fNumerator;}  
  protected:
  private:
  public:
    ClassDef(AliHBTQLongResolVsQLongFctn,1)
 };


/***********************************************************************/
/***********************************************************************/
class AliHBTPairThetaResolVsQInvFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTPairThetaResolVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   
   virtual ~AliHBTPairThetaResolVsQInvFctn(){}
   
   TH1* GetResult(){return fNumerator;}  
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     Double_t partTheta = partpair->Particle1()->Theta() - partpair->Particle2()->Theta();
     Double_t trackTheta = trackpair->Particle1()->Theta() - trackpair->Particle2()->Theta();
     y = partTheta - trackTheta;
     x = partpair->GetQInv();
    }
  protected:
  private: 
  public:
    ClassDef(AliHBTPairThetaResolVsQInvFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTPairPhiResolVsQInvFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTPairPhiResolVsQInvFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   
   virtual ~AliHBTPairPhiResolVsQInvFctn(){}
   
   TH1* GetResult(){return fNumerator;}  
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     Double_t partPhi = partpair->Particle1()->Phi() - partpair->Particle2()->Phi();
     Double_t trackPhi = trackpair->Particle1()->Phi() - trackpair->Particle2()->Phi();
     y = partPhi - trackPhi;
     x = partpair->GetQInv();
    }
  protected:
  private: 
  public:
    ClassDef(AliHBTPairPhiResolVsQInvFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTPairThetaResolVsKtFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTPairThetaResolVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   
   virtual ~AliHBTPairThetaResolVsKtFctn(){}
   
   TH1* GetResult(){return fNumerator;}  
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     Double_t partTheta = partpair->Particle1()->Theta() - partpair->Particle2()->Theta();
     Double_t trackTheta = trackpair->Particle1()->Theta() - trackpair->Particle2()->Theta();
     y = partTheta - trackTheta;
     x = partpair->GetKt();
    }
  protected:
  private: 
  public:
    ClassDef(AliHBTPairThetaResolVsKtFctn,1)
 };

/***********************************************************************/
/***********************************************************************/
class AliHBTPairPhiResolVsKtFctn: public AliHBTTwoPairFctn2D
 {
  public: 
   AliHBTPairPhiResolVsKtFctn(Int_t nXbins = 200, Double_t maxXval = 0.2, Double_t minXval = 0.0, 
                             Int_t nYbins = 500, Double_t maxYval = .15, Double_t minYval =-0.15);
   
   virtual ~AliHBTPairPhiResolVsKtFctn(){}
   
   TH1* GetResult(){return fNumerator;}  
   void GetValues(AliHBTPair* trackpair, AliHBTPair* partpair, Double_t& x, Double_t& y)
    {
     Double_t partPhi = partpair->Particle1()->Phi() - partpair->Particle2()->Phi();
     Double_t trackPhi = trackpair->Particle1()->Phi() - trackpair->Particle2()->Phi();
     y = partPhi - trackPhi;
     x = partpair->GetKt();
    }
  protected:
  private: 
  public:
    ClassDef(AliHBTPairPhiResolVsKtFctn,1)
 };

#endif
