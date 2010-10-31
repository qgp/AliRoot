// -*- Mode: C++ -*-
// @(#) $Id$

#ifndef ALIHLTTPCPAD_H
#define ALIHLTTPCPAD_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTTPCPad.h
    @author Matthias Richter
    @date   
    @brief  Container Class for TPC Pads.
*/

#include "AliHLTLogging.h"
#include "AliHLTTPCClusters.h"
#include "AliHLTTPCDigitReader.h"
#include "TH1F.h"
#include <vector>

class AliHLTTPCDigitData;

typedef Int_t AliHLTTPCSignal_t;


/**
 * @class AliHLTTPCPad
 * The class is a container for the raw ADC data of one TPCS pad. In order to
 * avoid multiple unpacking/handling of the incoming raw data, the class holds
 * a copy of the incoming data for one event. The copy is released when the
 * end of the event was announced.
 * The class calculates the base line of a TPC channel. Currently, only the
 * average value is calculated, but further extension will include channel by
 * channel histograming. The baseline history is kept and used for baseline
 * re-evaluation and correction of subsequent events.
 * @ingroup alihlt_tpc
 */
class AliHLTTPCPad : public AliHLTLogging {
public:
  /** standard constructor */
  AliHLTTPCPad();

  /** 
   * Constructor
   * @param dummy  dummy determines if one need the raw data. If a dummy number is given, the pad object will be initialized without using memory for raw data
   */
  AliHLTTPCPad(Int_t dummy);

  /** 
   * Constructor
   * @param offset   The number of bins to ignore at the beginning
   *                 of the channels
   * @param nofBins  The total number of bins for one channel
   */
  AliHLTTPCPad(Int_t offset, Int_t nofBins);

  /** standard destructor */
  virtual ~AliHLTTPCPad();

  
  struct AliClusterData
  {
    UInt_t fTotalCharge;   //tot charge of cluster
    UInt_t fPad;           //pad value
    UInt_t fTime;          //time value
    ULong64_t fPad2;       //for error in XY direction
    ULong64_t fTime2;      //for error in Z  direction
    UInt_t fMean;          //mean in time
    UInt_t fFlags;         //different flags
    UInt_t fChargeFalling; //for deconvolution
    UInt_t fLastCharge;    //for deconvolution
    UInt_t fLastMergedPad; //dont merge twice per pad
  }; 

  typedef struct AliClusterData AliClusterData; //!


  /**
   * Set the address the pad.
   * The address consists of row and pad number.
   * @param rowno    The row number
   * @param padno    The pad number.
   */
  Int_t SetID(Int_t rowno, Int_t padno);

  /**
   * Get the row number.
   * @return Row number
   */
  Int_t GetRowNumber() const {return fRowNo;}

  /**
   * Get the pad number on the row.
   * @return Pad number on row
   */
  Int_t GetPadNumber() const {return fPadNo;}

  /**
   * Start accumulation for a new event.
   * The class holds internally a history of the event data. The data can
   * be fetched from the class without unpacking the input stream again.
   * @return neg. error value if failed
   *         - ENOMEM memory allocation failed
   *         - EALREADY event data acquisition already started
   */
  Int_t StartEvent();

  /**
   * Check whether the event is started
   * @return 1 if started, 0 if not
   */
  Int_t IsStarted() const { return fpRawData!=NULL;};

  /**
   * Calculate the base line from the current event data.
   * Only available within an event cycle. <br>
   * The calculation requires a minimum number of bins which contribute
   * to the sum, which can be specified by \e reqMinCount. The base line
   * calculation will also be skipped if the number of contributing bins is 
   * less than half of the total number of time bins. 
   * @param reqMinCount    the minimum number of bins contributing to the sum
   * @return neg. error value if failed
   *         - ENODATA to little contributing bins
   *         - ENOBUFS no raw data available
   */
  Int_t CalculateBaseLine(Int_t reqMinCount=1);

  /**
   * Stop processing of one event.
   * The data history of the event will be released.
   * @return neg. error value if failed
   *         - EBADF event not started 
   */
  Int_t StopEvent();

  /**
   * Reset the base line history.
   * @return neg. error code if failed 
   */
  Int_t ResetHistory();

  /**
   * Set threshold.
   * The threshold effects to corrected data, signals smaller than threshold
   * are suppressed.
   * @param thresh   Threshold for signal correction
   * @return neg. error code if failed 
   */
  Int_t SetThreshold(AliHLTTPCSignal_t thresh);

  /**
   * Set the raw data value of a certain channel.
   * @param bin      Channel number
   * @param value    ADC value
   * @return neg. error value if failed
   *         - ERANGE bin out of range
   *         - BADF event cycle not started
   */
  Int_t SetRawData(Int_t bin, AliHLTTPCSignal_t value);

  /**
   * Get the raw data value of the current bin.
   * The class holds the current read position which can be incremented by
   * @ref Next() and reseted by @ref Rewind().
   * Raw data is only available within an event cycle.
   * @return raw data value
   */
  AliHLTTPCSignal_t GetRawData() const {return GetRawData(fReadPos);}

  /**
   * Get the corrected data value the current bin.
   * Corrected raw data is only available within an event cycle.
   * The base line value is substracted from the bin's value and suppressed
   * by the threshold. Bins smaller than the first bin considered for base
   * line calculation return 0.
   * The class holds the current read position which can be incremented by
   * @ref Next() and reseted by @ref Rewind().
   * @return corrected value
   */
  AliHLTTPCSignal_t GetCorrectedData() const {return GetCorrectedData(fReadPos);}

  /**
   * Increment the read position.
   * @param bZeroSuppression  skip all bins effected by the zero suppression
   * @return 1 if more data available, 0 if not
   */
  Int_t Next(Int_t bZeroSuppression=kTRUE);

  /**
   * Rewind the read position.
   * The read position is set to the first bin over the zero suppression.
   * @param bZeroSuppression  skip all bins effected by the zero suppression
   * @return 1 if more data available, 0 if not
   */
  Int_t Rewind(Int_t bZeroSuppression=kTRUE);

  /**
   * Get the current read position.
   * @return read position, -1 if no data available
   */
  Int_t GetCurrentPosition() const {return fReadPos<fNofBins?fReadPos:-1;}

  /**
   * Get the raw data value of a certain bin.
   * Raw data is only available within an event cycle.
   * @param bin      Channel number
   * @return raw data value
   */
  AliHLTTPCSignal_t GetRawData(Int_t bin) const;

  /**
   * Get the corrected data value of a certain bin.
   * Corrected raw data is only available within an event cycle.
   * The base line value is substracted from the bin's value and suppressed
   * by the threshold. Bins smaller than the first bin considered for base
   * line calculation return 0.
   * @param bin      Channel number
   * @return corrected value
   */
  AliHLTTPCSignal_t GetCorrectedData(Int_t bin) const;

  /**
   * Get the base line value of a certain channel.
   * @param bin      Channel number
   * @return base line value at bin
   */
  AliHLTTPCSignal_t GetBaseLine(Int_t bin) const;

  /**
   * Get the avarage base line value.
   * @return average base line value
   */ 
  AliHLTTPCSignal_t GetAverage() const;

  /**
   * Get the occupancy for the pad in fractions of 1
   * The occupancy is calculated from the number of time bins with non zero data
   * after zero suppression.
   * @return occupancy in percent
   */
  Float_t GetOccupancy() const;

  /**
   * Get the occupancy average for the pad in fractions of 1
   * The occupancy is calculated from the number of time bins with non zero data
   * after zero suppression and averaged over all events.
   * @return occupancy in percent
   */
  Float_t GetAveragedOccupancy() const;

  /**
   * Get the size (number of time bins) of the pad
   * @return number of bins 
   */
  Int_t GetSize() const {return fNofBins;}
  
  /*
   * Clears the vector of cluster candidates.
   */
  void ClearCandidates();

  /**
   * Set the data array to -1
   */
  void SetDataToDefault();

  /**
   * Stores the signal in the data array, stores the timebin number of the signal,
   * and increments a counter.
   * @param bin       Timebin nuber of signal
   * @param signal    Size of signal 
   */
  void SetDataSignal(Int_t bin,Int_t signal);

  /**
   * Returns the signal in the specified bin
   * @param bin       Timebin number
   * @return          Signal in the given timebin
   */
  Int_t GetDataSignal(Int_t bin) const;

  /**
   * Zerosuppression where one can choose wether one want to cut on sigma(default 3) or a given adc threshold above baseline
   * It works like this: Finds the signals above baseline+threshold, it then looks to the right of the signals adding the 
   * the signal below threshold but over the average. It stops when the adc value rises (since you should expect a fall) 
   * or if the signal in that bin is below average. It now does the same thing to the left. 
   * This is a very timeconsuming approach(but good), and will likely only be used only for cosmics and laser.
   * If you use sigma approach you can give as input n sigma or stick with default=3. For channels with very large signals 
   * the (value-average)² is not calculated when value-average>50. This is due to sigma shooting sky high for only a few values.
   * The method is checked with 2006 cosmics data, and it looks good.
   * If you want to use the threshold approach you HAVE to set nSigma=-1 and threshold>0. For example: If you want all signals 
   * 30 adc counts above threshold you should call the function like this: ZeroSuppress(-1,30)
   * @param nRMS               Specify nRMS threshold
   * @param threshold          Specify what adc threshold above average (remember to give nRMS=-1 if you want to use this approach)
   * @param reqMinPoint        Required minimum number of points to do zerosuppression default AliHLTTPCTransform::GetNTimeBins/2 (1024/2).
   * @param beginTime          Lowest timebin value. Gating grid causes some problems in the first timebins.
   * @param endTime            Highest timebin value.
   * @param timebinsLeft       Timebins to include left of the signals above threshold (to include tails)
   * @param timebinsRight      Timebins to include right of the signals above threshold (to include tails)
   * @param valueBelowAverage  The number of adc-counts below the average value. (sometimes there can be useful to also add some signals below average for your signals, especially when there is a lot of noise) It means that more of the tails of the signal is added.
   * @param speedup            Do not the full zero suppression but terminate if it is clear wheter the channel has some signal or not.
   */
  void ZeroSuppress(Double_t nRMS,Int_t threshold,Int_t reqMinPoint,Int_t beginTime,Int_t endTime,Int_t timebinsLeft, Int_t timebinsRight, Int_t valueBelowAverage, bool speedup=false);
  
  /**
   * Bool method which returns the timein number of the first signal and number of consecutive signals, used together with GetPointer(bin) to access data 
   * @param time            Refernence to timebin number
   * @param bunchSize       Refernence to number of consecutive signals
   * @return                True if there are more signals
  */
  Bool_t GetNextGoodSignal(Int_t &time,Int_t &bunchSize);

  /**
   * Returns number of signals added
   * @return                Number of signals
   */
  UInt_t GetNAddedSignals(){return fSizeOfSignalPositionArray;}

  /**
   * Returns the pointer to the first data signal
   * @param bin             Timebin number
   * @return                pointer to data signal in bin
   */
  UInt_t* GetPointer(Int_t bin){return (UInt_t*)&fDataSignals[bin];  }

  /**
   * Adds cluster candidate to the fClusterCandidates.
   */
  void AddClusterCandidate(AliHLTTPCClusters candidate);

  /**
   * Adds the digits belonging to the candidate.
   */
  void AddCandidateDigits(vector<AliHLTTPCDigitData> candidateDigits);

  
  /**
   * Returns the digit vector belonging to the candidate
   */
  vector<AliHLTTPCDigitData> *GetCandidateDigits(Int_t candidateIndex);

  void ClearCandidateDigits(){fCandidateDigitsVector.clear();}

  /**
   * Prints the raw data og this pad.
   */
  void PrintRawData();

  /**
   * Vector of cluster candidates
   */
  vector<AliHLTTPCClusters> fClusterCandidates;                    //! transient

  /**
   * Vector of used clustercandidates, used so one do not use candidates multiple times
   */
  vector<Int_t> fUsedClusterCandidates;  //! transient
  Bool_t fSelectedPad;                   //! transient
  AliHLTUInt16_t fHWAddress;             //! transient
 
 private:
  /** copy constructor prohibited */
  AliHLTTPCPad(const AliHLTTPCPad&);
  /** assignment operator prohibited */
  AliHLTTPCPad& operator=(const AliHLTTPCPad&);

  /**
   * Add a value to the base line calculation.
   * The value is been added to the sum if it exceeds the current base line
   * and bin is equal or greater than the first bin for base line calculation.
   * @param bin      Channel number
   * @param value    ADC value
   */
  Int_t AddBaseLineValue(Int_t bin, AliHLTTPCSignal_t value);

  /** The row number of the pad */
  Int_t fRowNo;                                                    // see above
  /** The pad number of the pad */
  Int_t fPadNo;                                                    // see above
  /** Threshold for zero suppression */
  AliHLTTPCSignal_t fThreshold;                                    // see above
  /** The average base line value */
  AliHLTTPCSignal_t fAverage;                                      // see above
  /** Number of events included in the base line calculation*/
  Int_t fNofEvents;                                                // see above
  /** The sum within one event */
  AliHLTTPCSignal_t fSum;                                          // see above
  /** The number of bins contributing to the sum */
  Int_t fCount;                                                    // see above
  /** The total number of bins already set during the event */
  Int_t fTotal;                                                    // see above
  /** The maximum base line value within one event */
  AliHLTTPCSignal_t fBLMax;                                        // see above
  /** The bin for the maximum bl value within one event */
  Int_t fBLMaxBin;                                                 // see above
  /** The minimum base line value within one event */
  AliHLTTPCSignal_t fBLMin;                                        // see above
  /** The bin for the minimum bl value within one event */
  Int_t fBLMinBin;                                                 // see above
  /** The first bin included in the base line calculation */
  Int_t fFirstBLBin;                                               // see above
  /** Number of bins */
  Int_t fNofBins;                                                  // see above

  /** The current read position */
  Int_t fReadPos;                                                  // see above

  /** The raw data history */
  AliHLTTPCSignal_t* fpRawData;                                    //! transient

  /**
   * Array containing the data
   */
  AliHLTTPCSignal_t* fDataSignals;                                 //! transient

  /**
   * Array containing info on which bins have signals
   */
  Int_t *fSignalPositionArray;                                     //! transient

  /** Size of signals in fSignalPositionArray */
  Int_t fSizeOfSignalPositionArray;                                //! transient

  /** Number of good signals sent (good signals is signals surviving ZeroSuppression) */
  Int_t fNGoodSignalsSent;
  
  vector<vector<AliHLTTPCDigitData> > fCandidateDigitsVector;

  ClassDef(AliHLTTPCPad, 7)
};
#endif // ALIHLTTPCPAD_H
