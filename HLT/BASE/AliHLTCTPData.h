//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTCTPDATA_H
#define ALIHLTCTPDATA_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTCTPData.h
    @author Matthias Richter
    @date   2009-08-20
    @brief  Container for CTP trigger classes and counters
*/

#include "TNamed.h"
#include "TClonesArray.h"
#include "TArrayL64.h"
#include "AliHLTLogging.h"
#include "AliHLTDataTypes.h"

/**
 * @class AliHLTCTPData
 * This is a container for the CTP trigger classes, the mapping to the bit
 * field, and counters.
 *
 * The class is initialized from CTP_TRIGGER_CLASSES part of the ECS parameters.
 * and stores internally a list of trigger classes holding the information on bit
 * position, class name and involved detectors. The general format og the parameter
 * is as follows:
 * <pre>
 * [bit position]:[Trigger class identifier string]:[detector-id-nr]-[detector-id-nr]-...,[bit position]:.....
 * </pre>
 *
 * The list of participating detectors is converted into an AliHLTReadoutList
 * object named after the trigger class name, and can be used as mask for the
 * readout list generated by a component.
 *
 * The object is also stored as part of the HLTGlobalTriggerDecision
 * @ingroup alihlt_trigger
 */
class AliHLTCTPData: public TNamed, public AliHLTLogging
{
 public:
  /// default constructor
  AliHLTCTPData();
  /// standard constructor including initialization from CTP_TRIGGER_CLASS
  AliHLTCTPData(const char* parameter);
  /// copy constructor
  AliHLTCTPData(const AliHLTCTPData&);
  ///assignment operator
  AliHLTCTPData& operator=(const AliHLTCTPData&);
  /// destructor
  virtual ~AliHLTCTPData();

  /// Add counters
  AliHLTCTPData& operator += (const AliHLTCTPData&);
  /// Add counters
  AliHLTCTPData operator + (const AliHLTCTPData&) const;

  /// Subtract counters
  AliHLTCTPData& operator -= (const AliHLTCTPData&);
  /// Subtract counters
  AliHLTCTPData operator - (const AliHLTCTPData&) const;

  /**
   * Init the class ids and mapping from the CTP_TRIGGER_CLASS parameter.
   * The general format of the parameter is as follows:
   */
  int InitCTPTriggerClasses(const char* ctpString);

  /// etract the active trigger mask from the trigger data
  static AliHLTUInt64_t ActiveTriggers(const AliHLTComponentTriggerData& trigData);

  /**
   * Evaluate an expression of trigger class ids with respect to the trigger mask.
   */
  bool EvaluateCTPTriggerClass(const char* expression, const AliHLTComponentTriggerData& trigData) const;

  /**
   * Evaluate an expression of trigger class ids with respect to the trigger mask.
   */
  bool EvaluateCTPTriggerClass(const char* expression, AliHLTUInt64_t triggerMask) const;

  /**
   * Evaluate an expression of trigger class ids with respect to the current trigger mask.
   */
  bool EvaluateCTPTriggerClass(const char* expression) const {
    return EvaluateCTPTriggerClass(expression, fTriggers);
  }

  /**
   * Reset all counters
   */
  void ResetCounters();

  /**
   * Get index of a trigger class in the tigger pattern
   */
  int Index(const char* name) const;

  /**
   * Increment counter for CTP trigger classes
   * @param classIds  comma separated list of class ids
   */
  void Increment(const char* classIds);

  /**
   * Increment counter for CTP trigger classes
   * @param triggerPattern  corresponds to the 50bit trigger mask in the CDH
   */
  void Increment(AliHLTUInt64_t triggerPattern);

  /**
   * Increment counter for a CTP trigger class
   * @param classIdx  index of the class in the 50bit trigger mask
   */
  void Increment(int classIdx);

  /**
   * Increment counters according to the trigger data struct.
   * First extract trigger pattern from the CDH and then
   * increment from the trigger pattern.
   */
  int Increment(AliHLTComponentTriggerData& trigData);

  /**
   * Return a readout list for the active trigger classes.
   * The list is an 'OR' of the active trugger classes.
   */
  AliHLTEventDDL ReadoutList(const AliHLTComponentTriggerData& trigData) const;

  /**
   * Return a readout list for the active trigger classes.
   * The list is an 'OR' of the active trugger classes.
   */
  AliHLTEventDDL ReadoutList(AliHLTUInt64_t  triggerMask) const;

  /**
   * Return a readout list for the active trigger classes.
   * The list is an 'OR' of the active trugger classes.
   */
  AliHLTEventDDL ReadoutList() const {
    return ReadoutList(fTriggers);
  }

  /**
   * Inherited from TObject, this prints the contents of the trigger decision.
   */
  virtual void Print(Option_t* option = "") const;

  AliHLTUInt64_t   Mask() const { return fMask; }
  AliHLTUInt64_t   Triggers() const { return fTriggers; }
  void             SetTriggers(AliHLTUInt64_t triggers) { fTriggers=triggers; }
  void             SetTriggers(AliHLTComponentTriggerData trigData) {SetTriggers(ActiveTriggers(trigData));}
  const TArrayL64& Counters() const { return fCounters; }
  AliHLTUInt64_t   Counter(int index) const;
  AliHLTUInt64_t   Counter(const char* classId) const;
  const char*      Name(int index) const;

 protected:
 private:
  /**
   * Add counters.
   * Base methods for operators.
   * @param src    instance to add
   * @param factor +1/-1 for addition/subtraction
   * @skipped      target to get the numner of not matching class names
   */
  int Add(const AliHLTCTPData& src, int factor, int &skipped);

  AliHLTUInt64_t fMask;      /// mask of initialized trigger classes
  AliHLTUInt64_t fTriggers;  /// current trigger
  TClonesArray   fClassIds;  /// array of trigger class ids
  TArrayL64      fCounters;  /// trigger class counters

  ClassDef(AliHLTCTPData, 1)
};

#endif
