#ifndef AliHMPID_h
#define AliHMPID_h
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <AliDetector.h>  //base class
#include <TClonesArray.h> //XxxCreate() 
#include <TObjArray.h>    //fDig,fClu field

//.
//HMPID base class
//.

class AliHMPID : public AliDetector //TObject-TNamed-AliModule-AliDetector-AliHMPID
{
public:
//ctor & dtor    
            AliHMPID(const char *nm,const char *ttl);                                                           //named ctor
            AliHMPID(                              ):AliDetector(    ),fDoFeed(kTRUE),fSdi(0),fDig(0),fClu(0) {}  //default ctor          
  virtual  ~AliHMPID();                                            
//framework part  
  virtual void  CreateMaterials (                )=0;          //from AliModule invoked from AliMC::ConstructGeometry() to define detector materials
  virtual void  CreateGeometry  (                )=0;          //from AliModule invoked from AliMC::ConstructGeometry() to build detector for simulation

  virtual Int_t IsVersion       (                )const=0;     //from AliModule not used        
  virtual void  Init            (                )=0;          //from AliModule invoked from AliMC::InitGeometry() after CreateGeometry() to do VolID initialization
  virtual void  DefineOpticalProperties() {}                   //from AliModule invoked from AliMC::ConstructOpGeometry() to set Cerenkov properties
          void  MakeBranch      (Option_t *opt="");            //from AliModule invokde from AliRun::Tree2Tree() to make requested HMPID branch
          void  SetTreeAddress  (                );            //from AliModule invoked from AliRun::GetEvent(), AliLoader::SetTAddrInDet()
  virtual void  StepManager     (                )=0;          //from AliModule invoked from AliMC
//private part +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
  void          HitCreate(         )     {if(fHits)return; fHits=new TClonesArray("AliHMPIDHit"); fNhits=0;     }//create hits list
              
  TClonesArray* SdiLst   (         )const{return fSdi;                                                          }//get sdigits list 
  void          SdiCreate(         )     {if(fSdi)return; fSdi=new TClonesArray("AliHMPIDDigit");               }//create sdigits list
  void          SdiReset (         )     {if(fSdi)  fSdi ->Clear();                                             }//clean sdigits list
         
  TObjArray*    DigLst   (         )const{return fDig;                                                          }//get digits list for all chambers
  TClonesArray* DigLst   (Int_t c  )const{return fDig ? (TClonesArray *)fDig->At(c):0;                          }//get digits list for chamber
  void          DigCreate(         )     {
    if (fDig) return; //PH do not recreate existing containers
    fDig=new TObjArray(7);for(Int_t i=0;i<7;i++)fDig->AddAt(new TClonesArray("AliHMPIDDigit"),i);               }//create digits list
  void          DigReset (         )     {if(fDig)for(int i=0;i<7;i++)fDig->At(i)->Clear();                     }//clean digits list 
          
  TObjArray*    CluLst   (         )const{return fClu;                                                          }//get clusters list for all chambers
  TClonesArray* CluLst   (Int_t c  )const{return fClu ? (TClonesArray *)fClu->At(c):0;                          }//get clusters list for chamber
  void          CluCreate(         )     {
    if (fClu) return; //PH do not recreate existing containers
    fClu=new TObjArray(7); for(Int_t i=0;i<7;i++)fClu->AddAt(new TClonesArray("AliHMPIDCluster"),i);            }//create clusters list
         void   CluReset (         )     {if(fClu)for(int i=0;i<7;i++)fClu->At(i)->Clear();                     }//clean clusters list
protected:  
  Bool_t                fDoFeed;                  //! Flag to switch on/off Feedback photon creation
  TClonesArray         *fSdi;                     //! list of sdigits  
  TObjArray            *fDig;                     //! each chamber holds it's one list of digits
  TObjArray            *fClu;                     //! each chamber holds it's one list of clusters 
  
private:
  AliHMPID(const AliHMPID &rich           );
  AliHMPID&  operator=(const AliHMPID&);

  ClassDef(AliHMPID,11)                            //Main HMPID class 
};//class AliHMPID  

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#endif
