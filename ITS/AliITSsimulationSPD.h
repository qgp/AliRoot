#ifndef ALIITSSIMULATIONSPD_H
#define ALIITSSIMULATIONSPD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*
$Id$
*/
#include "AliITSresponseSPD.h"
#include "AliITSsegmentationSPD.h"
#include "AliITSsimulation.h"

class AliITSMapA2;
class AliITSpList;
class AliITSmodule;

//-------------------------------------------------------------------

class AliITSsimulationSPD : public AliITSsimulation {

 public:        
    AliITSsimulationSPD(); // Default constructor
    // Standard constructor
    AliITSsimulationSPD(AliITSsegmentation *seg, AliITSresponse *res);
    ~AliITSsimulationSPD();// destructor
    AliITSsimulationSPD(const AliITSsimulationSPD &source); // copy constructo
    // assignment operator
    AliITSsimulationSPD& operator=(const AliITSsimulationSPD &source);
    // Initilizes the variables
    void Init(AliITSsegmentationSPD *seg, AliITSresponseSPD *resp);

    // Sum digitize module
    // Create maps to build the lists of tracks for each summable digit
    void InitSimulationModule(Int_t module,Int_t events);
    // Digitize module from the sum of summable digits.
    void FinishSDigitiseModule();
    void SDigitiseModule(AliITSmodule *mod, Int_t dummy0,Int_t dummy1);
    // digitize module. Also need to digitize modules with only noise.
    void DigitiseModule(AliITSmodule *mod,Int_t dummy0, Int_t dummy1);
    // sum digits to Digits.
    void SDigitsToDigits(Int_t module,AliITSpList *pList);
    // updates the Map of signal, adding the energy  (ene) released by
    // the current track
    void UpdateMapSignal(Int_t row,Int_t col,Int_t trk,Int_t hit,Int_t mod,
			 Double_t ene,AliITSpList *pList);
    // updates the Map of noise, adding the energy  (ene) give my noise
    void UpdateMapNoise(Int_t row,Int_t col,Int_t mod,Double_t ene,
			AliITSpList *pList);
    // Loops over all hits to produce Analog/floting point digits. This
    // is also the first task in producing standard digits.
    void HitsToAnalogDigits(AliITSmodule *mod,Int_t *frowpixel,
			    Int_t *fcolpixel,Double_t *fenepixel,
			    AliITSpList *pList);
    //  Steering function to determine the digits associated to a given
    // hit (hitpos)
    // The digits are created by charge sharing (ChargeSharing) and by
    // capacitive coupling (SetCoupling). At all the created digits is
    // associated the track number of the hit (ntrack)
    void HitToDigit(AliITSmodule *mod, Int_t hitpos,Int_t *frowpixel,
		    Int_t *fcolpixel, Double_t *fenepixel,AliITSpList *pList);
    //  Take into account the geometrical charge sharing when the track
    //  crosses more than one pixel.
    void ChargeSharing(Float_t x1l,Float_t z1l,Float_t x2l,Float_t z2l,
		       Int_t c1,Int_t r1,Int_t c2,Int_t r2,Float_t etot,
		       Int_t &npixel,Int_t *frowpixel,Int_t *fcolpixel,
		       Double_t *fenepixel);
    //  Take into account the coupling between adiacent pixels.
    //  The parameters probcol and probrow are the fractions of the
    //  signal in one pixel shared in the two adjacent pixels along
    //  the column and row direction, respectively. Now done in a statistical
    //  way and not "mechanical" as in the Old version.
    void SetCoupling(Int_t row,Int_t col,Int_t ntrack,Int_t idhit,Int_t module,
		     AliITSpList *pList);
    //  Take into account the coupling between adiacent pixels.
    //  The parameters probcol and probrow are the fractions of the
    //  signal in one pixel shared in the two adjacent pixels along
    //  the column and row direction, respectively.
    void SetCouplingOld(Int_t row,Int_t col,Int_t ntrack,Int_t idhit,
			Int_t module,AliITSpList *pList);
    // The pixels are fired if the energy deposited inside them is above
    // the threshold parameter ethr. Fired pixed are interpreted as digits
    // and stored in the file digitfilename. One also needs to write out
    // cases when there is only noise (nhits==0).
    void CreateDigit(Int_t module,AliITSpList *pList);
    //  Set the electronic noise and threshold non-uniformities to all the
    //  pixels in a detector.
    //  The parameter fSigma is the squared sum of the sigma due to noise
    //  and the sigma of the threshold distribution among pixels.
    void SetFluctuations(AliITSpList *pList,Int_t module);
    //  Apply a mask to the SPD module. 1% of the pixel channels are
    //  masked. When the database will be ready, the masked pixels
    //  should be read from it.
    void SetMask();
    // Create Histograms
    void CreateHistograms();
    // Reset histograms for this detector
    void ResetHistograms();
    // Fills the Summable digits Tree
    void WriteSDigits(AliITSpList *pList);
    // Fills fMap2A from the pList of Summable digits
    void FillMapFrompList(AliITSpList *pList);
    // get hist array
    TObjArray*  GetHistArray() {return fHis;}

 private:
    // Getters for data kept in fSegmentation and fResponse.
    // Returns the Threshold in electrons
    Double_t GetThreshold(){Float_t a=0.0,b=0.0;
    ((AliITSresponseSPD*)fResponse)->Thresholds(a,b); return a;}
    // Returns the threshold and rms noise.
    void GetThresholds(Float_t &t,Float_t &s){
    ((AliITSresponseSPD*)fResponse)->Thresholds(t,s);}
    // Returns the couplings Columb and Row.
    void GetCouplings(Float_t &cc,Float_t &cr){
	((AliITSresponseSPD*)fResponse)->GetNoiseParam(cc,cr);}
    // Returns the number of pixels in x
    Int_t GetNPixelsX(){return ((AliITSsegmentationSPD*)fSegmentation)->Npx();}
    // Returns the number of pixels in z
    Int_t GetNPixelsZ(){return ((AliITSsegmentationSPD*)fSegmentation)->Npz();}

 private:
/*    Float_t      fThresh;   //! Threshold from fResponse
    Float_t      fSigma;    //! Noise from fResponse
    Float_t      fCouplCol; //! Coupling along columns from fResponse
    Float_t      fCouplRow; //! Coupling along rows from fResponse
    Int_t        fNPixelsX; //! NPixelsX from fSegmentation
    Int_t        fNPixelsZ; //! NPixelsZ from fSegmentation
*/
//    AliITSpList  *fList;    //! Internal map and sdigits
//    Int_t         fModule;  //! Module number being simulated
//    Int_t         fEvent;   //! Event number being simulated
    AliITSMapA2  *fMapA2;   //! MapA2 for Local internal use only
    TObjArray    *fHis;     //! just in case for histogramming for Local
                            // internal use only

    ClassDef(AliITSsimulationSPD,1)  // Simulation of SPD clusters

};

#endif
