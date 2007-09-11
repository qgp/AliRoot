#ifndef ALIEMCALCALIBCOEFS_H
#define ALIEMCALCALIBCOEFS_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */

/* $Id$ */

//_________________________________________________________________________
//  Table of Calibration coefficients  
//  Should be extended.
//                  
//*-- Author: Aleksei Pavlinov (WSU, Detroit, USA) 

// --- ROOT system ---
#include <TNamed.h>
#include <TObjArray.h>

// unit is GeV
class calibCoef : public TObject {
 public:
  virtual const char* GetName() const {return Form("CC%5.5i",absId);}
  calibCoef();
  calibCoef(const Int_t id, const Double_t c, const Double_t ec);
  virtual ~calibCoef() {};

  Int_t    absId; // absolute id of cell 
  Double_t cc;    // Calib. coef
  Double_t eCc;   // Calib. coef. error
  // 
  ClassDef(calibCoef,1) // Cell calibration information 
};

class TH1F;
class AliEMCALCalibData;

class AliEMCALCalibCoefs : public TNamed {
 public:
  enum EEmcalCalibType {kMC, kEQUALIZATION, kMIP, kPI0}; // type of EMCAL calibrations 

  AliEMCALCalibCoefs(); // default constractor
  AliEMCALCalibCoefs(const char* name, const Int_t nrow);
  virtual ~AliEMCALCalibCoefs();

  AliEMCALCalibCoefs & operator = (const AliEMCALCalibCoefs  & /*rvalue*/) {
    // assignement operator requested by coding convention but not needed
    Fatal("operator =", "not implemented");
    return *this;
  };
  void        AddAt(calibCoef* r);
  calibCoef*  GetTable(Int_t i) const;
  Int_t       GetSize()  const {return fTable->GetSize();}
  Int_t       GetNRows() const {return fCurrentInd;}
  void        Purge() {/* nothing */};

  void  SetCalibMethod(Int_t var) {fCalibMethod=var;}
  Int_t GetCalibMethod() {return fCalibMethod;}
  calibCoef* GetRow(const int absId);
  // Get initial Calib Data from DB
  static AliEMCALCalibCoefs *GetCalibTableFromDb(const char *tn="CCIN", AliEMCALCalibData **calData=0);
  //  const char* dbLocation="local:///data/r22b/ALICE/PROD/CALIBRATION_May_2007/PI0/PDSF/10GEV/DECALIB/DeCalibDB");
  static TH1F *GetHistOfCalibTableFromDb(const char *tn="CCIN");
  //const char* dbLocation="local:///data/r22b/ALICE/PROD/CALIBRATION_May_2007/PI0/PDSF/10GEV/DECALIB/DeCalibDB");
  // Create DB calib table
  static AliEMCALCalibData*  GetCalibTableForDb(const AliEMCALCalibCoefs *tab, 
  const char* dbLocation="local://.", // current directory
  const char* coment="pi0 calibration, MC, Jun 2007");
  // Menu
  void PrintTable();                 // *MENU*
  void PrintTable(const Int_t i);    // *MENU*
  void PrintRec(calibCoef *r);

  //
 protected:
  TObjArray *fTable;
  Int_t fCurrentInd;
  Int_t fCalibMethod;  // method of calibration - EEmcalCalibType

  ClassDef(AliEMCALCalibCoefs,2) // Table of Calibration coefficients  
};

#endif // ALIEMCALCalibCoefs_H
