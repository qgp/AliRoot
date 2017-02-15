#include "AliDataFile.h"
#include "AliLog.h"
#include "TSystem.h"
#include <iostream>

std::string AliDataFile::GetFileName(const std::string &url) {
  // trying to read a data file from the following sources
  // (in order of priority):
  //
  // $ALICE_DATA/<url>
  // $ALICE_PHYSICS/<url>
  // $ALICE_ROOT/<url>
  // $ALICE_ROOT/../../../../data/analysis/YYYY/VVVV/<url>
  // /cvmfs/alice.cern.ch/data/analysis/YYYY/VVVV/<url>
  // root://eospublic.cern.ch//eos/experiment/alice/analysis-data/<url>

  std::string buf, ver, year;

  if (const char *env = gSystem->Getenv("ALICE_DATA")) {
    buf = std::string(env) + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str())) {
      AliDebugClass(2, TString::Format("Reading requested data file <%s> from <%s>", url.c_str(), buf.c_str()));
      return buf;
    }
  }

  if (const char *env = gSystem->Getenv("ALICE_PHYSICS")) {
    buf = std::string(env) + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str())) {
      AliDebugClass(2, TString::Format("Reading requested data file <%s> from <%s>", url.c_str(), buf.c_str()));
      return buf;
    }
  }

  if (const char *env = gSystem->Getenv("ALICE_ROOT")) {
    buf = std::string(env) + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str())) {
      AliDebugClass(2, TString::Format("Reading requested data file <%s> from <%s>", url.c_str(), buf.c_str()));
      return buf;
    }
  }

  if (const char *env = gSystem->Getenv("ALIPHYSICS_VERSION")) {
    buf = env;
    if (buf.length() > 12) {
      ver = buf.substr(0, 12);
      year = buf.substr(4, 4);
    }
  }

  if (!ver.empty() && !year.empty()) {
    if (const char *env = gSystem->Getenv("ALICE_ROOT")) {
      buf = std::string(env) + "/../../../../data/analysis/" + year + "/" + ver + "/" + url;
      if (!gSystem->AccessPathName(buf.c_str())) {
        AliDebugClass(2, TString::Format("Reading requested data file <%s> from <%s>", url.c_str(), buf.c_str()));
        return buf;
      }
    }
    buf = "/cvmfs/alice.cern.ch/data/analysis/" + year + "/" + ver + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str())) {
      AliDebugClass(2, TString::Format("Reading requested data file <%s> from <%s>", url.c_str(), buf.c_str()));
      return buf;
    }
  }

  buf = "root://eospublic.cern.ch//eos/experiment/alice/analysis-data/" + url;
  AliDebugClass(2, TString::Format("Reading requested data file <%s> from <%s>", url.c_str(), buf.c_str()));
  return buf;
}
