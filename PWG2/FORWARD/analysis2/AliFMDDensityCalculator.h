// This class calculates the inclusive charged particle density
// in each for the 5 FMD rings. 
//
#ifndef ALIFMDDENSITYCALCULATOR_H
#define ALIFMDDENSITYCALCULATOR_H
#include <TNamed.h>
#include <TList.h>
#include <TArrayI.h>
#include "AliForwardUtil.h"
class AliESDFMD;
class TH2D;
class TH1D;
class TProfile;
class AliFMDCorrELossFit;

/** 
 * This class calculates the inclusive charged particle density
 * in each for the 5 FMD rings. 
 *
 * @par Input:
 *   - AliESDFMD object possibly corrected for sharing
 *
 * @par Output:
 *   - 5 RingHistos objects - each with a number of vertex dependent 
 *     2D histograms of the inclusive charge particle density 
 * 
 * @par Corrections used: 
 *   - AliFMDAnaCalibEnergyDistribution 
 *   - AliFMDDoubleHitCorrection 
 *   - AliFMDDeadCorrection 
 *
 * @ingroup pwg2_forward_algo
 */
class AliFMDDensityCalculator : public TNamed
{
public:
  /** 
   * Constructor 
   */
  AliFMDDensityCalculator();
  /** 
   * Constructor 
   * 
   * @param name Name of object
   */
  AliFMDDensityCalculator(const char* name);
  /** 
   * Copy constructor 
   * 
   * @param o Object to copy from 
   */
  AliFMDDensityCalculator(const AliFMDDensityCalculator& o);
  /** 
   * Destructor 
   */
  virtual ~AliFMDDensityCalculator();
  /** 
   * Assignement operator
   * 
   * @param o Object to assign from 
   * 
   * @return Reference to this object
   */
  AliFMDDensityCalculator& operator=(const AliFMDDensityCalculator& o);
  /** 
   * Initialize this sub-algorithm
   * 
   * @param etaAxis Not used 
   */
  virtual void Init(const TAxis& etaAxis);
  /** 
   * Do the calculations 
   * 
   * @param fmd      AliESDFMD object (possibly) corrected for sharing
   * @param hists    Histogram cache
   * @param vtxBin   Vertex bin 
   * @param lowFlux  Low flux flag. 
   * 
   * @return true on successs 
   */
  virtual Bool_t Calculate(const AliESDFMD& fmd, 
			   AliForwardUtil::Histos& hists, 
			   UShort_t vtxBin, Bool_t lowFlux);
  /** 
   * Scale the histograms to the total number of events 
   * 
   * @param dir     where to put the output
   * @param nEvents Number of events 
   */
  virtual void ScaleHistograms(TList* dir, Int_t nEvents);
  /** 
   * Output diagnostic histograms to directory 
   * 
   * @param dir List to write in
   */  
  virtual void DefineOutput(TList* dir);
  /** 
   * Set the debug level.  The higher the value the more output 
   * 
   * @param dbg Debug level 
   */
  void SetDebug(Int_t dbg=1) { fDebug = dbg; }
  /** 
   * Maximum particle weight to use 
   * 
   * @param m 
   */
  void SetMaxParticles(UShort_t m) { fMaxParticles = m; }  
  /** 
   * Set the lower multiplicity cut.  This overrides the setting in
   * the energy loss fits.
   * 
   * @param cut Cut to use 
   */
  void SetMultCut(Double_t cut) { fMultCut = cut; }
  /** 
   * Get the multiplicity cut.  If the user has set fMultCut (via
   * SetMultCut) then that value is used.  If not, then the lower
   * value of the fit range for the enery loss fits is returned.
   * 
   * @return Lower cut on multiplicity
   */
  Double_t GetMultCut() const;
  /** 
   * Print information 
   * 
   * @param option Print options 
   *   - max  Print max weights 
   */
  void Print(Option_t* option="") const;
protected:
  /** 
   * Find the max weight to use for FMD<i>dr</i> in eta bin @a iEta
   * 
   * @param cor   Correction
   * @param d     Detector 
   * @param r     Ring 
   * @param iEta  Eta bin 
   */
  Int_t FindMaxWeight(AliFMDCorrELossFit* cor,
		      UShort_t d, Char_t r, Int_t iEta) const;

  /** 
   * Find the max weights and cache them 
   * 
   */  
  void CacheMaxWeights();
  /** 
   * Find the (cached) maximum weight for FMD<i>dr</i> in 
   * @f$\eta@f$ bin @a iEta
   * 
   * @param d     Detector
   * @param r     Ring
   * @param iEta  Eta bin
   * 
   * @return max weight or <= 0 in case of problems 
   */
  Int_t GetMaxWeight(UShort_t d, Char_t r, Int_t iEta) const;
  /** 
   * Find the (cached) maximum weight for FMD<i>dr</i> iat
   * @f$\eta@f$ 
   * 
   * @param d     Detector
   * @param r     Ring
   * @param eta   Eta bin
   * 
   * @return max weight or <= 0 in case of problems 
   */
  Int_t GetMaxWeight(UShort_t d, Char_t r, Float_t eta) const;

  /** 
   * Get the number of particles corresponding to the signal mult
   * 
   * @param mult     Signal
   * @param d        Detector
   * @param r        Ring 
   * @param s        Sector 
   * @param t        Strip (not used)
   * @param v        Vertex bin 
   * @param eta      Pseudo-rapidity 
   * @param lowFlux  Low-flux flag 
   * 
   * @return The number of particles 
   */
  virtual Float_t NParticles(Float_t mult, 
			     UShort_t d, Char_t r, UShort_t s, UShort_t t, 
			     UShort_t v, Float_t eta, Bool_t lowFlux) const;
  /** 
   * Get the inverse correction factor.  This consist of
   * 
   * - acceptance correction (corners of sensors) 
   * - double hit correction (for low-flux events) 
   * - dead strip correction 
   * 
   * @param d        Detector
   * @param r        Ring 
   * @param s        Sector 
   * @param t        Strip (not used)
   * @param v        Vertex bin 
   * @param eta      Pseudo-rapidity 
   * @param lowFlux  Low-flux flag 
   * 
   * @return 
   */
  virtual Float_t Correction(UShort_t d, Char_t r, UShort_t s, UShort_t t, 
			     UShort_t v, Float_t eta, Bool_t lowFlux) const;
  /** 
   * Get the acceptance correction for strip @a t in an ring of type @a r
   * 
   * @param r  Ring type ('I' or 'O')
   * @param t  Strip number 
   * 
   * @return Inverse acceptance correction 
   */
  virtual Float_t AcceptanceCorrection(Char_t r, UShort_t t) const;
  /** 
   * Generate the acceptance corrections 
   * 
   * @param r Ring to generate for 
   * 
   * @return Newly allocated histogram of acceptance corrections
   */
  virtual TH1D*   GenerateAcceptanceCorrection(Char_t r) const;
  /** 
   * Internal data structure to keep track of the histograms
   */
  struct RingHistos : public AliForwardUtil::RingHistos
  { 
    /** 
     * Default CTOR
     */
    RingHistos();
    /** 
     * Constructor
     * 
     * @param d detector
     * @param r ring 
     */
    RingHistos(UShort_t d, Char_t r);
    /** 
     * Copy constructor 
     * 
     * @param o Object to copy from 
     */
    RingHistos(const RingHistos& o);
    /** 
     * Assignment operator 
     * 
     * @param o Object to assign from 
     * 
     * @return Reference to this 
     */
    RingHistos& operator=(const RingHistos& o);
    /** 
     * Destructor 
     */
    ~RingHistos();
    /** 
     * Make output 
     * 
     * @param dir Where to put it 
     */
    void Output(TList* dir);
    /** 
     * Scale the histograms to the total number of events 
     * 
     * @param dir     Where the output is 
     * @param nEvents Number of events 
     */
    void ScaleHistograms(TList* dir, Int_t nEvents);
    TH2D*     fEvsN;         // Correlation of Eloss vs uncorrected Nch
    TH2D*     fEvsM;         // Correlation of Eloss vs corrected Nch
    TProfile* fEtaVsN;       // Average uncorrected Nch vs eta
    TProfile* fEtaVsM;       // Average corrected Nch vs eta
    TProfile* fCorr;         // Average correction vs eta
    TH2D*     fDensity;      // Distribution inclusive Nch
    ClassDef(RingHistos,1);
  };
  /** 
   * Get the ring histogram container 
   * 
   * @param d Detector
   * @param r Ring 
   * 
   * @return Ring histogram container 
   */
  RingHistos* GetRingHistos(UShort_t d, Char_t r) const;

  TList    fRingHistos;    //  List of histogram containers
  Double_t fMultCut;       //  Low cut on scaled energy loss
  TH1D*    fSumOfWeights;  //  Histogram
  TH1D*    fWeightedSum;   //  Histogram
  TH1D*    fCorrections;   //  Histogram
  UShort_t fMaxParticles;  //  Maximum particle weight to use 
  TH1D*    fAccI;          //  Acceptance correction for inner rings
  TH1D*    fAccO;          //  Acceptance correction for outer rings
  TArrayI  fFMD1iMax;      //  Array of max weights 
  TArrayI  fFMD2iMax;      //  Array of max weights 
  TArrayI  fFMD2oMax;      //  Array of max weights 
  TArrayI  fFMD3iMax;      //  Array of max weights 
  TArrayI  fFMD3oMax;      //  Array of max weights 
  Int_t    fDebug;         //  Debug level 

  ClassDef(AliFMDDensityCalculator,1); // Calculate Nch density 
};

#endif
// Local Variables:
//   mode: C++
// End:

