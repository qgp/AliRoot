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


/*
  Produces the data needed to calculate the quality assurance. 
  All data must be mergeable objects.
  B.K. Nandi
*/

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TH1I.h> 
#include <TH2F.h> 

// --- Standard library ---

// --- AliRoot header files ---

#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliPMDQADataMakerRec.h"
#include "AliQAChecker.h"
#include "AliPMDdigit.h" 
#include "AliPMDrecpoint1.h" 
#include "AliPMDRawStream.h"
#include "AliPMDddldata.h"
#include "AliPMDUtility.h"
#include "AliESDPmdTrack.h"
//#include "AliPMDRecoParam.h"

ClassImp(AliPMDQADataMakerRec)
           
//____________________________________________________________________________ 
  AliPMDQADataMakerRec::AliPMDQADataMakerRec() : 
  AliQADataMakerRec(AliQAv1::GetDetName(AliQAv1::kPMD), "PMD Quality Assurance Data Maker")
{
  // ctor
}

//____________________________________________________________________________ 
AliPMDQADataMakerRec::AliPMDQADataMakerRec(const AliPMDQADataMakerRec& qadm) :
  AliQADataMakerRec()
{
  //copy ctor 
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliPMDQADataMakerRec& AliPMDQADataMakerRec::operator = (const AliPMDQADataMakerRec& qadm )
{
  // Equal operator.
  this->~AliPMDQADataMakerRec();
  new(this) AliPMDQADataMakerRec(qadm);
  return *this;
}
 

//____________________________________________________________________________ 
void AliPMDQADataMakerRec::InitRaws()
{
  // create Raws histograms in Raws subdir

  const Bool_t expert   = kTRUE ; 
  const Bool_t saveCorr = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  // Preshower plane

    TH1I * h0 = new TH1I("hPreEdepM0","ADC Distribution PRE - Module 0;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h0->Sumw2();
    Add2RawsList(h0, 0, !expert, image, !saveCorr); 

    TH1I * h1 = new TH1I("hPreEdepM1","ADC Distribution PRE - Module 1;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h1->Sumw2();
    Add2RawsList(h1, 1, !expert, image, !saveCorr); 

    TH1I * h2 = new TH1I("hPreEdepM2","ADC Distribution PRE - Module 2;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h2->Sumw2();
    Add2RawsList(h2, 2, !expert, image, !saveCorr); 
    
    TH1I * h3 = new TH1I("hPreEdepM3","ADC Distribution PRE - Module 3;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h3->Sumw2(); 
    Add2RawsList(h3, 3, !expert, image, !saveCorr); 

    TH1I * h4 = new TH1I("hPreEdepM4","ADC Distribution PRE - Module 4;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h4->Sumw2(); 
    Add2RawsList(h4, 4, !expert, image, !saveCorr); 

    TH1I * h5 = new TH1I("hPreEdepM5","ADC Distribution PRE - Module 5;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h5->Sumw2(); 
    Add2RawsList(h5, 5, !expert, image, !saveCorr); 

    TH1I * h6 = new TH1I("hPreEdepM6","ADC Distribution PRE - Module 6;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h6->Sumw2(); 
    Add2RawsList(h6, 6, !expert, image, !saveCorr); 

    TH1I * h7 = new TH1I("hPreEdepM7","ADC Distribution PRE - Module 7;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h7->Sumw2(); 
    Add2RawsList(h7, 7, !expert, image, !saveCorr); 

    TH1I * h8 = new TH1I("hPreEdepM8","ADC Distribution PRE - Module 8;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h8->Sumw2(); 
    Add2RawsList(h8, 8, !expert, image, !saveCorr); 

    TH1I * h9 = new TH1I("hPreEdepM9","ADC Distribution PRE - Module 9;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h9->Sumw2(); 
    Add2RawsList(h9, 9, !expert, image, !saveCorr); 

    TH1I * h10 = new TH1I("hPreEdepM10","ADC Distribution PRE - Module 10;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h10->Sumw2(); 
    Add2RawsList(h10, 10, !expert, image, !saveCorr); 

    TH1I * h11 = new TH1I("hPreEdepM11","ADC Distribution PRE - Module 11;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h11->Sumw2(); 
    Add2RawsList(h11, 11, !expert, image, !saveCorr); 

    TH1I * h12 = new TH1I("hPreEdepM12","ADC Distribution PRE - Module 12;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h12->Sumw2(); 
    Add2RawsList(h12, 12, !expert, image, !saveCorr); 

    TH1I * h13 = new TH1I("hPreEdepM13","ADC Distribution PRE - Module 13;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h13->Sumw2(); 
    Add2RawsList(h13, 13, !expert, image, !saveCorr); 

    TH1I * h14 = new TH1I("hPreEdepM14","ADC Distribution PRE - Module 14;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h14->Sumw2(); 
    Add2RawsList(h14, 14, !expert, image, !saveCorr); 

    TH1I * h15 = new TH1I("hPreEdepM15","ADC Distribution PRE - Module 15;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h15->Sumw2(); 
    Add2RawsList(h15, 15, !expert, image, !saveCorr); 

    TH1I * h16 = new TH1I("hPreEdepM16","ADC Distribution PRE - Module 16;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h16->Sumw2(); 
    Add2RawsList(h16, 16, !expert, image, !saveCorr); 

    TH1I * h17 = new TH1I("hPreEdepM17","ADC Distribution PRE - Module 17;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h17->Sumw2(); 
    Add2RawsList(h17, 17, !expert, image, !saveCorr); 

    TH1I * h18 = new TH1I("hPreEdepM18","ADC Distribution PRE - Module 18;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h18->Sumw2(); 
    Add2RawsList(h18, 18, !expert, image, !saveCorr); 

    TH1I * h19 = new TH1I("hPreEdepM19","ADC Distribution PRE - Module 19;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h19->Sumw2(); 
    Add2RawsList(h19, 19, !expert, image, !saveCorr); 

    TH1I * h20 = new TH1I("hPreEdepM20","ADC Distribution PRE - Module 20;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h20->Sumw2(); 
    Add2RawsList(h20, 20, !expert, image, !saveCorr);
 
    TH1I * h21 = new TH1I("hPreEdepM21","ADC Distribution PRE - Module 21;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h21->Sumw2(); 
    Add2RawsList(h21, 21, !expert, image, !saveCorr); 

    TH1I * h22 = new TH1I("hPreEdepM22","ADC Distribution PRE - Module 22;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h22->Sumw2(); 
    Add2RawsList(h22, 22, !expert, image, !saveCorr); 

    TH1I * h23 = new TH1I("hPreEdepM23","ADC Distribution PRE - Module 23;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h23->Sumw2(); 
    Add2RawsList(h23, 23, !expert, image, !saveCorr);

    // CPV histos

    TH1I * h24 = new TH1I("hCpvEdepM24","ADC Distribution CPV - Module 24;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h24->Sumw2();
    Add2RawsList(h24, 24, !expert, image, !saveCorr); 

    TH1I * h25 = new TH1I("hCpvEdepM25","ADC Distribution CPV - Module 25;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h25->Sumw2(); 
    Add2RawsList(h25, 25, !expert, image, !saveCorr); 

    TH1I * h26 = new TH1I("hCpvEdepM26","ADC Distribution CPV - Module 26;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h26->Sumw2(); 
    Add2RawsList(h26, 26, !expert, image, !saveCorr); 

    TH1I * h27 = new TH1I("hCpvEdepM27","ADC Distribution CPV - Module 27;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h27->Sumw2(); 
    Add2RawsList(h27, 27, !expert, image, !saveCorr); 

    TH1I * h28 = new TH1I("hCpvEdepM28","ADC Distribution CPV - Module 28;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h28->Sumw2(); 
    Add2RawsList(h28, 28, !expert, image, !saveCorr); 

    TH1I * h29 = new TH1I("hCpvEdepM29","ADC Distribution CPV - Module 29;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h29->Sumw2(); 
    Add2RawsList(h29, 29, !expert, image, !saveCorr); 

    TH1I * h30 = new TH1I("hCpvEdepM30","ADC Distribution CPV - Module 30;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h30->Sumw2(); 
    Add2RawsList(h30, 30, !expert, image, !saveCorr); 

    TH1I * h31 = new TH1I("hCpvEdepM31","ADC Distribution CPV - Module 31;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h31->Sumw2(); 
    Add2RawsList(h31, 31, !expert, image, !saveCorr); 

    TH1I * h32 = new TH1I("hCpvEdepM32","ADC Distribution CPV - Module 32;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h32->Sumw2();
    Add2RawsList(h32, 32, !expert, image, !saveCorr); 

    TH1I * h33 = new TH1I("hCpvEdepM33","ADC Distribution CPV - Module 33;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h33->Sumw2();
    Add2RawsList(h33, 33, !expert, image, !saveCorr); 

    TH1I * h34 = new TH1I("hCpvEdepM34","ADC Distribution CPV - Module 34;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h34->Sumw2(); 
    Add2RawsList(h34, 34, !expert, image, !saveCorr); 

    TH1I * h35 = new TH1I("hCpvEdepM35","ADC Distribution CPV - Module 35;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h35->Sumw2(); 
    Add2RawsList(h35, 35, !expert, image, !saveCorr); 

    TH1I * h36 = new TH1I("hCpvEdepM36","ADC Distribution CPV - Module 36;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h36->Sumw2(); 
    Add2RawsList(h36, 36, !expert, image, !saveCorr); 

    TH1I * h37 = new TH1I("hCpvEdepM37","ADC Distribution CPV - Module 37;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h37->Sumw2(); 
    Add2RawsList(h37, 37, !expert, image, !saveCorr); 

    TH1I * h38 = new TH1I("hCpvEdepM38","ADC Distribution CPV - Module 38;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h38->Sumw2(); 
    Add2RawsList(h38, 38, !expert, image, !saveCorr); 

    TH1I * h39 = new TH1I("hCpvEdepM39","ADC Distribution CPV - Module 39;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h39->Sumw2(); 
    Add2RawsList(h39, 39, !expert, image, !saveCorr); 

    TH1I * h40 = new TH1I("hCpvEdepM40","ADC Distribution CPV - Module 40;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h40->Sumw2(); 
    Add2RawsList(h40, 40, !expert, image, !saveCorr); 

    TH1I * h41 = new TH1I("hCpvEdepM41","ADC Distribution CPV - Module 41;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h41->Sumw2();
    Add2RawsList(h41, 41, !expert, image, !saveCorr); 

    TH1I * h42 = new TH1I("hCpvEdepM42","ADC Distribution CPV - Module 42;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h42->Sumw2(); 
    Add2RawsList(h42, 42, !expert, image, !saveCorr); 

    TH1I * h43 = new TH1I("hCpvEdepM43","ADC Distribution CPV - Module 43;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h43->Sumw2(); 
    Add2RawsList(h43, 43, !expert, image, !saveCorr); 

    TH1I * h44 = new TH1I("hCpvEdepM44","ADC Distribution CPV - Module 44;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h44->Sumw2(); 
    Add2RawsList(h44, 44, !expert, image, !saveCorr); 

    TH1I * h45 = new TH1I("hCpvEdepM45","ADC Distribution CPV - Module 45;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h45->Sumw2();
    Add2RawsList(h45, 45, !expert, image, !saveCorr); 

    TH1I * h46 = new TH1I("hCpvEdepM46","ADC Distribution CPV - Module 46;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h46->Sumw2(); 
    Add2RawsList(h46, 46, !expert, image, !saveCorr); 

    TH1I * h47 = new TH1I("hCpvEdepM47","ADC Distribution CPV - Module 47;Amplitude [ADC counts];Counts", 100, 0, 2000);
    h47->Sumw2(); 
    Add2RawsList(h47, 47, !expert, image, !saveCorr); 


    // Y vs. X for PRE and CPV planes

    TH2F * h48 = new TH2F("hPreXY","PRE plane;X [cm];Y [cm]",200,-100.,100.,200,-100.,100.);
    Add2RawsList(h48, 48, !expert, !image, saveCorr);//Ajay
    //Add2RawsList(h48, 48);//Ajay
    TH2F * h49 = new TH2F("hCpvXY","CPV plane;X [cm];Y [cm]",200,-100.,100.,200,-100.,100.);
    Add2RawsList(h49, 49, !expert, !image, saveCorr);//Ajay
    //Add2RawsList(h49, 49);//Ajay

}

//____________________________________________________________________________
void AliPMDQADataMakerRec::InitDigits()
{
  // create Digits histograms in Digits subdir
  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 
  
  TH1F *h0 = new TH1F("hPreDigitsEdep","Digits energy distribution in PRE(PMD);Amplitude [ADC counts];Counts", 100, 0., 2000.);
  h0->Sumw2();
  Add2DigitsList(h0, 0, !expert, image);
  
  TH1F *h1 = new TH1F("hCpvDigitsEdep","Digits energy distribution in CPV(PMD);Amplitude [ADC counts];Counts", 100, 0., 2000.); 
  h1->Sumw2();
  Add2DigitsList(h1, 1, !expert, image);
  
  TH1I *h2 = new TH1I("hPreDigitsMult","Digits multiplicity distribution in PRE(PMD);# of Digits;Entries", 500, 0, 1000) ; 
  h2->Sumw2();
  Add2DigitsList(h2, 2, !expert, image);
  
  TH1I *h3 = new TH1I("hCpvDigitsMult","Digits multiplicity distribution in CPV(PMD);# of Digits;Entries", 500, 0, 1000);
  h3->Sumw2();
  Add2DigitsList(h3, 3, !expert, image);  
}

//____________________________________________________________________________ 
void AliPMDQADataMakerRec::InitRecPoints()
{
  // create Reconstructed Points histograms in RecPoints subdir
  
  /*
    TH2F * h0 = new TH2F("hPreXY","RecPoints Y vs X PRE(PMD)", 100,-100.,100.,100,-100.,100.);
    Add2RecPointsList(h0,0);
    
    TH2F * h1 = new TH2F("hCpvXY","RecPoints Y vs X CPV(PMD)", 100,-100.,100.,100,-100.,100.);
    Add2RecPointsList(h1,1);
  */

    //  Ncell distribution in a cluster
  
    // PRE plane

  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 

  TH1F * h0 = new TH1F("hPreDdl0Ncell","PRE: Ddl0 Ncell in a cluster;# of cells;Counts",50,0.,50.);
  h0->Sumw2();
  Add2RecPointsList(h0, 0, !expert, image);


  TH1F * h1 = new TH1F("hPreDdl1Ncell","PRE: Ddl1 Ncell in a cluste;# of cells;Countsr",50,0.,50.);
  h1->Sumw2();
  Add2RecPointsList(h1, 1, !expert, image);


  TH1F * h2 = new TH1F("hPreDdl2Ncell","PRE: Ddl2 Ncell in a cluster;# of cells;Counts",50,0.,50.);
  h2->Sumw2();
  Add2RecPointsList(h2, 2, !expert, image);


  TH1F * h3 = new TH1F("hPreDdl3Ncell","PRE: Ddl3 Ncell in a cluster;# of cells;Counts",50,0.,50.);
  h3->Sumw2();
  Add2RecPointsList(h3, 3, !expert, image);

  // CPV plane

  TH1F * h4 = new TH1F("hCpvDdl4Ncell","CPV: Ddl4 Ncell in a cluster;# of cells;Counts",50,0.,50.);
  h4->Sumw2();
  Add2RecPointsList(h4, 4, !expert, image);

  TH1F * h5 = new TH1F("hCpvDdl5Ncell","CPV: Ddl5 Ncell in a cluster;# of cells;Counts",50,0.,50.);
  h5->Sumw2();
  Add2RecPointsList(h5, 5, !expert, image);

  // Correlation plot

  TH2I *h6 = new TH2I("hPre10","Cluster - DDL1 vs DDL0;DDL0;DDL1", 100,0,200,100,0,200);
  Add2RecPointsList(h6,6, !expert, image);

  TH2I *h7 = new TH2I("hPre32","Cluster - DDL3 vs DDL2;DDL2;DDL3", 100,0,200,100,0,200);
  Add2RecPointsList(h7,7, !expert, image);

  TH2I *h8 = new TH2I("hCpv54","Cluster - DDL5 vs DDL4;DDL4;DDL5", 100,0,200,100,0,200);
  Add2RecPointsList(h8,8, !expert, image);



}

//____________________________________________________________________________ 

void AliPMDQADataMakerRec::InitESDs()
{
  //Create histograms to controll ESD

  const Bool_t expert   = kTRUE ; 
  const Bool_t image    = kTRUE ; 

  TH1F *h0 = new TH1F("hPreClADC","Cluster ADC of PRE plane;# of clusters;Counts",500,0.,10000.);
  h0->Sumw2();
  Add2ESDsList(h0, 0, !expert, image)  ;

  TH1F *h1 = new TH1F("hCpvClADC","Cluster ADC of CPV plane;# of clusters;Counts",500,0.,10000.);
  h1->Sumw2();
  Add2ESDsList(h1, 1, !expert, image)  ;

  TH2I *h2 = new TH2I("hPmdClMult","Cluster Multiplicity: PRE vs. CPVplane;CPV multiplicity;PRE Multiplicity",100,0,1000,100,0,1000);
  h2->Sumw2();
  Add2ESDsList(h2, 2, !expert, image)  ;

/*
  TH1I * h3 = new TH1I("hCpvClMult","Cluster Multiplicity of CPV plane",100,0.,1000.);
  h3->Sumw2();
  Add2ESDsList(h3, 3, !expert, image)  ;
*/

}

//____________________________________________________________________________
void AliPMDQADataMakerRec::MakeRaws(AliRawReader* rawReader)
{
    //Fill prepared histograms with Raw digit properties

  TObjArray *pmdddlcont = 0x0;
    pmdddlcont = new TObjArray();
    AliPMDRawStream stream(rawReader);
    
    AliPMDddldata *pmdddl = 0x0;

    Int_t   iddl = -1;
    Int_t   xpad = -1;
    Int_t   ypad = -1;
    Float_t xx, yy;

    AliPMDUtility cc;

    while ((iddl = stream.DdlData(pmdddlcont)) >=0)
    {
	Int_t ientries = pmdddlcont->GetEntries();
	//printf(" ======= DDLNO = %d ientries = %d \n", iddl, ientries);
	
	for (Int_t ient = 0; ient < ientries; ient++)
	  {
	      //AliPMDddldata *pmdddl = (AliPMDddldata*)pmdddlcont->UncheckedAt(ient);
	    pmdddl = (AliPMDddldata*)pmdddlcont->UncheckedAt(ient);

	    Int_t det = pmdddl->GetDetector();
	    Int_t smn = pmdddl->GetSMN();
	    Int_t mcm = pmdddl->GetMCM();
	    //Int_t chno = pmdddl->GetChannel();
	    Int_t row = pmdddl->GetRow();
	    Int_t col = pmdddl->GetColumn();
	    Int_t sig = pmdddl->GetSignal();
	    
	    if (mcm == 0) continue;
	    if (det < 0 || det > 1)  continue;
	    if (smn < 0 || smn > 23) continue;
	    if (row < 0 || row > 47) continue;
	    if (col < 0 || col > 95) continue;

	    if (det == 0)
	    {
		GetRawsData(smn)->Fill(sig);
		if(smn < 12)
		{
		    xpad = col;
		    ypad = row;
		}
		else if(smn >= 12 && smn < 24)
		{
		    xpad = row;
		    ypad = col;
		}
		cc.RectGeomCellPos(smn,xpad,ypad,xx,yy);
		GetRawsData(48)->Fill(xx,yy);
	    }
	    if (det == 1)
	    {
		GetRawsData(24+smn)->Fill(sig);
		if(smn < 12)
		{
		    xpad = col;
		    ypad = row;
		}
		else if(smn >= 12 && smn < 24)
		{
		    xpad = row;
		    ypad = col;
		}

		cc.RectGeomCellPos(smn,xpad,ypad,xx,yy);
		GetRawsData(49)->Fill(xx,yy);

	    }

	  }

	pmdddlcont->Delete();
    }

    delete pmdddlcont;
    pmdddlcont = 0x0;

}
//____________________________________________________________________________
void AliPMDQADataMakerRec::MakeDigits()
{
  // makes data from Digits
  
   Int_t cpvmul = 0, premul = 0;
  
  TIter next(fDigitsArray) ; 
  AliPMDdigit * digit ; 
  while ( (digit = dynamic_cast<AliPMDdigit *>(next())) )
    {
    if(digit->GetDetector() == 0)
      {
	    GetDigitsData(0)->Fill( digit->GetADC()) ;
	    premul++;
      }
    if(digit->GetDetector() == 1)
      {
	    GetDigitsData(1)->Fill( digit->GetADC());
	    cpvmul++;
      }
    }  
  
  if (premul > 0) GetDigitsData(2)->Fill(premul);
  if (cpvmul > 0) GetDigitsData(3)->Fill(cpvmul);
  
  
}

//____________________________________________________________________________
void AliPMDQADataMakerRec::MakeDigits(TTree * digitTree)
{
  // makes data from Digit Tree
  
  if (fDigitsArray) 
    fDigitsArray->Clear() ; 
  else
    fDigitsArray = new TClonesArray("AliPMDdigit", 1000) ; 
  
  TBranch * branch = digitTree->GetBranch("PMDDigit") ;
  branch->SetAddress(&fDigitsArray) ;
  
  if ( ! branch )
    {
    AliWarning("PMD branch in Digit Tree not found") ; 
    }
  else
    {
    for (Int_t ient = 0; ient < branch->GetEntries(); ient++)
      {
	    branch->GetEntry(ient) ; 
	    MakeDigits() ; 
      }
    
    }
}

//____________________________________________________________________________
void AliPMDQADataMakerRec::MakeRecPoints(TTree * clustersTree)
{
    // makes data from RecPoints

  Int_t multDdl0 = 0, multDdl1 = 0, multDdl2 = 0;
    Int_t multDdl3 = 0, multDdl4 = 0, multDdl5 = 0;

    AliPMDrecpoint1 * recpoint; 

  if (fRecPointsArray) 
    fRecPointsArray->Clear() ; 
  else 
    fRecPointsArray = new TClonesArray("AliPMDrecpoint1", 1000) ; 
    
    TBranch * branch = clustersTree->GetBranch("PMDRecpoint") ;
    branch->SetAddress(&fRecPointsArray) ;

    if ( ! branch )
    {
	AliWarning("PMD branch in Recpoints Tree not found") ; 
    }
    else
    {
	for (Int_t imod = 0; imod < branch->GetEntries(); imod++)
	{
	    branch->GetEntry(imod) ;

	    TIter next(fRecPointsArray) ; 

	    while ( (recpoint = dynamic_cast<AliPMDrecpoint1 *>(next())) )
	      {
		//Float_t xpos = recpoint->GetClusX();
		//Float_t ypos = recpoint->GetClusY();
		//Int_t smn = recpoint->GetSMNumber();
		
		  if(recpoint->GetDetector() == 0)
		  {
		    if(recpoint->GetSMNumber() >= 0 && recpoint->GetSMNumber() < 6)
		      {
			GetRecPointsData(0)->Fill(recpoint->GetClusCells());
			multDdl0++;
		      }
		    if(recpoint->GetSMNumber() >= 6 && recpoint->GetSMNumber() < 12)
		      {
			GetRecPointsData(1)->Fill(recpoint->GetClusCells());
			multDdl1++;
		      }
		    if(recpoint->GetSMNumber() >= 12 && recpoint->GetSMNumber() < 18)
		      {
			GetRecPointsData(2)->Fill(recpoint->GetClusCells());
			multDdl2++;
		      }
		    if(recpoint->GetSMNumber() >= 18 && recpoint->GetSMNumber() < 24)
		      {
			GetRecPointsData(3)->Fill(recpoint->GetClusCells());
			multDdl3++;
		      }
		  }

		if(recpoint->GetDetector() == 1)
		  {
		    if((recpoint->GetSMNumber() >= 0 && recpoint->GetSMNumber() < 6) || 
		       (recpoint->GetSMNumber() >= 18 && recpoint->GetSMNumber() < 24))
		      {
			GetRecPointsData(4)->Fill(recpoint->GetClusCells());
			multDdl4++;
		      }
		    if(recpoint->GetSMNumber() >= 6 && recpoint->GetSMNumber() < 18 )
		      {
			GetRecPointsData(5)->Fill(recpoint->GetClusCells());
			multDdl5++;
		      }
		  }
	      } 
	}
    }
    
    GetRecPointsData(6)->Fill(multDdl0,multDdl1);
    GetRecPointsData(7)->Fill(multDdl2,multDdl3);
    GetRecPointsData(8)->Fill(multDdl4,multDdl5);
}

//____________________________________________________________________________

void AliPMDQADataMakerRec::MakeESDs(AliESDEvent * esd)
{
  // make QA data from ESDs

  Int_t premul = 0, cpvmul = 0;

  for (Int_t icl = 0; icl < esd->GetNumberOfPmdTracks(); icl++)
    {
      AliESDPmdTrack *pmdtr = esd->GetPmdTrack(icl);
      
      //Int_t   det   = pmdtr->GetDetector(); 
      //Float_t clsX  = pmdtr->GetClusterX();
      //Float_t clsY  = pmdtr->GetClusterY();
      //Float_t clsZ  = pmdtr->GetClusterZ();
      //Float_t ncell = pmdtr->GetClusterCells();
      Float_t adc   = pmdtr->GetClusterADC();
      //Float_t pid   = pmdtr->GetClusterPID();

      if (pmdtr->GetDetector() == 0)
	{
	  GetESDsData(0)->Fill(adc);
	  premul++;
	}
      if (pmdtr->GetDetector() == 1)
	{
	  GetESDsData(1)->Fill(adc) ;
	  cpvmul++;
	}
    }
  
  GetESDsData(2)->Fill(cpvmul,premul) ;
  //GetESDsData(3)->Fill(cpvmul) ;  
}

//____________________________________________________________________________ 

void AliPMDQADataMakerRec::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle
  
}
//____________________________________________________________________________ 
void AliPMDQADataMakerRec::EndOfDetectorCycle(AliQAv1::TASKINDEX_t task, TObjArray ** list)
{
  //Detector specific actions at end of cycle
  // do the QA checking
  AliQAChecker::Instance()->Run(AliQAv1::kPMD, task, list) ;  
}
