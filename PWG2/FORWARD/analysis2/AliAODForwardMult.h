//
// See implementation or Doxygen comments for more information
//
#ifndef ALIAODFORWARDMULT_H
#define ALIAODFORWARDMULT_H
#include <TObject.h>
#include <TH2D.h>
class TBrowser;
class TH1I;
/**
 * Class that contains the forward multiplicity data per event 
 *
 * This class contains a histogram of 
 * @f[
 *   \frac{d^2N_{ch}}{d\eta d\phi}\quad,
 * @f]
 * as well as a trigger mask for each analysed event.  
 * 
 * The eta acceptance of the event is stored in the underflow bins of
 * the histogram.  So to build the final histogram, one needs to
 * correct for this acceptance (properly weighted by the events), and
 * the vertex efficiency.  This simply boils down to defining a 2D
 * histogram and summing the event histograms in that histogram.  One
 * should of course also do proper book-keeping of the accepted event.
 *
 * @code 
 * TTree* GetAODTree()
 * { 
 *    TFile* file = TFile::Open("AliAODs.root","READ");
 *    TTree* tree = static_cast<TTree*>(file->Get("aodTree"));
 *    return tree;
 * }
 * 
 * void Analyse()
 * { 
 *   TH2D*              sum        = 0;                  // Summed hist
 *   TTree*             tree       = GetAODTree();       // AOD tree
 *   AliAODForwardMult* mult       = 0;                  // AOD object
 *   Int_t              nTriggered = 0;                  // # of triggered ev.
 *   Int_t              nWithVertex= 0;                  // # of ev. w/vertex
 *   Int_t              nAccepted  = 0;                  // # of ev. used
 *   Int_t              nAvailable = tree->GetEntries(); // How many entries
 *   Float_t            vzLow      = -10;                // Lower ip cut
 *   Float_t            vzHigh     =  10;                // Upper ip cut
 *   Int_t              mask       = AliAODForwardMult::kInel;// Trigger mask
 *   tree->SetBranchAddress("forward", &forward);        // Set the address
 * 
 *   for (int i = 0; i < nAvailable; i++) { 
 *     // Create sum histogram on first event - to match binning to input
 *     if (!sum) sum = static_cast<TH2D*>(mult->Clone("d2ndetadphi"));
 * 
 *     tree->GetEntry(i);
 * 
 *     // Other trigger/event requirements could be defined 
 *     if (!mult->IsTriggerBits(mask)) continue; 
 *     nTriggered++;
 *
 *     // Check if we have vertex 
 *     if (!mult->HasIpZ()) continue;
 *     nWithVertex++;
 * 
 *     // Select vertex range (in centimeters) 
 *     if (!mult->InRange(vzLow, vzHigh) continue; 
 *     nAccepted++;
 * 
 *     // Add contribution from this event
 *     sum->Add(&(mult->GetHistogram()));
 *   }
 * 
 *   // Get acceptance normalisation from underflow bins 
 *   TH1D* norm   = sum->Projection("norm", 0, 1, "");
 *   // Project onto eta axis - _ignoring_underflow_bins_!
 *   TH1D* dndeta = sum->Projection("dndeta", 1, -1, "e");
 *   // Normalize to the acceptance 
 *   dndeta->Divide(norm);
 *   // Scale by the vertex efficiency 
 *   dndeta->Scale(Double_t(nWithVertex)/nTriggered, "width");
 *   // And draw the result
 *   dndeta->Draw();
 * }
 * @endcode   
 *     
 * The above code will draw the final @f$ dN_{ch}/d\eta@f$ for the
 * selected event class and vertex range
 *
 * The histogram can be used as input for other kinds of analysis too, 
 * like flow, event-plane, centrality, and so on. 
 *
 * @ingroup pwg2_forward 
 */
class AliAODForwardMult : public TObject
{
public:
  /** 
   * Bits of the trigger pattern
   */
  enum { 
    /** In-elastic collision */
    kInel     = 0x001, 
    /** In-elastic collision with at least one SPD tracklet */
    kInelGt0  = 0x002, 
    /** Non-single diffractive collision */
    kNSD      = 0x004, 
    /** Empty bunch crossing */
    kEmpty    = 0x008, 
    /** A-side trigger */
    kA        = 0x010, 
    /** B(arrel) trigger */
    kB        = 0x020, 
    /** C-side trigger */
    kC        = 0x080,  
    /** Empty trigger */
    kE        = 0x100,
    /** pileup from SPD */
    kPileUp   = 0x200,    
    /** true NSD from MC */
    kMCNSD    = 0x400    
    
  };
  /** 
   * Bin numbers in trigger histograms 
   */
  enum { 
    kBinAll=1,
    kBinInel, 
    kBinInelGt0, 
    kBinNSD, 
    kBinA, 
    kBinB, 
    kBinC, 
    kBinE,
    kBinPileUp, 
    kBinMCNSD,
    kWithTrigger, 
    kWithVertex, 
    kAccepted
  };
  /** 
   * Default constructor 
   * 
   * Used by ROOT I/O sub-system - do not use
   */
  AliAODForwardMult();
  /** 
   * Constructor 
   * 
   * @param isMC Whether this was from MC or not 
   */
  AliAODForwardMult(Bool_t isMC);
  /** 
   * Destructor 
   */
  virtual ~AliAODForwardMult() {} // Destructor 
  /** 
   * Initialize 
   * 
   * @param etaAxis  Pseudo-rapidity axis
   */
  void Init(const TAxis& etaAxis);
  /** 
   * Get the @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   *
   * @return @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   */  
  const TH2D& GetHistogram() const { return fHist; } // Get histogram 
  /** 
   * Get the @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   *
   * @return @f$ d^2N_{ch}/d\eta d\phi@f$ histogram, 
   */  
  TH2D& GetHistogram() { return fHist; } // Get histogram 
  /** 
   * Get the trigger mask 
   * 
   * @return Trigger mask 
   */
  UInt_t GetTriggerMask() const { return fTriggers; } // Get triggers
  /** 
   * Set the trigger mask 
   * 
   * @param trg Trigger mask
   */
  void SetTriggerMask(UInt_t trg) { fTriggers = trg; } // Set triggers 
  /** 
   * Set bit(s) in trigger mask 
   * 
   * @param bits bit(s) to set 
   */
  void SetTriggerBits(UInt_t bits) { fTriggers |= bits; } // Set trigger bits
  /** 
   * Check if bit(s) are set in the trigger mask 
   * 
   * @param bits Bits to test for 
   * 
   * @return 
   */
  Bool_t IsTriggerBits(UInt_t bits) const;
  /** 
   * Whether we have any trigger bits 
   */
  Bool_t HasTrigger() const { return fTriggers != 0; } // Check for triggers
  /** 
   * Clear all data 
   * 
   * @param option  Passed on to TH2::Reset verbatim
   */
  void Clear(Option_t* option="");
  /** 
   * browse this object 
   * 
   * @param b Browser 
   */
  void Browse(TBrowser* b);
  /** 
   * This is a folder 
   * 
   * @return Always true
   */
  Bool_t IsFolder() const { return kTRUE; } // Always true 
  /** 
   * Print content 
   * 
   * @param option Passed verbatim to TH2::Print 
   */
  void Print(Option_t* option="") const;
  /** 
   * Set the z coordinate of the interaction point
   * 
   * @param ipZ Interaction point z coordinate
   */
  void SetIpZ(Float_t ipZ) { fIpZ = ipZ; } // Set Ip's Z coordinate
  /** 
   * Set the center of mass energy per nucleon-pair.  This is stored 
   * in the (0,0) of the histogram 
   * 
   * @param sNN Center of mass energy per nucleon pair (GeV)
   */
  void SetSNN(UShort_t sNN); 
  /** 
   * Get the collision system number
   * - 0: Unknown 
   * - 1: pp
   * - 2: PbPb
   * 
   * @param sys Collision system number
   */
  void SetSystem(UShort_t sys);
  /** 
   * Set the event centrality 
   * 
   * @param c Centrality 
   */
  void SetCentrality(Float_t c) { fCentrality = c; }
  /** 
   * Set the z coordinate of the interaction point
   * 
   * @return Interaction point z coordinate
   */
  Float_t GetIpZ() const { return fIpZ; } // Get Ip's Z coordinate 
  /** 
   * Check if we have a valid z coordinate of the interaction point
   *
   * @return True if we have a valid interaction point z coordinate
   */
  Bool_t HasIpZ() const;
  /** 
   * Get the center of mass energy per nucleon pair (GeV)
   * 
   * @return Center of mass energy per nucleon pair (GeV)
   */
  UShort_t GetSNN() const;
  /** 
   * Get the collision system number
   * - 0: Unknown 
   * - 1: pp
   * - 2: PbPb
   * 
   * @return Collision system number
   */
  UShort_t GetSystem() const;
  /** 
   * Check if the z coordinate of the interaction point is within the
   * given limits.  Note that the convention used corresponds to the
   * convention used in ROOTs TAxis.
   * 
   * @param low  Lower cut (inclusive)
   * @param high Upper cut (exclusive)
   * 
   * @return true if @f$ low \ge ipz < high@f$ 
   */
  Bool_t InRange(Float_t low, Float_t high) const;
  /** 
   * Get the event centrality 
   * 
   * 
   * @return 
   */
  Float_t GetCentrality() const { return fCentrality; }
  /** 
   * Check if we have a valid centrality 
   * 
   * 
   * @return 
   */
  Bool_t  HasCentrality() const { return !(fCentrality  < 0); }
  
  /** 
   * Get the name of the object 
   * 
   * @return Name of object 
   */
  const Char_t* GetName() const { return (fIsMC ? "ForwardMC" : "Forward"); }
  /** 
   * Check if event meets the passses requirements 
   * 
   * @param triggerMask  Trigger mask
   * @param vzMin        Minimum @f$ v_z@f$ (in centimeters)
   * @param vzMax        Maximum @f$ v_z@f$ (in centimeters) 
   * @param cMin         Minimum centrality (in percent)
   * @param cMax         Maximum centrality (in percent)
   * @param hist         Histogram to fill 
   * 
   * @return @c true if the event meets the requirements 
   */
  Bool_t CheckEvent(Int_t    triggerMask=kInel,
		    Double_t vzMin=-10, Double_t vzMax=10,
		    UShort_t cMin=0,    UShort_t cMax=100, 
		    TH1*     hist=0) const;
  /** 
   * Get a string correspondig to the trigger mask
   * 
   * @param mask Trigger mask 
   * 
   * @return Static string (copy before use)
   */
  static const Char_t* GetTriggerString(UInt_t mask);
  /** 
   * Make a histogram to record triggers in 
   * 
   * @param name Name of the histogram 
   * 
   * @return Newly allocated histogram 
   */
  static TH1I* MakeTriggerHistogram(const char* name);
protected: 
  Bool_t  fIsMC;     // Whether this is from MC 
  TH2D    fHist;     // Histogram of d^2N_{ch}/(deta dphi) for this event
  UInt_t  fTriggers; // Trigger bit mask 
  Float_t fIpZ;      // Z coordinate of the interaction point
  Float_t fCentrality; // Event centrality 

  static const Float_t fgkInvalidIpZ; // Invalid IpZ value 
  ClassDef(AliAODForwardMult,2); // AOD forward multiplicity 
};

//____________________________________________________________________
inline Bool_t
AliAODForwardMult::InRange(Float_t low, Float_t high) const 
{
  return HasIpZ() && fIpZ >= low && fIpZ < high;
}

//____________________________________________________________________
inline Bool_t 
AliAODForwardMult::IsTriggerBits(UInt_t bits) const 
{ 
  return HasTrigger() && ((fTriggers & bits) != 0); 
}

#endif
// Local Variables:
//  mode: C++
// End:

