// @(#) $Id$
// Original: AliHLTLog.h,v 1.2 2004/06/11 16:06:33 loizides Exp $

//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

#ifndef ALIHLTTPCLOG_H
#define ALIHLTTPCLOG_H

#include <sstream>
#include <iostream>
#include "AliHLTLogging.h"

using namespace std;

/** key to indicate the origin part */
#define AliHLTTPCLogKeyOrigin  "__origin"
/** key to indicate the keyword part */
#define AliHLTTPCLogKeyKeyword "__key" 
/** key to indicate the message part */
#define AliHLTTPCLogKeyMessage "__message"

/**
 * @class AliHLTTPCLog
 * This class implements the old HLT TPC logging mechanism.
 * Logging messages are now forwarded to the HLT logging system
 * \em Note: the old LOG and ENDLOG macros should be used any longer,
 * use the HLT logging macros or AliRoot logging macros instead. 
 * @see AliHLTLogging
 *
 * @ingroup alihlt_tpc
 */
class AliHLTTPCLog  {
  public:
  enum TLogLevel { kNone = 0, kBenchmark=0x01,kDebug= 0x02, kInformational = 0x04, kWarning = 0x08, kError = 0x10 , kFatal = 0x20, kPrimary = 0x80, kAll = 0xBF };

 private:
  /** not used */
  static const char* kEnd;                                         //! transient
  /** not used */
  static const char* kPrec;                                        //! transient
 public:
  /** stream manipulator for hex output, but empty in the implementation */
  static const char* kHex;                                         //! transient
  /** stream manipulator for decimal output, but empty in the implementation */
  static const char* kDec;                                         //! transient

  /**
   * Flush the stringstream and print output to the HLT logging system.
   * The attributes are set before the message is streamed into the
   * stringstream.<br>
   * The LOG macro sets the attributes from the macro arguments and provides
   * the stringstream.<br>
   * The ENDLOG macro calls the Flush method after the message was streamed
   * into the stringstream.
   */
  static const char* Flush();

  /**
   * Get the stream.
   */
  static stringstream& GetStream() {return fgStream;}

  /**
   * Get the logging level.
   */
  static TLogLevel GetLevel() {return fgLevel;}

 private:
  /** a stringstream to receive the output */
  static stringstream fgStream;                                    // see above

  /** the logging filter */
  static TLogLevel fgLevel;                                        // see above

  /** HLT logging instance */
  static AliHLTLogging fgHLTLogging;                               // see above

  /** copy constructor prohibited */
  AliHLTTPCLog(const AliHLTTPCLog&);
  /** assignment operator prohibited */
  AliHLTTPCLog& operator=(const AliHLTTPCLog&);

};

/** LOG macro to be used by the TPC code 
 * \em Note: this macro should be used any longer 
 */
#define LOG( lvl, origin, keyword ) \
  if (lvl>=AliHLTTPCLog::GetLevel()) AliHLTTPCLog::GetStream() << lvl	\
                           << " " << AliHLTTPCLogKeyOrigin  << " " << origin \
                           << " " << AliHLTTPCLogKeyKeyword << " " << keyword \
			   << " " << AliHLTTPCLogKeyMessage << " "

/** ENDLOG macro calls the Flush method 
 * \em Note: this macro should be used any longer 
 */
#define ENDLOG AliHLTTPCLog::Flush()

#endif /* ALIHLTTPCLOG_H */
