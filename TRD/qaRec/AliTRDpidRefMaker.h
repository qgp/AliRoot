#ifndef ALITRDPIDREFMAKER_H
#define ALITRDPIDREFMAKER_H

//////////////////////////////////////////////////////
//
// Task to build PID reference tree for the training
// of neural networs for the TRD PID
//
// Authors: Alex Wilk    <wilka@uni-muenster.de>
//          Markus Heide <mheide@uni-muenster.de>
//
///////////////////////////////////////////////////////

#ifndef ALITRDRECOTASK_H
#include "AliTRDrecoTask.h"
#endif

class TTree;
class TEventList;
class TMultiLayerPerceptron;
class AliPID;
class AliTRDtrackV1;
class AliTRDReconstructor;
class AliTRDpidRefMaker : public AliTRDrecoTask
{

public:
  enum  {
    k006  =  0
    ,k008 =  1
    ,k010 =  2
    ,k015 =  3
    ,k020 =  4
    ,k030 =  5
    ,k040 =  6
    ,k050 =  7
    ,k060 =  8
    ,k080 =  9
    ,k100 = 10
    ,kAll = 11
  };

  enum {
    kMoniTrain = 50
  };

  AliTRDpidRefMaker();
  AliTRDpidRefMaker(const Char_t *InFileNN, const Char_t *InFileLQ); 
/*   AliTRDpidRefMaker(const Char_t *InFileNN, const Char_t *InFileLQ, AliTRDCalMomentumBin bin = kAll);  */
  virtual ~AliTRDpidRefMaker();
  
  void    CreateOutputObjects();
  void    Exec(Option_t *option);
  void    GetRefFigure(Int_t ifig, Int_t &first, Int_t &last, Option_t *opt);  
  Int_t   GetEpochs() {return fEpochs;};
  Int_t   GetMinTrain() {return fMinTrain;};
  Int_t   GetTrainMomBin() {return fTrainMomBin;};

  Bool_t  PostProcess();

  void    SetEpochs(Int_t epochs) {fEpochs = epochs;};
  void    SetMinTrain(Int_t mintrain) {fMinTrain = mintrain;};
  void    SetTrainMomBin(Int_t trainmombin) {fTrainMomBin = trainmombin;};

  void    Terminate(Option_t *);

private:
  AliTRDpidRefMaker(const AliTRDpidRefMaker&);              // not implemented
  AliTRDpidRefMaker& operator=(const AliTRDpidRefMaker&);   // not implemented

  void GetV0info(AliTRDtrackV1 *TRDtrack, Float_t *v0pdg);  // get the v0 information
  void MakeTrainingLists();                                 // build the training and the test list
  void TrainNetworks(Int_t mombin);                         // train the neural networks for a given momentum bin
  void BuildLQRefs(Int_t mombin);                           // build the 2dim histos for a given momentum bin
  void MonitorTraining(Int_t mombin);                       // monitor training process

  AliTRDReconstructor *fReconstructor;     //! reconstructor needed for recalculation the PID
  TTree         *fNN;                      // NN data
  TTree         *fLQ;                      // LQ data
  TEventList *fTrain[AliTRDCalPID::kNMom][AliTRDCalPID::kNPlane];          // Training list for each momentum 
  TEventList *fTest[AliTRDCalPID::kNMom][AliTRDCalPID::kNPlane];           // Test list for each momentum 
  TMultiLayerPerceptron *fNet[AliTRDCalPID::kNPlane]; // artificial neural network
/*   TMultiLayerPerceptron *fNet[AliTRDCalPID::kNMom][AliTRDCalPID::kNPlane]; // artificial neural network */

  Int_t         fLayer;                    // TRD layer index 
  Int_t         fTrainMomBin;              // momentum bin for the training
  Int_t         fEpochs;                   // Number of epochs for the training of the NNs
  Int_t         fMinTrain;                 // minimum of events needed for training
  Int_t         fDate;                     // date stamp for training of the NNs
  Float_t       fMom;                      // momentum
  Float_t       *fdEdx[10];                // dEdx array
  Float_t       fv0pid[AliPID::kSPECIES];  // pid from v0s

  ClassDef(AliTRDpidRefMaker, 1); // TRD reference  maker for NN
};

#endif
