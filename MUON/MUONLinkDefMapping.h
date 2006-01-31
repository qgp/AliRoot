/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $MpId: basicLinkDefIn.h,v 1.7 2005/09/26 16:05:45 ivana Exp $ 

#pragma link C++ class  AliMpArea+;
#pragma link C++ class  AliMpConstants+;
#pragma link C++ class  AliMpIntPair+;
#pragma link C++ class  AliMpExMap-;
#pragma link C++ class  AliMpPad+;
#pragma link C++ class  AliMpPadPair+;
#pragma link C++ class  AliMpVIndexed+;
#pragma link C++ class  AliMpVSegmentation+;
#pragma link C++ class  AliMpVPadIterator+;
#pragma link C++ class  AliMpPadIteratorPtr+;
#pragma link C++ class  AliMpFiles!;

#pragma link C++ enum   AliMpDirection;
#pragma link C++ enum   AliMpXDirection;
#pragma link C++ enum   AliMpPlaneType;
#pragma link C++ enum   AliMpStationType;

#pragma link C++ function operator-(const AliMpIntPair& ,const AliMpIntPair& );
#pragma link C++ function operator+(const AliMpIntPair& ,const AliMpIntPair& );
#pragma link C++ function operator<<(ostream& ,const AliMpIntPair& );
#pragma link C++ function operator<<(ostream& ,const AliMpPad& );
#pragma link C++ function operator<<(ostream& ,const AliMpArea& );
#pragma link C++ function operator<(const AliMpPad& ,const AliMpPad& );

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $MpId: motifLinkDefIn.h,v 1.3 2005/08/24 09:53:33 ivana Exp $ 

#pragma link C++ class  AliMpMotifReader+;
#pragma link C++ class  AliMpMotifMap+;
#pragma link C++ class  AliMpVMotif+;
#pragma link C++ class  AliMpMotif+;
#pragma link C++ class  AliMpMotifSpecial+;
#pragma link C++ class  AliMpMotifType+;
#pragma link C++ class  AliMpMotifTypePadIterator+;
#pragma link C++ class  AliMpMotifPosition+;
#pragma link C++ class  AliMpMotifPositionPadIterator+;
#pragma link C++ class  AliMpConnection+;

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $MpId: sectorLinkDefIn.h,v 1.4 2005/08/24 10:07:31 ivana Exp $ 

#pragma link C++ class  AliMpSectorReader+;
#pragma link C++ class  AliMpSector+;
#pragma link C++ class  AliMpSectorPadIterator+;
#pragma link C++ class  AliMpSectorAreaHPadIterator+;
#pragma link C++ class  AliMpSectorAreaVPadIterator+;
#pragma link C++ class  AliMpSectorSegmentation+;
#pragma link C++ class  AliMpNeighboursPadIterator+;
#pragma link C++ class  AliMpZone+;
#pragma link C++ class  AliMpSubZone+;
#pragma link C++ class  AliMpRow+;
#pragma link C++ class  AliMpVRowSegment+;
#pragma link C++ class  AliMpVRowSegmentSpecial+;
#pragma link C++ class  AliMpRowSegment+;
#pragma link C++ class  AliMpRowSegmentLSpecial+;
#pragma link C++ class  AliMpRowSegmentRSpecial+;
#pragma link C++ class  AliMpPadRow+;
#pragma link C++ class  AliMpVPadRowSegment+;
#pragma link C++ class  AliMpPadRowLSegment+;
#pragma link C++ class  AliMpPadRowRSegment+;

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $MpId: slatLinkDefIn.h,v 1.4 2005/09/19 19:01:09 ivana Exp $ 

#pragma link C++ class  AliMpHelper+;
#pragma link C++ class  AliMpSt345Reader+;
#pragma link C++ class  AliMpSlat+;
#pragma link C++ class  AliMpSlatSegmentation+;
#pragma link C++ class  AliMpPCB+;
#pragma link C++ class  AliMpSlatPadIterator+;
#pragma link C++ class  AliMpSlatZonePadIterator+;

#pragma link C++ class  AliMpTriggerReader+;
#pragma link C++ class  AliMpTrigger+;
#pragma link C++ class  AliMpTriggerSegmentation+;
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $MpId: graphicsLinkDefIn.h,v 1.4 2005/08/24 09:42:12 ivana Exp $ 

#pragma link C++ class  AliMpGraphContext+;
#pragma link C++ class  AliMpVPainter+;
#pragma link C++ class  AliMpMotifPainter+;
#pragma link C++ class  AliMpRowPainter+;
#pragma link C++ class  AliMpRowSegmentPainter+;
#pragma link C++ class  AliMpSectorPainter+;
#pragma link C++ class  AliMpSubZonePainter+;
#pragma link C++ class  AliMpZonePainter+;
#pragma link C++ class  AliMpSlatPainter+;
#pragma link C++ class  AliMpPCBPainter+;

