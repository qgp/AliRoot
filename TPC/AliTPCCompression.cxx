/**************************************************************************
 * Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */


// This class contains the implementation of the 
// compression and decompression algorithms 
// Compression is performed reading the Altro data block (called packet) backward.
// Similarly decompression is also done backward so that the final file is restored
// after the compression and decompression phase.

#include <stdlib.h>
#include <TObjArray.h>
#include <Riostream.h>
#include <TMath.h>
#include "AliTPCCompression.h"
#include "AliTPCBuffer160.h"
#include "AliTPCHuffman.h"

ClassImp(AliTPCCompression)
//////////////////////////////////////////////////////////////////////////////////////////////////
AliTPCCompression::AliTPCCompression(){
  //Defaul constructor
  fDimBuffer=sizeof(ULong_t)*8;
  fFreeBitsBuffer=fDimBuffer;
  fReadBits=0;
  fPos=0;
  fBuffer=0;
  fVerbose=0;
  fFillWords=0;
  return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
AliTPCCompression::AliTPCCompression(const AliTPCCompression &source){
  //Constructor
  this->fDimBuffer=source.fDimBuffer;
  this->fFreeBitsBuffer=source.fFreeBitsBuffer;
  this->fReadBits=source.fReadBits;
  this->fPos=source.fPos;
  this->fBuffer=source.fBuffer;
  this->fVerbose=source.fVerbose;
  this->fFillWords=source.fFillWords;
  return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
AliTPCCompression&  AliTPCCompression::operator=(const AliTPCCompression &source){
  //Redefinition of the assignment operator
  this->fDimBuffer=source.fDimBuffer;
  this->fFreeBitsBuffer=source.fFreeBitsBuffer;
  this->fReadBits=source.fReadBits;
  this->fPos=source.fPos;
  this->fBuffer=source.fBuffer;
  this->fVerbose=source.fVerbose;
  this->fFillWords=source.fFillWords;
  return *this;
} 
//////////////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::NextTable(Int_t Val,Int_t &NextTableType,Int_t &BunchLen,Int_t &Count)const{
  //Depending on the data type (5 types of data) a specific table is called
  /*
    Table index:
    0==> Bunch length value     
    1==> Time Bin value 
    2==> 1-samples bunch
    3==> Central samples
    4==> Border samples
  */  
  switch (NextTableType){
  case 0:{
    BunchLen=Val-2;
    NextTableType=1;
    break;
  }//end case 0
  case 1:{
    if (BunchLen==1)NextTableType=2;
    else{
      NextTableType=4;
      Count=1;
    }
    break;
  }//end case 1
  case 2:{
    NextTableType=0;
    break;
  }//end case 2
  case 3:{
    Count++;
    if (Count==(BunchLen-1)){
      NextTableType=4;
    }
    break;
  }//end case 3
  case 4:{
    if (Count==1){
      if (BunchLen>2)
	NextTableType=3;
      else
	Count++;
    }
    else
      NextTableType=0;
    break;
  }//end case 4
  }//end switch
  return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////

Int_t AliTPCCompression::FillTables(const char* fSource,AliTPCHTable* table[],const Int_t NumTables){
  //This method is used to compute the frequencies of the symbols in the source file
  AliTPCBuffer160 buff(fSource,0);
  ULong_t countWords=0;
  ULong_t countTrailer=0;
  Int_t numWords,padNum,rowNum,secNum=0;
  Int_t value=0;
  ULong_t stat[5]={0,0,0,0,0};
  Int_t endFill=0;
  Int_t end=1;
  while(buff.ReadTrailerBackward(numWords,padNum,rowNum,secNum) !=-1 ){
    if(end){
      endFill=buff.GetFillWordsNum();
      end=0;
    }//endif
    countTrailer++;
    if (numWords%4){
      fFillWords+=4-numWords%4;
      for(Int_t j=0;j<(4-numWords%4);j++){
	value=buff.GetNextBackWord();
      }//end for
    }//end if
    
    Int_t packet[1024];
    Int_t timePos[345];
    Int_t tp=0;
    for(Int_t i=0;i<345;i++)timePos[i]=0;
    for(Int_t i=0;i<1024;i++)packet[i]=0;
    
    Int_t nextTableType=0;
    Int_t bunchLen=0;
    Int_t count=0;
    for(Int_t i=0;i<numWords;i++){
      value=buff.GetNextBackWord();
      packet[i]=value;
      if(nextTableType==1){
	timePos[tp]=i;
	tp++;
      }
      NextTable(value,nextTableType,bunchLen,count);
    }//end for
    //computing the Time gap between two bunches
    Int_t temp=0;
    tp--;
    Int_t previousTime=packet[timePos[tp]];
    for(Int_t i=tp-1;i>=0;i--){
      Int_t timPos=timePos[i];
      Int_t bunchLen=packet[timPos-1]-2;
      temp=packet[timPos];
      packet[timPos]=packet[timPos]-previousTime-bunchLen;
      previousTime=temp;
    }
    nextTableType=0;
    count=0;
    bunchLen=0;
    for(Int_t i=0;i<numWords;i++){
      value=packet[i];
      table[nextTableType]->SetFrequency(value);
      stat[nextTableType]++;
      NextTable(value,nextTableType,bunchLen,count);
      countWords++;
    }//end for
  }//end while
  cout<<"Number of words:       "<<countWords<<endl;
  cout<<"Number of trailers:    "<<countTrailer<<endl;
  cout<<"Number of fill words   "<<fFillWords+endFill<<endl;
  cout<<"Total number of words: "<<countWords+countTrailer*4+fFillWords<<endl;
  //STATISTICS  
  fStat.open("Statistics");
  fStat<<"Number of words:..........................................."<<countWords<<endl;
  fStat<<"Number of trailers (4 10 bits words in each one)..........."<<countTrailer<<endl;
  fStat<<"Number of fill words:......................................"<<fFillWords+endFill<<endl;
  fStat<<"Total number of words:....................................."<<countWords+countTrailer*4+fFillWords+endFill<<endl;
  fStat<<"-----------------------------------------"<<endl;
  fStat<<"Number of Bunches............."<<stat[0]<<endl;
  fStat<<"Number of Time bin............"<<stat[1]<<endl;
  fStat<<"Number of One Samples Bunch..."<<stat[2]<<endl;
  fStat<<"Number of Central Samples....."<<stat[3]<<endl;
  fStat<<"Number of Border Samples......"<<stat[4]<<endl;
  fStat<<"-----------------------------------------"<<endl;
  ULong_t fileDimension=(ULong_t)TMath::Ceil(double((countTrailer*4+countWords+fFillWords+endFill)*10/8));
  fStat<<"Total file Size in bytes.."<<fileDimension<<endl;
  Double_t percentage=TMath::Ceil((fFillWords+endFill)*125)/fileDimension;
  fStat<<"Fill Words................"<<(ULong_t)TMath::Ceil((fFillWords+endFill)*10/8)<<" bytes   "<<percentage<<"%"<<endl;  
  percentage=(Double_t)countTrailer*500/fileDimension;
  fStat<<"Trailer..................."<<countTrailer*5<<" bytes   "<<percentage<<"%"<<endl;

  percentage=(Double_t)((stat[0]+stat[1]+stat[2]+stat[3]+stat[4])) *125/fileDimension;
  fStat<<"Data......................"<<(ULong_t)TMath::Ceil((stat[0]+stat[1]+stat[2]+stat[3]+stat[4])*10/8)<<" bytes   "<<percentage<<"%"<<endl;

  percentage=(Double_t)(stat[0]*125)/fileDimension;
  fStat<<"Bunch....................."<<(ULong_t)TMath::Ceil(stat[0]*10/8)<<" bytes  "<<percentage<<"%"<<endl;  //  
  percentage=(Double_t)(stat[1]*125)/fileDimension;
  fStat<<"Time......................"<<(ULong_t)TMath::Ceil(stat[1]*10/8)<<" bytes  "<<percentage<<"%"<<endl;  //  


  percentage=(Double_t)((stat[2]+stat[3]+stat[4])) *125/fileDimension;
  fStat<<"Amplitude values.........."<<(ULong_t)TMath::Ceil((stat[2]+stat[3]+stat[4])*10/8)<<" bytes  "<<percentage<<"%"<<endl;
  percentage=(Double_t)(stat[2]*125)/fileDimension;
  fStat<<"     One Samples..............."<<(ULong_t)TMath::Ceil(stat[2]*10/8)<<" bytes  "<<percentage<<"%"<<endl;  //  
  percentage=(Double_t)(stat[3]*125)/fileDimension;
  fStat<<"     Central Samples..........."<<(ULong_t)TMath::Ceil(stat[3]*10/8)<<" bytes  "<<percentage<<"%"<<endl;  //  
  percentage=(Double_t)(stat[4]*125)/fileDimension;
  fStat<<"     Border Samples............"<<(ULong_t)TMath::Ceil(stat[4]*10/8)<<" bytes  "<<percentage<<"%"<<endl;  //  
  fStat.close();
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::StoreTables(AliTPCHTable* table[],const Int_t NumTable){
  //This method stores the tables in a sequence of binary file
  char filename[15];
  ofstream fTable;
  for(Int_t k=0;k<NumTable;k++){
    sprintf(filename,"Table%d.dat",k);
#ifndef __DECCXX 
    fTable.open(filename,ios::binary);
#else
    fTable.open(filename);
#endif
    Int_t dim=table[k]->Size();
    //Table dimension is written into a file
    fTable.write((char*)(&dim),sizeof(Int_t));
    //One table is written into a file
    for(Int_t i=0;i<dim;i++){
      UChar_t codeLen=table[k]->CodeLen()[i];
      //      ULong_t code=(ULong_t)table[k]->Code()[i];
      Double_t code=table[k]->Code()[i];
      fTable.write((char*)(&codeLen),sizeof(UChar_t));
      //fTable.write((char*)(&code),sizeof(ULong_t));
      fTable.write((char*)(&code),sizeof(Double_t));
    } //end for
    fTable.close();
  }//end for
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::CreateTableFormula(Double_t beta,ULong_t  M,Int_t dim,Int_t Type){
  // Type = 0 for Bunch length
  // Type = 1 for Time Gap
  ULong_t freq;
  Double_t sum=0;
  Double_t min=10;
  Double_t alpha=0;
  Double_t A=0;
  AliTPCHTable *Table=new AliTPCHTable(dim);
  
  freq=1;
  Double_t FreqArray[1024];
  for(Int_t i=0;i<1024;i++){
    FreqArray[i]=0;
  }
  alpha=M*0.000000602+0.0104;
  if (fVerbose)
    cout<<"alpha "<<alpha<<endl;
  for(Int_t x=0;x<dim;x++){
    if (Type==1)
      FreqArray[x]=TMath::Power((x+1),-beta)*TMath::Exp(-alpha*(x+1));
    else
      FreqArray[x]=TMath::Power((x+1),-beta);
    sum+=FreqArray[x];
    if (FreqArray[x]<min)min=FreqArray[x];
  }//end for
  if (fVerbose)
    cout<<"Minimun Value "<<min<<endl;
  A=1/sum;
  if (fVerbose)
    cout<<"A Value: "<<A<<endl;
  for(Int_t x=0;x<dim;x++){
    if (Type==0)//Bunch length
      if (x>=3)//minimum bunch length
	Table->SetValFrequency(x,A*FreqArray[x]*1000);
      else
	Table->SetValFrequency(x,0);
    else //Time table
      Table->SetValFrequency(x,A*FreqArray[x]);
  }
  Table->BuildHTable();
  ofstream fTable;
  char filename[15];
  sprintf(filename,"Table%d.dat",Type); 
#ifndef __DECCXX 
  fTable.open(filename,ios::binary);
#else
  fTable.open(filename);
#endif
  Int_t dimTable=Table->Size();
  //Table dimension is written into a file
  fTable.write((char*)(&dimTable),sizeof(Int_t));
  //One table is written into a file
  for(Int_t i=0;i<dimTable;i++){
    UChar_t CodeLen=Table->CodeLen()[i];
    Double_t Code=Table->Code()[i];
    fTable.write((char*)(&CodeLen),sizeof(UChar_t));
    fTable.write((char*)(&Code),sizeof(Double_t));
  } //end for
  fTable.close();
  delete Table;
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::CreateTables(const char* fSource,const Int_t NumTables){
  //Tables manager
  /*
    Table index:
    0==> Bunch length values     
    1==> Time Bin values 
    2==> 1-samples bunch
    3==> Central samples
    4==> Border samples
  */
  Int_t n=10;// 10 bits per symbol 
  AliTPCHTable ** table = new AliTPCHTable*[NumTables];
  //The table is inizialized with the rigth number of rows 
  for(Int_t i=0;i<NumTables;i++){
    table[i]=new  AliTPCHTable((Int_t)(TMath::Power(2,n)));
    table[i]->SetVerbose(fVerbose);
  }
  //The frequencies are calculated and the tables are filled
  if (fVerbose)
    cout<<"Filling tables...\n";
  //The get the frequencies 
  FillTables(fSource,table,NumTables);

  //This part will be used in the table optimization phase
  /*
  for(Int_t i=0;i<NumTables;i++){
    table[i]->CompleteTable(i);
  }
  */
  if(fVerbose){
    cout<<"Entropy of Bunch length table........."<<table[0]->GetEntropy()<<endl;
    cout<<"Entropy of Time bin table............."<<table[1]->GetEntropy()<<endl;
    cout<<"Entropy of one Sample bunch table....."<<table[2]->GetEntropy()<<endl;
    cout<<"Entropy of Central Sample table......."<<table[3]->GetEntropy()<<endl;
    cout<<"Entropy Border Samples table.........."<<table[4]->GetEntropy()<<endl;
  }
  fStat.open("Statistics",ios::app);
  fStat<<endl;
  fStat<<"----------------- ENTROPY for castomized tables --------------------------"<<endl;
  fStat<<"Entropy of Bunch length table......."<<table[0]->GetEntropy()<<endl;
  fStat<<"Entropy of Time bin table..........."<<table[1]->GetEntropy()<<endl;
  fStat<<"Entropy of one Sample bunch table..."<<table[2]->GetEntropy()<<endl;
  fStat<<"Entropy of Central Sample table....."<<table[3]->GetEntropy()<<endl;
  fStat<<"Entropy Border Samples table........"<<table[4]->GetEntropy()<<endl;
  fStat.close();
 
  if (fVerbose)
    cout<<"Tables filled \n";
  
  //Frequencies normalization
  table[0]->NormalizeFrequencies();
  table[1]->NormalizeFrequencies();
  table[2]->NormalizeFrequencies();
  table[3]->NormalizeFrequencies();
  table[4]->NormalizeFrequencies();
  
  //Tables are saved in a sequence of text file and using the macro Histo.C is it possible to get
  //a series of histograms rappresenting the frequency distribution
  table[0]->StoreFrequencies("BunchLenFreq.txt");
  table[1]->StoreFrequencies("TimeFreq.txt");
  table[2]->StoreFrequencies("Sample1Freq.txt");
  table[3]->StoreFrequencies("SCentralFreq.txt");
  table[4]->StoreFrequencies("SBorderFreq.txt");
  if (fVerbose)
    cout<<"Creating Tables..\n";
  //One Huffman tree is created for each table starting from the frequencies of the symbols
  for(Int_t i=0;i<NumTables;i++){
    table[i]->BuildHTable();
    if (fVerbose==2){
      cout<<"Number of elements inside the table:"<<table[i]->GetWordsNumber();
      switch(i){
      case 0:{
	cout<<" (Bunch Length)"<<endl;
	break;
      }
      case 1:{
	cout<<" (Time Bin)"<<endl;
	break;
      }
      case 2:{
	cout<<" (1 Samples Bunch)"<<endl;
	break;
      }
      case 3:{
	cout<<" (Central Samples)"<<endl;
	break;
      }
      case 4:{
	cout<<" (Border Samples)"<<endl;
	break;
      }
      }//end switch
      table[i]->PrintTable();
    }
  }
  //The tables are saved ad binary files
  StoreTables(table,NumTables);
  //The tables stored in memory are deleted; 
  for(Int_t i=0;i<NumTables;i++)delete table[i];
  delete [] table;
  return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::RetrieveTables(AliTPCHTable* table[],Int_t NumTable){
  //This method retrieve the Huffman tables from a sequence of binary files
  if (fVerbose)
    cout<<"Retrieving tables from files \n";
  //  ULong_t code;
  Double_t code;
  UChar_t codeLen;
  ifstream fTable;  
  char filename[15];
  //The following for loop is used to generate the Huffman trees acording to the tables
  for(Int_t k=0;k<NumTable;k++){
    Int_t dim;//this variable contains the table dimension
    sprintf(filename,"Table%d.dat",k); 
#ifndef __DECCXX 
    fTable.open(filename,ios::binary);
#else
    fTable.open(filename);
#endif
    if(!fTable){cout<<"File doesn't exist:"<<filename<<endl; exit(1);}
    fTable.read((char*)(&dim),sizeof(Int_t));
    if (fVerbose)
      cout<<"Table dimension: "<<dim<<endl;
    table[k]=new AliTPCHTable(dim);
    for(Int_t i=0;i<dim;i++){
      fTable.read((char*)(&codeLen),sizeof(UChar_t));
      table[k]->SetCodeLen(codeLen,i);
      //      fTable.read((char*)(&code),sizeof(ULong_t));
      fTable.read((char*)(&code),sizeof(Double_t));
      table[k]->SetCode(Mirror((ULong_t)code,codeLen),i);
    }//end for 
    fTable.close();
  }//end for 
  if (fVerbose)
    cout<<"Trees generated \n";
  //At this point the trees are been built
  return 0;
}

Int_t AliTPCCompression::CreateTablesFromTxtFiles(Int_t NumTable){
  //This method creates a set of binary tables, needed by the Huffman
  //algorith, starting from a set of frequencies tables stored in form of
  //txt files
  if (fVerbose)
    cout<<"Retrieving frequencies from txt files \n";
  ifstream fTable;  
  char filename[15];
  //Tables are read from the files (Each codeword has been "Mirrored")
  AliTPCHTable **table = new AliTPCHTable*[NumTable];
  for(Int_t k=0;k<NumTable;k++){
    sprintf(filename,"Table%d.txt",k); 
    cout<<filename<<endl;
    fTable.open(filename);
    if(!fTable){cout<<"File doesn't exist: "<<filename<<endl; exit(1);}
    Int_t symbol=0;
    Double_t freq=0;
    table[k]=new AliTPCHTable(1024);
    while(!fTable.eof()){
      fTable>>freq;
      if (fTable.good()){
        if (freq<0){
	  cout<<"Frequency cannot be negative !!!\n";
	  exit(1);
	}
	table[k]->SetValFrequency(symbol,freq);
      }
      symbol++;
    }//end while
    fTable.clear();
    fTable.close();
  }//end for
  fStat.open("Statistics",ios::app);
  fStat<<endl;
  fStat<<"----------------- ENTROPY for external txt tables --------------------------"<<endl;
  fStat<<"Entropy of Bunch length table......."<<table[0]->GetEntropy()<<endl;
  fStat<<"Entropy of Time bin table..........."<<table[1]->GetEntropy()<<endl;
  fStat<<"Entropy of one Sample bunch table..."<<table[2]->GetEntropy()<<endl;
  fStat<<"Entropy of Central Sample table....."<<table[3]->GetEntropy()<<endl;
  fStat<<"Entropy Border Samples table........"<<table[4]->GetEntropy()<<endl;
  fStat.close();
  for(Int_t k=0;k<NumTable;k++){
    table[k]->BuildHTable();
  }//end for
  //The tables are saved ad binary files
  StoreTables(table,NumTable);  
  //The tables stored in memory are deleted; 
  for(Int_t i=0;i<NumTable;i++)delete table[i];
  delete [] table;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
/*                               COMPRESSION                                          */
////////////////////////////////////////////////////////////////////////////////////////

void AliTPCCompression::StoreValue(ULong_t val,UChar_t len){
  //This method stores the value "val" of "len" bits into the internal buffer "fBuffer"
  if (len<=fFreeBitsBuffer){           // val is not splitted in two buffer
    fFreeBitsBuffer-=len;
    fBuffer=fBuffer<<len;
    fBuffer=fBuffer|val;    
    if(!fFreeBitsBuffer){              // if the buffer is full it is written into a file 
      f.write((char*)(&fBuffer),sizeof(ULong_t));	
      fFreeBitsBuffer=fDimBuffer;
      fBuffer=0;
    }
  }//end if
  else{                               //val has to be splitted in two buffers
    fBuffer=fBuffer<<fFreeBitsBuffer;
    ULong_t temp;
    temp=val;
    temp=temp>>(len-fFreeBitsBuffer);
    fBuffer=fBuffer|temp;
    f.write((char*)(&fBuffer),sizeof(ULong_t));
    fFreeBitsBuffer=fDimBuffer-(len-fFreeBitsBuffer);
    val=val<<fFreeBitsBuffer;
    val=val>>fFreeBitsBuffer;
    fBuffer=val;
  }//end else
  return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::Flush(){
  //The last buffer cannot be completely full so to save it 
  //into the output file it is first necessary to fill it with an hexadecimal pattern
  if(fFreeBitsBuffer<fDimBuffer){
    fBuffer=fBuffer<<fFreeBitsBuffer;
    f.write((char*)(&fBuffer),sizeof(ULong_t));	 
  }//end if
  return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
ULong_t AliTPCCompression::Mirror(ULong_t val,UChar_t len)const{
  //This method inverts the digits of the number "val" and length "len"
  //indicates the number of digits of the number considered in binary notation
  ULong_t specular=0;
  ULong_t mask=0x1;
  ULong_t bit;
  for(Int_t i=0;i<len;i++){
    bit=val&mask;
    bit=bit>>i;
    specular=specular<<1;
    specular=specular|bit;
    mask=mask<<1;
  }
  return specular;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::CompressData(AliTPCHTable* table[],Int_t NumTable,const char* fSource,const char* fDest){
  //This method is used to compress the data stored in the Altro format file using specific tables
  //calculated considering the frequencies of the symbol of the file that has to be compressed
  cout<<" COMPRESSION "<<endl;
  cout<<"compression of the file "<<fSource<<" Output File: "<<fDest<<endl;
  //the output file is open
#ifndef __DECCXX 
  f.open(fDest,ios::binary|ios::out);
#else
  f.open(fDest,ios::out);
#endif
  //Tables are written into the output file
  for(Int_t k=0;k<NumTable;k++){
    Int_t dim=table[k]->Size();
    //Table dimension is written into a file
    f.write((char*)(&dim),sizeof(Int_t));
    //One table is written into a file
    for(Int_t i=0;i<dim;i++){
      UChar_t codeLen=table[k]->CodeLen()[i];
      ULong_t code=(ULong_t)table[k]->Code()[i];
      f.write((char*)(&codeLen),sizeof(UChar_t));
      f.write((char*)(&code),sizeof(ULong_t));
    } //end for
  }//end for

  // Source file is open
  AliTPCBuffer160 buff(fSource,0);
  //coded words are written into the output file
  Int_t numWords,padNum,rowNum,secNum=0;
  ULong_t storedWords=0;
  Int_t value=0;
  ULong_t numPackets=0;
  while(buff.ReadTrailerBackward(numWords,padNum,rowNum,secNum) !=-1 ){
    numPackets++;
    if (numWords%4){
      for(Int_t j=0;j<(4-numWords%4);j++){
	value=buff.GetNextBackWord();
      }//end for
    }//end if

    Int_t packet[1024];
    Int_t timePos[345];
    Int_t tp=0;
    for(Int_t i=0;i<345;i++)timePos[i]=0;
    for(Int_t i=0;i<1024;i++)packet[i]=0;

    Int_t nextTableType=0;
    Int_t bunchLen=0;
    Int_t count=0;
    for(Int_t i=0;i<numWords;i++){
      value=buff.GetNextBackWord();
      packet[i]=value;
      if(nextTableType==1){
	timePos[tp]=i;
	tp++;
      }
      NextTable(value,nextTableType,bunchLen,count);
    }//end for
    //computing the Time gap between two bunches
    Int_t temp=0;
    tp--;
    Int_t previousTime=packet[timePos[tp]];
    for(Int_t i=tp-1;i>=0;i--){
      Int_t timPos=timePos[i];
      Int_t bunchLen=packet[timPos-1]-2;
      temp=packet[timPos];
      packet[timPos]=packet[timPos]-previousTime-bunchLen;
      previousTime=temp;
    }//end for
    nextTableType=0;
    count=0;
    bunchLen=0;
    Int_t timeBin=0;
    //All the words for one pad are compressed and stored in the compress file
    for(Int_t i=0;i<numWords;i++){
      value=packet[i];
      if(nextTableType==1)timeBin=value;
      if(nextTableType>1){
	//	ULong_t val=(ULong_t)table[nextTableType]->Code()[value];     // val is the code
	Double_t val=table[nextTableType]->Code()[value];     // val is the code
	UChar_t len=table[nextTableType]->CodeLen()[value];  // len is the length (number of bits)of val
	StoreValue(Mirror((ULong_t)val,len),len);
	storedWords++;
      }//end if
      NextTable(value,nextTableType,bunchLen,count);
      if(nextTableType==0){
	//	ULong_t val=(ULong_t)table[1]->Code()[timeBin];     // val is the code
	Double_t val=table[1]->Code()[timeBin];     // val is the code
	UChar_t len=table[1]->CodeLen()[timeBin];  // len is the length (number of bits)of val
	StoreValue(Mirror((ULong_t)val,len),len);
	//val=(ULong_t)table[nextTableType]->Code()[(bunchLen+2)];     // val is the code
	val=table[nextTableType]->Code()[(bunchLen+2)];     // val is the code
	len=table[nextTableType]->CodeLen()[(bunchLen+2)];  // len is the length (number of bits)of val
	StoreValue(Mirror((ULong_t)val,len),len);
	storedWords+=2;
      }
    }//end for
    //Trailer
    StoreValue(numWords,10);
    StoreValue(padNum,10);
    StoreValue(rowNum,10);
    StoreValue(secNum,9);
    StoreValue(1,1);
    storedWords+=4;
  }//end  while
  StoreValue(numPackets,32);
  cout<<"Number of strored packet: "<<numPackets<<endl;
  StoreValue(1,1);
  //The last buffen cannot be completely full
  Flush();
  cout<<"Number of stored words: "<<storedWords<<endl;
  f.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::CompressDataOptTables(Int_t NumTable,const char* fSource,const char* fDest){
  //This method compress an Altro format file using a general set of tables stored as binary files to be provided
  if (fVerbose){
    cout<<" BackWord COMPRESSION "<<endl;
    cout<<"compression of the file "<<fSource<<" Output File: "<<fDest<<endl;
  }
  //Tables are read from the files (Each codeword has been "Mirrored")
  AliTPCHTable **table = new AliTPCHTable*[NumTable];
  RetrieveTables(table,NumTable);
  //the output file is open
#ifndef __DECCXX 
  f.open(fDest,ios::binary|ios::out);
#else
  f.open(fDest,ios::out);
#endif
  // Source file is open
  AliTPCBuffer160 buff(fSource,0);
  //coded words are written into a file
  Int_t numWords,padNum,rowNum,secNum=0;
  ULong_t  storedWords=0;
  Int_t    value=0;
  ULong_t  numPackets=0;
  Double_t stat[5]={0.,0.,0.,0.,0.};
  ULong_t  trailerNumbers=0;
  Double_t numElem[5]={0,0,0,0,0};
  Double_t fillWords=0.;
  fStat.open("Statistics",ios::app);
  fStat<<endl;
  fStat<<"-------------------COMPRESSION STATISTICS----------"<<endl;
  Int_t end=1;
  while(buff.ReadTrailerBackward(numWords,padNum,rowNum,secNum) !=-1 ){
    if(end){
      fillWords=buff.GetFillWordsNum();
      end=0;
    }//endif

    numPackets++;
    if (numWords%4){
      fillWords+=4-numWords%4;
      for(Int_t j=0;j<(4-numWords%4);j++){
	value=buff.GetNextBackWord();
      }//end for
    }//end if

    Int_t packet[1024];
    Int_t timePos[345];
    Int_t tp=0;
    for(Int_t i=0;i<345;i++)timePos[i]=0;
    for(Int_t i=0;i<1024;i++)packet[i]=0;

    Int_t nextTableType=0;
    Int_t bunchLen=0;
    Int_t count=0;
    for(Int_t i=0;i<numWords;i++){
      value=buff.GetNextBackWord();
      packet[i]=value;
      if(nextTableType==1){
	timePos[tp]=i;
	tp++;
      }
      NextTable(value,nextTableType,bunchLen,count);
    }//end for
    //computing the Time gap between two bunches
    Int_t temp=0;
    tp--;
    Int_t previousTime=packet[timePos[tp]];
    for(Int_t i=tp-1;i>=0;i--){
      Int_t timPos=timePos[i];
      Int_t bunchLen=packet[timPos-1]-2;
      temp=packet[timPos];
      packet[timPos]=packet[timPos]-previousTime-bunchLen;
      previousTime=temp;
    }//end for

    nextTableType=0;
    count=0;
    bunchLen=0;
    Int_t timeBin=0;
    for(Int_t i=0;i<numWords;i++){
      value=packet[i];
      if(nextTableType==1)timeBin=value;
      if(nextTableType>1){
	//ULong_t val=(ULong_t)table[nextTableType]->Code()[value];     // val is the code
	Double_t val=table[nextTableType]->Code()[value];     // val is the code
	UChar_t len=table[nextTableType]->CodeLen()[value];  // len is the length (number of bits)of val
	stat[nextTableType]+=len;
	numElem[nextTableType]++;
	StoreValue((ULong_t)val,len);
	storedWords++;
      }//end if
      NextTable(value,nextTableType,bunchLen,count);
      if(nextTableType==0){
	//	ULong_t val=(ULong_t)table[1]->Code()[timeBin];     // val is the code
	Double_t val=table[1]->Code()[timeBin];     // val is the code
	UChar_t len=table[1]->CodeLen()[timeBin];  // len is the length (number of bits)of val
	stat[1]+=len;
	numElem[1]++;
	StoreValue((ULong_t)val,len);
	//	val=(ULong_t)table[nextTableType]->Code()[(bunchLen+2)];     // val is the code
	val=table[nextTableType]->Code()[(bunchLen+2)];     // val is the code
	len=table[nextTableType]->CodeLen()[(bunchLen+2)];  // len is the length (number of bits)of val
	StoreValue((ULong_t)val,len);
	stat[nextTableType]+=len;
	numElem[nextTableType]++;
	storedWords+=2;
      }
    }//end for
    //Trailer
    StoreValue(numWords,10);
    StoreValue(padNum,10);
    StoreValue(rowNum,10);
    StoreValue(secNum,9);
    StoreValue(1,1);
    storedWords+=4;
    trailerNumbers++;
  }//end  while
  StoreValue(numPackets,32);
  if(fVerbose)
    cout<<"Number of strored packets: "<<numPackets<<endl;
  StoreValue(1,1);
  //The last buffen cannot be completely full
  Flush();
  if(fVerbose)
    cout<<"Number of stored words: "<<storedWords<<endl;
  f.close();
  //Tables are deleted
  for(Int_t i=0;i<NumTable;i++){
    delete table[i];
  }//end for
  delete [] table;
  Double_t dimension=(ULong_t)TMath::Ceil((stat[0]+stat[1]+stat[2]+stat[3]+stat[4])/8)+trailerNumbers*5;
  fStat<<"Trailer Dimension in bytes......"<<trailerNumbers*5<<endl;
  fStat<<"Data Dimension in bytes........."<<(ULong_t)TMath::Ceil((stat[0]+stat[1]+stat[2]+stat[3]+stat[4])/8)<<endl;
  fStat<<"Compressed file dimension......."<<(ULong_t)dimension<<endl;
  /*
  fStat<<(ULong_t)trailerNumbers<<endl;
  fStat<<(ULong_t)fillWords<<endl;
  fStat<<(ULong_t)numElem[0]<<endl;
  fStat<<(ULong_t)numElem[1]<<endl;
  fStat<<(ULong_t)numElem[2]<<endl;
  fStat<<(ULong_t)numElem[3]<<endl;
  fStat<<(ULong_t)numElem[4]<<endl;
  */
  fillWords=(fillWords+numElem[0]+numElem[1]+numElem[2]+numElem[3]+numElem[4]+trailerNumbers*4)*10/8;
  fStat<<"Original file dimension........."<<(ULong_t)fillWords<<endl;

  Double_t ratio=(dimension/fillWords)*100;
  fStat<<"Compression ratio (Compressed/Uncompressed)..."<<ratio<<"%"<<endl;
  fStat<<endl;
  if (numElem[0])
    fStat<<"Bunch length size in bytes......"<<(ULong_t)TMath::Ceil(stat[0]/8)<<" Comppression.."<<(stat[0]/numElem[0])*10<<"%"<<endl;
  if (numElem[1])  
    fStat<<"Time gap size in bytes.........."<<(ULong_t)TMath::Ceil(stat[1]/8)<<" Comppression.."<<(stat[1]/numElem[1])*10<<"%"<<endl;
  if (numElem[2]+numElem[3]+numElem[4])  
    fStat<<"Amplitude values in bytes......."<<(ULong_t)TMath::Ceil((stat[2]+stat[3]+stat[4])/8)<<" Comppression.."<<
      ((stat[2]+stat[3]+stat[4])/(numElem[2]+numElem[3]+numElem[4]))*10<<"%"<<endl;
  if (numElem[2])
  fStat<<"     One Samples in bytes............"<<(ULong_t)TMath::Ceil(stat[2]/8)<<" Comppression.."<<(stat[2]/numElem[2])*10<<"%"<<endl;
  if (numElem[3])
  fStat<<"     Central Samples size in bytes..."<<(ULong_t)TMath::Ceil(stat[3]/8)<<" Comppression.."<<(stat[3]/numElem[3])*10<<"%"<<endl;
  if (numElem[4])
  fStat<<"     Border Samples size in bytes...."<<(ULong_t)TMath::Ceil(stat[4]/8)<<" Comppression.."<<(stat[4]/numElem[4])*10<<"%"<<endl;
  fStat<<endl;
  fStat<<"Average number of bits per word"<<endl;
  if (numElem[0])
    fStat<<"Bunch length ......"<<stat[0]/numElem[0]<<endl;
  if (numElem[1])
    fStat<<"Time gap .........."<<stat[1]/numElem[1]<<endl;
  if (numElem[2])
    fStat<<"One Samples........"<<stat[2]/numElem[2]<<endl;
  if (numElem[3])
    fStat<<"Central Samples ..."<<stat[3]/numElem[3]<<endl;
  if (numElem[4])
    fStat<<"Border Samples....."<<stat[4]/numElem[4]<<endl;
  fStat.close();
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
/*                               DECOMPRESSION                                        */
////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::CreateTrees(AliTPCHNode *RootNode[],const Int_t NumTables){
  //The first part of the compressed file cotains the tables
  //The following for loop is used to generate the Huffman trees acording to the tables
  if(fVerbose)
    cout<<"Creating the Huffman trees \n";
  AliTPCHNode *node=0;
  //  ULong_t code;
  Double_t code;
  UChar_t codeLen;
  //loop over the numbero of tables
  for(Int_t k=0;k<NumTables;k++){
    RootNode[k]=new AliTPCHNode(); //RootNode is the root of the tree
    Int_t dim;//this variable contains the table dimension
    f.read((char*)(&dim),sizeof(Int_t));
    if (fVerbose)
      cout<<"Table dimension: "<<dim<<endl;
    //loop over the words of a table
    for(Int_t i=0;i<dim;i++){
      f.read((char*)(&codeLen),sizeof(UChar_t));
      //f.read((char*)(&code),sizeof(ULong_t));
      f.read((char*)(&code),sizeof(Double_t));
      node=RootNode[k];
      for(Int_t j=1;j<=codeLen;j++){
	ULong_t bit,val=0;
	val=(ULong_t)TMath::Power(2,codeLen-j);
	bit=(ULong_t)code&val; 
	AliTPCHNode *temp=node;
	if(bit){
	  node=node->GetRight();
	  if(!node){
	    node=new AliTPCHNode();
	    temp->SetRight(node);
	  }//end if
	}//end if
	else{
	  node=node->GetLeft();
	  if(!node){
	    node=new AliTPCHNode();
	    temp->SetLeft(node);
	  }//end if
	}//end else
      }//end for
      if(codeLen){
	node->SetSymbol(i);
	node->SetFrequency(codeLen);
      }//end if
      //cout<<node->GetSymbol()<<"  "<<(Int_t)node->GetFrequency()<<endl;
    }//end for 
  }//end for 
  if (fVerbose)
    cout<<"Trees generated \n";
  //At this point the trees are been built
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::CreateTreesFromFile(AliTPCHNode *RootNode[],const Int_t NumTables){
  //For each table this method builds the associate Huffman tree starting from the codeword and 
  //the codelength of each symbol 
  if(fVerbose)
    cout<<"Creating the Huffman trees \n";
  AliTPCHNode *node=0;
  // ULong_t code;
  Double_t code;
  UChar_t codeLen;
  ifstream fTable;  
  char filename[15];
  //The following for loop is used to generate the Huffman trees acording to the tables
  //loop over the tables
  for(Int_t k=0;k<NumTables;k++){
    RootNode[k]=new AliTPCHNode(); //RootNode is the root of the tree
    Int_t dim=0;//this variable contains the table dimension
    sprintf(filename,"Table%d.dat",k); 
#ifndef __DECCXX 
    fTable.open(filename,ios::binary);
#else
    fTable.open(filename);
#endif
    fTable.read((char*)(&dim),sizeof(Int_t));
    if (fVerbose)
      cout<<"Table dimension: "<<dim<<endl;
    //loop over the words of one table
    for(Int_t i=0;i<dim;i++){
      fTable.read((char*)(&codeLen),sizeof(UChar_t));
      //fTable.read((char*)(&code),sizeof(ULong_t));
      fTable.read((char*)(&code),sizeof(Double_t));
      node=RootNode[k];
      for(Int_t j=1;j<=codeLen;j++){
	ULong_t bit,val=0;
	val=(ULong_t)TMath::Power(2,codeLen-j);
	bit=(ULong_t)code&val; 
	AliTPCHNode *temp=node;
	if(bit){
	  node=node->GetRight();
	  if(!node){
	    node=new AliTPCHNode();
	    temp->SetRight(node);
	  }//end if 
	}//end if
	else{
	  node=node->GetLeft();
	  if(!node){
	    node=new AliTPCHNode();
	    temp->SetLeft(node);
	  }//end if
	}//end else
      }//end for
      if(codeLen){
	node->SetSymbol(i);
	node->SetFrequency(codeLen);
      }//end if
    }//end for 
    fTable.close();
  }//end for 
  if (fVerbose)
    cout<<"Trees generated \n";
  //At this point the trees are been built
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::DeleteHuffmanTree(AliTPCHNode* node){
  //This function deletes all the nodes of an Huffman tree
  //In an Huffman tree any internal node has always two children 
  if (node){
    DeleteHuffmanTree(node->GetLeft());
    DeleteHuffmanTree(node->GetRight());
    //    cout<<node->GetSymbol()<<"  "<<(Int_t)node->GetFrequency()<<endl;
    delete node;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::VisitHuffmanTree(AliTPCHNode* node){
  //This function realizes an in order visit of a binary tree 
  if (node){
    cout<<node->GetSymbol()<<" "<<node->GetFrequency()<<endl;
    VisitHuffmanTree(node->GetLeft());
    VisitHuffmanTree(node->GetRight());
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
ULong_t AliTPCCompression::ReadWord(Int_t NumberOfBit){
  //This method retrieves a word of a specific number of bits from the file through the internal buffer 
  ULong_t result=0;
  ULong_t bit=0;
  for (Int_t i=0;i<NumberOfBit;i++){
    if (fReadBits==32){
      fPos-=sizeof(ULong_t);
      f.seekg(fPos);
      f.read((char*)(&fBuffer),sizeof(ULong_t));
      fReadBits=0;
    }//end if
    ULong_t mask=0;
    mask=(ULong_t)TMath::Power(2,fReadBits);
    bit=fBuffer&mask;
    bit=bit>>fReadBits;
    fReadBits++;
    bit=bit<<i;
    result=result|bit;
  }//end for
  return result;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void AliTPCCompression::ReadTrailer(Int_t &WordsNumber,Int_t &PadNumber,Int_t &RowNumber,Int_t &SecNumber){
  //It retrieves a trailer 
  ReadWord(1);
  SecNumber=ReadWord(9);
  RowNumber=ReadWord(10);
  PadNumber=ReadWord(10);
  WordsNumber=ReadWord(10);
  return;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
ULong_t AliTPCCompression::GetDecodedWord(AliTPCHNode* root){
  //This method retrieves a decoded word.
  AliTPCHNode *node=root;
  ULong_t symbol=0;
  Bool_t decoded=0;
  while(!decoded){
    ULong_t bit=ReadWord(1);
    if(bit)
      node=node->GetRight();
    else
      node=node->GetLeft();
    if (!(node->GetLeft())){
      symbol=node->GetSymbol();
      decoded=1;
    }
  }//end while
  return symbol;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AliTPCCompression::DecompressData(Int_t NumTables,const char* fname,char* fDest){
  //Decompression method 
  cout<<"   DECOMPRESSION:"<<endl;
  cout<<"Source File "<<fname<<" Destination File "<<fDest<<endl;
#ifndef __DECCXX 
  f.open(fname,ios::binary|ios::in);
#else
  f.open(fname,ios::in);
#endif
  if(!f){cout<<"File doesn't exist:"<<fname<<endl;;return -1;}
  AliTPCHNode ** rootNode = new AliTPCHNode*[NumTables];
  //Creation of the Huffman trees
  CreateTrees(rootNode,NumTables);
  //to go to the end of the file
  f.seekg(0,ios::end);
  //to get the file dimension in byte
  fPos=f.tellg();
  fPos-=sizeof(ULong_t);
  f.seekg(fPos);
  fReadBits=0;
  fBuffer=0;
  f.read((char*)(&fBuffer),sizeof(ULong_t));
  Int_t bit=0;
  ULong_t mask=0x1;
  while(!bit){
    bit=fBuffer&mask;
    mask=mask<<1;
    fReadBits++;
  }
  ULong_t packetNumber=ReadWord(sizeof(ULong_t)*8);
  cout<<"Number of Packect: "<<packetNumber<<endl;
  AliTPCBuffer160 bufferFile(fDest,1);
  ULong_t k=0;
  ULong_t wordsRead=0; //number of read coded words 
  while(k<packetNumber){
    Int_t numWords,padNumber,rowNumber,secNumber=0;
    ReadTrailer(numWords,padNumber,rowNumber,secNumber);
    k++;
    wordsRead+=4;
    Int_t previousTime=-1;
    Int_t time=0;
    Int_t nextTableType=0;
    Int_t bunchLen=0;
    Int_t count=0;
    for(Int_t i=0;i<numWords;i++){
      ULong_t symbol=GetDecodedWord(rootNode[nextTableType]);
      wordsRead++;
      //Time reconstruction
      if (nextTableType==1){
	if (previousTime!=-1){
	  previousTime=symbol+previousTime+bunchLen;
	}
	else previousTime=symbol;
	time=previousTime;
      }
      if(nextTableType>1)
	bufferFile.FillBuffer(symbol);
      NextTable(symbol,nextTableType,bunchLen,count); 
      if(nextTableType==0){
	bufferFile.FillBuffer(time);
	bufferFile.FillBuffer(bunchLen+2);
	bunchLen=0;
      }
    }//end for
    bufferFile.WriteTrailer(numWords,padNumber,rowNumber,secNumber);
  }//end while
  cout<<"Number of decoded words:"<<wordsRead<<endl;
  f.close();
  //The trees are deleted 
  for(Int_t j=0;j<NumTables;j++){
      DeleteHuffmanTree(rootNode[j]);
  }//end for
  delete [] rootNode; 
  return 0; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Int_t AliTPCCompression::DecompressDataOptTables(Int_t NumTables,const char* fname,char* fDest){
  //This method decompress a file using separate Huffman tables
  if(fVerbose){
    cout<<"   DECOMPRESSION:"<<endl;
    cout<<"Source File "<<fname<<" Destination File "<<fDest<<endl; 
  }
  AliTPCHNode ** rootNode = new AliTPCHNode*[NumTables];
  //Creation of the Huffman trees
  CreateTreesFromFile(rootNode,NumTables);
#ifndef __DECCXX
  f.open(fname,ios::binary|ios::in);
#else
  f.open(fname,ios::in);
#endif
  if(!f){cout<<"File doesn't exist:"<<fname<<endl;;return -1;}
  //to go to the end of the file
  f.seekg(0,ios::end);
  //to get the file dimension in byte
  fPos=f.tellg();
  fPos-=sizeof(ULong_t);
  f.seekg(fPos);
  fReadBits=0;
  fBuffer=0;
  f.read((char*)(&fBuffer),sizeof(ULong_t));
  Int_t bit=0;
  ULong_t mask=0x1;
  while(!bit){
    bit=fBuffer&mask;
    mask=mask<<1;
    fReadBits++;
  }
  ULong_t packetNumber=ReadWord(sizeof(ULong_t)*8);
  if(fVerbose){
    cout<<"Number of Packect: "<<packetNumber<<endl;
  }
  AliTPCBuffer160 bufferFile(fDest,1);
  ULong_t k=0;
  ULong_t wordsRead=0; //number of read coded words 
  while(k<packetNumber){
    Int_t numWords,padNumber,rowNumber,secNumber=0;
    ReadTrailer(numWords,padNumber,rowNumber,secNumber);
    k++;
    wordsRead+=4;
    Int_t previousTime=-1;
    Int_t time=0;
    Int_t nextTableType=0;
    Int_t bunchLen=0;
    Int_t count=0;
    for(Int_t i=0;i<numWords;i++){
      ULong_t symbol=GetDecodedWord(rootNode[nextTableType]);
      wordsRead++;
      //Time reconstruction
      if (nextTableType==1){
	if (previousTime!=-1){
	  previousTime=symbol+previousTime+bunchLen;
	}
	else previousTime=symbol;
	time=previousTime;
      }
      if(nextTableType>1)
	bufferFile.FillBuffer(symbol);
      NextTable(symbol,nextTableType,bunchLen,count); 
      if(nextTableType==0){
	bufferFile.FillBuffer(time);
	bufferFile.FillBuffer(bunchLen+2);
	bunchLen=0;
      }
    }//end for
    bufferFile.WriteTrailer(numWords,padNumber,rowNumber,secNumber);
  }//end while
  if(fVerbose){
    cout<<"Number of decoded words:"<<wordsRead<<endl;
  }
  f.close();
  //The trees are deleted 
  for(Int_t j=0;j<NumTables;j++){
      DeleteHuffmanTree(rootNode[j]);
  }//end for
  delete [] rootNode;
  return 0; 
}

///////////////////////////////////////////////////////////////////////////////////////////

void AliTPCCompression::ReadAltroFormat(char* fileOut,char* fileIn)const{
  //This method creates a text file containing the same information stored in 
  //an Altro file. The information in the text file is organized pad by pad and 
  //and for each pad it consists in a sequence of bunches (Bunch length +2,
  //Time bin of the last amplitude sample in the bunch, amplitude values)
  //It is used mainly for debugging
  ofstream ftxt(fileOut);
  AliTPCBuffer160 buff(fileIn,0);
  Int_t numWords,padNum,rowNum,secNum=0;
  Int_t value=0;
  if (fVerbose) cout<<"Creating a txt file from an Altro Format file"<<endl;
  while(buff.ReadTrailerBackward(numWords,padNum,rowNum,secNum) !=-1 ){
    ftxt<<"S:"<<secNum<<" R:"<<rowNum<<" P:"<<padNum<<" W:"<<numWords<<endl;
    if (numWords%4){
      for(Int_t j=0;j<(4-numWords%4);j++){
	value=buff.GetNextBackWord();
      }//end for
    }//end if
    for(Int_t i=0;i<numWords;i++){
      value=buff.GetNextBackWord();
      ftxt<<value<<endl;
    }//end for
  }//end while
  ftxt.close();
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////
