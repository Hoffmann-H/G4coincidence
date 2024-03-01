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
/// \file PrimaryMessenger.cc
/// \brief Implementation of the PrimaryMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryMessenger.hh"

#include "PrimaryGenerator.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4Tokenizer.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryMessenger::PrimaryMessenger(PrimaryGenerator* Primary)
:G4UImessenger(), 
 fPrimary(Primary)//, fPrimaryWriteCmd(nullptr), fFilenameCmd(nullptr)
{
  fPrimaryDir = new G4UIdirectory("/primary/");
  fPrimaryDir->SetGuidance("Primary particle settings");

  fPrimaryIonCmd = new G4UIcmdWithAString("/primary/ion",this);
  fPrimaryIonCmd->SetGuidance("define Z and A of primary ion");
  fPrimaryIonCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPrimaryECbranchingCmd = new G4UIcmdWithADouble("/primary/ECbranching",this);
  fPrimaryECbranchingCmd->SetGuidance("define branching ratio of hand-made EC");
  fPrimaryECbranchingCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryMessenger::~PrimaryMessenger()
{
  //delete fPrimaryWriteCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ // Given arguments will be found in the newValue string, otherwise it is empty.
  if (command == fPrimaryIonCmd)
  {
      // Extract two integers following examples/advanced/underground_physics/src/DMXParticleSourceMessenger.cc
      G4Tokenizer next(newValue);
      G4double Z = StoI(next());
      G4double A = StoI(next());
      fPrimary->SetIonZandA(Z, A);
  }
  if (command == fPrimaryECbranchingCmd)
  {
      fPrimary->SetECbranching(fPrimaryECbranchingCmd->GetNewDoubleValue(newValue));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
