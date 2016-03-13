// basic raw data reading test with a trigger condition
#include "O2Muon.h"
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
  std::string cdbpath = "raw://";
  std::string rawDataInputFile;
  std::string triggerClass = "";

  if (argc==1) {
    std::cout << argv[0] << " [-cdbpath ocdb] [-trigger triggerclassname] -raw inputfile"
    << std::endl;
    return 2;
  }

  for ( int i = 1; i < argc; ++i ) {

    std::cout << "argv[" << i << "]=" << argv[i] << std::endl;

      if (strcmp(argv[i],"--cdbpath")==0) {
          if ( argc > i+1 ) {
            cdbpath = argv[i+1];
          }
          ++i;
          continue;
      }

      if (strcmp(argv[i],"--raw")==0) {
          if ( argc > i+1 ) {
            rawDataInputFile = argv[i+1];
          }
          ++i;
          continue;
      }

      if (strcmp(argv[i],"--trigger")==0) {
          if ( argc > i+1 ) {
            triggerClass = argv[i+1];
          }
          ++i;
          continue;
      }

  }

  std::cout << cdbpath << " " << rawDataInputFile << " " << triggerClass << std::endl;

  if (rawDataInputFile.empty()) {
    std::cerr << "Required argument -raw missing" << std::endl;
    return 1;
  }

  O2Muon o2(cdbpath.c_str());

  return o2.filterRaw(rawDataInputFile.c_str(), triggerClass.c_str());

}
