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

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4Tokenizer.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det)
:G4UImessenger(), 
 fDetectorConstruction(Det)
{
  fSetupDir = new G4UIdirectory("/geometry/setup/");
  fSetupDir->SetGuidance("geometry settings concerning the experimental setup");

  fTU2Cmd = new G4UIcmdWithAString("/geometry/setup/TU2",this);
  fTU2Cmd->SetGuidance("specify construction of TU2 detector. Parameters: (X|Y|Z) Rotation/degrees translation(X Y Z)/mm");
  fTU2Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTU4Cmd = new G4UIcmdWithAString("/geometry/setup/TU4",this);
  fTU4Cmd->SetGuidance("specify construction of TU4 detector. Parameters: (X|Y|Z) Rotation/degrees translation(X Y Z)/mm");
  fTU4Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fTU5Cmd = new G4UIcmdWithAString("/geometry/setup/TU5",this);
  fTU5Cmd->SetGuidance("specify construction of TU2 detector. Parameters: (X|Y|Z) Rotation/degrees translation(X Y Z)/mm");
  fTU5Cmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fScintCmd = new G4UIcmdWithAString("/geometry/setup/scint",this);
  fScintCmd->SetGuidance("specify construction of scintillator detector. Parameters: (X|Y|Z) Rotation/degrees translation(X Y Z)/mm");
  fScintCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSampleCmd = new G4UIcmdWithAString("/geometry/setup/sample",this);
  fSampleCmd->SetGuidance("specify construction of As sample container. Parameters: (X|Y|Z) Rotation/degrees translation(X Y Z)/mm");
  fSampleCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fSetupDir;
  delete fTU2Cmd;
  delete fTU4Cmd;
  delete fTU5Cmd;
  delete fScintCmd;
  delete fSampleCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ // Given arguments will be found in the newValue string, otherwise it is empty.
  if (command == fTU2Cmd || command == fTU4Cmd
   || command == fTU5Cmd || command == fScintCmd
   || command == fSampleCmd)
  {
      // Find index of the part to be edited/removed
      G4int part_index = 0;
      if (command == fTU2Cmd)
        part_index = 0;
      if (command == fTU4Cmd)
        part_index = 1;
      if (command == fTU5Cmd)
        part_index = 2;
      if (command == fScintCmd)
        part_index = 3;
      if (command == fSampleCmd)
        part_index = 4;
      // With a simple argument "off", remove it
      if (newValue == "off") {
        fDetectorConstruction->SetRemovePart(part_index);
        return;
      }
      // Extract a string and four doubles following examples/advanced/underground_physics/src/DMXParticleSourceMessenger.cc
      G4Tokenizer next(newValue);
      G4String RotAxis = next();
      G4double Rotation = StoD(next());
      G4double TransX = StoD(next());
      G4double TransY = StoD(next());
      G4double TransZ = StoD(next());
      fDetectorConstruction->SetConstructPart(part_index, RotAxis, Rotation, TransX, TransY, TransZ);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
