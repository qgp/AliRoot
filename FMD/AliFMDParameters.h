#ifndef ALIFMDPARAMETERS_H
#define ALIFMDPARAMETERS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * Latest changes by Christian Holm Christensen <cholm@nbi.dk>
 *
 * See cxx source for full Copyright notice                               
 */
//____________________________________________________________________
//
//  Singleton class to handle various parameters (not geometry) of the
//  FMD
//  Should get ata fromm Conditions DB.
//
/** @file    AliFMDParameters.h
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Mon Mar 27 12:44:43 2006
    @brief   Manager of FMD parameters
*/
#ifndef ROOT_TNamed
# include <TNamed.h>
#endif
#ifndef ROOT_TArrayI
# include <TArrayI.h>
#endif
#ifndef ALIFMDUSHORTMAP_H
# include <AliFMDUShortMap.h>
#endif
#ifndef ALIFMDBOOLMAP_H
# include <AliFMDBoolMap.h>
#endif
typedef AliFMDUShortMap AliFMDCalibZeroSuppression;
typedef AliFMDBoolMap   AliFMDCalibDeadMap;
class AliFMDCalibPedestal;
class AliFMDCalibGain;
class AliFMDCalibSampleRate;
class AliFMDCalibStripRange;
class AliFMDAltroMapping;
class AliCDBEntry;
class AliFMDPreprocessor;

//____________________________________________________________________
//
//  Singleton class to handle various parameters (not geometry) of the
//  FMD
//  Should get ata fromm Conditions DB.
//

/** @brief This class is a singleton that handles various parameters
    of the FMD detectors.  
    This class reads from the Conditions DB to get the various
    parameters, which code can then request from here. In that way,
    all code uses the same data, and the interface is consistent.
     
    Some of the parameter managed are 
    - @c fPedestal, @c fPedestalWidth
      Mean and width of the pedestal.  The pedestal is simulated
      by a Guassian, but derived classes my override MakePedestal
      to simulate it differently (or pick it up from a database).
    - @c fVA1MipRange
      The dymamic MIP range of the VA1_ALICE pre-amplifier chip 
    - @c fAltroChannelSize
      The largest number plus one that can be stored in one
      channel in one time step in the ALTRO ADC chip. 
    - @c fSampleRate
      How many times the ALTRO ADC chip samples the VA1_ALICE
      pre-amplifier signal.   The VA1_ALICE chip is read-out at
      10MHz, while it's possible to drive the ALTRO chip at
      25MHz.  That means, that the ALTRO chip can have time to
      sample each VA1_ALICE signal up to 2 times.  Although it's
      not certain this feature will be used in the production,
      we'd like have the option, and so it should be reflected in
      the code.

    @ingroup FMD_base
*/
class AliFMDParameters : public TNamed
{
public:
  /** Enumeration of things to initialize */ 
  enum What { 
    /** Pulser gain */ 
    kPulseGain = 0x1, // Pulser gain 
    /** Pedestals and noise */ 
    kPedestal = 0x2, // Pedestal and noise 
    /** Dead channel map */ 
    kDeadMap = 0x4, // Dead channel map
    /**  Over sampling rate */ 
    kSampleRate = 0x8, // Over sampling rate 
    /** Zero suppression parameters */ 
    kZeroSuppression = 0x10, // Zero suppression parameters 
    /** ALTRO data map */ 
    kAltroMap = 0x20, // Altro channel map
    /** Strip Range */
    kStripRange = 0x40 //Strip range
    
  };
  
  /** Singleton access
      @return  single to */
  static AliFMDParameters* Instance();

  /** Initialize the manager.  This tries to read the parameters from
      CDB.  If that fails, the class uses the hard-coded parameters. 
   */
  void Init(Bool_t forceReInit=kFALSE, 
	    UInt_t what = (kPulseGain|kPedestal|kDeadMap|kSampleRate|
			   kZeroSuppression|kAltroMap|kStripRange));
  /** Initialize the manager.  This tries to read the parameters from
      CDB.  If that fails, the class uses the hard-coded parameters. 
   */
  void Init(AliFMDPreprocessor* pp, 
	    Bool_t forceReInit=kFALSE, 
	    UInt_t what = (kPulseGain|kPedestal|kDeadMap|kSampleRate|
			   kZeroSuppression|kAltroMap|kStripRange));
  /** Print all parameters. 
      @param option Option string */
  void Print(Option_t* option="A") const;
  /** Draw parameters. 
      @param option What to draw. Should be one of 
      - dead	  Dead channels
      - threshold Threshold
      - gain	  Gain
      - pedestal  Pedestal
      - noise	  Noise (or pedestal width)
      - zero	  Zero suppression
      - rate	  Sampling rate (VA1 clock / ALTRO clock)
      - min	  Minimum strip read out
      - max 	  Maximum strip read out
      - map	  hardware address
  */
  void Draw(Option_t* option="pedestal");
  
  /** @{ */
  /** @name Set various `Fixed' parameters */
  /** @param r How many MIP signals we can fit in the VA1
      pre-amps. (default and design is 20) */
  void SetVA1MipRange(UShort_t r=20)          { fVA1MipRange = r; }
  /** @param s Maximum number of the ADC (ALTRO).  This is a 10 bit
      ADC so, the maximum number is 1024 */
  void SetAltroChannelSize(UShort_t s=1024)   { fAltroChannelSize = s;}
  /** @param size The number of strips multiplexed into one ALTRO
      channel. That is, how many strips is connected to one VA1
      pre-amp. */
  void SetChannelsPerAltro(UShort_t size=128) { fChannelsPerAltro = size; }
  /** @param f Factor to use for accepting a signal. */
  void SetPedestalFactor(Float_t f=3)         { fPedestalFactor = f; }
  /** @param n Number of pre-samples to keep during zero-suppression -
      only used in simulation. */
  void SetZSPreSamples(UShort_t n=1) { fZSPre = (n & 0x3); }
  /** @param n Number of post-samples to keep during zero-suppression -
      only used in simulation. */
  void SetZSPostSamples(UShort_t n=1) { fZSPost = (n & 0x3); }
  /** @param use If true, do pedestal subtraction before zero
      suppression - only used in simulation */
  void SetZSPedSubtract(Bool_t use=kTRUE) { fZSPedSubtract = use; }
  /** @} */

  /** @{ */
  /** @name Set various variable parameter defaults */
  /** @param s Zero suppression threshold in ADC counts */
  void SetZeroSuppression(UShort_t s=0)       { fFixedZeroSuppression = s; }
  /** @param r How many times we oversample each strip. */
  void SetSampleRate(UShort_t r=1)            { fFixedSampleRate = r ;}//(r>2?2:r);}
  void SetSampleRate(AliFMDCalibSampleRate* r) { fSampleRate = r; }
  /** @param p Pedestal value in ADC counts */
  void SetPedestal(Float_t p=10)              { fFixedPedestal = p; }
  /** @param w Pedestal width in ADC counts */
  void SetPedestalWidth(Float_t w=1)          { fFixedPedestalWidth = w; }
  /** @param t Threshold used for 1 MIP acceptance. */
  void SetThreshold(Float_t t=0)              { fFixedThreshold = t; }
  /** Range of strips read out 
      @param min Minimum strip number (0-127). 
      @param max Maximum strip number (0-127). */
  void SetStripRange(UShort_t min=0, UShort_t max=127);
  void SetStripRange(AliFMDCalibStripRange* r) { fStripRange = r; }
  /** Whether raw data has full common data header (8 32bit words) or
      the older invalid format (7 32bit words with bogus entries)
      @param yes if true the raw data has complete data header */ 
  void UseCompleteHeader(Bool_t yes=kTRUE) { fHasCompleteHeader = yes; } 
  /** @} */

  /** @{ */
  /** @name Get `Fixed' various parameters */
  /** @return Number of MIP signals that fit inside a VA1 channel  */
  UShort_t GetVA1MipRange()          const { return fVA1MipRange; }
  /** @return The maximum count in the ADC */
  UShort_t GetAltroChannelSize()     const { return fAltroChannelSize; }
  /** @return Number of strips muliplexed into one ADC channel */
  UShort_t GetChannelsPerAltro()     const { return fChannelsPerAltro; }
  /** @return The average energy deposited by one MIP */
  Float_t  GetEdepMip()              const;
  /** @return The factor used of signal acceptance */
  Float_t  GetPedestalFactor()	     const { return fPedestalFactor; }
  /** @param n Number of pre-samples to keep during zero-suppression -
      only used in simulation. */
  UShort_t GetZSPreSamples() const { return fZSPre; }
  /** @param n Number of post-samples to keep during zero-suppression -
      only used in simulation. */
  UShort_t GetZSPostSamples() const { return fZSPost; }
  /** @param use If true, do pedestal subtraction before zero
      suppression - only used in simulation */
  Bool_t IsZSPedSubtract() const { return fZSPedSubtract; }
  /** @} */

  /** @{ */
  /** @name Various varible conditions */
  /** Whether the strip is considered dead
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return @c true if the strip is considered dead, @c false if
      it's OK. */
  Bool_t   IsDead(UShort_t detector, 
		  Char_t ring, 
		  UShort_t sector, 
		  UShort_t strip) const;
  Float_t  GetThreshold() const;
  /** Gain of pre-amp. 
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return Gain of pre-amp.  */
  Float_t  GetPulseGain(UShort_t detector, 
			Char_t ring, 
			UShort_t sector, 
			UShort_t strip) const;
  /** Get mean of pedestal
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return Mean of pedestal */
  Float_t  GetPedestal(UShort_t detector, 
		       Char_t ring, 
		       UShort_t sector, 
		       UShort_t strip) const;
  /** Width of pedestal
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return Width of pedestal */
  Float_t  GetPedestalWidth(UShort_t detector, 
			    Char_t ring, 
			    UShort_t sector, 
			    UShort_t strip) const;
  /** zero suppression threshold (in ADC counts)
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return zero suppression threshold (in ADC counts) */
  UShort_t GetZeroSuppression(UShort_t detector, 
			      Char_t ring, 
			      UShort_t sector, 
			      UShort_t strip) const;
  /** Get the sampling rate
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return The sampling rate */
  UShort_t GetSampleRate(UShort_t detector, 
			 Char_t ring, 
			 UShort_t sector, 
			 UShort_t strip) const;
  /** Get the minimum strip in the read-out range
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return Minimum strip */
  UShort_t GetMinStrip(UShort_t detector, 
		       Char_t ring, 
		       UShort_t sector, 
		       UShort_t strip) const;
  /** Get the maximum strip in the read-out range
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return Maximum strip */
  UShort_t GetMaxStrip(UShort_t detector, 
		       Char_t ring, 
		       UShort_t sector, 
		       UShort_t strip) const;
  /** Get the number of pre-samples in ALTRO channels
      @param detector Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sector   Sector number (0-39)
      @param strip    Strip number (0-511)
      @return Maximum strip */
  UShort_t GetPreSamples(UShort_t, 
			 Char_t, 
			 UShort_t, 
			 UShort_t) const { return 14+5; }
  /** @} */
  
  /** @{ 
      @name Hardware to detector translation (and inverse) */
  /** Map a hardware address into a detector index. 
      @param ddl        Hardware DDL number 
      @param board      FEC number
      @param altro      ALTRO number 
      @param channel    Channel number 
      @param timebin    Timebin 
      @param det        On return, the detector #
      @param ring       On return, the ring ID
      @param sec        On return, the sector #
      @param str        On return, the base of strip #
      @param sam        On return, the sample number for this strip
      @return @c true on success, false otherwise */
  Bool_t Hardware2Detector(UInt_t    ddl,        UInt_t    board, 
			   UInt_t    altro,      UInt_t    chan,
			   UShort_t  timebin,   
			   UShort_t& det,        Char_t&   ring, 
			   UShort_t& sec,        Short_t& str,
			   UShort_t& sam) const;
  /** Translate hardware address to detector coordinates 
      @param ddl      DDL number 
      @param board    Board address
      @param chip     Chip #
      @param channel  Channel #
      @param det      On return, Detector # (1-3)
      @param ring     On return, Ring ID ('I' or 'O')
      @param sec      On return, Sector number (0-39)
      @param str      On return, Strip number (0-511)
      @return @c true on success. */
  Bool_t   Hardware2Detector(UInt_t ddl,    UInt_t    board, 
			     UInt_t chip,   UInt_t    channel, 
			     UShort_t& det, Char_t&   ring, 
			     UShort_t& sec, Short_t& str) const;
  /** Map a hardware address into a detector index. 
      @param ddl        Hardware DDL number 
      @param hwaddr     Hardware address.  
      @param timebin    Timebin 
      @param det        On return, the detector #
      @param ring       On return, the ring ID
      @param sec        On return, the sector #
      @param str        On return, the base of strip #
      @param sam        On return, the sample number for this strip
      @return @c true on success, false otherwise */
  Bool_t Hardware2Detector(UInt_t    ddl,        UInt_t    hwaddr, 
			   UShort_t  timebin,    
			   UShort_t& det,        Char_t&   ring, 
			   UShort_t& sec,        Short_t& str,
			   UShort_t& sam) const;
  /** Translate hardware address to detector coordinates 
      @param ddl      DDL number 
      @param addr     Hardware address
      @param det      On return, Detector # (1-3)
      @param ring     On return, Ring ID ('I' or 'O')
      @param sec      On return, Sector number (0-39)
      @param str      On return, Strip number (0-511)
      @return @c true on success. */
  Bool_t   Hardware2Detector(UInt_t ddl, UInt_t addr, UShort_t& det,
			     Char_t& ring, UShort_t& sec, Short_t& str) const;

  /** Map a detector index into a hardware address. 
      @param det         The detector #
      @param ring        The ring ID
      @param sec         The sector #
      @param str         The strip #
      @param sam         The sample number 
      @param ddl         On return, hardware DDL number 
      @param board       On return, the FEC board address (local to DDL)
      @param altro       On return, the ALTRO number (local to FEC)
      @param channel     On return, the channel number (local to ALTRO)
      @param timebin     On return, the timebin number (local to ALTRO)
      @return @c true on success, false otherwise */
  Bool_t Detector2Hardware(UShort_t  det,        Char_t    ring, 
			   UShort_t  sec,        UShort_t  str,
			   UShort_t  sam, 
			   UInt_t&   ddl,        UInt_t&   board, 
			   UInt_t&   altro,      UInt_t&   channel, 
			   UShort_t& timebin) const;
  /** Translate detector coordinates to hardware address 
      @param det      Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sec      Sector number (0-39)
      @param str      Strip number (0-511)
      @param ddl      On return, DDL number 
      @param board    On return, Board address
      @param chip     On return, Chip #
      @param channel  On return, Channel #
      @return @c true on success. */
  Bool_t   Detector2Hardware(UShort_t det, Char_t ring, 
			     UShort_t sec, UShort_t str, 
			     UInt_t& ddl,  UInt_t& board,
			     UInt_t& chip, UInt_t& channel) const;
  /** Map a detector index into a hardware address. 
      @param det         The detector #
      @param ring        The ring ID
      @param sec         The sector #
      @param str         The strip #
      @param sam         The sample number 
      @param ddl         On return, hardware DDL number 
      @param hwaddr      On return, hardware address.  
      @param timebin     On return, the timebin number (local to ALTRO)
      @return @c true on success, false otherwise */
  Bool_t Detector2Hardware(UShort_t  det,        Char_t    ring, 
			   UShort_t  sec,        UShort_t  str,
			   UShort_t  sam, 
			   UInt_t&   ddl,        UInt_t&   hwaddr, 
			   UShort_t& timebin) const;
  /** Translate detector coordinates to hardware address 
      @param det      Detector # (1-3)
      @param ring     Ring ID ('I' or 'O')
      @param sec      Sector number (0-39)
      @param str      Strip number (0-511)
      @param ddl      On return, DDL number 
      @param addr     On return, Hardware address
      @return @c true on success. */
  Bool_t   Detector2Hardware(UShort_t det, Char_t ring, UShort_t sec, 
			     UShort_t str, UInt_t& ddl, UInt_t& addr) const;
  /** Get the map that translates hardware to detector coordinates 
      @return Get the map that translates hardware to detector
      coordinates */ 
  AliFMDAltroMapping* GetAltroMap() const;
  /** Whether raw data has full common data header (8 32bit words) or
      the older invalid format (7 32bit words with bogus entries)
      @return false if the raw data has incomplete data header */ 
  Bool_t HasCompleteHeader() const { return fHasCompleteHeader; } 

  /** @} */

  static const char* PulseGainPath()       { return fgkPulseGain; }
  static const char* PedestalPath()        { return fgkPedestal; }
  static const char* DeadPath()            { return fgkDead; }
  static const char* SampleRatePath()      { return fgkSampleRate; }
  static const char* AltroMapPath()        { return fgkAltroMap; }
  static const char* ZeroSuppressionPath() { return fgkZeroSuppression; }
  static const char* StripRangePath()      { return fgkStripRange; }
  static const char* GetPedestalShuttleID()   {return fkPedestalShuttleID;}
  static const char* GetGainShuttleID()       {return fkGainShuttleID;}
  static const char* GetConditionsShuttleID()   {return fkConditionsShuttleID;}
protected:
  /** CTOR  */
  AliFMDParameters();
  /** CTOR  */
  AliFMDParameters(const AliFMDParameters& o) 
    : TNamed(o), 
      fIsInit(o.fIsInit),
      fkSiDeDxMip(o.fkSiDeDxMip),
      fVA1MipRange(o.fVA1MipRange),
      fAltroChannelSize(o.fAltroChannelSize),
      fChannelsPerAltro(o.fChannelsPerAltro),
      fPedestalFactor(o.fPedestalFactor),
      fZSPre(o.fZSPre),
      fZSPost(o.fZSPost),
      fZSPedSubtract(o.fZSPedSubtract),
      fFixedPedestal(o.fFixedPedestal),
      fFixedPedestalWidth(o.fFixedPedestalWidth),
      fFixedZeroSuppression(o.fFixedZeroSuppression),
      fFixedSampleRate(o.fFixedSampleRate),
      fFixedThreshold(o.fFixedThreshold),
      fFixedMinStrip(o.fFixedMinStrip),
      fFixedMaxStrip(o.fFixedMaxStrip),
      fFixedPulseGain(o.fFixedPulseGain),
      fEdepMip(o.fEdepMip),
      fHasCompleteHeader(o.fHasCompleteHeader),
      fZeroSuppression(o.fZeroSuppression),
      fSampleRate(o.fSampleRate),
      fPedestal(o.fPedestal),
      fPulseGain(o.fPulseGain),
      fDeadMap(o.fDeadMap),
      fAltroMap(o.fAltroMap),
      fStripRange(o.fStripRange)
  {}
  /** Assignement operator 
      @return Reference to this */
  AliFMDParameters& operator=(const AliFMDParameters&) { return *this; }
  /** DTOR */
  virtual ~AliFMDParameters() {}
  /** Singleton instance  */
  static AliFMDParameters* fgInstance;   // Static singleton instance
  /** Get an entry from either global AliCDBManager or passed
      AliFMDPreprocessor. 
      @param path  Path to CDB object. 
      @param pp    AliFMDPreprocessor 
      @param fatal If true, raise a fatal flag if we didn't get the entry.
      @return AliCDBEntry if found */ 
  AliCDBEntry* GetEntry(const char* path, AliFMDPreprocessor* pp, 
			Bool_t fatal=kTRUE) const;
  /** Initialize gains.  Try to get them from CDB */
  void InitPulseGain(AliFMDPreprocessor* pp=0);
  /** Initialize pedestals.  Try to get them from CDB */
  void InitPedestal(AliFMDPreprocessor* pp=0);
  /** Initialize dead map.  Try to get it from CDB */
  void InitDeadMap(AliFMDPreprocessor* pp=0);
  /** Initialize sample rates.  Try to get them from CDB */
  void InitSampleRate(AliFMDPreprocessor* pp=0);
  /** Initialize zero suppression thresholds.  Try to get them from CDB */
  void InitZeroSuppression(AliFMDPreprocessor* pp=0);
  /** Initialize hardware map.  Try to get it from CDB */
  void InitAltroMap(AliFMDPreprocessor* pp=0);
  /** Initialize strip range.  Try to get it from CDB */
  void InitStripRange(AliFMDPreprocessor* pp=0);

  Bool_t          fIsInit;                   // Whether we've been initialised  

  static const char* fgkPulseGain;	     // Path to PulseGain calib object
  static const char* fgkPedestal;	     // Path to Pedestal calib object
  static const char* fgkDead;	             // Path to Dead calib object
  static const char* fgkSampleRate;	     // Path to SampleRate calib object
  static const char* fgkAltroMap;	     // Path to AltroMap calib object
  static const char* fgkZeroSuppression;     // Path to ZeroSuppression cal object
  static const char* fgkStripRange;          // Path to strip range cal object
  const Float_t   fkSiDeDxMip;               // MIP dE/dx in Silicon
  UShort_t        fVA1MipRange;              // # MIPs the pre-amp can do    
  UShort_t        fAltroChannelSize;         // Largest # to store in 1 ADC ch.
  UShort_t        fChannelsPerAltro;         // Number of pre-amp. chan/adc chan.
  Float_t         fPedestalFactor;           // Number of pedestal widths
  UShort_t        fZSPre;                    // Number of pre-samples in ZS
  UShort_t        fZSPost;                   // Number of post-samples in ZS
  Bool_t          fZSPedSubtract;            // Pedestal subtraction before ZS

  Float_t         fFixedPedestal;            // Pedestal to subtract
  Float_t         fFixedPedestalWidth;       // Width of pedestal
  UShort_t        fFixedZeroSuppression;     // Threshold for zero-suppression
  UShort_t        fFixedSampleRate;          // Times the ALTRO samples pre-amp.
  Float_t         fFixedThreshold;           // Threshold in ADC counts
  UShort_t        fFixedMinStrip;            // Minimum strip read-out
  UShort_t        fFixedMaxStrip;            // Maximum strip read-out 
  mutable Float_t fFixedPulseGain;           //! Gain (cached)
  mutable Float_t fEdepMip;                  //! Cache of energy loss for a MIP
  Bool_t          fHasCompleteHeader;        // raw data has incomplete data header
  
  static const char* fkPedestalShuttleID;    // Shuttle/preprocessor ID for pedestals
  static const char* fkGainShuttleID;        // Shuttle/preprocessor ID for gains
  static const char* fkConditionsShuttleID;  // Shuttle/preprocessor ID for conditions
  
  AliFMDCalibZeroSuppression* fZeroSuppression; // Zero suppression from CDB
  AliFMDCalibSampleRate*      fSampleRate;      // Sample rate from CDB 
  AliFMDCalibPedestal*        fPedestal;        // Pedestals 
  AliFMDCalibGain*            fPulseGain;       // Pulser gain
  AliFMDCalibDeadMap*         fDeadMap;         // Pulser gain
  AliFMDAltroMapping*         fAltroMap;        // Map of hardware
  AliFMDCalibStripRange*      fStripRange;      // Strip range
  
  ClassDef(AliFMDParameters,6) // Manager of parameters
};

#endif
//____________________________________________________________________
//
// Local Variables:
//   mode: C++
// End:
//
// EOF
//

