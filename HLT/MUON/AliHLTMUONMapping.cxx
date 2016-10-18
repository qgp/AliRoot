#include "AliHLTMUONMapping.h"
#include <fstream>
#include <cassert>
#include <iostream>

using namespace std;

AliHLTMUONMapping::mpDE* AliHLTMUONMapping::ReadMapping(const char* mapfile)
{
  ifstream in(mapfile,ios::binary);

  if (!in.is_open()) {
    return 0x0;
  }

  int totalNumberOfPads(0);

  Int_t numberOfDetectionElements(0);

  in.read((char*)&numberOfDetectionElements,sizeof(Int_t));

  std::cout << "numberOfDetectionElements=" << numberOfDetectionElements << std::endl;

  AliHLTMUONMapping::mpDE* detectionElements = new AliHLTMUONMapping::mpDE[numberOfDetectionElements];

  for ( int i = 0; i < numberOfDetectionElements; ++i ) {

    AliHLTMUONMapping::mpDE& de(detectionElements[i]);

    in.read((char*)&de.id,sizeof(Int_t));

    in.read((char*)&de.iCath[0],sizeof(UChar_t)*2);
    in.read((char*)&de.nPads[0],sizeof(UShort_t)*2);

    int n = de.nPads[0] + de.nPads[1];

    de.pads = new mpPad[n];

    for ( int ip = 0; ip < n; ++ip ) {
      in.read((char*)&(de.pads[ip]),sizeof(struct mpPad));
      ++totalNumberOfPads;
    }

    int mapsize = 2*n;
    Long64_t* themap = new Long64_t[mapsize];
    int ix(0);

    in.read((char*)&themap[0],sizeof(Long64_t)*mapsize);

    for ( int cathode = 0; cathode < 2; ++cathode) {
      for ( int i = 0; i < de.nPads[cathode]; ++i ) {
        de.padIndices[cathode].Add(themap[ix],themap[ix+1]);
        ix += 2;
      }
    }

    assert(ix==2*n);
    assert(de.padIndices[0].GetSize() + de.padIndices[1].GetSize() == n);

    delete[] themap;
  }

  if ( totalNumberOfPads != 1064008 ) {
    delete[] detectionElements;
    return 0x0;
  }

  return detectionElements;
}

//_________________________________________________________________________________________________
Bool_t AliHLTMUONMapping::AreOverlapping(Float_t area1[2][2], Float_t area2[2][2], Float_t precision)
{
  /// check if the two areas overlap
  /// precision in cm: positive = increase pad size / negative = decrease pad size

  if (area1[0][0] - area2[0][1] > precision) return kFALSE;
  if (area2[0][0] - area1[0][1] > precision) return kFALSE;
  if (area1[1][0] - area2[1][1] > precision) return kFALSE;
  if (area2[1][0] - area1[1][1] > precision) return kFALSE;

  return kTRUE;

}
