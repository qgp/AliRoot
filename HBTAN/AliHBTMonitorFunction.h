//Zbigniew.Chajecki@cern.ch

#ifndef ALIMONITORFUNCTION_H
#define ALIMONITORFUNCTION_H

#include "AliHBTParticleCut.h"
#include "AliHBTParticle.h"  

#include <TH2.h>
#include <TH3.h>

//class AliHBTAnalysis;

class AliHBTMonitorFunction: public TNamed
//Abstract base class for HBT functions
{
  public:
    AliHBTMonitorFunction();
    AliHBTMonitorFunction(const char* name,const char* title);
    virtual ~AliHBTMonitorFunction();
    
    virtual TH1* GetResult() = 0;

    virtual void Write();
    
    void Rename(const Char_t * name); 
    void Rename(const Char_t * name, const Char_t * title); 
    
    void SetParticleCut(AliHBTParticleCut*);

    virtual AliHBTParticle* CheckParticle(AliHBTParticle* particle);

  protected:
    AliHBTParticleCut*      fParticleCut;
    
  public:  
   ClassDef(AliHBTMonitorFunction,1)
};
/******************************************************************/
/******************************************************************/
inline AliHBTParticle* AliHBTMonitorFunction::CheckParticle(AliHBTParticle* particle)
{
  //check if particle meets the cut criteria
  if(fParticleCut->Pass(particle)) //if the particle is BAD
       { 
        return 0x0;//it is BAD as well - so return
       }
  return particle; 
}

/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHBTMonOneParticleFctn: public AliHBTMonitorFunction
{
  public:
    AliHBTMonOneParticleFctn(){}
    AliHBTMonOneParticleFctn(const Char_t *name, const Char_t *title):AliHBTMonitorFunction(name,title){}
    virtual ~AliHBTMonOneParticleFctn(){}
    
    virtual void ProcessSameEventParticles(AliHBTParticle* particle) = 0;
    
  protected:
  public:  
   ClassDef(AliHBTMonOneParticleFctn,1)
  
};
/******************************************************************/
class AliHBTMonOneParticleFctn1D: public AliHBTMonOneParticleFctn
{
 public:
  AliHBTMonOneParticleFctn1D();
  AliHBTMonOneParticleFctn1D(Int_t nbins, Double_t maxXval, Double_t minXval);
  AliHBTMonOneParticleFctn1D(const Char_t *name, const Char_t *title,
                      Int_t nbins = 100, Double_t maxXval = 1.4, Double_t minXval = 0.0);
  virtual ~AliHBTMonOneParticleFctn1D();
  
  TH1* GetResult(){return fResult;}

  void ProcessSameEventParticles(AliHBTParticle* particle);

 protected:
  virtual Double_t GetValue(AliHBTParticle* particle) = 0; 

  TH1D* fResult;
  
 public:
  ClassDef(AliHBTMonOneParticleFctn1D,2)
};
/******************************************************************/
 
class AliHBTMonOneParticleFctn2D: public AliHBTMonOneParticleFctn
{
 public:
  AliHBTMonOneParticleFctn2D(Int_t nXbins = 200, Double_t maxXval = 1.5, Double_t minXval = 0.0, 
                      Int_t nYbins = 200, Double_t maxYval = 1.5, Double_t minYval =-0.1);
  ~AliHBTMonOneParticleFctn2D();
  
  TH1* GetResult(){return fResult;}
  
  void ProcessSameEventParticles(AliHBTParticle* particle);

 protected:
  virtual void GetValues(AliHBTParticle* particle, Double_t&, Double_t&) = 0;

  TH2D* fResult;
  
 public:
  ClassDef(AliHBTMonOneParticleFctn2D,1)
};
/******************************************************************/
/******************************************************************/
/******************************************************************/

class AliHBTMonOneParticleFctn3D: public AliHBTMonOneParticleFctn
{
 public:
  AliHBTMonOneParticleFctn3D(Int_t nXbins = 200, Double_t maxXval = 1.5, Double_t minXval = 0.0, 
                      Int_t nYbins = 200, Double_t maxYval = .15, Double_t minYval =-0.15, 
                      Int_t nZbins = 200, Double_t maxZval = .15, Double_t minZval =-0.15);
	    
  virtual ~AliHBTMonOneParticleFctn3D();

  TH1* GetResult(){return fResult;}

 protected:
  TH3D* fResult;

 public:
  ClassDef(AliHBTMonOneParticleFctn3D,1)
};
/******************************************************************/
/******************************************************************/
class AliHBTMonTwoParticleFctn: public AliHBTMonitorFunction
{
  public:
    AliHBTMonTwoParticleFctn(){};
    AliHBTMonTwoParticleFctn(const Char_t *name, const Char_t *title):AliHBTMonitorFunction(name,title){}
    virtual ~AliHBTMonTwoParticleFctn(){};
    
    virtual void 
    ProcessSameEventParticles(AliHBTParticle* trackparticle, AliHBTParticle* partparticle) = 0;
	     
  protected:
  public:  
   ClassDef(AliHBTMonTwoParticleFctn,1)
  
};
/******************************************************************/

class AliHBTMonTwoParticleFctn1D: public AliHBTMonTwoParticleFctn
{
 public:
  AliHBTMonTwoParticleFctn1D(Int_t nbins = 200, Double_t maxval = 1.5, Double_t minval = 0.0);
  AliHBTMonTwoParticleFctn1D(const char*,const char*,
                      Int_t nbins = 200, Double_t maxval = 1.5, Double_t minval = 0.0);
  ~AliHBTMonTwoParticleFctn1D();
  
  TH1* GetResult(){return fResult;}
  
  void ProcessSameEventParticles(AliHBTParticle* trackparticle, AliHBTParticle* partparticle);
  
 protected:
  virtual Double_t GetValue(AliHBTParticle* trackparticle, AliHBTParticle* partparticle) = 0;

  TH1D* fResult;

 public:
  ClassDef(AliHBTMonTwoParticleFctn1D,1)
};
/******************************************************************/
class AliHBTMonTwoParticleFctn2D: public AliHBTMonTwoParticleFctn
{
 public:
  AliHBTMonTwoParticleFctn2D(Int_t nXbins = 200, Double_t maxXval = 1.5, Double_t minXval = 0.0, 
                       Int_t nYbins = 200, Double_t maxYval = .15, Double_t minYval =-0.15);
  virtual ~AliHBTMonTwoParticleFctn2D();
  
  TH1* GetResult(){return fResult;}
  
  void ProcessSameEventParticles(AliHBTParticle* trackparticle, AliHBTParticle* partparticle);
  void ProcessDiffEventParticles(AliHBTParticle* trackparticle, AliHBTParticle* partparticle){};
 
  
 protected:
  virtual void GetValues(AliHBTParticle*,AliHBTParticle*, Double_t&, Double_t&) = 0;

  TH2D* fResult;
  
 public:
  ClassDef(AliHBTMonTwoParticleFctn2D,1)
};


/******************************************************************/
class AliHBTMonTwoParticleFctn3D: public AliHBTMonTwoParticleFctn
{
 public:
  AliHBTMonTwoParticleFctn3D(Int_t nXbins = 200, Double_t maxXval = 1.5, Double_t minXval = 0.0, 
                       Int_t nYbins = 200, Double_t maxYval = .15, Double_t minYval =-0.15, 
                       Int_t nZbins = 200, Double_t maxZval = .15, Double_t minZval =-0.15){}
  virtual ~AliHBTMonTwoParticleFctn3D(){}
  
  TH1* GetResult(){return fResult;}
  
  void ProcessSameEventParticles(AliHBTParticle* trackparticle, AliHBTParticle* partparticle);
  void ProcessDiffEventParticles(AliHBTParticle* trackparticle, AliHBTParticle* partparticle){};
 
  
 protected:
  virtual void GetValues(AliHBTParticle*,AliHBTParticle*, Double_t&, Double_t&,Double_t&) = 0;

  TH3D* fResult;
  
 public:
  ClassDef(AliHBTMonTwoParticleFctn3D,1)
};

/******************************************************************/
/******************************************************************/
/******************************************************************/
/******************************************************************/

#endif
