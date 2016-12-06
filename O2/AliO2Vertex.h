/// \file AliO2Vertex.h
/// \author R.G.A. Deckers
/// \brief Definition of the AliO2Vertex class.
///
/// See implementation file for copyright details.

#ifndef ALIO2VERTEX_H
#define ALIO2VERTEX_H
#include "O2Vertex.h"
#include <AliVVertex.h>

#include "O2NotImplemented.h"

/// New Vertex class, still highly unspecified, is not a ROOT object.
class AliO2Vertex : public AliVVertex, public O2Vertex {
public:
  AliO2Vertex(const O2Vertex *);
  /// Default constructor
  AliO2Vertex(float positionX = 0, float positionY = 0, float positionZ = 0,
              timestamp_t detectionTime = 0.0f,
              timestamp_t detectionTimeResolution = 0.0f);
  /// Default constructor
  AliO2Vertex(const AliVVertex *vertex, timestamp_t detectionTime,
              timestamp_t detectionTimeResolution);
  /// Destructor
  ~AliO2Vertex();

  // vertex properties
  virtual void GetXYZ(Double_t position[3]) const { EXIT_NOT_IMPLEMENTED(); }
  Double_t GetX() const { return O2Vertex::GetX(); }
  Double_t GetY() const { return O2Vertex::GetX(); }
  Double_t GetZ() const { return O2Vertex::GetX(); }
  virtual void GetCovarianceMatrix(Double_t covmatrix[6]) const {
    for (int i = 0; i < 6; i++) {
      covmatrix[i] = mCovariance[i];
    }
  }

  virtual Double_t GetChi2perNDF() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Double_t GetChi2() const { EXIT_NOT_IMPLEMENTED(); }
  virtual Int_t GetNDF() const { EXIT_NOT_IMPLEMENTED(); }

  virtual Int_t GetNContributors() const { EXIT_NOT_IMPLEMENTED(); }
  virtual void PrintIndices() const { EXIT_NOT_IMPLEMENTED(); }
  virtual void Print(Option_t *option = "") const { EXIT_NOT_IMPLEMENTED(); }

protected:
  // protected stuff goes here

private:
  /// copy constructor prohibited
  // AliO2Vertex(const AliO2Vertex &);
  /// assignment operator prohibited
  // AliO2Vertex &operator=(const AliO2Vertex &);
  ClassDef(AliO2Vertex, 1);
};

#endif
