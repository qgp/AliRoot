// @(#) $Id$

#ifndef ALIHLTFILEWRITER_H
#define ALIHLTFILEWRITER_H
/* This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTFileWriter.h
    @author Matthias Richter
    @date   
    @brief  An HLT file dump (data sink) component.
*/

#include "AliHLTDataSink.h"
#include <TString.h>
//#include <TList.h>

/**
 * @class AliHLTFileWriter
 * An HLT data sink component which writes data to file(s).
 *
 * Component ID: \b FileWriter <br>
 * Library: \b libAliHLTUtil.so
 *
 * Mandatory arguments: <br>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 *
 * Optional arguments: <br>
 * <!-- NOTE: ignore the \li. <i> and </i>: it's just doxygen formatting -->
 * \li -datafile     <i> filename   </i> <br>
 *      file name base
 * \li -directory    <i> directory  </i> <br>
 *      target directory
 * \li -subdir[=pattern] <br>
 *      create sub dir for each event, the format patern can contain printf
 *      specifiers to print the evntn no into the dir name, default is
 *      'event%03d' (-subdir w/o additional pattern)
 * \li -idfmt[=pattern] <br>
 *      format specifier for the event id in the file name,                <br>
 *      default: on, default pattern: '_0x%08x'
 * \li -specfmt[=pattern] <br>
 *      format specifier for the data specification in the file name       <br>
 *      default: off, default pattern: '_0x%08x'
 * \li -blocknofmt[=pattern] <br>
 *      format specifier for the block no in the file name                 <br>
 *      default: on, default pattern: '_0x%02x'
 * \li -enumerate <br>
 *      don't use the event number but an event counter beginning from 0
 * \li -concatenate-blocks <br>
 *      concatenate all blocks of one event into one file, this skips
 *      the block no, and the block data type in the file name
 * \li -concatenate-events <br>
 *      concatenate all events into one file, this skips the event no,
 *      the block no, and the block data type in the file name. Currently,
 *      this implies the -concatenate-blocks option.
 *
 * By default, file name is built from the basename, the event number, the
 * block number and the data type in the format:
 * <pre>
 * basename_eventno_dt
 * </pre>
 * If the basename was not given, \em 'event' ist used instead. A file
 * extension after the last dot is separated from the basename and appended
 * to the final name.
 *
 * The naming rule can be changed by the -xxfmt options, which can contain
 * printf format specifiers in order to print the corresponding variable. E.g.
 * <pre>
 * -specfmt             append specification
 * -subdir=test_%d      store in sub folders
 * -blcknofmt=_0x%x     format block no in hex
 * -idfmt=_%04d         print id in 4-digits decimal number
 * </pre>
 *
 * The class can be used as a base class for file writers. Additional
 * argument scan can be implemented in @ref ScanArgument which is called
 * for each unknown argument.
 * @ingroup alihlt_component
 */
class AliHLTFileWriter : public AliHLTDataSink  {
 public:
  /** standard constructor */
  AliHLTFileWriter();
  /** destructor */
  virtual ~AliHLTFileWriter();

  virtual const char* GetComponentID();
  virtual void GetInputDataTypes( vector<AliHLTComponentDataType>& list);
  virtual AliHLTComponent* Spawn();

 protected:
  /**
   * Init method.
   */
  int DoInit( int argc, const char** argv );

  /**
   * Deinit method.
   */
  int DoDeinit();

  /**
   * Init the writer.
   * The DoInit function is not available for child classes. InitWriter is the
   * corresponding function for classes derived from AliHLTFileWriter.
   */
  virtual int InitWriter();

  /**
   * Close the writer.
   * The DoDeinit function is not available for child classes. CloseWriter is the
   * corresponding function for classes derived from AliHLTFileWriter.
   */
  virtual int CloseWriter();

  /**
   * Data processing method for the component.
   * The function can be overloaded by other file writer components.
   * @param evtData       event data structure
   * @param trigData	  trigger data structure
   */
  virtual int DumpEvent( const AliHLTComponentEventData& evtData,
			 AliHLTComponentTriggerData& trigData );

  using AliHLTDataSink::DumpEvent;

  /**
   * Scan one argument and adjacent parameters.
   * Can be overloaded by child classes in order to add additional arguments
   * beyond the standard arguments of the file publisher. The method is called
   * whenever a non-standard argument is recognized. Make sure to return 
   * <tt> -EPROTO </tt> if the argument is not recognized be the child.
   * @param argc           size of the argument array
   * @param argv           agument array for component initialization
   * @return number of processed members of the argv <br>
   *         -EINVAL unknown argument <br>
   *         -EPROTO parameter for argument missing <br>
   */
  virtual int ScanArgument(int argc, const char** argv);

  /**
   * Build file name from eventID data type and the specified directory and basename.
   * @param eventID [in]   the ID of the event
   * @param blockID [in]   the ID of the current block
   *                       no block string appended if -1
   * @param dataType [in]  the data type of the data block
   *                       no type string appanded if @ref kAliHLTVoidDataType
   * @param specification  data specification of the block
   * @param filename [out] string to receive the file name
   */
  int BuildFileName(const AliHLTEventID_t eventID, const int blockID,
		    const AliHLTComponentDataType& dataType,
		    const AliHLTUInt32_t specification,
		    TString& filename);

  /**
   * Set a mode flag.
   * @return current mode flags
   */
  int SetMode(Short_t mode);
    
  /**
   * Clear a mode flag.
   * @return current mode flags
   */
  int ClearMode(Short_t mode);

  /**
   * Check a mode flag.
   * @return 1 if flag is set, 0 if not
   */
  int CheckMode(Short_t mode) const;

  /**
   * Get the currently set file extension.
   */
  TString GetExtension() {return fExtension;}

  /**
   * Set the file extension.
   */
  void SetExtension(const char* extension) {fExtension=extension!=NULL?extension:"";}

  /**
   * Working modes of the writer
   * @internal
   */
  enum TWriterMode {
    /**
     * flag to indicate whether to write each incoming block to separate files
     * or all blocks of one event to one file. set = concatenate (one file).
     */
    kConcatenateBlocks = 0x1,

    /**
     * flag to indicate whether to concatenate incoming blocks of the same type
     * for all events to one file. If also @ref kConcatenateBlocks is set,
     * or all blocks of all events are written to the same file.
     */
    kConcatenateEvents = 0x2,

    /** event enumeration flag */
    kEnumerate = 0x4

  };

 private:
  /** copy constructor prohibited */
  AliHLTFileWriter(const AliHLTFileWriter&);
  /** assignment operator prohibited */
  AliHLTFileWriter& operator=(const AliHLTFileWriter&);

  /** the basename of the output file */
  TString    fBaseName;                                            // see above
  /** the extension of the output file */
  TString    fExtension;                                           // see above
  /** target directory */
  TString    fDirectory;                                           // see above
  /** base name of the event sub directories */
  TString    fSubDirFormat;                                        // see above
  /** event id format string (when added to file name) */
  TString    fIdFormat;                                            // see above
  /** specification format string (when added to file name) */
  TString    fSpecFormat;                                          // see above
  /** format string for block no (when added to file name) */
  TString    fBlcknoFormat;                                        // see above
 protected:
  /** enumeration format string */
  TString    fCurrentFileName;                                     // see above
 private:

  /** mode specifier, see @ref TWriterMode */
  Short_t    fMode;                                                // see above

  ClassDef(AliHLTFileWriter, 2)
};
#endif
