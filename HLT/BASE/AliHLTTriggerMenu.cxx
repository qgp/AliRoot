// $Id$
/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Artur Szostak <artursz@iafrica.com>                   *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/// @file   AliHLTTriggerMenu.cxx
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   19 Dec 2008
/// @brief  Implementation of the AliHLTTriggerMenu base class.
///
/// The AliHLTTriggerMenu class implements the HLT global trigger menu,
/// which defines how and on what events the HLT triggers.

#include "AliHLTTriggerMenu.h"
#include "Riostream.h"

ClassImp(AliHLTTriggerMenu)


AliHLTTriggerMenu::AliHLTTriggerMenu() :
  TObject(),
  fName("Unknown"),
  fSymbols(AliHLTTriggerMenuSymbol::Class(), 100),
  fItems(AliHLTTriggerMenuItem::Class(), 100),
  fDefaultDescription(),
  fDefaultDomain(),
  fDefaultConditionOperator("||"),
  fDefaultDomainOperator("|")
{
  // Default constructor.
}


AliHLTTriggerMenu::~AliHLTTriggerMenu()
{
  // Default destructor.
}


AliHLTTriggerMenu::AliHLTTriggerMenu(const AliHLTTriggerMenu& obj) :
  TObject(obj),
  fName(obj.fName),
  fSymbols(AliHLTTriggerMenuSymbol::Class(), obj.fSymbols.GetEntriesFast()),
  fItems(AliHLTTriggerMenuItem::Class(), obj.fItems.GetEntriesFast()),
  fDefaultDescription(obj.fDefaultDescription),
  fDefaultDomain(obj.fDefaultDomain),
  fDefaultConditionOperator(obj.fDefaultConditionOperator),
  fDefaultDomainOperator(obj.fDefaultDomainOperator)
{
  // Copy constructor performs a deep copy.
  
  for (UInt_t i = 0; i < obj.NumberOfSymbols(); i++)
  {
    AddSymbol(*obj.Symbol(i));
  }
  for (UInt_t i = 0; i < obj.NumberOfItems(); i++)
  {
    AddItem(*obj.Item(i));
  }
}


AliHLTTriggerMenu& AliHLTTriggerMenu::operator = (const AliHLTTriggerMenu& obj)
{
  // Assignment operator performs a deep copy.
  
  if (this != &obj)
  {
    TObject::operator = (obj);
    fName = obj.fName;
    fSymbols.Clear();
    for (UInt_t i = 0; i < obj.NumberOfSymbols(); i++)
    {
      AddSymbol(*obj.Symbol(i));
    }
    fItems.Clear();
    for (UInt_t i = 0; i < obj.NumberOfItems(); i++)
    {
      AddItem(*obj.Item(i));
    }
    fDefaultDescription = obj.fDefaultDescription;
    fDefaultDomain = obj.fDefaultDomain;
    fDefaultConditionOperator = obj.fDefaultConditionOperator;
    fDefaultDomainOperator = obj.fDefaultDomainOperator;
  }
  return *this;
}


void AliHLTTriggerMenu::Print(Option_t* option) const
{
  // Prints the contents of the trigger menu.
  
  cout << "HLT Trigger Menu: " << fName.Data();
  TString opt = option;
  if (opt.Contains("short"))
  {
    cout << ", contains " << NumberOfItems() << " entries." << endl;
    return;
  }
  cout << endl;
  cout << setw(10) << "Prescalar" <<  " | "
       << setw(10) << "Priority" <<  " | "
       << setw(60) << "Trigger condition" << " | "
       << setw(60) << "Domain merge expression" << setw(0) << endl;
  cout << setfill('-') << setw(10) << "-" <<  "-+-" << setw(10) << "-" <<  "-+-"
       << setw(60) << "-" << "-+-"
       << setw(60) << "-" << setw(0) << setfill(' ') << endl;
  for (UInt_t i = 0; i < NumberOfItems(); i++)
  {
    Item(i)->Print("compact");
  }
  if (NumberOfItems() == 0) cout << "(none)" << endl;
  cout << "Symbol list:" << endl;
  cout << setw(15) << "Name"
       << " | " << setw(20) << "Data type"
       << " | " << setw(24) << "Block type & spec"
       << " | " << setw(20) << "Class name"
       << " | " << setw(25) << "Assigned value"
       << " | " << setw(25) << "Default value"
       << setw(0) << endl;
  cout << setw(15) << setfill('-') << "-"
       << "-+-" << setw(20) << "-"
       << "-+-" << setw(24) << "-"
       << "-+-" << setw(20) << "-"
       << "-+-" << setw(25) << "-"
       << "-+-" << setw(25) << "-"
       << setw(0) << setfill(' ') << endl;
  for (UInt_t i = 0; i < NumberOfSymbols(); i++)
  {
    Symbol(i)->Print("compact");
  }
  if (NumberOfSymbols() == 0) cout << "(none)" << endl;
  cout << "Default trigger condition operator: " << fDefaultConditionOperator << endl;
  cout << "   Default trigger domain operator: " << fDefaultDomainOperator << endl;
  cout << "       Default trigger description: \"" << fDefaultDescription << "\"" << endl;
  cout << "Default "; fDefaultDomain.Print();
}


void AliHLTTriggerMenu::Clear(Option_t* option)
{
  // Clears the internal symbol and items arrays.
  
  fSymbols.Clear(option);
  fItems.Clear(option);
}


void AliHLTTriggerMenu::AddSymbol(const AliHLTTriggerMenuSymbol& entry)
{
  // Adds a new symbol to the trigger menu.
  
  for (Int_t i = 0; i < fSymbols.GetEntriesFast(); i++)
  {
    const char* symbolname = static_cast<AliHLTTriggerMenuSymbol*>(fSymbols.UncheckedAt(i))->Name();
    if ( strcmp(symbolname, entry.Name()) == 0 )
    {
      Warning("AliHLTTriggerMenu::AddSymbol",
              "The symbol '%s' already exists in the trigger menu. Will not add the new entry.",
              entry.RealName()
             );
      return;
    }
  }
  new (fSymbols[fSymbols.GetEntriesFast()]) AliHLTTriggerMenuSymbol(entry);
}

