#ifndef ALIQA_H
#define ALIQA_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//
// Quality Assurance Object
//

#include <TNamed.h> 
class TFile ; 

#include "AliLog.h"

class AliQA : public TNamed {
public:

	enum DETECTORINDEX_t {
    kNULLDET=-1, kITS, kTPC, kTRD, kTOF, kPHOS, kHMPID, kEMCAL, kMUON, kFMD,
    kZDC, kPMD, kT0, kVZERO, kACORDE, kHLT, kGLOBAL, kNDET };
	enum ALITASK_t {
    kNULLTASK=-1, kRAW, kSIM, kREC, kESD, kANA, kNTASK };
	enum QABIT_t {
    kNULLBit=-1, kINFO, kWARNING, kERROR, kFATAL, kNBIT };
    enum RUNTYPE_t {
      kNULLTYPE=-1, kUNKOWN, kAUTO_TEST, kCALIBRATION, kCALIBRATION_PULSER, kCHANNEL_DELAY_TUNING, kCOSMIC, kCOSMICS, kDAQ_FO_UNIF_SCAN, 
		kDAQ_GEN_DAC_SCAN, kDAQ_MEAN_TH_SCAN, kDAQ_MIN_TH_SCAN, kDAQ_NOISY_PIX_SCAN, kDAQ_PIX_DELAY_SCAN, kDAQ_UNIFORMITY_SCAN, 
		kDCS_FO_UNIF_SCAN, kDCS_MEAN_TH_SCAN, kDCS_MIN_TH_SCAN, kDCS_PIX_DELAY_SCAN, kDCS_UNIFORMITY_SCAN, kDDL_TEST, kGAIN, 
		kPEDESTAL, kINJECTOR,  kLASER, kMONTECARLO, kNOISE, kNOISY_PIX_SCAN,  kPHYSICS, kPULSER, kSTANDALONE, kSTANDALONE_BC, 
		kSTANDALONE_CENTRAL, kSTANDALONE_COSMIC, kSTANDALONE_EMD, kSTANDALONE_LASER, kSTANDALONE_MB, kSTANDALONE_PEDESTAL, 
		kSTANDALONE_SEMICENTRAL, kSTANDALONE_PULSER, kNTYPE};
	
	enum TASKINDEX_t {
    kRAWS, kHITS, kSDIGITS, kDIGITS, kRECPOINTS, kTRACKSEGMENTS, kRECPARTICLES, kESDS, kNTASKINDEX };
  
	// Creators - destructors
	AliQA(); // beware singleton, not to be used
	AliQA(const ALITASK_t tsk) ;
	AliQA(const DETECTORINDEX_t det) ;
	AliQA(const AliQA& qa) ;   
	AliQA& operator = (const AliQA& qa) ;
	virtual ~AliQA();
 
	static  AliQA *        Instance() ;
	static  AliQA *        Instance(const DETECTORINDEX_t det) ;
	static  AliQA *        Instance(const ALITASK_t tsk) ;
	static  AliQA *        Instance(const TASKINDEX_t tsk) ;
	const Bool_t           CheckFatal() const ;
	static void            Close() ; 
	static const char *    GetAliTaskName(ALITASK_t tsk) ;
        static const TString   GetLabLocalFile() { return fkgLabLocalFile ; } 
        static const TString   GetLabLocalOCDB() { return fkgLabLocalOCDB ; } 
        static const TString   GetLabAliEnOCDB() { return fkgLabAliEnOCDB ; } 
	static const DETECTORINDEX_t GetDetIndex(const char * name) ; 
	static const TString   GetDetName(DETECTORINDEX_t det) { return fgDetNames[det] ; }
	static const char *    GetDetName(Int_t det) ;
        static const TString   GetGRPPath() { return fgGRPPath ; }  
	static TFile *         GetQADataFile(const char * name, const Int_t run, const Int_t cycle) ; 
	static TFile *	       GetQADataFile(const char * fileName) ;
	static const char *    GetQADataFileName(const char * name, const Int_t run, const Int_t cycle) 
								{return Form("%s.%s.%d.%d.root", name, fgQADataFileName.Data(), run, cycle)  ; }
	static const char *    GetQADataFileName() { return fgQADataFileName.Data() ; }
	static const char *    GetQAName() { return fkgQAName ; } 
	static TFile *         GetQAResultFile() ; 
	static const char  *   GetQAResultFileName() { return (fgQAResultDirName + fgQAResultFileName).Data() ; }
	static const char  *   GetQARefDefaultStorage() { return fkgQARefOCDBDefault.Data() ; }
	static const char  *   GetQARefFileName() { return fgQARefFileName ; }
	static const char  *   GetQARefStorage() { return fgQARefDirName.Data() ; }
	static const char  *   GetRefOCDBDirName() { return fkgRefOCDBDirName.Data() ; }
	static const char  *   GetRefDataDirName() { return fkgRefDataDirName.Data() ; }
	static const TString   GetRunTypeName(RUNTYPE_t rt = kNULLTYPE) ;
	static const TString   GetTaskName(TASKINDEX_t tsk) { return fgTaskNames[tsk] ; }
	const Bool_t           IsSet(DETECTORINDEX_t det, ALITASK_t tsk, QABIT_t bit) const ;
	const Bool_t           IsSetAny(DETECTORINDEX_t det, ALITASK_t tsk) const ;
	const Bool_t           IsSetAny(DETECTORINDEX_t det) const ;
	void                   Set(QABIT_t bit) ;
	static void			   SetQAResultDirName(const char * name) ; 
	static void            SetQARefStorage(const char * name) ; 
	static void            SetQARefDataDirName(RUNTYPE_t rt) { fkgRefDataDirName = GetRunTypeName(rt) ; }
	static void            SetQARefDataDirName(const char * name) ;
        void                   Show() const { ShowStatus(fDet) ; }
	void                   ShowAll() const ;
	void                   UnSet(QABIT_t bit) ;

private:      

	const Bool_t         CheckRange(DETECTORINDEX_t det) const ;
	const Bool_t         CheckRange(ALITASK_t tsk) const ;
	const Bool_t         CheckRange(QABIT_t bit) const ;
	const char *         GetBitName(QABIT_t bit) const ;
	const ULong_t        GetStatus(DETECTORINDEX_t det) const  { return fQA[det] ;}
	void                 Finish() const ;  
	const ULong_t        Offset(ALITASK_t tsk) const ;
	void                 ShowStatus(DETECTORINDEX_t det) const ;
	void                 ShowASCIIStatus(DETECTORINDEX_t det, ALITASK_t tsk, ULong_t status) const ; 
	void                 ResetStatus(DETECTORINDEX_t det) { fQA[det] = 0 ; }
	void                 Set(DETECTORINDEX_t det) { fDet = det ;}
	void                 Set(ALITASK_t tsk) { fTask = tsk ; AliDebug(1, Form("Ready to set QA status in %s", GetAliTaskName(tsk) )) ; }
	void                 SetStatus(DETECTORINDEX_t det, UShort_t status) { fQA[det] = status ; }
	void                 SetStatusBit(DETECTORINDEX_t det, ALITASK_t tsk, QABIT_t bit) ;
	void                 UnSetStatusBit(DETECTORINDEX_t det, ALITASK_t tsk, QABIT_t bit) ;

	static AliQA *       fgQA		    ; // pointer to the instance of the singleton
        Int_t                fNdet     	            ; // number of detectors
	ULong_t    *         fQA		    ; //[fNdet] the status word 4 bits for SIM, REC, ESD, ANA each
	DETECTORINDEX_t      fDet		    ; //!  the current detector (ITS, TPC, ....)
	ALITASK_t            fTask	            ; //!  the current environment (SIM, REC, ESD, ANA)
	static TString       fgDetNames[]	    ; //! list of detector names   
        static TString       fgGRPPath              ; //! path of the GRP object in OCDB
	static TFile *       fgQADataFile	    ; //! the output file where the quality assurance maker store their results
	static TString       fgQADataFileName       ; //! the name of the file where the quality assurance maker store their results
	static TFile *       fgQARefFile	    ; //! the output file where the quality assurance maker store their results
	static TString       fgQARefDirName	    ; //! name of directory where to find the reference data file
	static TString       fgQARefFileName        ; //! file name where to find the reference data
	static TFile *       fgQAResultFile         ; //! File where to find the QA result
	static TString       fgQAResultDirName      ; //! the location of the output file where the QA results are stored  
	static TString       fgQAResultFileName     ; //! the output file where the QA results are stored  
	static TString       fgRTNames[]	    ; //! list of Run Type names   
	static TString       fgTaskNames[]	    ; //! list of tasks names   
	static const TString fkgLabLocalFile        ; //! label to identify a file as local 
	static const TString fkgLabLocalOCDB        ; //! label to identify a file as local OCDB 
	static const TString fkgLabAliEnOCDB        ; //! label to identify a file as AliEn OCDB 
	static const TString fkgRefFileName         ; //! name of Reference File Name 
	static const TString fkgQAName              ; //! name of QA object 
	static const TString fkgRefOCDBDirName      ; //! name of Reference directory name in OCDB  	
	static       TString fkgRefDataDirName      ; //! name of Reference directory name in OCDB for data  	
	static const TString fkgQARefOCDBDefault    ; //! default storage for QA in OCDB 

 ClassDef(AliQA,1)  //ALICE Quality Assurance Object
};
#endif
