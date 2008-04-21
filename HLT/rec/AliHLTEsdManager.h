//-*- Mode: C++ -*-
// $Id$

#ifndef ALIHLTESDMANAGER_H
#define ALIHLTESDMANAGER_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTEsdManager.h
    @author Matthias Richter
    @date   
    @brief  Manager for merging and writing of HLT ESDs
*/

#include "AliHLTDataTypes.h"
#include "AliHLTLogging.h"
#include "TString.h"
#include <vector>

class AliESDEvent;
class TTree;
class TFile;

/**
 * @class AliHLTEsdManager
 */
class AliHLTEsdManager : public AliHLTLogging {
 public:
  /** constructor */
  AliHLTEsdManager();
  /** destructor */
  virtual ~AliHLTEsdManager();

  /**
   * Convert data buffer to ESD.
   * The buffer is supposed to describe a streamed AliESDEvent object.
   * If no target object is specified, the ESD is written to a file AliHLTdetESDs.root,
   * where 'det' is derived from the data type origin. Each time the function is invoked
   * a new event is created. Dummy events are added if the previous events did not contain
   *
   * @param pBuffer  [in] the data buffer
   * @param size     [in] data buffer size
   * @param dt       [in] data type of the block
   * @param tgtesd   [out] optional target
   * @param eventno  [in] optional event no
   */
  int WriteESD(const AliHLTUInt8_t* pBuffer, AliHLTUInt32_t size, AliHLTComponentDataType dt,
	       AliESDEvent* tgtesd=NULL, int eventno=-1);

  /**
   * Align all ESD to the same number of events.
   * The function adds empty events to all ESD files if their event number
   * does not match the specified one.
   * @param eventno     the desired event no
   * @return neg. error code if failed
   */
  int PadESDs(int eventno);

  /**
   * Set the target directory for the ESD files.
   */
  void SetDirectory(const char* directory);

  /**
   * Get the list of the internally created files.
   * Returns a blank separated list of the file names.
   */
  TString GetFileNames(AliHLTComponentDataType dt=kAliHLTAnyDataType) const;

  /**
   * Embed an ESD into a TTree object.
   * The tree object needs to be deleted by the caller.
   */
  static TTree* EmbedIntoTree(AliESDEvent* pESD, const char* name="esdTree", const char* title="Tree with HLT ESD objects");

 protected:

 private:
  /** copy constructor prohibited */
  AliHLTEsdManager(const AliHLTEsdManager&);
  /** assignment operator prohibited */
  AliHLTEsdManager& operator=(const AliHLTEsdManager&);

  class AliHLTEsdListEntry : public AliHLTLogging {
  public:
    /** constructor */
    AliHLTEsdListEntry(AliHLTComponentDataType dt);
    /** destructor */
    ~AliHLTEsdListEntry();

    /**
     * Write the ESD to the corresponding file.
     * The tree is first synchronized with the eventno and additional empty
     * events might be inserted if there was a gap. Since we are writing
     * several files in parallel, we have to make sure that those files contain
     * the same number of events.
     * @param pESD        ESD to write
     * @param eventno     optional event no for tree synchronization
     */
    int WriteESD(AliESDEvent* pESD, int eventno=-1);

    /**
     * Set the target directory for the ESD file.
     */
    void SetDirectory(const char* directory);

    /**
     * Delete the ESD file.
     */
    void Delete();

    /**
     * Get name of the ESD file.
     */
    const char* GetFileName() const;

    bool operator==(AliHLTComponentDataType dt) const;

  private:
    /** copy constructor prohibited */
    AliHLTEsdListEntry(const AliHLTEsdListEntry& src);
    /** assignment operator prohibited */
    AliHLTEsdListEntry& operator=(const AliHLTEsdListEntry& src);

    /**
     * Write ESD to temporary file.
     * The ESD is embedded into a tree and saved to a temporary file.
     * The file name is retrieved by TSystem::GetTempFileName and returned
     * on success.
     * @return file name, empty on failure
     */
    TString WriteTempFile(AliESDEvent* pESD) const;

    /** root file name */
    TString fName; //!transient
    /** target directory */
    TString fDirectory; //!transient
    /** data type of the corresponding block */
    AliHLTComponentDataType fDt; //!transient
  };

  typedef vector<AliHLTEsdListEntry*> AliHLTEsdPList;

  /**
   * Find list entry for given data type
   */
  AliHLTEsdListEntry* Find(AliHLTComponentDataType dt) const;

  /** the list of the ESDs */
  AliHLTEsdPList fESDs; //!transient

  /** target directory */
  TString fDirectory; //!transient

  ClassDef(AliHLTEsdManager, 0)
};

#endif
