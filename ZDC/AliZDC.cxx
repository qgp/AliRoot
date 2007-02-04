/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  			Zero Degree Calorimeter			             //
//  	     This class contains the basic functions for the ZDCs;           //
//            functions specific to one particular geometry are              //
//                      contained in the derived classes                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// --- ROOT system
#include <TBRIK.h>
#include <TClonesArray.h>
#include <TGeometry.h>
#include <TNode.h>
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>

// --- AliRoot header files
#include "AliDetector.h"
#include "AliZDC.h"
#include "AliZDCHit.h"
#include "AliZDCSDigit.h"
#include "AliZDCDigit.h"
#include "AliZDCDigitizer.h"
#include "AliZDCRawStream.h"
#include "AliZDCCalibData.h"

#include "AliRawDataHeader.h"
#include "AliLoader.h"
#include "AliRun.h"
#include "AliMC.h"
#include "AliLog.h"

 
ClassImp(AliZDC)

AliZDC *gAliZDC;
 
//_____________________________________________________________________________
AliZDC::AliZDC()
{
  //
  // Default constructor for the Zero Degree Calorimeter base class
  //
  
  fIshunt     = 1;
  fNoShower   = 0;

  fHits       = 0;
  fNhits      = 0;

  fDigits     = 0;
  fNdigits    = 0;
  
  fCalibData  = 0;

}
 
//_____________________________________________________________________________
AliZDC::AliZDC(const char *name, const char *title)
  : AliDetector(name,title)
{
  //
  // Standard constructor for the Zero Degree Calorimeter base class
  //

  fIshunt   = 1;
  fNoShower = 0;

  // Allocate the hits array  
  fHits   = new TClonesArray("AliZDCHit",1000);
  gAlice->GetMCApp()->AddHitList(fHits);
  
  char sensname[5],senstitle[25];
  sprintf(sensname,"ZDC");
  sprintf(senstitle,"ZDC dummy");
  SetName(sensname); SetTitle(senstitle);

  fDigits     = 0;
  fNdigits    = 0;
  
  fCalibData  = 0;

  gAliZDC=this;

}
//____________________________________________________________________________ 
AliZDC::~AliZDC()
{
  //
  // ZDC destructor
  //

  fIshunt   = 0;
  gAliZDC=0;

  delete fCalibData;

}
//_____________________________________________________________________________
void AliZDC::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
  //
  // 		Add a ZDC hit to the hit list.
  // -> We make use of 2 array of hits:
  // [1]  fHits (the usual one) that contains hits for each PRIMARY
  // [2]  fStHits that contains hits for each EVENT and is used to
  //	  obtain digits at the end of each event
  //
  
  static Float_t primKinEn, xImpact, yImpact, sFlag;

  AliZDCHit *newquad, *curprimquad;
  newquad = new AliZDCHit(fIshunt, track, vol, hits);
  TClonesArray &lhits = *fHits;
  
  if(fNhits==0){
      // First hit -> setting flag for primary or secondary particle
      Int_t primary = gAlice->GetMCApp()->GetPrimary(track);     
      if(track != primary){
        newquad->SetSFlag(1);  // SECONDARY particle entering the ZDC
      }
      else if(track == primary){
        newquad->SetSFlag(0);  // PRIMARY particle entering the ZDC
      }  
      sFlag 	= newquad->GetSFlag();
      primKinEn = newquad->GetPrimKinEn();
      xImpact 	= newquad->GetXImpact();
      yImpact 	= newquad->GetYImpact();
   }
   else{       
      newquad->SetPrimKinEn(primKinEn);
      newquad->SetXImpact(xImpact);
      newquad->SetYImpact(yImpact);
      newquad->SetSFlag(sFlag);
   }
 
  Int_t j;
  for(j=0; j<fNhits; j++){
    // If hits are equal (same track, same volume), sum them.
     curprimquad = (AliZDCHit*) lhits[j];
     if(*curprimquad == *newquad){
        *curprimquad = *curprimquad+*newquad;
	delete newquad;
	return;
     } 
  }

    //Otherwise create a new hit
    new(lhits[fNhits]) AliZDCHit(newquad);
    fNhits++;
    
    delete newquad;
}

//_____________________________________________________________________________
void AliZDC::BuildGeometry()
{
  //
  // Build the ROOT TNode geometry for event display 
  // in the Zero Degree Calorimeter
  // This routine is dummy for the moment
  //

  TNode *node, *top;
  TBRIK *brik;
  const int kColorZDC  = kBlue;
  
  //
  top=gAlice->GetGeometry()->GetNode("alice");
  
  // ZDC
    brik = new TBRIK("S_ZDC","ZDC box","void",300,300,5);
    top->cd();
    node = new TNode("ZDC","ZDC","S_ZDC",0,0,600,"");
    node->SetLineColor(kColorZDC);
    fNodes->Add(node);
}

//____________________________________________________________________________
Float_t AliZDC::ZMin(void) const
{
  // Minimum dimension of the ZDC module in z
  return -11600.;
}

//____________________________________________________________________________
Float_t AliZDC::ZMax(void) const
{
  // Maximum dimension of the ZDC module in z
  return -11750.;
}
  

//_____________________________________________________________________________
void AliZDC::MakeBranch(Option_t *opt)
{
  //
  // Create Tree branches for the ZDC
  //

  char branchname[10];
  sprintf(branchname,"%s",GetName());

  const char *cH = strstr(opt,"H");
  
  if (cH && fLoader->TreeH())
   fHits   = new TClonesArray("AliZDCHit",1000); 
  
  AliDetector::MakeBranch(opt);
}

//_____________________________________________________________________________
void AliZDC::Hits2SDigits()
{
  // Create summable digits from hits
  
  AliDebug(1,"\n	Entering AliZDC::Hits2Digits() ");
  
  fLoader->LoadHits("read");
  fLoader->LoadSDigits("recreate");
  AliRunLoader* runLoader = fLoader->GetRunLoader();
  AliZDCSDigit sdigit;
  AliZDCSDigit* psdigit = &sdigit;

  // Event loop
  for (Int_t iEvent = 0; iEvent < runLoader->GetNumberOfEvents(); iEvent++) {
    Float_t pmCZN = 0, pmCZP = 0, pmQZN[4], pmQZP[4], pmZEM1 = 0, pmZEM2 = 0;
    for (Int_t i = 0; i < 4; i++) pmQZN[i] = pmQZP[i] = 0;

    runLoader->GetEvent(iEvent);
    TTree* treeH = fLoader->TreeH();
    Int_t ntracks = (Int_t) treeH->GetEntries();
    ResetHits();

    // Tracks loop
    Int_t sector[2];
    for (Int_t itrack = 0; itrack < ntracks; itrack++) {
      treeH->GetEntry(itrack);
      for (AliZDCHit* zdcHit = (AliZDCHit*)FirstHit(-1); zdcHit;
                      zdcHit = (AliZDCHit*)NextHit()) { 
		      
	sector[0] = zdcHit->GetVolume(0);
	sector[1] = zdcHit->GetVolume(1);
	if ((sector[1] < 1) || (sector[1] > 4)) {
	  Error("Hits2SDigits", "sector[0] = %d, sector[1] = %d", 
		sector[0], sector[1]);
	  continue;
	}
	Float_t lightQ = zdcHit->GetLightPMQ();
	Float_t lightC = zdcHit->GetLightPMC();
     
	if (sector[0] == 1) {          //ZN 
	  pmCZN += lightC;
	  pmQZN[sector[1]-1] += lightQ;
	} else if (sector[0] == 2) {   //ZP 
	  pmCZP += lightC;
	  pmQZP[sector[1]-1] += lightQ;
	} else if (sector[0] == 3) {   //ZEM 
	  if (sector[1] == 1) pmZEM1 += lightC;
	  else                pmZEM2 += lightQ;
	}
      }//Hits loop
    }

    // create the output tree
    fLoader->MakeTree("S");
    TTree* treeS = fLoader->TreeS();
    const Int_t kBufferSize = 4000;
    treeS->Branch(GetName(), "AliZDCSDigit", &psdigit, kBufferSize);

    // Create sdigits for ZN
    sector[0] = 1; // Detector = ZN
    sector[1] = 0; // Common PM ADC
    new(psdigit) AliZDCSDigit(sector, pmCZN);
    if (pmCZN > 0) treeS->Fill();
    for (Int_t j = 0; j < 4; j++) {
      sector[1] = j+1; // Towers PM ADCs
      new(psdigit) AliZDCSDigit(sector, pmQZN[j]);
      if (pmQZN[j] > 0) treeS->Fill();
    }
  
    // Create sdigits for ZP
    sector[0] = 2; // Detector = ZP
    sector[1] = 0; // Common PM ADC
    new(psdigit) AliZDCSDigit(sector, pmCZP);
    if (pmCZP > 0) treeS->Fill();
    for (Int_t j = 0; j < 4; j++) {
      sector[1] = j+1; // Towers PM ADCs
      new(psdigit) AliZDCSDigit(sector, pmQZP[j]);
      if (pmQZP[j] > 0) treeS->Fill();
    }

    // Create sdigits for ZEM
    sector[0] = 3; 
    sector[1] = 1; // Detector = ZEM1
    new(psdigit) AliZDCSDigit(sector, pmZEM1);
    if (pmZEM1 > 0) treeS->Fill();
    sector[1] = 2; // Detector = ZEM2
    new(psdigit) AliZDCSDigit(sector, pmZEM2);
    if (pmZEM2 > 0) treeS->Fill();

    // write the output tree
    fLoader->WriteSDigits("OVERWRITE");
  }

  fLoader->UnloadHits();
  fLoader->UnloadSDigits();
}

//_____________________________________________________________________________
AliDigitizer* AliZDC::CreateDigitizer(AliRunDigitizer* manager) const
{
  // Create the digitizer for ZDC

  return new AliZDCDigitizer(manager);
}

//_____________________________________________________________________________
void AliZDC::Digits2Raw()
{
  // Convert ZDC digits to raw data

  // preliminary format: 12 interger values (ZNC, ZNQ1-4, ZPC, ZPQ1-4, ZEM1,2)
  // For the CAEN module V965 we have an header, the Data Words and an End Of Block
  const int knADCData1=24, knADCData2=20;
  UInt_t lADCHeader1; 
  UInt_t lADCData1[knADCData1];
  //
  UInt_t lADCHeader2; 
  UInt_t lADCData2[knADCData2];
  //
  UInt_t lADCEndBlock;

  // load the digits
  fLoader->LoadDigits("read");
  AliZDCDigit digit;
  AliZDCDigit* pdigit = &digit;
  TTree* treeD = fLoader->TreeD();
  if (!treeD) return;
  treeD->SetBranchAddress("ZDC", &pdigit);

  // Fill data array
  // ADC header
  UInt_t lADCHeaderGEO = 0;
  UInt_t lADCHeaderCRATE = 0;
  UInt_t lADCHeaderCNT1 = knADCData1;
  UInt_t lADCHeaderCNT2 = knADCData2;
    
  lADCHeader1 = lADCHeaderGEO << 27 | 0x1 << 25 | lADCHeaderCRATE << 16 |
               lADCHeaderCNT1 << 8 ;
//	       
  lADCHeader2 = lADCHeaderGEO << 27 | 0x1 << 25 | lADCHeaderCRATE << 16 |
               lADCHeaderCNT2 << 8 ;

  //printf("lADCHeader = %d\n",lADCHeader);
      
  // ADC data word
  UInt_t lADCDataGEO = lADCHeaderGEO;
  UInt_t lADCDataValue1[knADCData1];
  UInt_t lADCDataValue2[knADCData2];
  UInt_t lADCDataOvFlw1[knADCData1];
  UInt_t lADCDataOvFlw2[knADCData2];
  for(Int_t i = 0; i<knADCData1 ; i++){
    lADCDataValue1[i] = 0;
    lADCDataOvFlw1[i] = 0;
  }
  for(Int_t i = 0; i<knADCData2 ; i++){
    lADCDataValue2[i] = 0;
    lADCDataOvFlw2[i] = 0;
  }
  UInt_t lADCDataChannel = 0;
  
  // loop over digits
  for (Int_t iDigit = 0; iDigit < treeD->GetEntries(); iDigit++) {
    treeD->GetEntry(iDigit);
    if (!pdigit) continue;

    //ADC data
    Int_t index1 = 0, index2 = 0;
    if(digit.GetSector(0)==1 || digit.GetSector(0)==2 || digit.GetSector(0)==3){
      if(digit.GetSector(0)==1 || digit.GetSector(0)==2){
        index1 = (digit.GetSector(0)-1) + digit.GetSector(1)*4;
        lADCDataChannel = (digit.GetSector(0)-1)*8 + digit.GetSector(1);
      }
      else if(digit.GetSector(0)==3){
        index1 = 19 + digit.GetSector(1);
        lADCDataChannel = 5 + digit.GetSector(1)*8;
      }
      lADCDataValue1[index1] = digit.GetADCValue(0);
      if (lADCDataValue1[index1] > 2047) lADCDataOvFlw1[index1] = 1; 
      lADCDataValue1[index1+2] = digit.GetADCValue(1);
      if (lADCDataValue1[index1+2] > 2047) lADCDataOvFlw1[index1+2] = 1; 
    
      lADCData1[index1] =   lADCDataGEO << 27 | lADCDataChannel << 17 | 
                        lADCDataOvFlw1[index1] << 12 | (lADCDataValue1[index1] & 0xfff); 
      lADCData1[index1+2] = lADCDataGEO << 27 | lADCDataChannel << 17 | 0x1 << 16 |
                        lADCDataOvFlw1[index1+2] << 12 | (lADCDataValue1[index1+2] & 0xfff);                    
    }
    else if(digit.GetSector(0)==4 || digit.GetSector(0)==5){
      index2 = (digit.GetSector(0)-4) + digit.GetSector(1)*4;
      lADCDataChannel = (digit.GetSector(0)-4)*8 + digit.GetSector(1);

      lADCDataValue2[index2] = digit.GetADCValue(0);
      if (lADCDataValue2[index2] > 2047) lADCDataOvFlw2[index2] = 1; 
      lADCDataValue2[index2+2] = digit.GetADCValue(1);
      if (lADCDataValue2[index2+2] > 2047) lADCDataOvFlw2[index2+2] = 1; 
      //
      lADCData2[index2] =   lADCDataGEO << 27 | lADCDataChannel << 17 | 
                        lADCDataOvFlw2[index2] << 12 | (lADCDataValue2[index2] & 0xfff); 
      lADCData2[index2+2] = lADCDataGEO << 27 | lADCDataChannel << 17 | 0x1 << 16 |
                        lADCDataOvFlw2[index2+2] << 12 | (lADCDataValue2[index2+2] & 0xfff);                    
    } 
    if ((index1 < 0) || (index1 >= 22)) {
      Error("Digits2Raw", "sector[0] = %d, sector[1] = %d", 
	    digit.GetSector(0), digit.GetSector(1));
      continue;
    }
    if ((index2 < 0) || (index2 >= 18)) {
      Error("Digits2Raw", "sector[0] = %d, sector[1] = %d", 
	    digit.GetSector(0), digit.GetSector(1));
      continue;
    }
    
    
  }
  //for (Int_t i=0;i<24;i++)printf("ADCData1[%d] = %d\n",i,lADCData1[i]);
  //for (Int_t i=0;i<20;i++)printf("ADCData2[%d] = %d\n",i,lADCData2[i]);
 
  // End of Block
  UInt_t lADCEndBlockGEO = lADCHeaderGEO;
  UInt_t lADCEndBlockEvCount = gAlice->GetEventNrInRun();
  
  lADCEndBlock = lADCEndBlockGEO << 27 | 0x1 << 26 | lADCEndBlockEvCount;
  
  //printf("ADCEndBlock = %d\n",lADCEndBlock);


  // open the output file
  char fileName[30];
  sprintf(fileName, "ZDC_%d.ddl", AliZDCRawStream::kDDLOffset);
#ifndef __DECCXX
  ofstream file(fileName, ios::binary);
#else
  ofstream file(fileName);
#endif

  // write the DDL data header
  AliRawDataHeader header;
  header.fSize = sizeof(header) + sizeof(lADCHeader1) + sizeof(lADCData1) + 
  		sizeof(lADCEndBlock)+ sizeof(lADCHeader2) + sizeof(lADCData2) + sizeof(lADCEndBlock);
  //printf("sizeof header = %d, ADCHeader = %d, ADCData = %d, ADCEndBlock = %d\n",
  //        sizeof(header),sizeof(lADCHeader),sizeof(lADCData),sizeof(lADCEndBlock));
  header.SetAttribute(0);  // valid data
  file.write((char*)(&header), sizeof(header));

  // write the raw data and close the file
  file.write((char*) &lADCHeader1, sizeof (lADCHeader1));
  file.write((char*)(lADCData1), sizeof(lADCData1));
  file.write((char*) &lADCEndBlock, sizeof(lADCEndBlock));
  file.write((char*) &lADCHeader2, sizeof (lADCHeader2));
  file.write((char*)(lADCData2), sizeof(lADCData2));
  file.write((char*) &lADCEndBlock, sizeof(lADCEndBlock));
  file.close();

  // unload the digits
  fLoader->UnloadDigits();
}

//______________________________________________________________________
void AliZDC::SetTreeAddress(){
  // Set branch address for the Trees.
  // Inputs:
  //      none.
  // Outputs:
  //      none.
  // Return:
  //      none.
  if (fLoader->TreeH() && (fHits == 0x0))
    fHits   = new TClonesArray("AliZDCHit",1000);
      
  AliDetector::SetTreeAddress();
}
 
 
//Calibration methods (by Alberto Colla)
 
 
//________________________________________________________________
void AliZDC::CreateCalibData()
{
  // 
  //if (fCalibData) delete fCalibData; // delete previous version
  fCalibData = new AliZDCCalibData(GetName());
}
//________________________________________________________________
void AliZDC::WriteCalibData(Int_t option)
{
  //
  const int kCompressLevel = 9;
  char* fnam = GetZDCCalibFName();
  if (!fnam || fnam[0]=='\0') {
    fnam = gSystem->ExpandPathName("$(ALICE)/$(ALICE_LEVEL)/data/AliZDCCalib.root");
    Warning("WriteCalibData","No File Name is provided, using default %s",fnam);
  }
  TFile* cdfile = TFile::Open(fnam,"UPDATE","",kCompressLevel);

  // Writes Calibration Data to current directory. 
  // User MUST take care of corresponding file opening and ->cd()... !!!
  // By default, the object is overwritten. Use 0 option for opposite.
  if (option) option = TObject::kOverwrite;
  if (fCalibData) fCalibData->Write(0,option);
  else if (fCalibData) fCalibData->Write(0,option);

  cdfile->Close();
  delete cdfile;
}

//________________________________________________________________
void AliZDC::LoadCalibData()
{
  //
  char* fnam = GetZDCCalibFName();
  if (!fnam || fnam[0]=='\0') return; 
  if (!gAlice->IsFileAccessible(fnam)) {
    Error("LoadCalibData","ZDC Calibration Data file is not accessible, %s",fnam);
    exit(1);
  }
  TFile* cdfile = TFile::Open(fnam);

  // Loads Calibration Data from current directory. 
  // User MUST take care of corresponding file opening and ->cd()...!!!
  //
  if (fCalibData) delete fCalibData; // delete previous version
  TString dtname = "Calib_";
  dtname += GetName();
  fCalibData = (AliZDCCalibData*) gDirectory->Get(dtname.Data());
  if (!fCalibData) { 
    Error("LoadCalibData","No Calibration data found for %s",GetName());
    exit(1);
  }

  cdfile->Close();
  delete cdfile;
}


//Calibration methods (by Alberto Colla)
