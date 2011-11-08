// $Id$
//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        *
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Thorsten Kollegger <kollegge@ikf.uni-frankfurt.de>    *
//*                  for The ALICE HLT Project.                            *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/// @file   AliHLTHuffman.cxx
/// @author Thorsten Kollegger, Matthias Richter
/// @date   2011-08-14
/// @brief  Huffman code generator/encoder/decoder

#include "AliHLTHuffman.h"

#include <iostream>
#include <set>
#include <bitset>
#include <algorithm>

AliHLTHuffmanNode::AliHLTHuffmanNode() 
	: TObject()
	, fValue(-1)
	, fWeight(0.)
{
        // nop
}

AliHLTHuffmanNode::AliHLTHuffmanNode(const AliHLTHuffmanNode& other)
	: TObject()
	, fValue(other.GetValue())
	, fWeight(other.GetWeight())
{
}

AliHLTHuffmanNode& AliHLTHuffmanNode::operator =(const AliHLTHuffmanNode& other) {
        /// assignment operator
	this->fValue = other.fValue;
	this->fWeight = other.fWeight;
	return *this;
}

AliHLTHuffmanNode::~AliHLTHuffmanNode() {
}

void AliHLTHuffmanNode::AssignCode(bool bReverse) {
        /// assign code to this node loop to right and left nodes
        /// the decoding always has to start from the least significant bit since the
        /// code length is variable. Thats why the bit corresponding to the parent node
        /// has to be right of the bit of child nodes, i.e. bits correspond to the
        /// current code length. For storage in a bit stream however, bits are stored
        /// in a stream from MSB to LSB and overwrapping to the MSBs of the next byte.
        /// Here the reverse code is needed and the word of fixed length read from the
        /// stream needs to be reversed before decoding.
        /// Note: by changing the AliHLTDataDeflater interface to write from LSB to MSB
        /// this can be avoided.
	if (GetLeftChild()) {
	  if (bReverse) {
		std::bitset < 64 > v = (this->GetBinaryCode() << 1);
		v.set(0);
		GetLeftChild()->SetBinaryCode(this->GetBinaryCodeLength() + 1, v);
	  } else {
		std::bitset < 64 > v = (this->GetBinaryCode());
		int codelen=this->GetBinaryCodeLength();
		v.set(codelen);
		GetLeftChild()->SetBinaryCode(codelen + 1, v);
	  }
	  GetLeftChild()->AssignCode(bReverse);
	}
	if (GetRightChild()) {
	  if (bReverse) {
		std::bitset < 64 > v = (this->GetBinaryCode() << 1);
		v.reset(0);
		GetRightChild()->SetBinaryCode(this->GetBinaryCodeLength() + 1, v);
	  } else {
		std::bitset < 64 > v = (this->GetBinaryCode());
		int codelen=this->GetBinaryCodeLength();
		v.reset(codelen);
		GetRightChild()->SetBinaryCode(codelen + 1, v);
	  }
	  GetRightChild()->AssignCode(bReverse);
	}
}

void AliHLTHuffmanNode::Print(Option_t* /*option*/) const {
        /// print info
	std::cout << "value=" << GetValue() << ", weight=" << GetWeight() << ", length="
			<< GetBinaryCodeLength() << ", code=" << GetBinaryCode().to_string()
			<< std::endl;
	if (GetLeftChild()) {
		GetLeftChild()->Print();
	}
	if (GetRightChild()) {
		GetRightChild()->Print();
	}
}

ClassImp(AliHLTHuffmanNode)

///////////////////////////////////////////////////////////////////////////////////////////////

AliHLTHuffmanTreeNode::AliHLTHuffmanTreeNode() 
	: AliHLTHuffmanNode()
	, fBinaryCodeLength(0)
	, fBinaryCode(0)
	, fLeft(NULL)
	, fRight(NULL)
{
        // nop
}

AliHLTHuffmanTreeNode::AliHLTHuffmanTreeNode(const AliHLTHuffmanTreeNode& other)
	: AliHLTHuffmanNode()
	, fBinaryCodeLength(other.fBinaryCodeLength)
	, fBinaryCode(other.fBinaryCode)
	, fLeft(other.GetLeftChild())
	, fRight(other.GetRightChild())
{

}

AliHLTHuffmanTreeNode& AliHLTHuffmanTreeNode::operator =(const AliHLTHuffmanTreeNode& other)
{
        /// assignment operator
        if (&other==this) return *this;
	this->fBinaryCodeLength = other.GetBinaryCodeLength();
	this->fBinaryCode = other.GetBinaryCode();
	this->fLeft = other.GetLeftChild();
	this->fRight = other.GetRightChild();
	AliHLTHuffmanNode::operator=(other);
	return *this;
}

AliHLTHuffmanTreeNode::AliHLTHuffmanTreeNode(AliHLTHuffmanNode* l, AliHLTHuffmanNode* r)
	: AliHLTHuffmanNode()
	, fBinaryCodeLength(0)
	, fBinaryCode(0)
	, fLeft(l)
	, fRight(r) {
	if (l && r) {
		SetWeight(l->GetWeight() + r->GetWeight());
	} else if (l && !r) {
		SetWeight(l->GetWeight());
	} else if (!l && r) {
		SetWeight(r->GetWeight());
	}
}

AliHLTHuffmanTreeNode::~AliHLTHuffmanTreeNode() 
{
        // nop
}

ClassImp(AliHLTHuffmanTreeNode)

///////////////////////////////////////////////////////////////////////////////////////////////

AliHLTHuffmanLeaveNode::AliHLTHuffmanLeaveNode() 
	: AliHLTHuffmanNode()
	, fBinaryCodeLength(0)
	, fBinaryCode(0)
	, fLeft(NULL)
	, fRight(NULL)
{
        // nop
}

AliHLTHuffmanLeaveNode::AliHLTHuffmanLeaveNode(const AliHLTHuffmanLeaveNode& other)
	: AliHLTHuffmanNode()
	, fBinaryCodeLength(other.fBinaryCodeLength)
	, fBinaryCode(other.fBinaryCode)
	, fLeft(other.GetLeftChild())
	, fRight(other.GetRightChild())
{

}

AliHLTHuffmanLeaveNode& AliHLTHuffmanLeaveNode::operator =(const AliHLTHuffmanLeaveNode& other)
{
        /// assignment operator
        if (&other==this) return *this;
	this->fBinaryCodeLength = other.GetBinaryCodeLength();
	this->fBinaryCode = other.GetBinaryCode();
	this->fLeft = other.GetLeftChild();
	this->fRight = other.GetRightChild();
	AliHLTHuffmanNode::operator=(other);
	return *this;
}

AliHLTHuffmanLeaveNode::~AliHLTHuffmanLeaveNode() 
{
        // nop
}

ClassImp(AliHLTHuffmanLeaveNode)

///////////////////////////////////////////////////////////////////////////////////////////////

AliHLTHuffman::AliHLTHuffman()
	: TNamed()
	, fMaxBits(0)
	, fMaxValue(0)
	, fNodes(0)
	, fHuffTopNode(NULL)
	, fReverseCode(true)
{
        /// nop
}

AliHLTHuffman::AliHLTHuffman(const AliHLTHuffman& other)
	: TNamed()
	, AliHLTLogging()
	, fMaxBits(other.fMaxBits)
	, fMaxValue(other.fMaxValue)
	, fNodes(other.fNodes)
	, fHuffTopNode(NULL)
	, fReverseCode(other.fReverseCode)
{
        /// nop
}

AliHLTHuffman::AliHLTHuffman(const char* name, UInt_t maxBits)
        : TNamed(name, name)
	, fMaxBits(maxBits)
	, fMaxValue((((AliHLTUInt64_t) 1) << maxBits) - 1)
	, fNodes((((AliHLTUInt64_t) 1) << maxBits))
	, fHuffTopNode(NULL)
	, fReverseCode(true)
 {
        /// standard constructor
	for (AliHLTUInt64_t i = 0; i <= fMaxValue; i++) {
		fNodes[i].SetValue(i);
	}
}

AliHLTHuffman::~AliHLTHuffman() {
        /// destructor, nop
}

const std::bitset<64>& AliHLTHuffman::Encode(const AliHLTUInt64_t v, AliHLTUInt64_t& codeLength) const {
        /// encode a value
	codeLength = 0;
	if (v <= fMaxValue) {
		// valid symbol/value
		if (fHuffTopNode) {
			// huffman code has been generated
			codeLength = fNodes[v].GetBinaryCodeLength();
			return fNodes[v].GetBinaryCode();
		} else {
		  HLTError("encoder '%s' does not seem to be initialized", GetName());
		}
	} else {
	  HLTError("encoder %s: value %llu exceeds range of %d bits", GetName(), v, GetMaxBits());
	}

	static const std::bitset<64> dummy;
	return dummy;
}

Bool_t AliHLTHuffman::Decode(std::bitset<64> bits, AliHLTUInt64_t& value,
			     AliHLTUInt32_t& length, AliHLTUInt32_t& codeLength) const {
	// TODO: check decoding logic, righ now it is just as written
	AliHLTHuffmanNode* currNode = fHuffTopNode;
	if (!currNode) return kFALSE;
	if (currNode->GetValue() >= 0) {
		// handle case with just one node - also quite unlikely
		value = currNode->GetValue();
		return kTRUE;
	}
	while (currNode) {
		if (bits[0] && currNode->GetLeftChild()) {
			// follow left branch
			currNode = currNode->GetLeftChild();
			bits >>= 1;
			if (currNode->GetValue() >= 0) {
				value = currNode->GetValue();
				length = fMaxBits;
				codeLength = currNode->GetBinaryCodeLength();
				return kTRUE;
			}
			continue;
		}
		if (!bits[0] && currNode->GetRightChild()) {
			currNode = currNode->GetRightChild();
			bits >>= 1;
			if (currNode->GetValue() >= 0) {
				value = currNode->GetValue();
				length = fMaxBits;
				codeLength = currNode->GetBinaryCodeLength();
				return kTRUE;
			}
			continue;
		}
		break;
	}
	value = ((AliHLTUInt64_t)1) << 63;
	return kFALSE;
}

Bool_t AliHLTHuffman::AddTrainingValue(const AliHLTUInt64_t value,
		const Float_t weight) {
	if (value > fMaxValue) {
		/* TODO: ERROR message */
		return kFALSE;
	}
	fNodes[value].AddWeight(weight);
	return kTRUE;
}

Bool_t AliHLTHuffman::GenerateHuffmanTree() {
	// insert pointer to nodes into ordered structure to build tree
	std::multiset<AliHLTHuffmanNode*, AliHLTHuffmanNode::less> nodeCollection;
	//	std::copy(fNodes.begin(), fNodes.end(),
	//			std::inserter(freq_coll, freq_coll.begin()));
	for (std::vector<AliHLTHuffmanLeaveNode>::iterator i = fNodes.begin(); i
			!= fNodes.end(); ++i) {
		nodeCollection.insert(&(*i));
	}
	while (nodeCollection.size() > 1) {
		// insert new node into structure, combining the two with lowest probability
		AliHLTHuffmanNode* node=new AliHLTHuffmanTreeNode(*nodeCollection.begin(), *++nodeCollection.begin());
		if (!node) return kFALSE;
		nodeCollection.insert(node);
		nodeCollection.erase(nodeCollection.begin());
		nodeCollection.erase(nodeCollection.begin());
	}
	//assign value
	fHuffTopNode = *nodeCollection.begin();
	fHuffTopNode->AssignCode(fReverseCode);
	return kTRUE;
}

void AliHLTHuffman::Print(Option_t* option) const {
        std::cout << GetName() << endl;
        bool bPrintShort=strcmp(option, "short")==0;
	if (fHuffTopNode && !bPrintShort) {
		std::cout << "Huffman tree:" << endl;
		fHuffTopNode->Print();
	}
	Double_t uncompressedSize = 0;
	Double_t compressedSize = 0;
	Double_t totalWeight = 0;
	if (!bPrintShort)
	  std::cout << std::endl << "Huffman codes:" << std::endl;
	for (AliHLTUInt64_t i = 0; i <= fMaxValue; i++) {
	  if (!bPrintShort) fNodes[i].Print();
		totalWeight += fNodes[i].GetWeight();
		uncompressedSize += fNodes[i].GetWeight() * fMaxBits;
		compressedSize += fNodes[i].GetWeight()
				* fNodes[i].GetBinaryCodeLength();
	}
	if (uncompressedSize > 0) {
		std::cout << "compression ratio: " << compressedSize
				/ uncompressedSize << std::endl;
		std::cout << "<bits> uncompressed: " << uncompressedSize / totalWeight
				<< std::endl;
		std::cout << "<bits> compressed:   " << compressedSize / totalWeight
				<< std::endl;
	}
}

AliHLTHuffman& AliHLTHuffman::operator =(const AliHLTHuffman& other) {
	fMaxValue = other.fMaxValue;
	fNodes = other.fNodes;
	fHuffTopNode = NULL;
	return *this;
}

bool AliHLTHuffman::CheckConsistency() const
{
  if (!fHuffTopNode) {
    cout << "huffman table not yet generated" << endl;
  }

  for (AliHLTUInt64_t v=0; v<GetMaxValue(); v++) {
    AliHLTUInt64_t codeLength=0;
    std::bitset<64> code=AliHLTHuffman::Encode(v, codeLength);
    AliHLTUInt64_t readback=0;
    AliHLTUInt32_t readbacklen=0;
    AliHLTUInt32_t readbackcodelen=0;
    if (fReverseCode) {
      // reverse if needed
      // Note: for optimized bit stream the huffman code is reversed, and
      // that needs to be taken into account
      std::bitset<64> rcode;
      for (AliHLTUInt64_t i=0; i<codeLength; i++) {rcode<<=1; rcode[0]=code[i];}
      code=rcode;
    }
    if (!Decode(code, readback, readbacklen, readbackcodelen)) {
      cout << "Decode failed" << endl;
      return false;
    }
    if (v!=readback) {
      cout << "readback of value " << v << " code length " << codeLength << " failed: got " << readback << " code length " << readbackcodelen << endl;
      return false;
    }
  }
  return true;
}

ClassImp(AliHLTHuffman)

