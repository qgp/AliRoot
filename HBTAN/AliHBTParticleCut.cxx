#include "AliHBTParticleCut.h"

#include <Riostream.h>

ClassImp(AliHBTParticleCut)
const Int_t AliHBTParticleCut::fkgMaxCuts = 50;
/******************************************************************/

AliHBTParticleCut::AliHBTParticleCut()
 {
   fCuts = new AliHbtBaseCut* [fkgMaxCuts];//last property in the property
                                         //property enum => defines number of properties
   fNCuts = 0;
   fPID  = 0;
 }
/******************************************************************/

AliHBTParticleCut::AliHBTParticleCut(const AliHBTParticleCut& in)
{
  fCuts = new AliHbtBaseCut* [fkgMaxCuts];//last property in the property
                                         //property enum => defines number of properties
  fNCuts = in.fNCuts;
  fPID  = in.fPID;
  for (Int_t i = 0;i<fNCuts;i++)
   {
     fCuts[i] = (AliHbtBaseCut*)in.fCuts[i]->Clone();//create new object (clone) and rember pointer to it
   }
}
/******************************************************************/

AliHBTParticleCut::~AliHBTParticleCut()
{
  for (Int_t i = 0;i<fNCuts;i++)
   {
     delete fCuts[i];
   }
  delete []fCuts;
} 
/******************************************************************/

Bool_t AliHBTParticleCut::Pass(AliHBTParticle* p)
{
//method checks all the cuts that are set (in the list)
//If any of the baseCuts rejects particle False(rejection) is returned
 if(!p) 
  {
    Warning("Pass()","No Pasaran! We never accept NULL pointers");
    return kTRUE;
  }
 if( (p->GetPdgCode() != fPID) && ( fPID != 0)) return kTRUE;
 
 for (Int_t i = 0;i<fNCuts;i++)
   {
    if ( (fCuts[i]->Pass(p)) ) return kTRUE; //if one of the cuts rejects, then reject
   }
  return kFALSE;
}
/******************************************************************/

void AliHBTParticleCut::AddBasePartCut(AliHbtBaseCut* basecut)
{
  //adds the base pair cut (cut on one value)
 
   if (!basecut) return;
   if( fNCuts == (fkgMaxCuts-1) )
    {
      Warning("AddBasePartCut","Not enough place for another cut");
      return;
    }
   fCuts[fNCuts++]=basecut;
 
}

/******************************************************************/
AliHbtBaseCut* AliHBTParticleCut::FindCut(AliHBTCutProperty property)
{
 
 for (Int_t i = 0;i<fNCuts;i++)
  {
    if (fCuts[i]->GetProperty() == property) 
       return fCuts[i]; //we found the cut we were searching for
  }
 
 return 0x0; //we did not found this cut
 
}
/******************************************************************/

void AliHBTParticleCut::SetMomentumRange(Double_t min, Double_t max)
{
  AliHBTMomentumCut* cut= (AliHBTMomentumCut*)FindCut(kHbtP);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTMomentumCut(min,max);
}
/******************************************************************/


void AliHBTParticleCut::SetPtRange(Double_t min, Double_t max)
{
  AliHBTPtCut* cut= (AliHBTPtCut*)FindCut(kHbtPt);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTPtCut(min,max);

}
/******************************************************************/

void AliHBTParticleCut::SetEnergyRange(Double_t min, Double_t max)
{
  AliHBTEnergyCut* cut= (AliHBTEnergyCut*)FindCut(kHbtE);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTEnergyCut(min,max);
 
}
/******************************************************************/

void AliHBTParticleCut::SetRapidityRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtRapidity);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTRapidityCut(min,max);

}
/******************************************************************/

void AliHBTParticleCut::SetPseudoRapidityRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtPseudoRapidity);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTPseudoRapidityCut(min,max);
 
}
/******************************************************************/

void AliHBTParticleCut::SetPxRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtPx);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTPxCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetPyRange(Double_t min, Double_t max)
{  
  AliHbtBaseCut* cut = FindCut(kHbtPy);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTPyCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetPzRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtPz);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTPzCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetPhiRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtPhi);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTPhiCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetThetaRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtTheta);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTThetaCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetVxRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtVx);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTVxCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetVyRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtVy);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTVyCut(min,max);
}
/******************************************************************/

void AliHBTParticleCut::SetVzRange(Double_t min, Double_t max)
{
  AliHbtBaseCut* cut = FindCut(kHbtVz);
  if(cut) cut->SetRange(min,max);
  else fCuts[fNCuts++] = new AliHBTVzCut(min,max);
}

/******************************************************************/
void AliHBTParticleCut::Streamer(TBuffer &b)
{
     // Stream all objects in the array to or from the I/O buffer.

   UInt_t R__s, R__c;
   if (b.IsReading()) 
    {
      Int_t i;
      for (i = 0;i<fNCuts;i++) delete fCuts[i];
      b.ReadVersion(&R__s, &R__c);
      TObject::Streamer(b);
      b >> fPID;
      b >> fNCuts;
      for (i = 0;i<fNCuts;i++) 
       {
         b >> fCuts[i];
       }
       b.CheckByteCount(R__s, R__c,AliHBTParticleCut::IsA());
    } 
   else 
    {
     R__c = b.WriteVersion(AliHBTParticleCut::IsA(), kTRUE);
     TObject::Streamer(b);
     b << fPID;
     b << fNCuts;
     for (Int_t i = 0;i<fNCuts;i++)
      {
       b << fCuts[i];
      }
     b.SetByteCount(R__c, kTRUE);
   }
}

void AliHBTParticleCut::Print(void)
{
  cout<<"Printing AliHBTParticleCut, this = "<<this<<endl;
  cout<<"fPID  "<<fPID<<endl;
  cout<<"fNCuts  "<<fNCuts <<endl;
  for (Int_t i = 0;i<fNCuts;i++)
      {
       cout<<"  fCuts["<<i<<"]  "<<fCuts[i]<<endl<<"   ";
       fCuts[i]->Print();
      }
}

/******************************************************************/
/******************************************************************/

ClassImp(AliHBTEmptyParticleCut)
void AliHBTEmptyParticleCut::Streamer(TBuffer &b)
 {
  AliHBTParticleCut::Streamer(b);
 }
/******************************************************************/
/******************************************************************/
/******************************************************************/

/******************************************************************/
/******************************************************************/
/******************************************************************/

ClassImp(AliHbtBaseCut)
void AliHbtBaseCut::Print(void)
{
  cout<<"fMin="<<fMin <<", fMax=" <<fMax<<"    ";
  PrintProperty();
}
void AliHbtBaseCut::PrintProperty(void)
{
 switch (fProperty)
  {
   case  kHbtP: 
     cout<<"kHbtP"; break;
   case  kHbtPt: 
     cout<<"kHbtPt"; break;
   case  kHbtE: 
     cout<<"kHbtE"; break;
   case  kHbtRapidity: 
     cout<<"kHbtRapidity"; break;
   case  kHbtPseudoRapidity: 
     cout<<"kHbtPseudoRapidity"; break;
   case  kHbtPx: 
     cout<<"kHbtPx"; break;
   case  kHbtPy: 
     cout<<"kHbtPy"; break;
   case  kHbtPz: 
     cout<<"kHbtPz"; break;   
   case  kHbtPhi: 
     cout<<"kHbtPhi"; break;
   case  kHbtTheta: 
     cout<<"kHbtTheta"; break;
   case  kHbtVx: 
     cout<<"kHbtVx"; break;
   case  kHbtVy: 
     cout<<"kHbtVy"; break;
   case  kHbtVz: 
     cout<<"kHbtVz"; break;
   case  kHbtNone: 
     cout<<"kHbtNone"; break;
   default: 
     cout<<"Property Not Found";
  }
 cout<<endl;
}
ClassImp( AliHBTMomentumCut )

ClassImp( AliHBTPtCut )
ClassImp( AliHBTEnergyCut )
ClassImp( AliHBTRapidityCut )
ClassImp( AliHBTPseudoRapidityCut )
ClassImp( AliHBTPxCut )
ClassImp( AliHBTPyCut )
ClassImp( AliHBTPzCut )
ClassImp( AliHBTPhiCut )
ClassImp( AliHBTThetaCut )
ClassImp( AliHBTVxCut )
ClassImp( AliHBTVyCut )
ClassImp( AliHBTVzCut )

ClassImp( AliHBTPIDCut )

ClassImp( AliHBTLogicalOperCut )

AliHBTLogicalOperCut::AliHBTLogicalOperCut():
 AliHbtBaseCut(-10e10,10e10,kHbtNone),
 fFirst(new AliHBTDummyBaseCut),
 fSecond(new AliHBTDummyBaseCut)
{

}
/******************************************************************/

AliHBTLogicalOperCut::AliHBTLogicalOperCut(AliHbtBaseCut* first, AliHbtBaseCut* second):
 AliHbtBaseCut(-10e10,10e10,kHbtNone),
 fFirst((first)?(AliHbtBaseCut*)first->Clone():0x0),
 fSecond((second)?(AliHbtBaseCut*)second->Clone():0x0)
{
  if ( (fFirst && fSecond) == kFALSE) 
   {
     Fatal("AliHBTLogicalOperCut","One of parameters is NULL!");
   }
}
/******************************************************************/

AliHBTLogicalOperCut::~AliHBTLogicalOperCut()
{
//destructor
  delete fFirst;
  delete fSecond;
}
/******************************************************************/

Bool_t AliHBTLogicalOperCut::AliHBTDummyBaseCut::Pass(AliHBTParticle*p)
{
  Warning("Pass","You are using dummy base cut! Probobly some logical cut is not set up properly");
  return kFALSE;//accept
}
/******************************************************************/

void AliHBTLogicalOperCut::Streamer(TBuffer &b)
{
  // Stream all objects in the array to or from the I/O buffer.
  UInt_t R__s, R__c;
  if (b.IsReading()) 
   {
     delete fFirst;
     delete fSecond;
     fFirst  = 0x0;
     fSecond = 0x0;

     b.ReadVersion(&R__s, &R__c);
     TObject::Streamer(b);
     b >> fFirst;
     b >> fSecond;
     b.CheckByteCount(R__s, R__c,AliHBTLogicalOperCut::IsA());
   } 
  else 
   {
     R__c = b.WriteVersion(AliHBTLogicalOperCut::IsA(), kTRUE);
     TObject::Streamer(b);
     b << fFirst;
     b << fSecond;
     b.SetByteCount(R__c, kTRUE);
  }
}

/******************************************************************/
ClassImp(AliHBTOrCut)

Bool_t AliHBTOrCut::Pass(AliHBTParticle * p)
{
  //returns true when rejected 
  //AND operation is a little bit misleading but is correct
  //User wants to build logical cuts with natural (positive) logic
  //while HBTAN use inernally reverse (returns true when rejected)
  if (fFirst->Pass(p) && fSecond->Pass(p)) return kTRUE;//rejected (both rejected, returned kTRUE)
  return kFALSE;//accepted, at least one accepted (returned kFALSE)
}
/******************************************************************/

ClassImp(AliHBTAndCut)

Bool_t AliHBTAndCut::Pass(AliHBTParticle * p)
{
  //returns true when rejected 
  //OR operation is a little bit misleading but is correct
  //User wants to build logical cuts with natural (positive) logic
  //while HBTAN use inernally reverse (returns true when rejected)
  if (fFirst->Pass(p) || fSecond->Pass(p)) return kTRUE;//rejected (any of two rejected(returned kTRUE) )
  return kFALSE;//accepted (both accepted (returned kFALSE))
}
/******************************************************************/
