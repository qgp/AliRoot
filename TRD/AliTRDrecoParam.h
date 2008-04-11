#ifndef ALITRDRECOPARAM_H
#define ALITRDRECOPARAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Parameter class for the TRD reconstruction                            //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef ALIDETECTORRECOPARAM_H
#include "AliDetectorRecoParam.h"
#endif

class AliTRDrecoParam : public AliDetectorRecoParam
{
  public:
	enum{
	  kNNslices = 8,
	  kLQslices = 3
	};
	
	AliTRDrecoParam();
	~AliTRDrecoParam() { }

	Double_t GetChi2Y() const                 { return fkChi2Y;    }
	Double_t GetChi2Z() const                 { return fkChi2Z;    }
	Bool_t   GetClusterSharing() const        { return fkClusterSharing;}
	Double_t GetFindableClusters() const      { return fkFindable; }
	Double_t GetMaxTheta() const              { return fkMaxTheta; }
	Double_t GetMaxPhi() const                { return fkMaxPhi;   }
	Int_t    GetNdEdxSlices() const           { return fkPIDMethod ? kNNslices : kLQslices;}
	Int_t    GetPIDMethod() const             { return fkPIDMethod;}
	Double_t GetRoad0y() const                { return fkRoad0y;   }
	Double_t GetRoad0z() const                { return fkRoad0z;   }

	Double_t GetRoad1y() const                { return fkRoad1y;   }
	Double_t GetRoad1z() const                { return fkRoad1z;   }

	Double_t GetRoad2y() const                { return fkRoad2y;   }
	Double_t GetRoad2z() const                { return fkRoad2z;   }

	Double_t GetPlaneQualityThreshold() const { return fkPlaneQualityThreshold; }

	Double_t GetTrackLikelihood() const       { return fkTrackLikelihood;       }

        Double_t GetClusMaxThresh() const         { return fClusMaxThresh;   };
        Double_t GetClusSigThresh() const         { return fClusSigThresh;   };
        Int_t    GetTCnexp() const                { return fTCnexp;          };
        Bool_t   LUTOn() const                    { return fLUTOn;           };
        Bool_t   TCOn() const                     { return fTCOn;            };

        Int_t    GetADCbaseline() const           { return fADCbaseline;     };

	static   AliTRDrecoParam *GetLowFluxParam();
        static   AliTRDrecoParam *GetHighFluxParam();
        static   AliTRDrecoParam *GetCosmicTestParam();

	void     SetClusterSharing(Bool_t share = kTRUE) { fkClusterSharing = share;  };
	void     SetPIDMethod(Int_t pid = 1)             { fkPIDMethod = pid ? 1 : 0; };
        void     SetLUT(Int_t lutOn = 1)                 { fLUTOn           = lutOn;  };
        void     SetClusMaxThresh(Float_t thresh)        { fClusMaxThresh   = thresh; };
        void     SetClusSigThresh(Float_t thresh)        { fClusSigThresh   = thresh; };
        void     SetTailCancelation(Int_t tcOn = 1)      { fTCOn            = tcOn;   };
        void     SetNexponential(Int_t nexp)             { fTCnexp          = nexp;   };
        void     SetADCbaseline(Int_t base)              { fADCbaseline     = base;   };

 private:

	// Tracking parameters
	Bool_t    fkClusterSharing;        // Toggle cluster sharing
	Int_t     fkPIDMethod;             // PID method selector 0(LQ) 1(NN)
	Double_t  fkMaxTheta;              // Maximum theta
	Double_t  fkMaxPhi;                // Maximum phi

	Double_t  fkRoad0y;                // Road for middle cluster
	Double_t  fkRoad0z;                // Road for middle cluster

	Double_t  fkRoad1y;                // Road in y for seeded cluster
	Double_t  fkRoad1z;                // Road in z for seeded cluster

	Double_t  fkRoad2y;                // Road in y for extrapolated cluster
	Double_t  fkRoad2z;                // Road in z for extrapolated cluster
	
	Double_t  fkPlaneQualityThreshold; // Quality threshold
	Double_t  fkFindable;              // Ratio of clusters from a track in one chamber which are at minimum supposed to be found.
	Double_t  fkChi2Z;                 // Max chi2 on the z direction for seeding clusters fit
	Double_t  fkChi2Y;                 // Max chi2 on the y direction for seeding clusters Rieman fit
	Double_t  fkTrackLikelihood;       // Track likelihood for tracklets Rieman fit

        // Clusterization parameter
        Double_t  fClusMaxThresh;          // Threshold value for cluster maximum
        Double_t  fClusSigThresh;          // Threshold value for cluster signal
        Int_t     fLUTOn;                  // Switch for the lookup table method  
        Int_t     fTCOn;                   // Switch for the tail cancelation
        Int_t     fTCnexp;                 // Number of exponentials, digital filter
  
	// ADC parameter
        Int_t     fADCbaseline;            // ADC baseline to be subtracted

	ClassDef(AliTRDrecoParam, 3)       // Reconstruction parameters for TRD detector

};
#endif
