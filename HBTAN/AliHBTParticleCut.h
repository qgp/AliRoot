//Piotr Skowronski@cern.ch
//Classes for single particle cuts
//User should use only AliHBTParticleCut, eventually EmptyCut which passes all particles
//There is all interface for setting cuts on all particle properties
//The main method is Pass - which returns 
//        True in order to reject particle
//        False in case it meets all the criteria of the given cut
//
//User should create (and also destroy) cuts himself 
//and then pass them to the Analysis or Function by a proper method
//
//more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html


#ifndef ALIHBTPARTICLECUT_H
#define ALIHBTPARTICLECUT_H

#include <TObject.h>
#include "AliHBTParticle.h"


class AliHBTEmptyParticleCut;
class AliHBTParticleCut;
class AliHBTPairCut;
class AliHBTPair;
class AliHbtBaseCut;


/******************************************************************/
/******************************************************************/
/******************************************************************/

enum AliHBTCutProperty
 {
//codes particle property
  kHbtP,  //Momentum
  kHbtPt, //Transverse momentum
  kHbtE,  //Energy
  kHbtRapidity, //
  kHbtPseudoRapidity,
  kHbtPx, //X coordinate of the momentum
  kHbtPy, //Y coordinate of the momentum
  kHbtPz, //Z coordinate of the momentum
  kHbtPhi,//angle
  kHbtTheta,//angle
  kHbtVx,  // vertex X coordinate
  kHbtVy,  // vertex Y coordinate
  kHbtVz,  // vertex Z coordinate
//_____________________________
  kHbtNone 
 };

/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHBTParticleCut: public TObject
{
//Class describing cut on pairs of particles
  public:
    AliHBTParticleCut();
    AliHBTParticleCut(const AliHBTParticleCut&);
    virtual ~AliHBTParticleCut();

    virtual Bool_t Pass(AliHBTParticle*);
    Bool_t IsEmpty() {return kFALSE;}
    
    void AddBasePartCut(AliHbtBaseCut*);
    
    Int_t GetPID() const { return fPID;}
    void SetPID(Int_t pid){fPID=pid;}
    void SetMomentumRange(Double_t min, Double_t max);
    void SetPRange(Double_t min, Double_t max){SetMomentumRange(min,max);}
    void SetPtRange(Double_t min, Double_t max);
    void SetEnergyRange(Double_t min, Double_t max);
    void SetRapidityRange(Double_t min, Double_t max);
    void SetYRange(Double_t min, Double_t max){SetRapidityRange(min,max);}
    void SetPseudoRapidityRange(Double_t min, Double_t max);
    void SetPxRange(Double_t min, Double_t max);
    void SetPyRange(Double_t min, Double_t max);
    void SetPzRange(Double_t min, Double_t max);
    void SetPhiRange(Double_t min, Double_t max);
    void SetThetaRange(Double_t min, Double_t max);
    void SetVxRange(Double_t min, Double_t max);
    void SetVyRange(Double_t min, Double_t max);
    void SetVzRange(Double_t min, Double_t max);
    
    void Print(void);
  protected:
     
     AliHbtBaseCut* FindCut(AliHBTCutProperty);
     
     AliHbtBaseCut ** fCuts;//! Array with cuts
     Int_t fNCuts;

     Int_t fPID; //particle PID  - if=0 (rootino) all pids are accepted
          
  private:
     static const Int_t fkgMaxCuts;
  public:
    ClassDef(AliHBTParticleCut,1)
 
};
/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHBTEmptyParticleCut:  public AliHBTParticleCut
{
//Empty - it passes possitively all particles - it means returns always False
//Class describing cut on pairs of particles
  public:
    AliHBTEmptyParticleCut(){};
    virtual ~AliHBTEmptyParticleCut(){};
    
    Bool_t Pass(AliHBTParticle*){return kFALSE;} //accpept everything
    Bool_t IsEmpty() {return kTRUE;}

    ClassDef(AliHBTEmptyParticleCut,1)
 
};

/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHbtBaseCut: public TObject
 {
   //This class defines the range of some property - pure virtual
   //Property is coded by AliHBTCutTypes type
   
   public:
     
     AliHbtBaseCut(Double_t min = 0.0, Double_t max = 0.0,AliHBTCutProperty prop = kHbtNone):
                   fProperty(prop),fMin(min),fMax(max){}

     virtual   ~AliHbtBaseCut(){}
     
     Bool_t    Pass(AliHBTParticle *);
     
     void      SetRange(Double_t min, Double_t max){fMin = min; fMax = max;}

     void      SetMinimum(Double_t min){fMin = min;}
     void      SetMaximum(Double_t max){fMax = max;}
     
     Double_t  GetMinimum() const {return fMin;}
     Double_t  GetMaximum() const {return fMax;}
     
     AliHBTCutProperty GetProperty() const {return fProperty;}
     void Print(void);     
   protected:
     virtual Double_t  GetValue(AliHBTParticle *) = 0;

     AliHBTCutProperty fProperty;
     Double_t fMin;
     Double_t fMax;
   private:
     void PrintProperty(void);
   public:
     ClassDef(AliHbtBaseCut,1)
   
 };

inline Bool_t
AliHbtBaseCut::Pass(AliHBTParticle *p)
 {
   if ( (GetValue(p) < fMin) || (GetValue(p) > fMax ) ) return kTRUE; //rejected
   else return kFALSE; //accepted
 }
/******************************************************************/
/******************************************************************/
/******************************************************************/

 
class AliHBTMomentumCut: public AliHbtBaseCut
 {
  public: 
    AliHBTMomentumCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtP){}
    virtual ~AliHBTMomentumCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->P();}
  public:  
    ClassDef(AliHBTMomentumCut,1)
  
 };


class AliHBTPtCut: public AliHbtBaseCut
 {
  public: 
    AliHBTPtCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtPt){}
    virtual ~AliHBTPtCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Pt();}
  public: 
    ClassDef(AliHBTPtCut,1)
  
 };


class AliHBTEnergyCut: public AliHbtBaseCut
 {
  public: 
    AliHBTEnergyCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtE){}
    virtual ~AliHBTEnergyCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Energy();}
  public: 
    ClassDef(AliHBTEnergyCut,1)
  
 };

class AliHBTRapidityCut: public AliHbtBaseCut
 {
  public: 
    AliHBTRapidityCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtRapidity){}
    virtual ~AliHBTRapidityCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Y();}
  public:   
    ClassDef(AliHBTRapidityCut,1)
  
 };

class AliHBTPseudoRapidityCut: public AliHbtBaseCut
 {
  public: 
    AliHBTPseudoRapidityCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtPseudoRapidity){}
    virtual ~AliHBTPseudoRapidityCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Eta();}
  public:   
    ClassDef(AliHBTPseudoRapidityCut,1)
  
 };

class AliHBTPxCut: public AliHbtBaseCut
 {
  public: 
    AliHBTPxCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtPx){}
    virtual ~AliHBTPxCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Px();}
  public:   
    ClassDef(AliHBTPxCut,1)
  
 };

class AliHBTPyCut: public AliHbtBaseCut
 {
  public: 
    AliHBTPyCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtPy){}
    virtual ~AliHBTPyCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Py();}
  public:   
    ClassDef(AliHBTPyCut,1)
  
 };


class AliHBTPzCut: public AliHbtBaseCut
 {
  public: 
    AliHBTPzCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtPz){}
    virtual ~AliHBTPzCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Pz();}
  public:   
    ClassDef(AliHBTPzCut,1)
  
 };

class AliHBTPhiCut: public AliHbtBaseCut
 {
  public: 
    AliHBTPhiCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtPhi){}
    virtual ~AliHBTPhiCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Phi();}
  public:    
    ClassDef(AliHBTPhiCut,1)
  
 };

class AliHBTThetaCut: public AliHbtBaseCut
 {
  public: 
    AliHBTThetaCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtTheta){}
    virtual ~AliHBTThetaCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Theta();}
  public:  
    ClassDef(AliHBTThetaCut,1)
  
 };

class AliHBTVxCut: public AliHbtBaseCut
 {
 //Cut of the X coordinate of the vertex position
  public: 
    AliHBTVxCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtVx){}
    virtual ~AliHBTVxCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Vx();} //retruns value of the vertex
  public:   
    ClassDef(AliHBTVxCut,1)
  
 };


class AliHBTVyCut: public AliHbtBaseCut
 {
 //Cut of the X coordinate of the vertex position
  public: 
    AliHBTVyCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtVy){}
    virtual ~AliHBTVyCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Vy();} //retruns value of the vertex
  public:   
    ClassDef(AliHBTVyCut,1)
  
 };

class AliHBTVzCut: public AliHbtBaseCut
 {
 //Cut of the X coordinate of the vertex position
  public: 
    AliHBTVzCut(Double_t min = 0.0, Double_t max = 0.0):AliHbtBaseCut(min,max,kHbtVz){}
    virtual ~AliHBTVzCut(){}
  protected:
    Double_t  GetValue(AliHBTParticle * p){return p->Vz();} //retruns value of the vertex
  public:   
    ClassDef(AliHBTVzCut,1)
  
 };





#endif
