//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file HistoMessenger.cc
/// \brief Implementation of the HistoMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoMessenger.hh"

#include "HistoManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoMessenger::HistoMessenger(HistoManager* histo)
:G4UImessenger(), 
 fHisto(histo)//, fHistoWriteCmd(nullptr), fFilenameCmd(nullptr)
{
  fHistoDir = new G4UIdirectory("/histo/");
  fHistoDir->SetGuidance("Histogram saving settings");

  //fHistoWriteCmd = new G4UIcmdWithABool("/histo/writehisto",this);
  //fHistoWriteCmd->SetGuidance("define if results will be saved as histogram");
  //fHistoWriteCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fFilenameCmd = new G4UIcmdWithAString("/histo/file",this);
  fFilenameCmd->SetGuidance("Specify the root file where to save results");
  fFilenameCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoMessenger::~HistoMessenger()
{
  //delete fHistoWriteCmd;
  delete fFilenameCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ // Given arguments will be found in the newValue string, otherwise it is empty.
  //if( command == fHistoWriteCmd )
  //{
  //    fHisto->SetWriteHisto(fHistoWriteCmd->GetNewBoolValue(newValue));
  //}
  if (command == fFilenameCmd)
  {
      G4cout << "DEBUG: Specifying results root file: " << newValue << G4endl;
      fHisto->SetResultsFile(newValue);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
