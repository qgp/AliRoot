#ifndef ALIPHOSQADataMakerRec_H
#define ALIPHOSQADataMakerRec_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

/*
  Produces the data needed to calculate the quality assurance. 
  All data must be mergeable objects.
  Y. Schutz CERN July 2007
*/


// --- ROOT system ---
class TH1F ; 
class TH1I ; 
class TObjArray ; 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliQADataMakerRec.h"
#include "AliPHOSRecoParam.h"

class AliPHOSQADataMakerRec: public AliQADataMakerRec {

public:
  //Histograms for Raw data control
  enum HRawType_t {kHGmod0,kHGmod1,kHGmod2,kHGmod3,kHGmod4,
		   kLGmod0,kLGmod1,kLGmod2,kLGmod3,kLGmod4,
		   kNmodLG,kNmodHG,kNRCUHG,kNRCUHGnorm,
		   kNtotLG,kNtotHG,kEtotLG,kEtotHG,
		   kLGtime,kHGtime,kSpecLG,kSpecHG,
		   kHGqualMod0,kHGqualMod1,kHGqualMod2,kHGqualMod3,kHGqualMod4,
		   kLGqualMod0,kLGqualMod1,kLGqualMod2,kLGqualMod3,kLGqualMod4,
		   kHGpedRMSMod0,kHGpedRMSMod1,kHGpedRMSMod2,kHGpedRMSMod3,kHGpedRMSMod4,
		   kLGpedRMSMod0,kLGpedRMSMod1,kLGpedRMSMod2,kLGpedRMSMod3,kLGpedRMSMod4,
		   kHGpedRMS,kLGpedRMS,
                   kNtotHG_1_0,kNtotHG_1_1,kNtotHG_1_2,kNtotHG_1_3,
                   kNtotHG_2_0,kNtotHG_2_1,kNtotHG_2_2,kNtotHG_2_3,
                   kNtotHG_3_0,kNtotHG_3_1,kNtotHG_3_2,kNtotHG_3_3,} ;
  //Histograms for RecPoints  control
  enum HRPType_t {kRPmod1,kRPmod2,kRPmod3,kRPmod4,kRPmod5,
                kRPNtot,kRPEtot,kRPSpec,kRPTime,kRPNcpv} ;
  //Histograms for ESDs  control
  enum HESDType_t {kESDNtot,kESDEtot,kESDSpec,kESDpid} ;
  enum HDigitType_t  {kDigits=0, kDigitsMul} ; 

                 

public:
  AliPHOSQADataMakerRec() ;          // ctor
  AliPHOSQADataMakerRec(const AliPHOSQADataMakerRec& qadm) ;   
  AliPHOSQADataMakerRec& operator = (const AliPHOSQADataMakerRec& qadm) ;
  virtual ~AliPHOSQADataMakerRec() {;} // dtor
  
private:
  const AliPHOSRecoParam* GetRecoParam() { return dynamic_cast<const AliPHOSRecoParam *>(fRecoParam); }

  virtual void   EndOfDetectorCycle(AliQAv1::TASKINDEX_t, TObjArray ** list) ;
  virtual void   InitESDs() ; 
  virtual void   InitDigits() ; 
  virtual void   InitRecPoints() ; 
  virtual void   InitRaws() ; 
  virtual void   MakeESDs(AliESDEvent * esd) ;
  virtual void   MakeDigits() ;
  virtual void   MakeDigits(TTree * recpoTree) ; 
  virtual void   MakeRecPoints(TTree * recpoTree) ; 
  virtual void   MakeRaws(AliRawReader* rawReader) ; 
  virtual void   StartOfDetectorCycle() ; 

  ClassDef(AliPHOSQADataMakerRec,1)  // description 

};

#endif // AliPHOSQADataMakerRec_H
