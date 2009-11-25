//-*- Mode: C++ -*-
// $Id$
#ifndef ALIHLTTRIGGERDECISION_H
#define ALIHLTTRIGGERDECISION_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/// @file   AliHLTTriggerDecision.h
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   21 Nov 2008
/// @brief  Declaration of the AliHLTTriggerDecision class storing the a AliHLTTrigger component's decision.

#include "TString.h"
#include "AliHLTReadoutList.h"
#include "AliHLTTriggerDomain.h"

/**
 * \class AliHLTTriggerDecision
 * Stores the information and result of a trigger decision made by a component
 * deriving from AliHLTTrigger. The information includes the DDL readout list
 * indicating which DDLs to readout and the trigger domain specifying which HLT
 * raw data blocks to forward to HLTOUT.
 */
class AliHLTTriggerDecision : public TObject
{
 public:
  
  /**
   * Default constructor.
   */
  AliHLTTriggerDecision();
  
  /**
   * Copy constructor performs a deep copy.
   */
  AliHLTTriggerDecision(const AliHLTTriggerDecision& obj);
  
  /**
   * Constructor specifying the result and trigger name.
   * \param result  The result of the trigger decision.
   * \param name  The name of the trigger decision. Should be the name of the
   *     AliHLTTrigger component.
   */
  AliHLTTriggerDecision(bool result, const char* name);
  
  /**
   * Constructor specifying all information fields.
   * \param result  The result of the trigger decision.
   * \param name  The name of the trigger decision. Should be the name of the
   *     AliHLTTrigger component.
   * \param triggerDomain  The trigger domain for the trigger decision.
   * \param description  The description of (reason for) the trigger decision.
   */
  AliHLTTriggerDecision(
      bool result, const char* name, const AliHLTTriggerDomain& triggerDomain,
      const char* description = ""
    );
  
  /**
   * Default destructor.
   */
  virtual ~AliHLTTriggerDecision();
  
  /**
   * Inherited from TObject. Returns the name of the trigger decision.
   */
  virtual const char* GetName() const { return fName.Data(); }
  
  /**
   * Inherited from TObject. Returns the description of the trigger decision.
   */
  virtual const char* GetTitle() const { return fDescription.Data(); }
  
  /**
   * Inherited from TObject. This prints the contents of the trigger decision.
   * \param option  Can be "short" which will print the short format.
   */
  virtual void Print(Option_t* option = "") const;

  /**
   * Inherited from TObject. Copy this to the specified object.
   */
  virtual void Copy(TObject &object) const;

  /**
   * Inherited from TObject. Create a new clone.
   */
  virtual TObject *Clone(const char *newname="") const;

  /**
   * Inherited from TObject. Return the result of the trigger.
   * @return   "0" or "1" (note: its a string)
   */
  virtual Option_t *GetOption() const;

  /**
   * Returns the result of the trigger decision.
   * \returns true if the event was triggered and should be readout.
   */
  bool EventTriggered() const { return Result(); }
  
  /**
   * Returns the result of the trigger decision.
   * The decision is stored in bit 15 of the fBits field.
   * \returns true if the event was triggered and should be readout.
   */
  bool Result() const;
  
  /**
   * Sets the result of the trigger decision.
   * The decision is stored in bit 15 of the fBits field.
   * \param value  The value to set; true if the event triggered and should be
   *     readout and false otherwise.
   */
  void Result(bool value);
  
  /**
   * Returns the name of the trigger decision.
   */
  const char* Name() const { return fName.Data(); }
  
  /**
   * Sets the name of the trigger decision.
   */
  void Name(const char* name) { fName = name; }
  
  /**
   * Returns the description of (reason for) the trigger decision.
   */
  const char* Description() const { return fDescription.Data(); }
  
  /**
   * Sets the description of the trigger decision.
   */
  void Description(const char* value) { fDescription = value; }
  
  /**
   * Returns the DDL readout list associated with this trigger decision.
   */
  AliHLTReadoutList ReadoutList() const { return AliHLTReadoutList(fTriggerDomain); }
  
  /**
   * Sets the DDL readout list associated with this trigger decision.
   */
  void ReadoutList(const AliHLTReadoutList& value);
  
  /**
   * Returns the trigger domain associated with this trigger decision.
   */
  const AliHLTTriggerDomain& TriggerDomain() const { return fTriggerDomain; }
  
  /**
   * Returns the trigger domain associated with this trigger decision for editing.
   */
  AliHLTTriggerDomain& TriggerDomain() { return fTriggerDomain; }
  
  /**
   * Sets the trigger domain associated with this trigger decision.
   */
  void TriggerDomain(const AliHLTTriggerDomain& value) { fTriggerDomain = value; }
  
  /**
   * Assignment operator performs a deep copy.
   */
  AliHLTTriggerDecision& operator = (const AliHLTTriggerDecision& obj);
  
 private:
  
  TString fName; /// The name of the trigger decision. Should be the name of the trigger component that generated it.
  TString fDescription; /// Optional descriptive text giving the reason for the trigger.
  AliHLTTriggerDomain fTriggerDomain;  /// The trigger domain associated with this trigger. i.e. the HLT data blocks to read out.
  
  ClassDef(AliHLTTriggerDecision, 2) // HLT trigger decision object storing information about the readout list, trigger domain and result.
};

#endif // ALIHLTTRIGGERDECISION_H

