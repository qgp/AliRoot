set(CTEST_PROJECT_NAME "AliRoot")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "alirootbuildcmake.cern.ch")
set(CTEST_DROP_LOCATION "/submit.php?project=AliRoot")
set(CTEST_DROP_SITE_CDASH TRUE)
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_ERRORS "1000")
set(CTEST_CUSTOM_MAXIMUM_NUMBER_OF_WARNINGS "1000")
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE "5000")
set(CTEST_PROJECT_SUBPROJECTS STEER PHOS TRD TPC ZDC MUON PMD FMD TOF ITS
      AD ACORDE HMPID T0 BCM STRUCT EVGEN RALICE VZERO 
      THijing THbtp EMCAL 
      THerwig TEPEMGEN FASTSIM TPHIC RAW MONITOR ANALYSIS 
      JETAN HLT LHC ESDCheck STAT TTherminator CORRFW DPMJET TDPMjet 
      PWG0 PWGPP PWG2 PWG3 PWG4 TRIGGER
      TUHKMgen EPOS PYTHIA8 EVE TFluka
      THydjet SHUTTLE PYTHIA6
      LHAPDF HIJING MICROCERN HERWIG
      ALIROOT
)

