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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "HPGeSD.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction()
{
  fWorldSize = 1.0*m;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  //
  // define a material
  //   
  G4Material* Air =
  G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR"); 

  G4Material* mat_Ge = 
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Ge");
  
  G4Material* mat_Si = 
  G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
  
  //     
  // World
  //
  G4Box*  
  solidWorld = new G4Box("World",                          //its name
                   fWorldSize/2,fWorldSize/2,fWorldSize/2);//its size
                   
  G4LogicalVolume*                         
  logicWorld = new G4LogicalVolume(solidWorld,             //its solid
                                   Air,                    //its material
                                   "World");               //its name
  G4VPhysicalVolume*                                   
  physiWorld = new G4PVPlacement(0,                      //no rotation
                                 G4ThreeVector(),        //at (0,0,0)
                                 logicWorld,             //its logical volume
                                 "World",                //its name
                                 0,                      //its mother  volume
                                 false,                  //no boolean operation
                                 0);                     //copy number
  // 
  // radioactive sample
  //
//TODO
  // store source logical volume
  //fSourceVolume = logicSample;

  // 
  // HPGe
  //
  G4double HPGe_rmin = 0.0;
  G4double HPGe_rmax = 45.0 * mm;
  G4double HPGe_dz = 160.0 * mm;
  G4double HPGe_sphi = 0.0;
  G4double HPGe_dphi = 2*M_PI;
  G4ThreeVector* HPGe_trans = new G4ThreeVector(
    0, 0, -90.0*mm);
  G4Tubs* solidGe = new G4Tubs("solidHPGe",
    HPGe_rmin, HPGe_rmax, HPGe_dz/2, HPGe_sphi, HPGe_dphi);
  G4LogicalVolume* logicGe = new G4LogicalVolume(
    solidGe, mat_Ge, "logicHPGe");
  G4VPhysicalVolume* physiGe = new G4PVPlacement(
    0, *HPGe_trans, logicGe, "physiGe", logicWorld, false, 0);

  //
  // SDD
  //
  G4double SDD_rmin = 0.0;
  G4double SDD_rmax = 6.9*mm;
  G4double SDD_dz = 0.45*mm;
  G4double SDD_sphi = 0.0;
  G4double SDD_dphi = 2.0*M_PI;
  G4ThreeVector* SDD_trans = new G4ThreeVector(
    0, 0, 10.0*mm);
  G4Tubs* solidSi = new G4Tubs("solidSDD", 
    SDD_rmin, SDD_rmax, SDD_dz/2, SDD_sphi, SDD_dphi);
  G4LogicalVolume* logicSi = new G4LogicalVolume(
    solidSi, mat_Si, "logicSDD");
  G4VPhysicalVolume* physiSi = new G4PVPlacement(
    0, *SDD_trans, logicSi, "physiSi", logicWorld, false, 0);

  //
  //always return the physical World
  //  
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  G4String HPGeSDname = "HPGeSD";
  HPGeSD* aHPGeSD = new HPGeSD(HPGeSDname, "HPGeHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aHPGeSD);
  // Setting SD to a logical volume by
  SetSensitiveDetector("logicHPGe", aHPGeSD, true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
