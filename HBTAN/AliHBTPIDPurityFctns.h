#ifndef ALIHBTPIDPURITYFCTNS_H
#define ALIHBTPIDPURITYFCTNS_H
//_______________________________________________________________________________
/////////////////////////////////////////////////////////////////////////////////
//
// class AliHBTMonPIDPurityVsPtFctn;
// class AliHBTMonThetaResolutionVsPtFctn;
//
// file: AliHBTPIDPurityFctns.cxx AliHBTPIDPurityFctns.h
//
// Classes for calculating PID purity, efficiency and other things connected with PID
// xxxxxxxxxx
// xxxxxxxxxx
// xxxxxxxxxx
// xxxxxxxxxx
// xxxxxxxxxx
//
// added by Piotr.Skowronski@cern.ch
//
//////////////////////////////////////////////////////////////////////////////////

#include "AliHBTFunction.h"
#include "AliHBTMonitorFunction.h"

class AliHBTMonPIDPurityVsPtFctn: public AliHBTMonTwoParticleFctn1D, public AliHBTCorrelFunction
{
  public: 
    AliHBTMonPIDPurityVsPtFctn(Int_t nbins = 20, Double_t maxXval = 2.0, Double_t minXval = 0.0);
    AliHBTMonPIDPurityVsPtFctn(const AliHBTMonPIDPurityVsPtFctn& /*in*/);
    virtual ~AliHBTMonPIDPurityVsPtFctn();
    
    AliHBTMonPIDPurityVsPtFctn& operator=(const AliHBTMonPIDPurityVsPtFctn& /*in*/);
    
    void Init();
    void Write();
    void Rename(const Char_t * name);
    void Rename(const Char_t * name, const Char_t * title);
    TH1* GetResult();
    Double_t GetValue(AliVAODParticle * /*track*/,AliVAODParticle * /*part*/) const { return 0.0; }
    void Process(AliVAODParticle * track,AliVAODParticle * part);
  protected:
    TH1D* fGood;//histogram filled with correctly identified particles
    TH1D* fAll;//histogram filled with all particles
    ClassDef(AliHBTMonPIDPurityVsPtFctn,1)
};
/***********************************************************************/

class AliHBTMonPIDContaminationVsPtFctn: public AliHBTMonTwoParticleFctn1D, public AliHBTCorrelFunction
{
  public: 
    AliHBTMonPIDContaminationVsPtFctn(Int_t nbins = 20, Double_t maxXval = 2.0, Double_t minXval = 0.0);
    AliHBTMonPIDContaminationVsPtFctn(const AliHBTMonPIDContaminationVsPtFctn& /*in*/);
    virtual ~AliHBTMonPIDContaminationVsPtFctn();    

    AliHBTMonPIDContaminationVsPtFctn& operator=(const AliHBTMonPIDContaminationVsPtFctn& /*in*/);

    void Init();
    void Write();
    void Rename(const Char_t * name);
    void Rename(const Char_t * name, const Char_t * title);
    TH1* GetResult();
    Double_t GetValue(AliVAODParticle * /*track*/,AliVAODParticle * /*part*/) const { return 0.0; }
    void Process(AliVAODParticle * track,AliVAODParticle * part);
  protected:
    TH1D* fWrong;//histogram filled with wrongly identified particles
    TH1D* fAll;//histogram filled with all particles
    ClassDef(AliHBTMonPIDContaminationVsPtFctn,1)
};
/*************************************************************************************/

class AliHBTQInvCorrelFctnPerfectPID: public AliHBTTwoPairFctn1D, public AliHBTCorrelFunction
{
//Q Invaraint Correlation Function
//1D two particle function
//Fills the function only for correctly reconstructed PID
//Together with 
 public:
   AliHBTQInvCorrelFctnPerfectPID(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
   virtual ~AliHBTQInvCorrelFctnPerfectPID(){};
   void ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
   void ProcessDiffEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
   TH1* GetResult();
 protected:
   Double_t GetValue(AliHBTPair* trackpair, AliHBTPair* /*partpair*/) const {return trackpair->GetQInv();}
 private:
   ClassDef(AliHBTQInvCorrelFctnPerfectPID,1)
};

/*************************************************************************************/

class AliHBTWeightQInvCorrelFctnPerfectPID: public AliHBTTwoPairFctn1D, public AliHBTCorrelFunction
{
//Weight Q Invaraint Correlation Function
//1D two particle function
//Fills the function only for correctly reconstructed PID
//Together with regular 
 public:
   AliHBTWeightQInvCorrelFctnPerfectPID(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
   virtual ~AliHBTWeightQInvCorrelFctnPerfectPID(){};
   void ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
   void ProcessDiffEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
   TH1* GetResult();
 protected:
   Double_t GetValue(AliHBTPair* trackpair, AliHBTPair* /*partpair*/) const {return trackpair->GetQInv();}
 private:
   ClassDef(AliHBTWeightQInvCorrelFctnPerfectPID,1)
};
/*************************************************************************************/
class AliHBTWeightQOutSQideQLongFctnPerfectPID: public AliHBTTwoPairFctn3D, public AliHBTCorrelFunction
{

  public:
    AliHBTWeightQOutSQideQLongFctnPerfectPID(Int_t nXbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0,
                                            Int_t nYbins = 100, Double_t maxYval = 0.15, Double_t minYval = 0.0,
                                            Int_t nZbins = 100, Double_t maxZval = 0.15, Double_t minZval = 0.0);
    virtual  ~AliHBTWeightQOutSQideQLongFctnPerfectPID(){}

    TH1* GetResult();
    void ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
    void ProcessDiffEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);

  protected:
    void GetValues(AliHBTPair* /*trackpair*/, AliHBTPair* /*partpair*/, Double_t& /*x*/, Double_t& /*y*/, Double_t& /*z*/) const {}

    ClassDef(AliHBTWeightQOutSQideQLongFctnPerfectPID,1)
};

/*************************************************************************************/
class AliHBTQOutSQideQLongFctnPerfectPID: public AliHBTTwoPairFctn3D, public AliHBTCorrelFunction
{

  public:
    AliHBTQOutSQideQLongFctnPerfectPID(Int_t nXbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0,
                                            Int_t nYbins = 100, Double_t maxYval = 0.15, Double_t minYval = 0.0,
                                            Int_t nZbins = 100, Double_t maxZval = 0.15, Double_t minZval = 0.0);
    virtual  ~AliHBTQOutSQideQLongFctnPerfectPID(){}

    TH1* GetResult();
    void ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
    void ProcessDiffEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);

  protected:
    void GetValues(AliHBTPair* /*trackpair*/, AliHBTPair* /*partpair*/, Double_t& /*x*/, Double_t& /*y*/, Double_t& /*z*/) const {}

    ClassDef(AliHBTQOutSQideQLongFctnPerfectPID,1)
};


/*************************************************************************************/

class AliHBTPairPIDProbVsQInvFctn: public AliHBTOnePairFctn1D, public AliHBTCorrelFunction
{
//Q Invaraint Correlation Function
//1D two particle function
 public:
   AliHBTPairPIDProbVsQInvFctn(Int_t nbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0);
   virtual ~AliHBTPairPIDProbVsQInvFctn(){};
   void ProcessSameEventParticles(AliHBTPair* pair);
   void ProcessDiffEventParticles(AliHBTPair* pair);
   TH1* GetResult();
 protected:
   Double_t GetValue(AliHBTPair * pair) const {return pair->GetQInv();}
 private:
   ClassDef(AliHBTPairPIDProbVsQInvFctn,1)
};
/*************************************************************************************/

class AliHBTPairPIDProbVsQOutSQideQLongFctn: public AliHBTOnePairFctn3D, public AliHBTCorrelFunction
{

  public:
    AliHBTPairPIDProbVsQOutSQideQLongFctn(Int_t nXbins = 100, Double_t maxXval = 0.15, Double_t minXval = 0.0,
                                          Int_t nYbins = 100, Double_t maxYval = 0.15, Double_t minYval = 0.0,
                                          Int_t nZbins = 100, Double_t maxZval = 0.15, Double_t minZval = 0.0);
    virtual  ~AliHBTPairPIDProbVsQOutSQideQLongFctn(){}

    TH1* GetResult();
    void ProcessSameEventParticles(AliHBTPair* part);
    void ProcessDiffEventParticles(AliHBTPair* pair);

  protected:
    void GetValues(AliHBTPair* /*pair*/, Double_t& /*x*/, Double_t& /*y*/, Double_t& /*z*/) const {}

    ClassDef(AliHBTPairPIDProbVsQOutSQideQLongFctn,1)
};
/******************************************************************/

class AliHBTTwoTrackEffFctnPtThetaPhiPerfectPID: public AliHBTTwoPairFctn3D, public AliHBTCorrelFunction
 {
  public:
    AliHBTTwoTrackEffFctnPtThetaPhiPerfectPID(Int_t nXbins = 50, Double_t maxXval = 0.3, Double_t minXval = 0.0,
                                    Int_t nYbins = 50, Double_t maxYval = 0.3, Double_t minYval = 0.0,
	                Int_t nZbins = 50, Double_t maxZval = 0.3, Double_t minZval = 0.0);
    virtual ~AliHBTTwoTrackEffFctnPtThetaPhiPerfectPID(){}
    void ProcessSameEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
    void ProcessDiffEventParticles(AliHBTPair* trackpair, AliHBTPair* partpair);
    
    TH1* GetResult();
  protected:
    void GetValues(AliHBTPair* /*trackpair*/, AliHBTPair* /*partpair*/, Double_t& /*x*/, Double_t& /*y*/, Double_t& /*z*/) const {}
  private:
    ClassDef(AliHBTTwoTrackEffFctnPtThetaPhiPerfectPID,1)
 };
/*************************************************************************************/

class AliHBTPairPIDProbVsPtThetaPhiFctn: public AliHBTOnePairFctn3D, public AliHBTCorrelFunction
{

  public:
    AliHBTPairPIDProbVsPtThetaPhiFctn(Int_t nXbins = 50, Double_t maxXval = 0.3, Double_t minXval = 0.0,
                                      Int_t nYbins = 50, Double_t maxYval = 0.3, Double_t minYval = 0.0,
                                      Int_t nZbins = 50, Double_t maxZval = 0.3, Double_t minZval = 0.0);
    virtual  ~AliHBTPairPIDProbVsPtThetaPhiFctn(){}

    TH1* GetResult();
    void ProcessSameEventParticles(AliHBTPair* part);
    void ProcessDiffEventParticles(AliHBTPair* pair);

  protected:
    void GetValues(AliHBTPair* /*pair*/, Double_t& /*x*/, Double_t& /*y*/, Double_t& /*z*/) const {}

    ClassDef(AliHBTPairPIDProbVsPtThetaPhiFctn,1)
};



#endif
