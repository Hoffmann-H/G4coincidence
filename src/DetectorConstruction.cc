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
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "HPGeSD.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4Vector3D.hh"
#include "G4Point3D.hh"

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

void DetectorConstruction::BuildMaterials()
{ // Define materials via NIST manager
  auto nist = G4NistManager::Instance();
  mat["Ge"] = nist->FindOrBuildMaterial("G4_Ge");
  mat["alu"] = nist->FindOrBuildMaterial("G4_Al");
  mat["Si"] = nist->FindOrBuildMaterial("G4_Si");
  mat["Be"] = nist->FindOrBuildMaterial("G4_Be");
  mat["air"] = nist->FindOrBuildMaterial("G4_AIR");
  mat["Pb"] = nist->FindOrBuildMaterial("G4_Pb");
  mat["concrete"] = nist->FindOrBuildMaterial("G4_CONCRETE");
  mat["PMMA"] = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
  mat["Cu"] = nist->FindOrBuildMaterial("G4_Cu");
  //G4Ele
  //TODO: peltier, tantalum collimator
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructXrayDetector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the SDD X-ray detector, 
// with frontent point at given frontend_position and given rotation. 
// frontend point := intersection of sensor rotation axis with front layer of housing
{
  // Si sensitive part
  G4double sens_depth = 3.94*mm; // distance between front layer of housing and front layer of Si sensor
  G4double sens_dz = 0.45*mm;
  G4double sens_rmin = 0.0*mm;
  G4double sens_rmax = sqrt(150.0/M_PI)*mm; // area = 150mm^2 = pi*r^2
  G4double sens_sphi = 0.0;
  G4double sens_dphi = 2.0*M_PI;
  G4Transform3D sens_internal_rotate = G4Rotate3D();
  G4Transform3D sens_internal_translate = G4Translate3D(0.0, 0.0, -sens_depth-0.5*sens_dz);
  G4Transform3D sens_total_transform = global_transform*sens_internal_translate*sens_internal_rotate;
  
  // Be entrance window
  G4double window_dz = 0.025*mm;
  G4double window_rmin = sens_rmin;
  G4double window_rmax = sens_rmax;
  G4double window_sphi = sens_sphi;
  G4double window_dphi = sens_dphi;
  G4Transform3D window_internal_rotate = G4Rotate3D();
  G4Transform3D window_internal_translate = G4Translate3D(0.0, 0.0, -sens_depth+0.5*window_dz);
  G4Transform3D window_total_transform = global_transform*window_internal_translate*window_internal_rotate;

  G4Tubs* sens_solid = new G4Tubs("solidSDD", sens_rmin, sens_rmax, 0.5*sens_dz, sens_sphi, sens_dphi);
  G4LogicalVolume *sens_logical = new G4LogicalVolume(sens_solid, mat["Si"], "logicSDD");
  sens_logical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  new G4PVPlacement(sens_total_transform, sens_logical, "physiSDD", mother_logical, false, 0, checkOverlaps);

  G4Tubs* window_solid = new G4Tubs("solidBeWindow", window_rmin, window_rmax, 0.5*window_dz, window_sphi, window_dphi);
  G4LogicalVolume *window_logical = new G4LogicalVolume(window_solid, mat["Be"], "logicBeWindow");
  new G4PVPlacement(window_total_transform, window_logical, "physiBeWindow", mother_logical, false, 0, checkOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Option to switch on/off checking of volumes overlaps
  checkOverlaps = true;

  // define materials
  BuildMaterials();
  
  //     
  // World
  //
  G4Box*  
  solidWorld = new G4Box("World",                          //its name
                   fWorldSize/2,fWorldSize/2,fWorldSize/2);//its size
                   
  G4LogicalVolume*                         
  logicWorld = new G4LogicalVolume(solidWorld,             //its solid
                                   mat["air"],             //its material
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
    solidGe, mat["Ge"], "logicHPGe");
  G4VPhysicalVolume* physiGe = new G4PVPlacement(
    0, *HPGe_trans, logicGe, "physiGe", logicWorld, false, 0);

  //
  // SDD
  //
  G4Transform3D SDD_translate = G4Translate3D(0.0,0.0,0.0);
  //G4Transform3D SDD_rotate = G4RotateY3D(-0.5*M_PI);
  G4Transform3D SDD_rotate = G4RotateX3D(M_PI);
  G4Transform3D SDD_transform = SDD_translate*SDD_rotate;
  ConstructXrayDetector(logicWorld, SDD_transform);

  //
  //always return the physical World
  //  
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  HPGeSD* aHPGeSD = new HPGeSD("HPGeSD", "HPGeHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aHPGeSD);
  SetSensitiveDetector("logicHPGe", aHPGeSD, true);

  HPGeSD* aSDDSD = new HPGeSD("SDDSD", "SDDHitsCollection");
  G4SDManager::GetSDMpointer()->AddNewDetector(aSDDSD);
  SetSensitiveDetector("logicSDD", aSDDSD, true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
