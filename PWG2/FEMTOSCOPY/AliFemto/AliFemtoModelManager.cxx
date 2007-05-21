////////////////////////////////////////////////////////////////////////////////
///                                                                          ///
/// AliFemtoModelManager - main helper class for femtoscopy calculations     ///
/// Manages weight generation, freeze-out coordinates generation             ///
/// Authors: Adam Kisiel kisiel@mps.ohio-state.edu                           ///
///                                                                          ///
////////////////////////////////////////////////////////////////////////////////
#ifdef __ROOT__
  ClassImp(AliFemtoModelManager, 1)
#endif

#include "AliFemtoModelManager.h"
#include "AliFemtoModelHiddenInfo.h"

//_____________________________________________
AliFemtoModelManager::AliFemtoModelManager():
  fFreezeOutGenerator(0),
  fWeightGenerator(0),
  fCreateCopyHiddenInfo(kFALSE)
{
}
//_____________________________________________
AliFemtoModelManager::AliFemtoModelManager(const AliFemtoModelManager& aManager):
  fFreezeOutGenerator(0),
  fWeightGenerator(0),
  fCreateCopyHiddenInfo(aManager.fCreateCopyHiddenInfo)
{
  if (aManager.fFreezeOutGenerator) {
    fFreezeOutGenerator = aManager.fFreezeOutGenerator->Clone();
  }
  if (aManager.fWeightGenerator) {
    fWeightGenerator = aManager.fWeightGenerator->Clone();
  }
}
//_____________________________________________
AliFemtoModelManager::~AliFemtoModelManager()
{
  if (fFreezeOutGenerator) delete fFreezeOutGenerator;
  if (fWeightGenerator) delete fWeightGenerator;
}
//_____________________________________________
AliFemtoModelManager& AliFemtoModelManager::operator=(const AliFemtoModelManager& aManager)
{
  if (this == &aManager)
    return *this;
  if (aManager.fFreezeOutGenerator) {
    fFreezeOutGenerator = aManager.fFreezeOutGenerator->Clone();
  }
  else fFreezeOutGenerator = 0;
  if (aManager.fWeightGenerator) {
    fWeightGenerator = aManager.fWeightGenerator->Clone();
  }
  else fWeightGenerator = 0;
  fCreateCopyHiddenInfo = aManager.fCreateCopyHiddenInfo;
  
  return *this;
}
//_____________________________________________
void AliFemtoModelManager::AcceptFreezeOutGenerator(AliFemtoModelFreezeOutGenerator *aFreeze)
{
  fFreezeOutGenerator = aFreeze;
}
//_____________________________________________
void AliFemtoModelManager::AcceptWeightGenerator(AliFemtoModelWeightGenerator *aWeight)
{
  fWeightGenerator = aWeight;
}
//_____________________________________________
Double_t AliFemtoModelManager::GetWeight(AliFemtoPair *aPair)
{
  if (!fWeightGenerator) {
    cout << "No weight generator set! Cannot calculate weight" << endl;
    exit(0);
  }
  // Return femtoscopic weight for a fiven pair
  if (fCreateCopyHiddenInfo) {
    // Try to gess particle masses and pid from the weight generator
    Double_t tMass1, tMass2;
    Int_t tPid1, tPid2;
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkPionPlusPionPlus) {
      tMass1 = 0.13957;
      tMass2 = 0.13957;
      tPid1 = 211;
      tPid2 = 211;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkPionPlusPionMinus) {
      tMass1 = 0.13957;
      tMass2 = 0.13957;
      tPid1 = 211;
      tPid2 = -211;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkKaonPlusKaonPlus) {
      tMass1 = 0.493677;
      tMass2 = 0.493677;
      tPid1 = 321;
      tPid2 = 321;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkKaonPlusKaonMinus) {
      tMass1 = 0.493677;
      tMass2 = 0.493677;
      tPid1 = 321;
      tPid2 = -321;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkProtonProton) {
      tMass1 = 0.938272;
      tMass2 = 0.938272;
      tPid1 = 2212;
      tPid2 = 2212;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkProtonAntiproton) {
      tMass1 = 0.938272;
      tMass2 = 0.938272;
      tPid1 = 2212;
      tPid2 = -2212;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkPionPlusKaonPlus) {
      tMass1 = 0.13957;
      tMass2 = 0.493677;
      tPid1 = 211;
      tPid2 = 321;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkPionPlusKaonMinus) {
      tMass1 = 0.13957;
      tMass2 = 0.493677;
      tPid1 = 211;
      tPid2 = -321;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkPionPlusProton) {
      tMass1 = 0.13957;
      tMass2 = 0.938272;
      tPid1 = 211;
      tPid2 = 2212;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkPionPlusAntiproton) {
      tMass1 = 0.13957;
      tMass2 = 0.938272;
      tPid1 = 211;
      tPid2 = -2212;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkKaonPlusProton) {
      tMass1 = 0.493677;
      tMass2 = 0.938272;
      tPid1 = 321;
      tPid2 = 2212;
    }
    if (fWeightGenerator->GetPairType() == AliFemtoModelWeightGenerator::fgkKaonPlusAntiproton) {
      tMass1 = 0.493677;
      tMass2 = 0.938272;
      tPid1 = 321;
      tPid2 = -2212;
    }

    if (!(aPair->Track1()->HiddenInfo())) {
      AliFemtoModelHiddenInfo *inf1 = new AliFemtoModelHiddenInfo();
      inf1->SetTrueMomentum(aPair->Track1()->Track()->P());
      inf1->SetMass(tMass1);
      inf1->SetPDGPid(tPid1);
      aPair->Track1()->SetHiddenInfo(inf1);
      delete inf1;
    }
    if (!(aPair->Track2()->HiddenInfo())) {
      AliFemtoModelHiddenInfo *inf2 = new AliFemtoModelHiddenInfo();
      inf2->SetTrueMomentum(aPair->Track2()->Track()->P());
      inf2->SetMass(tMass2);
      inf2->SetPDGPid(tPid2);
      aPair->Track2()->SetHiddenInfo(inf2);
      delete inf2;
    }
  }

  if (fFreezeOutGenerator) {
    fFreezeOutGenerator->GenerateFreezeOut(aPair);
  }
  return fWeightGenerator->GenerateWeight(aPair);
}
//_____________________________________________
void AliFemtoModelManager::CreateCopyHiddenInfo()
{
  fCreateCopyHiddenInfo = kTRUE;
}
//_____________________________________________
AliFemtoModelFreezeOutGenerator* AliFemtoModelManager::GetFreezeOutGenerator()
{
  return fFreezeOutGenerator;
}
//_____________________________________________
AliFemtoModelWeightGenerator*    AliFemtoModelManager::GetWeightGenerator()
{
  return fWeightGenerator;
}
