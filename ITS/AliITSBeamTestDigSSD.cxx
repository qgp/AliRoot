/////////////////////////////////////////
// Class for SSD raw2digits conv       //
//                                     //
// Author: Enrico Fragiacomo           //
// Date: October 2004                  //
////////////////////////////////////////

#include "AliITS.h"
#include "AliITSgeom.h"
#include "AliITSdigitSSD.h"
#include "AliRawReaderDate.h"
#include "AliITSRawStreamSSDv1.h"
#include "AliITSBeamTestDigSSD.h"
#include <TBranch.h>
#include <TTree.h>

ClassImp(AliITSBeamTestDigSSD)

//_____________________________________________________________
AliITSBeamTestDigSSD::AliITSBeamTestDigSSD(): AliITSBeamTestDig() {
  //
  // Constructor
  //  
}

//_____________________________________________________________
AliITSBeamTestDigSSD::AliITSBeamTestDigSSD(const Text_t* name, const Text_t* title): AliITSBeamTestDig(name,title) {
  //
  // Constructor
  //
}

//__________________________________________________________________
AliITSBeamTestDigSSD::~AliITSBeamTestDigSSD() {
  //
  // Destructor
  //
}

//_______________________________________________________________________
void AliITSBeamTestDigSSD::Exec(Option_t* /*opt*/) {
  //Reads raw data for SSD, fill SSD digits tree, returns 1 if real data,
  //returns 2 if calibration (injector) data, returns 3 if calibration (test pul  //se) event

  TBranch* branch = fTreeD->GetBranch("ITSDigitsSSD");
  AliITSgeom* geom = fBt->GetITSgeom();
  Int_t nsdd=0;
  Int_t nspd=0;
  Int_t nssd=0;
  for(Int_t nlay=1;nlay<=geom->GetNlayers();nlay++){
    for(Int_t nlad=1;nlad<=geom->GetNladders(nlay);nlad++){
      for(Int_t ndet=1;ndet<=geom->GetNdetectors(nlay);ndet++){
	Int_t index=geom->GetModuleIndex(nlay,nlad,ndet);
	if(geom->GetModuleTypeName(index)=="kSPD") nspd++;
	if(geom->GetModuleTypeName(index)=="kSDD") nsdd++;
	if(geom->GetModuleTypeName(index)=="kSSD") nssd++;
      }
    }
  }
  Int_t maxn=nspd+nsdd+nssd;

  TClonesArray** newdigits = new TClonesArray*[maxn];

  Int_t* idig = new Int_t[maxn];

  for (Int_t idet =0; idet <maxn;idet++) {
     newdigits[idet] = new TClonesArray("AliITSdigitSSD");
     idig[idet]=0;  
   }
  
  // this constructor sets the flag to select SSD data only 
  // the Next method below will then jump to SSD data for this event
  AliITSRawStreamSSDv1 str(fReaderDate);

  // no selection of equipment 
  //fReaderDate->SelectEquipment(-1);
  //fReaderDate->SelectEquipment(17,102,102);

  while(str.Next()){   
    
    //if((str.GetADModule()!=2)&&(str.GetADModule()!=6)) continue;
  
    Int_t side = str.GetSideFlag();
    Int_t strip = str.GetStrip();
    Int_t signal = str.GetSignal();
    Int_t module = str.GetModuleID();
    Int_t module1= module-10+nsdd+nspd;
     if( (module<10) || (module>13) ) continue;
     if(module1>7) continue;
    const Int_t kdgt[3]={side,strip,signal};
     //  SSD modules 10, 11, 12 and 13
    new ( (*newdigits[module1])[idig[module1]] ) AliITSdigitSSD(kdgt);    
    idig[module1]++;
  } // end while
  
  for(Int_t n=0;n<maxn;n++){
    branch->SetAddress(&newdigits[n]);
    branch->Fill();  
  }
  
  fTreeD->SetEntries(maxn);
    
  fReaderDate->Reset();
  
  fTreeD->AutoSave();
  
  for(Int_t n=0;n<maxn;n++){
    delete newdigits[n];
  }
  
  delete [] newdigits;
  delete idig;
}

  

