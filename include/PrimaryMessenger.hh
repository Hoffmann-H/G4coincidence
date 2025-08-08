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
/// \file PrimaryMessenger.hh
/// \brief Definition of the PrimaryMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryMessenger_h
#define PrimaryMessenger_h 1

#include "TFile.h"

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGenerator;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryMessenger: public G4UImessenger
{
  public:
  
    PrimaryMessenger(PrimaryGenerator* Primary);
   ~PrimaryMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
  
    PrimaryGenerator*            fPrimary;
    
    G4UIdirectory*               fPrimaryDir;
    G4UIcmdWithADoubleAndUnit*   fGammaCmd;
    G4UIcmdWithAString*          fPrimaryIonCmd;
    G4UIcmdWithADouble*          fPrimaryECbranchingCmd;
    G4UIcmdWithADoubleAndUnit*   fDaughterExcitEnergyCmd;
    G4UIcmdWithADouble*          fSourceVolumeRadiusCmd;
    G4UIcmdWithADouble*          fSourceVolumeThicknessCmd;
    G4UIcmdWithADouble*          fSourceVolumeXcenterCmd;
    G4UIcmdWithADouble*          fSourceVolumeYcenterCmd;
    G4UIcmdWithADouble*          fSourceVolumeZcenterCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
