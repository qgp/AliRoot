// $Id$
// Category: run
//
// Author: I. Hrivnacova
//
// Class AliRunConfiguration
// -------------------------
// This class creates all Ali* specific action classes
// that will be initialized and managed by Geant4 kernel (G4RunManager).
// It creates also AliRunMessenger that implements commands for
// AliRun methods.

#ifndef ALI_RUN_CONFIGURATION_H
#define ALI_RUN_CONFIGURATION_H

#include "TG4VRunConfiguration.h"

#include <TString.h>

class AliRunMessenger;
class AliFiles;

class G4RunManager;

class AliRunConfiguration : public TG4VRunConfiguration
{
  public:
    AliRunConfiguration();
    // --> protected
    // AliRunConfiguration(const AliRunConfiguration& right);
    virtual ~AliRunConfiguration();
    void SetConfigName(const char* name);
    void SetG3CallsName(const char* name);

  protected:
    AliRunConfiguration(const AliRunConfiguration& right);

    // operators
    AliRunConfiguration& operator=(const AliRunConfiguration& right);

    // methods
    virtual void CreateUserConfiguration();
    
  private:
    AliRunMessenger*  fRunMessenger;  //messenger 
    AliFiles*         fFiles;         //file paths  
    
  ClassDef(AliRunConfiguration, 1) // Ali* specific action classes for Geant4  
};

#endif //ALI_RUN_CONFIGURATION_H

