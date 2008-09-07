#ifndef ALITRDCALIBRAFILLHISTO_H
#define ALITRDCALIBRAFILLHISTOs_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class for the HLT parameters                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#  include <TObject.h>
#endif
#ifndef ROOT_TLinearFitter
#  include <TLinearFitter.h> 
#endif
#ifndef ROOT_TProfile2D
#  include <TProfile2D.h> 
#endif
#ifndef ROOT_TH2I
#  include <TH2I.h> 
#endif

class TProfile2D;
class TObjArray;
class TH1F;
class TH2I;
class TH2F;
class TH2;
class TLinearFitter;
class TTreeSRedirector;

class AliLog;
class AliRawReader;

class AliTRDCalibraMode;
class AliTRDCalibraVector;
class AliTRDCalibraVdriftLinearFit;
class AliTRDrawStreamBase;
class AliTRDcluster;
class AliTRDtrackV1;
class AliTRDtrack;
class AliTRDseedV1;
class AliTRDmcm;
class AliTRDmcmTracklet;
class AliTRDgeometry;
class AliTRDCalDet;
class AliTRDCalROC;


struct eventHeaderStruct;

class AliTRDCalibraFillHisto : public TObject {

 public: 

  // Instance
  static AliTRDCalibraFillHisto *Instance();
  static void Terminate();
  static void Destroy();
  void DestroyDebugStreamer();


  AliTRDCalibraFillHisto(const AliTRDCalibraFillHisto &c);
  AliTRDCalibraFillHisto &operator=(const AliTRDCalibraFillHisto &) { return *this; }

  // Functions for initialising and filling with AliTRDtrackV1
          Bool_t  Init2Dhistos();
	  Bool_t  UpdateHistograms(AliTRDtrack *t);
	  Bool_t  UpdateHistogramsV1(AliTRDtrackV1 *t);
 
  // Process events DAQ
	  Int_t   ProcessEventDAQ(AliTRDrawStreamBase *rawStream, Bool_t nocheck = kFALSE);
	  Int_t   ProcessEventDAQ(AliRawReader *rawReader, Bool_t nocheck = kFALSE);
	  Int_t   ProcessEventDAQ(eventHeaderStruct *event, Bool_t nocheck = kFALSE);

	  Int_t   ProcessEventDAQV1(AliTRDrawStreamBase *rawStream, Bool_t nocheck = kFALSE);
	  Int_t   ProcessEventDAQV1(AliRawReader *rawReader, Bool_t nocheck = kFALSE);
	  Int_t   ProcessEventDAQV1(eventHeaderStruct *event, Bool_t nocheck = kFALSE);

  // Is Pad on
          Bool_t   IsPadOn(Int_t detector, Int_t row, Int_t col) const;

  // Functions for write
	  void     Write2d(const Char_t *filename = "TRD.calibration.root", Bool_t append = kFALSE);

  //For the statistics
	  Double_t *StatH(TH2 *ch, Int_t i);
	  Double_t *GetMeanMedianRMSNumberCH();
	  Double_t *GetMeanMedianRMSNumberLinearFitter() const;
       
     	 
  //
  // Set of Get the variables
  //

  // Choice to fill or not the 2D
          void     SetMcmCorrectAngle(Bool_t mcmcorrectangle = kTRUE)        { fMcmCorrectAngle = mcmcorrectangle;   }
          void     SetPH2dOn(Bool_t ph2don = kTRUE)                          { fPH2dOn          = ph2don;            }
          void     SetCH2dOn(Bool_t ch2don = kTRUE)                          { fCH2dOn          = ch2don;            }
          void     SetPRF2dOn(Bool_t prf2don = kTRUE)                        { fPRF2dOn         = prf2don;           }
          void     SetHisto2d(Bool_t histo2d = kTRUE)                        { fHisto2d         = histo2d;           }
          void     SetVector2d(Bool_t vector2d = kTRUE)                      { fVector2d        = vector2d;          }
	  void     SetLinearFitterOn(Bool_t linearfitteron = kTRUE)          { fLinearFitterOn      = linearfitteron;}
	  void     SetLinearFitterDebugOn(Bool_t debug = kTRUE)              { fLinearFitterDebugOn = debug;         }
	 	  
  
          Bool_t   GetMcmCorrectAngle() const                                { return fMcmCorrectAngle;        }
          Bool_t   GetPH2dOn() const                                         { return fPH2dOn;                 }
          Bool_t   GetCH2dOn() const                                         { return fCH2dOn;                 }
          Bool_t   GetPRF2dOn() const                                        { return fPRF2dOn;                }
          Bool_t   GetHisto2d() const                                        { return fHisto2d;                }
          Bool_t   GetVector2d() const                                       { return fVector2d;               }
          Bool_t   GetLinearFitterOn() const                                 { return fLinearFitterOn;         }
	  Bool_t   GetLinearFitterDebugOn() const                            { return fLinearFitterDebugOn; }


  // Get stuff that are filled
  TH2I            *GetCH2d();
  TProfile2D      *GetPH2d(Int_t nbtimebin=24, Float_t samplefrequency= 10.0);
  TProfile2D      *GetPRF2d() const                                          { return fPRF2d;                  } 
  TObjArray        GetLinearFitterArray() const                              { return fLinearFitterArray;      }
  TLinearFitter   *GetLinearFitter(Int_t detector, Bool_t force=kFALSE);
  AliTRDCalibraVdriftLinearFit *GetVdriftLinearFit() const                   { return fLinearVdriftFit;        }
  
 
  // How to fill the 2D
          void     SetRelativeScale(Float_t relativeScale);                      
          void     SetThresholdClusterPRF2(Float_t thresholdClusterPRF2)     { fThresholdClusterPRF2 = thresholdClusterPRF2; }
	  void     SetLimitChargeIntegration(Bool_t limitChargeIntegration)  { fLimitChargeIntegration = limitChargeIntegration; }
	  void     SetFillWithZero(Bool_t fillWithZero)                      { fFillWithZero = fillWithZero;   }
	  void     SetNz(Int_t i, Short_t nz);
          void     SetNrphi(Int_t i, Short_t nrphi);
          void     SetProcent(Float_t procent)                               { fProcent              = procent;              }
          void     SetDifference(Short_t difference)                         { fDifference           = difference;           }
          void     SetNumberClusters(Short_t numberClusters)                 { fNumberClusters       = numberClusters;       }
	  void     SetNumberClustersf(Short_t numberClustersf)               { fNumberClustersf      = numberClustersf;      }
          void     SetNumberBinCharge(Short_t numberBinCharge)               { fNumberBinCharge      = numberBinCharge;      }
          void     SetNumberBinPRF(Short_t numberBinPRF)                     { fNumberBinPRF         = numberBinPRF;         }
	  void     SetNumberGroupsPRF(Short_t numberGroupsPRF);
  
          Float_t  GetRelativeScale() const                                  { return fRelativeScale;          }
          Float_t  GetThresholdClusterPRF2() const                           { return fThresholdClusterPRF2;   }
	  Bool_t   GetLimitChargeIntegration() const                         { return fLimitChargeIntegration; }
	  Bool_t   GetFillWithZero() const                                   { return fFillWithZero;           }
	  Float_t  GetProcent() const                                        { return fProcent;                }
          Short_t  GetDifference() const                                     { return fDifference;             }
          Short_t  GetNumberClusters() const                                 { return fNumberClusters;         }
	  Short_t  GetNumberClustersf() const                                { return fNumberClustersf;        }
          Short_t  GetNumberBinCharge() const                                { return fNumberBinCharge;        }
          Short_t  GetNumberBinPRF() const                                   { return fNumberBinPRF;           }
	  Short_t  GetNumberGroupsPRF() const                                { return fNgroupprf;              }
	  Int_t    *GetEntriesLinearFitter() const                           { return fEntriesLinearFitter;    }

 // Debug
          void     SetDebugLevel(Short_t level)                              { fDebugLevel = level;           }

  // Vector method
AliTRDCalibraVector *GetCalibraVector() const                                { return fCalibraVector;          }   
  
 protected:

  // Geometry
  AliTRDgeometry  *fGeo;                    //! The TRD geometry

  // Choice to fill or not the 2D
          Bool_t   fMcmCorrectAngle;        // Apply correction due to the mcmtrackletangle in the z direction (only) assuming  from vertex
          Bool_t   fCH2dOn;                 // Chose to fill the 2D histos or vectors for the relative gain calibration 
          Bool_t   fPH2dOn;                 // Chose to fill the 2D histos or vectors for the drift velocity and T0
          Bool_t   fPRF2dOn;                // Chose to fill the 2D histos or vectors for the pad response function calibration
          Bool_t   fHisto2d;                // Chose to fill the 2D histos
          Bool_t   fVector2d;               // Chose to fill vectors
	  Bool_t   fLinearFitterOn;         // Method with linear fit for drift velocity
	  Bool_t   fLinearFitterDebugOn;    // Method with linear fit for drift velocity

  // How to fill the 2D
          Float_t  fRelativeScale;          // Scale of the deposited charge
          Float_t  fThresholdClusterPRF2;   // Threshold on cluster pad signals
          Bool_t   fLimitChargeIntegration; // Integration range for the gain calibration
	  Bool_t   fFillWithZero;           // Fill with zero or not the average pulse height
  // Calibration mode
	  AliTRDCalibraMode *fCalibraMode;  // Calibration mode

  //For debugging
	  TTreeSRedirector          *fDebugStreamer;                 //!Debug streamer
          Short_t     fDebugLevel;                                   // Flag for debugging
  //
  // Internal variables
  //

  // Fill the 2D histos in the offline tracking
	  Int_t    fDetectorPreviousTrack;  // Change of detector
	  Int_t    fMCMPrevious;            // Change of MCM
	  Int_t    fROBPrevious;            // Change of ROB
	  Short_t  fNumberClusters;         // Minimum number of clusters in the tracklets
	  Short_t  fNumberClustersf;        // Maximum number of clusters in the tracklets
          Float_t  fProcent;                // Limit to take the info of the most important calibration group if the track goes through 2 groups (CH)
          Short_t  fDifference;             // Limit to take the info of the most important calibration group if the track goes through 2 groups (CH)
          Int_t    fNumberTrack;            // How many tracks could be used (Debug for the moment)
          Int_t    fNumberUsedCh[2];        // How many tracks have been really used for the gain (0, strict; 1 with fProcent)
          Int_t    fNumberUsedPh[2];        // How many tracks have been really used for the drift velocity (0, strict; 1 with fDifference)
	  Int_t    fTimeMax;                // Number of time bins
          Float_t  fSf;                     // Sampling frequence
	  Short_t  fNumberBinCharge;        // Number of bins for the gain factor
	  Short_t  fNumberBinPRF;           // Number of bin for the PRF
	  Short_t  fNgroupprf;              // Number of groups in tnp bins for PRF /2.0

  // Variables per tracklet
	  Float_t       *fAmpTotal;                  // Energy deposited in the calibration group by the track
          Short_t       *fPHPlace;                   // Calibration group of PH
          Float_t       *fPHValue;                   // PH
	  Bool_t         fGoodTracklet;              // Good tracklet
	  TLinearFitter *fLinearFitterTracklet;      // linear fitter tracklet  
  //Statistics
	  Int_t         *fEntriesCH;                 // Number of entries CH
	  Int_t         *fEntriesLinearFitter;       // Number of entries LinearFitter


  //
  // Vector method
  //
  	  
	  AliTRDCalibraVector *fCalibraVector; // The vector object
 
 
  // Histograms to store the info from the digits, from the tracklets or from the tracks
	  TProfile2D      *fPH2d;                         // 2D average pulse height
	  TProfile2D      *fPRF2d;                        // 2D PRF
	  TH2I            *fCH2d;                         // 2D deposited charge
	  TObjArray       fLinearFitterArray;             // TObjArray of Linear Fitters for the detectors 
	  AliTRDCalibraVdriftLinearFit *fLinearVdriftFit; // Info Linear Fit
	  
 // Current calib object: to correct for the database used
	  AliTRDCalDet *fCalDetGain;                      // Current calib object gain
	  AliTRDCalROC *fCalROCGain;                      // Current calib object gain
	   
  //
  // A lot of internal functions......
  //
  // Create the 2D histo to be filled Online
          void     CreateCH2d(Int_t nn);
          void     CreatePH2d(Int_t nn);
          void     CreatePRF2d(Int_t nn);
  
  // Calibration with AliTRDtrackV1
          void     FillTheInfoOfTheTrackPH();
          void     FillTheInfoOfTheTrackCH(Int_t nbclusters);
	  Bool_t   FindP1TrackPHtracklet(AliTRDtrack *t, Int_t index0, Int_t index1);
	  Bool_t   FindP1TrackPHtrackletV1(const AliTRDseedV1 *tracklet, Int_t nbclusters);
	  Bool_t   HandlePRFtracklet(AliTRDtrack *t, Int_t index0, Int_t index1);
	  Bool_t   HandlePRFtrackletV1(const AliTRDseedV1 *tracklet, Int_t nbclusters);
	  void     ResetfVariablestracklet();
	  void     StoreInfoCHPHtrack(AliTRDcluster *cl, Double_t dqdl, Int_t *group, Int_t row, Int_t col);
	  void     FillCH2d(Int_t x, Float_t y);

  // Calibration on DAQ

	  Int_t    FillDAQ(Double_t phvalue[16][144][36]);
	  Int_t    FillDAQ(AliTRDmcm *mcm);
	  Int_t    TestTracklet( Int_t idet, Int_t row, Int_t iSeed, AliTRDmcm *mcm);
	  Bool_t   UpdateDAQ(Int_t det, Int_t /*row*/, Int_t /*col*/, Int_t timebin, Float_t signal, Int_t nbtimebins);
	  Int_t    UpdateHistogramcm(AliTRDmcmTracklet *trk);

  // row col calibration groups stuff
          Bool_t   LocalisationDetectorXbins(Int_t detector);
	  Int_t    CalculateTotalNumberOfBins(Int_t i);
	  void     CheckGoodTrackletV0(Int_t detector, Int_t row, Int_t col);
	  void     CheckGoodTrackletV1(AliTRDcluster *cl);
	  Int_t    CalculateCalibrationGroup(Int_t i, Int_t row, Int_t col) const;
 
  // LinearFitter
	  void     AnalyseLinearFitter();
	  
  // Clear
          void     ClearHistos();
      
  // Some basic geometry function
  virtual Int_t    GetLayer(Int_t d) const;
  virtual Int_t    GetStack(Int_t d) const;
  virtual Int_t    GetSector(Int_t d) const;
	  
          
  // Instance of this class and so on
  static  AliTRDCalibraFillHisto *fgInstance;                // Instance
  static  Bool_t   fgTerminated;                             // If terminated

 private:
  
  // This is a singleton, contructor is private!
  AliTRDCalibraFillHisto();
  virtual ~AliTRDCalibraFillHisto(); 
    
  ClassDef(AliTRDCalibraFillHisto,4)                         // TRD Calibration class

};
  
#endif


