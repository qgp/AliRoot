#ifndef ALIAODPARTICLECUT_H
#define ALIAODPARTICLECUT_H
//__________________________________________________________________________
////////////////////////////////////////////////////////////////////////////
//                                                                        //
// class AliAODParticleCut                                                //
//                                                                        //
// Classes for single particle cuts                                       //
// User should use only AliAODParticleCut, eventually                     //
// EmptyCut which passes all particles                                    //
// There is all interface for setting cuts on all particle properties     //
// The main method is Pass - which returns                                //
//         True to reject particle                                        //
//         False in case it meets all the criteria of the given cut       //
//                                                                        //
// User should create (and also destroy) cuts himself                     // 
// and then pass them to the Analysis And Function by a proper method     //
//                                                                        //
//                                                                        //
// more info: http://alisoft.cern.ch/people/skowron/analyzer/index.html   //
// responsible: Piotr Skowronski@cern.ch                                   //
//                                                                        //
////////////////////////////////////////////////////////////////////////////


#include <TObject.h>
#include "AliVAODParticle.h"


class AliAODParticleEmptyCut;
class AliAODParticleCut;
class AliAODParticleBaseCut;


/******************************************************************/
/******************************************************************/
/******************************************************************/

enum EAODCutProperty
 {
//codes particle property
  kAODP,  //Momentum
  kAODPt, //Transverse momentum
  kAODE,  //Energy
  kAODRapidity, //
  kAODPseudoRapidity,
  kAODPx, //X coAnddinate of the momentum
  kAODPy, //Y coAnddinate of the momentum
  kAODPz, //Z coAnddinate of the momentum
  kAODPhi,//angle
  kAODTheta,//angle
  kAODVx,  // vertex X coAnddinate
  kAODVy,  // vertex Y coAnddinate
  kAODVz,  // vertex Z coAnddinate
  kAODPid, // vertex Z coAnddinate
//_____________________________
  kAODNone
 };

/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliAODParticleCut: public TObject
{
//Class describing cut on particle
  public:
    AliAODParticleCut();
    AliAODParticleCut(const AliAODParticleCut& in);
    virtual ~AliAODParticleCut();
    AliAODParticleCut& operator = (const AliAODParticleCut& in);
    
    virtual Bool_t Rejected(AliVAODParticle* p) const;
    Bool_t IsEmpty() const {return kFALSE;}
    
    void AddBasePartCut(AliAODParticleBaseCut* basecut);
    
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
    
    void Print(void) const;
  protected:
     
    AliAODParticleBaseCut* FindCut(EAODCutProperty property);

    AliAODParticleBaseCut ** fCuts;//! Array with cuts
    Int_t fNCuts; //number of base cuts stored in fCuts

    Int_t fPID; //particle PID  - if=0 (rootino) all pids are accepted
          
  private:
    static const Int_t fgkMaxCuts; //Size of the fCuts array

    ClassDef(AliAODParticleCut,1)
};
/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliAODParticleEmptyCut:  public AliAODParticleCut
{
//Empty - it passes possitively all particles - it means returns always False
//Class describing cut on particles
  public:
    AliAODParticleEmptyCut(){};
    virtual ~AliAODParticleEmptyCut(){};
    
    Bool_t Rejected(AliVAODParticle*) const {return kFALSE;} //accept everything
    Bool_t IsEmpty() const {return kTRUE;}

    ClassDef(AliAODParticleEmptyCut,1)
 
};

/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliAODParticleBaseCut: public TObject
 {
   //This class defines the range of some property - pure virtual
   //Property is coded by AliAODCutTypes type
   
   public:
     
     AliAODParticleBaseCut(Double_t min = 0.0, Double_t max = 0.0,EAODCutProperty prop = kAODNone):
                   fProperty(prop),fMin(min),fMax(max){}

     virtual           ~AliAODParticleBaseCut(){}
     
     virtual Bool_t    Rejected(AliVAODParticle *p) const;
     
     void              SetRange(Double_t min, Double_t max){fMin = min; fMax = max;}

     void              SetMinimum(Double_t min){fMin = min;}
     void              SetMaximum(Double_t max){fMax = max;}
     
     Double_t          GetMinimum() const {return fMin;}
     Double_t          GetMaximum() const {return fMax;}
     
     EAODCutProperty GetProperty() const {return fProperty;}
     virtual void Print(void) const;
     
   protected:
     virtual Double_t  GetValue(AliVAODParticle *) const = 0;

     EAODCutProperty fProperty; //property that this cut describes
     Double_t fMin;//minimum value
     Double_t fMax;//maximum value
     
   private:
     void PrintProperty(void) const;
     ClassDef(AliAODParticleBaseCut,1)
   
 };

inline Bool_t
AliAODParticleBaseCut::Rejected(AliVAODParticle *p) const
{
  //cjecks if particle property fits in range
  if ( (GetValue(p) < fMin) || (GetValue(p) > fMax ) ) return kTRUE; //rejected
  else return kFALSE; //accepted
}
/******************************************************************/
/******************************************************************/
/******************************************************************/

 
class AliAODMomentumCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODMomentumCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODP){}
    virtual ~AliAODMomentumCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->P();}
    ClassDef(AliAODMomentumCut,1)
 };

class AliAODPtCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODPtCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODPt){}
    virtual ~AliAODPtCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Pt();}
    ClassDef(AliAODPtCut,1)
 };


class AliAODEnergyCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODEnergyCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODE){}
    virtual ~AliAODEnergyCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const {return p->E();}
    ClassDef(AliAODEnergyCut,1)
 };

class AliAODRapidityCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODRapidityCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODRapidity){}
    virtual ~AliAODRapidityCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Y();}
    ClassDef(AliAODRapidityCut,1)
 };

class AliAODPseudoRapidityCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODPseudoRapidityCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODPseudoRapidity){}
    virtual ~AliAODPseudoRapidityCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Eta();}
    ClassDef(AliAODPseudoRapidityCut,1)
 };

class AliAODPxCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODPxCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODPx){}
    virtual ~AliAODPxCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Px();}
    ClassDef(AliAODPxCut,1)
 };

class AliAODPyCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODPyCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODPy){}
    virtual ~AliAODPyCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Py();}
    ClassDef(AliAODPyCut,1)
 };


class AliAODPzCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODPzCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODPz){}
    virtual ~AliAODPzCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Pz();}
    ClassDef(AliAODPzCut,1)
 };

class AliAODPhiCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODPhiCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODPhi){}
    virtual ~AliAODPhiCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Phi();}
    ClassDef(AliAODPhiCut,1)
  
 };

class AliAODThetaCut: public AliAODParticleBaseCut
 {
  public: 
    AliAODThetaCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODTheta){}
    virtual ~AliAODThetaCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Theta();}
    ClassDef(AliAODThetaCut,1)
  
 };

class AliAODVxCut: public AliAODParticleBaseCut
 {
 //Cut of the X coAnddinate of the vertex position
  public: 
    AliAODVxCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODVx){}
    virtual ~AliAODVxCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Vx();} //retruns value of the vertex
    ClassDef(AliAODVxCut,1)
  
 };


class AliAODVyCut: public AliAODParticleBaseCut
 {
 //Cut of the X coAnddinate of the vertex position
  public: 
    AliAODVyCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODVy){}
    virtual ~AliAODVyCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Vy();} //retruns value of the vertex
    ClassDef(AliAODVyCut,1)
  
 };

class AliAODVzCut: public AliAODParticleBaseCut
 {
 //Cut of the X coAnddinate of the vertex position
  public: 
    AliAODVzCut(Double_t min = 0.0, Double_t max = 0.0):AliAODParticleBaseCut(min,max,kAODVz){}
    virtual ~AliAODVzCut(){}
  protected:
    Double_t  GetValue(AliVAODParticle * p)const{return p->Vz();} //retruns value of the vertex
    
    ClassDef(AliAODVzCut,1)
  
 };

class AliAODPIDCut:  public AliAODParticleBaseCut
 {
   public:
     AliAODPIDCut():AliAODParticleBaseCut(0.0,0.0,kAODPid),fPID(0){}
     AliAODPIDCut(Int_t pid, Double_t min = 0.0, Double_t max = 1.0):AliAODParticleBaseCut(min,max,kAODPid),fPID(pid){}
     virtual ~AliAODPIDCut(){}
     
     void SetPID(Int_t pid){fPID = pid;}
     void Print(void) const;
   protected:
     Double_t  GetValue(AliVAODParticle * p)const{return p->GetProbability(fPID);}
     Int_t     fPID; //pid of particle that the pid is set 
     ClassDef(AliAODPIDCut,1)
 };
//___________________________________________________
/////////////////////////////////////////////////////
//                                                 //
// class AliAODLogicalOperCut                      //
//                                                 //
// This cut is base class fAnd class that perfAndms  //
// logical operations on cuts                      //
//                                                 //
/////////////////////////////////////////////////////
class AliAODLogicalOperCut:  public AliAODParticleBaseCut
 {
   public:
     AliAODLogicalOperCut();
     AliAODLogicalOperCut(AliAODParticleBaseCut* first, AliAODParticleBaseCut* second);
     virtual   ~AliAODLogicalOperCut();
   protected:
     Double_t  GetValue(AliVAODParticle * /*part*/) const {MayNotUse("GetValue");return 0.0;}
     
     AliAODParticleBaseCut* fFirst;   //second cut
     AliAODParticleBaseCut* fSecond;  //first cut
   private:  
    class  AliAODDummyBaseCut: public AliAODParticleBaseCut 
     {
       Double_t  GetValue(AliVAODParticle * /*part*/) const {return 0.0;}
       Bool_t    Rejected(AliVAODParticle* /*part*/) const;
     };
     
    ClassDef(AliAODLogicalOperCut,1)
 };

class AliAODOrCut: public AliAODLogicalOperCut
{
   public:
     AliAODOrCut(){}
     AliAODOrCut(AliAODParticleBaseCut* first, AliAODParticleBaseCut* second):AliAODLogicalOperCut(first,second){}
     virtual   ~AliAODOrCut(){}
     Bool_t    Rejected(AliVAODParticle *p) const;
     ClassDef(AliAODOrCut,1)
};

class AliAODAndCut: public AliAODLogicalOperCut
{
   public:
     AliAODAndCut(){}
     AliAODAndCut(AliAODParticleBaseCut* first, AliAODParticleBaseCut* second):AliAODLogicalOperCut(first,second){}
     virtual   ~AliAODAndCut(){}
     Bool_t    Rejected(AliVAODParticle *p) const;
     ClassDef(AliAODAndCut,1)
};

#endif
