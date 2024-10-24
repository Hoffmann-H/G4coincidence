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

  fPlaceCmd = new G4UIcmdWithAString("/geometry/setup/place",this);
  fPlaceCmd->SetGuidance("specify construction of optional parts. They will be recognezed by name. Parameters: name (X|Y|Z) Rotation/degrees translation(X Y Z)/mm");
  fPlaceCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fSetupDir;
  delete fPlaceCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ // Given arguments will be found in the newValue string, otherwise it is empty.
  if (command == fPlaceCmd)
  {
      // Extract strings and doubles following examples/advanced/underground_physics/src/DMXParticleSourceMessenger.cc
      G4Tokenizer next(newValue);
      G4String partName = next();
      G4String RotAxis = next();
      G4double Rotation = StoD(next());
      G4double TransX = StoD(next());
      G4double TransY = StoD(next());
      G4double TransZ = StoD(next());
      fDetectorConstruction->SetConstructPart(partName,
 RotAxis, Rotation, TransX, TransY, TransZ);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
