#ifndef ALIDATAFILE_H
#define ALIDATAFILE_H

#include <stdexcept>

#include "TSystem.h"
#include "TFile.h"
#include "AliOADBContainer.h"

class AliDataFile {
  public:
    virtual ~AliDataFile() {};

    // get file name
    static std::string GetFileName(const std::string &url);

    // simple open for data files
    static TFile *Open(const std::string &url, Option_t *opts="") {
      return TFile::Open(GetFileName(url).c_str(), opts);
    }

    // opening OADB file, reproducing current usage
    static TFile *OpenOADB(const std::string &url, Option_t *opts="") {
      std::string path = "OADB/" + url;
      return Open(path.c_str(), opts);
    }

    // re-implementation of current (dirty) usage
    static AliOADBContainer *GetOADBContainerPointer(const std::string &url, const std::string container) {
      TFile *f = OpenOADB(url);
      if (f && !f->IsZombie())
        return dynamic_cast<AliOADBContainer*>(f->Get(container.c_str()));
      else
        return 0;
    }

    // modern C++ interface
    // (cheap with move construction)
    static AliOADBContainer GetOADBContainer(const std::string &url, const std::string container) {
      TFile *f = OpenOADB(url);
      if (!f || f->IsZombie())
        throw std::runtime_error("failed to open OADB file");

      AliOADBContainer *cont = dynamic_cast<AliOADBContainer*>(f->Get(container.c_str()));
      if (!cont)
        throw std::runtime_error("failure to get OADB container");

      AliOADBContainer contCopy = *cont;
      f->Close();
      return contCopy;
    }

    protected:
    static std::string GetEnv(const std::string &var) {
      const char *env = gSystem->Getenv("ALIPHYSICS_VERSION");
      std::string buf = env ? std::string(buf) : "";
      return buf;
    }

  /// \cond CLASSIMP
  ClassDef(AliDataFile, 0);
  /// \endcond
};

#endif
