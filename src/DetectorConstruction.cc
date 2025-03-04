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
#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Polycone.hh"
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
  fDetectorMessenger = new DetectorMessenger(this);

  genericHPGeDimensions.crystal_diameter = 5*cm;
  genericHPGeDimensions.crystal_length = 5*cm;
  genericHPGeDimensions.distance_crystal_cryostat_outside = 1*cm;
  genericHPGeDimensions.cryostat_thickness = 0.15*cm;
  genericHPGeDimensions.cryostat_diameter = 7*cm;
  genericHPGeDimensions.cryostat_length = 7*cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetConstructPart(G4String name, G4String axis, G4double rot, G4double x, G4double y, G4double z)
{
    fConstructPart.push_back(name);
    G4Transform3D rotate = G4Rotate3D();
    if (axis == "X") rotate = G4RotateX3D(rot/180.*M_PI);
    if (axis == "Y") rotate = G4RotateY3D(rot/180.*M_PI);
    if (axis == "Z") rotate = G4RotateZ3D(rot/180.*M_PI);
    G4Transform3D translate = G4Translate3D(x*mm, y*mm, z*mm);
    fTransformPart.push_back(translate*rotate);

    if (name == "GenericHPGe") {
        vecGenericHPGeDimensions.push_back(genericHPGeDimensions);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetGenericHPGeDimensions(G4double crystal_diameter, G4double crystal_length, G4double distance_crystal_cryostat_outside, G4double cryostat_thickness, G4double cryostat_diameter, G4double cryostat_length)
{
    G4cout << "DEBUG: Set generic HPGe detector dimensions: " << G4endl
           << "crystal_diameter: " << crystal_diameter << G4endl
           << "crystal_length: " << crystal_length << G4endl
           << "distance_crystal_cryostat_outside: " << distance_crystal_cryostat_outside << G4endl
           << "cryostat_thickness: " << cryostat_thickness << G4endl
           << "cryostat_diameter: " << cryostat_diameter << G4endl
           << "cryostat_length: " << cryostat_length << G4endl;

    genericHPGeDimensions.crystal_diameter = crystal_diameter;
    genericHPGeDimensions.crystal_length = crystal_length;
    genericHPGeDimensions.distance_crystal_cryostat_outside = distance_crystal_cryostat_outside;
    genericHPGeDimensions.cryostat_thickness = cryostat_thickness;
    genericHPGeDimensions.cryostat_diameter = cryostat_diameter;
    genericHPGeDimensions.cryostat_length = cryostat_length;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::BuildMaterials()
{ // Define materials via NIST manager
  auto nist = G4NistManager::Instance();
  mat["Ge"] = nist->FindOrBuildMaterial("G4_Ge");
  mat["Al"] = nist->FindOrBuildMaterial("G4_Al");
  mat["Si"] = nist->FindOrBuildMaterial("G4_Si");
  mat["Be"] = nist->FindOrBuildMaterial("G4_Be");
  mat["air"] = nist->FindOrBuildMaterial("G4_AIR");
  mat["Pb"] = nist->FindOrBuildMaterial("G4_Pb");
  mat["Cu"] = nist->FindOrBuildMaterial("G4_Cu");
  mat["concrete"] = nist->FindOrBuildMaterial("G4_CONCRETE");
  mat["PMMA"] = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
  mat["polycarbonate"] = nist->FindOrBuildMaterial("G4_POLYCARBONATE");
  G4Element* As = nist->FindOrBuildElement(33);
  G4Element* O = nist->FindOrBuildElement(8);
  mat["As2O3"] = new G4Material("As2O3", 3.74*g/cm3, 2, kStateSolid);
  mat["As2O3"]->AddElement(As, 2);
  mat["As2O3"]->AddElement(O, 3);
  G4Element* C = nist->FindOrBuildElement(6);
  G4Element* H = nist->FindOrBuildElement(1);
  mat["EJ260"] = new G4Material("EJ260", 1.023*g/cm3, 2, kStateSolid);
  mat["EJ260"]->AddElement(C, 9);
  mat["EJ260"]->AddElement(H, 11);

  //G4Ele
  //TODO: peltier, tantalum collimator
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::AddDetectorName(G4String name)
// Add a detector name to the list of detecor names. This is called during the derector construction. 
// Calling this for every detector will store the number of sensitive volumes and their names in the vecDetectorNames vector.
{
  vecDetectorNames.push_back(name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructTU5Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the SDD X-ray detector,
// with frontent point at given frontend_position and given rotation.
// frontend point := intersection of sensor rotation axis with front layer of housing
{
  // Si sensitive part
  G4double sens_depth = 3.94*mm; // distance between front layer of housing and front layer of Si sensor
  G4double sens_dz = 0.45*mm;
  G4double sens_rmin = 0.0*mm;
  //G4double sens_rmax = sqrt(150.0/M_PI)*mm; // area = 150mm^2 = pi*r^2
  G4double sens_rmax = 3.96*mm;// naively tuned in order to match experimental efficiency
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


  // unique detector name
  G4String detectorName = "TU5";
  // construct sensitive part
  G4Tubs* sens_solid = new G4Tubs("solidSDD", sens_rmin, sens_rmax, 0.5*sens_dz, sens_sphi, sens_dphi);
  G4LogicalVolume *sens_logical = new G4LogicalVolume(sens_solid, mat["Si"], "logicSDD");
  sens_logical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  new G4PVPlacement(sens_total_transform, sens_logical, "physiSDD", mother_logical, false, 0, checkOverlaps);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);

  G4Tubs* window_solid = new G4Tubs("solidBeWindow", window_rmin, window_rmax, 0.5*window_dz, window_sphi, window_dphi);
  G4LogicalVolume *window_logical = new G4LogicalVolume(window_solid, mat["Be"], "logicBeWindow");
  new G4PVPlacement(window_total_transform, window_logical, "physiBeWindow", mother_logical, false, 0, checkOverlaps);
}

void DetectorConstruction::ConstructTU4Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
{
  // HPGe properties as written in data sheet
  G4double HPGe_crystal_diameter = 58.7*mm;
  G4double HPGe_crystal_length = 71.1*mm;
  G4double HPGe_end_cap_to_crystal = 3.0*mm;
  G4double HPGe_end_cap_thickness = 2.0*mm;
  G4double HPGe_dead_layer = 1.65*mm;//0.3*mm;// naively tuned in order to match experimental efficiency

  // crystal
  G4double crystal_rmin = 0.0;
  G4double crystal_rmax = 0.5*HPGe_crystal_diameter;
  G4double crystal_dz = HPGe_crystal_length;
  G4Transform3D crystal_internal_rotate = G4Rotate3D();
  G4Transform3D crystal_internal_translate = G4Translate3D(0.0, 0.0,
    -0.5*HPGe_crystal_length - HPGe_end_cap_thickness - HPGe_end_cap_to_crystal);
  G4Transform3D crystal_total_transform = global_transform*crystal_internal_translate*crystal_internal_rotate;
  G4Tubs* solidCrystal = new G4Tubs("solidCrystal",
    crystal_rmin, crystal_rmax, crystal_dz/2, 0, 2*M_PI);
  G4LogicalVolume* logicCrystal = new G4LogicalVolume(
    solidCrystal, mat["Ge"], "logicCrystal");
  G4VPhysicalVolume* physiCrystal = new G4PVPlacement(
    crystal_total_transform, logicCrystal, "physiCrystal", mother_logical, false, 0);

  // sensitive part of crystal
  G4double sens_rmin = 0.0;
  G4double sens_rmax = 0.5*HPGe_crystal_diameter - HPGe_dead_layer;
  G4double sens_dz = HPGe_crystal_length - 2.0*HPGe_dead_layer;

  // unique detector name
  G4String detectorName = "TU4";
  // construct sensitive part
  G4Tubs* solidSens = new G4Tubs("solidHPGe",
    sens_rmin, sens_rmax, sens_dz/2, 0, 2*M_PI);
  G4LogicalVolume* logicSens = new G4LogicalVolume(
    solidSens, mat["Ge"], "logic"+detectorName);
  logicSens->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  G4VPhysicalVolume* physiSens = new G4PVPlacement(
    0, G4ThreeVector(), logicSens, "physiHPGe", logicCrystal, false, 0);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);

  // end cap
  const int cap_num_planes = 4;
  const G4double cap_z_plane[cap_num_planes] = {0.0,
    -HPGe_end_cap_thickness,
    -HPGe_end_cap_thickness,
    -HPGe_end_cap_thickness-HPGe_end_cap_to_crystal-HPGe_crystal_length};
  const G4double cap_inner_r[cap_num_planes] = {0.0, 0.0,
    HPGe_crystal_diameter/2+HPGe_end_cap_to_crystal,
    HPGe_crystal_diameter/2+HPGe_end_cap_to_crystal};
  const G4double cap_outer_r[cap_num_planes] = {
    HPGe_crystal_diameter/2+HPGe_end_cap_to_crystal+HPGe_end_cap_thickness,
    HPGe_crystal_diameter/2+HPGe_end_cap_to_crystal+HPGe_end_cap_thickness,
    HPGe_crystal_diameter/2+HPGe_end_cap_to_crystal+HPGe_end_cap_thickness,
    HPGe_crystal_diameter/2+HPGe_end_cap_to_crystal+HPGe_end_cap_thickness};
  G4Transform3D cap_total_transform = global_transform;
  G4Polycone* solidCap = new G4Polycone("solidCap", 0, 2*M_PI,
    cap_num_planes, cap_z_plane, cap_inner_r, cap_outer_r);
  G4LogicalVolume* logicCap = new G4LogicalVolume(
    solidCap, mat["Al"], "logicCap");
  G4VPhysicalVolume* physiCap = new G4PVPlacement(
    cap_total_transform, logicCap, "physiCap", mother_logical, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructTU2Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
{
  // HPGe properties as written in data sheet
  G4double HPGe_crystal_diameter = 72*mm;
  G4double HPGe_crystal_length = 65*mm;
  G4double HPGe_crystal_well_diameter = 21*mm;
  G4double HPGe_crystal_well_depth = 40*mm;
  G4double HPGe_cap_to_crystal_outside = 10.0*mm;
  G4double HPGe_cap_to_crystal_front = 0.01*mm; // guessed
  G4double HPGe_cap_to_crystal_inside = 0.01*mm; // guessed
  G4double HPGe_cap_thickness = 2.0*mm; // guessed
  G4double HPGe_dead_layer = 0.3*mm; // guessed

  // unique detector name
  G4String detectorName = "TU2";
  // crystal properties
  const int crystal_num_planes = 4;
  const G4double crystal_z_plane[crystal_num_planes] = {0.0,
    -HPGe_crystal_well_depth,
    -HPGe_crystal_well_depth,
    -HPGe_crystal_length};
  const G4double crystal_inner_r[crystal_num_planes] = {
    HPGe_crystal_well_diameter/2,
    HPGe_crystal_well_diameter/2,
    0.0, 0.0};
  const G4double crystal_outer_r[crystal_num_planes] = {
    HPGe_crystal_diameter/2,
    HPGe_crystal_diameter/2,
    HPGe_crystal_diameter/2,
    HPGe_crystal_diameter/2};
  G4Transform3D crystal_internal_translate = G4Translate3D(0.0, 0.0,
    - HPGe_cap_thickness - HPGe_cap_to_crystal_front);
  G4Transform3D crystal_total_transform = global_transform*crystal_internal_translate;
  // construct sensitive part
  G4Polycone* solidCrystal = new G4Polycone("solidCrystal", 0, 2*M_PI,
    crystal_num_planes, crystal_z_plane, crystal_inner_r, crystal_outer_r);
  G4LogicalVolume* logicSens = new G4LogicalVolume(
    solidCrystal, mat["Ge"], "logic"+detectorName);
  logicSens->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  G4VPhysicalVolume* physiSens = new G4PVPlacement(
    crystal_total_transform, logicSens, "physiHPGe", mother_logical, false, 0);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);

  // end cap
  const int cap_num_planes = 6;
  const G4double cap_z_plane[cap_num_planes] = {0.0,
    -HPGe_cap_thickness,
    -HPGe_cap_thickness,
    -HPGe_crystal_well_depth,
    -HPGe_crystal_well_depth,
    -HPGe_cap_thickness-HPGe_crystal_well_depth};
  const G4double cap_inner_r[cap_num_planes] = {
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside-HPGe_cap_thickness,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside-HPGe_cap_thickness,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside-HPGe_cap_thickness,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside-HPGe_cap_thickness,
    0.0, 0.0};
  const G4double cap_outer_r[cap_num_planes] = {
    HPGe_crystal_diameter/2+HPGe_cap_to_crystal_outside,
    HPGe_crystal_diameter/2+HPGe_cap_to_crystal_outside,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside,
    HPGe_crystal_well_diameter/2-HPGe_cap_to_crystal_inside};
  G4Transform3D cap_total_transform = global_transform;
  G4Polycone* solidCap = new G4Polycone("solidCap", 0, 2*M_PI,
    cap_num_planes, cap_z_plane, cap_inner_r, cap_outer_r);
  G4LogicalVolume* logicCap = new G4LogicalVolume(
    solidCap, mat["Al"], "logicCap");
  G4VPhysicalVolume* physiCap = new G4PVPlacement(
    cap_total_transform, logicCap, "physiCap", mother_logical, false, 0);

  // outer cap
  G4double outer_cap_inner_r = HPGe_crystal_diameter/2 + HPGe_cap_to_crystal_outside;
  G4double outer_cap_outer_r = outer_cap_inner_r + HPGe_cap_thickness;
  G4double outer_cap_dz = HPGe_crystal_length + HPGe_cap_to_crystal_front + HPGe_cap_thickness;
  G4Transform3D outer_cap_internal_transform = G4Translate3D(
    0.0, 0.0, -outer_cap_dz/2);
  G4Transform3D outer_cap_total_transform = global_transform*outer_cap_internal_transform;
  G4Tubs* solidOuter = new G4Tubs("solidOuter", outer_cap_inner_r, outer_cap_outer_r, 0.5*outer_cap_dz, 0.0, 2.0*M_PI);
  G4LogicalVolume *logicOuter = new G4LogicalVolume(solidOuter, mat["Al"], "logicOuter");
  new G4PVPlacement(outer_cap_total_transform, logicOuter, "physiOuter", mother_logical, false, 0, checkOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructScintillator(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the scintillator detector. 
// It is a cylinder with a cylindical hole for the As sample or a PTB source. 
// Outer z=2cm, r=2.54cm, inner z=6.5mm, r=15.25mm
{
  G4double outer_z = 20*mm;
  G4double outer_r = 25.4*mm;
  G4double inner_z = 6.5*mm;
  G4double inner_r = 15.25*mm;

  const int scint_num_planes = 6;
  const G4double scint_z_plane[] = {outer_z/2, inner_z/2, inner_z/2, -inner_z/2, -inner_z/2, -outer_z/2};
  const G4double scint_inner_r[] = {0, 0, inner_r, inner_r, 0, 0};
  const G4double scint_outer_r[] = {outer_r, outer_r, outer_r, outer_r, outer_r, outer_r};
  // unique detector name
  G4String detectorName = "Scint";
  // construct sensitive part
  G4Polycone* solidScint = new G4Polycone("solidScint", 0.0, 2*M_PI,
    scint_num_planes, scint_z_plane, scint_inner_r, scint_outer_r);
  G4LogicalVolume* logicScint = new G4LogicalVolume(
    solidScint, mat["EJ260"], "logic"+detectorName);
  G4VPhysicalVolume* physiScint = new G4PVPlacement(
    global_transform, logicScint, "physiScint", mother_logical, false, 0);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);
}

void DetectorConstruction::ConstructGenericHPGe(G4LogicalVolume* mother_logical, G4Transform3D& global_transform, G4int generic_HPGe_index)
{
    G4cout << "DEBUG: Construct HPGe detector " << generic_HPGe_index << " with dimensions: " << G4endl
           << "crystal_diameter: " << vecGenericHPGeDimensions[generic_HPGe_index].crystal_diameter << G4endl
           << "crystal_length: " << vecGenericHPGeDimensions[generic_HPGe_index].crystal_length << G4endl
           << "distance_crystal_cryostat_outside: " << vecGenericHPGeDimensions[generic_HPGe_index].distance_crystal_cryostat_outside << G4endl
           << "cryostat_thickness: " << vecGenericHPGeDimensions[generic_HPGe_index].cryostat_thickness << G4endl
           << "cryostat_diameter: " << vecGenericHPGeDimensions[generic_HPGe_index].cryostat_diameter << G4endl
           << "cryostat_length: " << vecGenericHPGeDimensions[generic_HPGe_index].cryostat_length << G4endl;    
    G4double crystal_diameter = vecGenericHPGeDimensions[generic_HPGe_index].crystal_diameter;
    G4double crystal_length = vecGenericHPGeDimensions[generic_HPGe_index].crystal_length;
    G4double distance_crystal_cryostat_outside = vecGenericHPGeDimensions[generic_HPGe_index].distance_crystal_cryostat_outside;
    G4double cryostat_thickness = vecGenericHPGeDimensions[generic_HPGe_index].cryostat_thickness;
    G4double cryostat_diameter = vecGenericHPGeDimensions[generic_HPGe_index].cryostat_diameter;
    G4double cryostat_length = vecGenericHPGeDimensions[generic_HPGe_index].cryostat_length;
    G4double dead_layer_front = 0.2*CLHEP::mm;
    G4double dead_layer_side = 0.2*CLHEP::mm;

    // crystal
    G4double crystal_rmin = 0.0;
    G4double crystal_rmax = 0.5*crystal_diameter;
    G4double crystal_dz = crystal_length;
    G4Transform3D crystal_internal_rotate = G4Rotate3D();
    G4Transform3D crystal_internal_translate = G4Translate3D(0.0, 0.0,
      -0.5*crystal_length - distance_crystal_cryostat_outside);
    G4Transform3D crystal_total_transform = global_transform*crystal_internal_translate*crystal_internal_rotate;
    G4Tubs* solidCrystal = new G4Tubs("solidCrystal",
      crystal_rmin, crystal_rmax, crystal_dz/2, 0, 2*M_PI);
    G4LogicalVolume* logicCrystal = new G4LogicalVolume(
      solidCrystal, mat["Ge"], "logicCrystal");
    G4VPhysicalVolume* physiCrystal = new G4PVPlacement(
      crystal_total_transform, logicCrystal, "physiCrystal", mother_logical, false, 0);

    // sensitive part of crystal
    G4double sens_rmin = 0.0;
    G4double sens_rmax = 0.5*crystal_diameter - dead_layer_side;
    G4double sens_dz = crystal_length - 2.0*dead_layer_front;

    // unique detector name
    G4String detectorName = "HPGe" + std::__cxx11::to_string(generic_HPGe_index);
    // construct sensitive part
    G4Tubs* solidSens = new G4Tubs("solid"+detectorName,
      sens_rmin, sens_rmax, sens_dz/2, 0, 2*M_PI);
    G4LogicalVolume* logicSens = new G4LogicalVolume(
      solidSens, mat["Ge"], "logic"+detectorName);
    logicSens->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
    G4VPhysicalVolume* physiSens = new G4PVPlacement(
      0, G4ThreeVector(), logicSens, "physi"+detectorName, logicCrystal, false, 0);
    // register the sensitive logic volume name for sensitive detector
    AddDetectorName(detectorName);

    // end cap
    const int cap_num_planes = 4;
    const G4double cap_z_plane[cap_num_planes] = {0.0,
      -cryostat_thickness,
      -cryostat_thickness,
      -distance_crystal_cryostat_outside-crystal_length};
    const G4double cap_inner_r[cap_num_planes] = {0.0, 0.0,
      cryostat_diameter/2-cryostat_thickness,
      cryostat_diameter/2-cryostat_thickness};
    const G4double cap_outer_r[cap_num_planes] = {
      cryostat_diameter/2,
      cryostat_diameter/2,
      cryostat_diameter/2,
      cryostat_diameter/2};
    G4Transform3D cap_total_transform = global_transform;
    G4Polycone* solidCap = new G4Polycone("solidCap", 0, 2*M_PI,
      cap_num_planes, cap_z_plane, cap_inner_r, cap_outer_r);
    G4LogicalVolume* logicCap = new G4LogicalVolume(
      solidCap, mat["Al"], "logicCap");
    G4VPhysicalVolume* physiCap = new G4PVPlacement(
      cap_total_transform, logicCap, "physiCap", mother_logical, false, 0);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructAsContainer1(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the polycarbonate sample holder including As.
// Central point equals the central point of the As layer.
{
  G4double As_thickness = 0.137*mm; // 195 mg of 3.74 g/cm3 in a cylinder of r=1.1cm
  G4double As_radius = 11.0*mm;
  G4double cont_window_thickness = 1.0*mm;
  G4double cont_total_z = 8.0*mm;
  G4double cont_total_r = 12.0*mm;
  G4double ring_inner_r = 9.0*mm;
  G4double bottom = -cont_window_thickness - 0.5*As_thickness; // z coordinate

  // construct the container
  const int cont_num_planes = 4;
  const G4double cont_z_plane[] = {bottom+cont_total_z, -bottom, -bottom, bottom};
  const G4double cont_inner_r[] = {ring_inner_r, ring_inner_r, 0.0, 0.0};
  const G4double cont_outer_r[] = {cont_total_r, cont_total_r, cont_total_r, cont_total_r};
  G4Polycone* solidCont = new G4Polycone("solidCont", 0.0, 2*M_PI,
    cont_num_planes, cont_z_plane, cont_inner_r, cont_outer_r);
  G4LogicalVolume* logicCont = new G4LogicalVolume(
    solidCont, mat["polycarbonate"], "logicCont");
  G4VPhysicalVolume* physiCont = new G4PVPlacement(
    global_transform, logicCont, "physiCont", mother_logical, false, 0);

  // Place the As layer into the container
  G4Tubs* solidSample = new G4Tubs("solidSample",
    0, As_radius, As_thickness/2, 0, 2*M_PI);
  G4LogicalVolume* logicSample = new G4LogicalVolume(
    solidSample, mat["As2O3"], "logicSample");
  logicSample->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)));
  G4VPhysicalVolume* physiSample = new G4PVPlacement(
    G4Transform3D(), logicSample, "physiSample", logicCont, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


void DetectorConstruction::ConstructAsContainer2(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the polycarbonate sample holder including As.
// Central point equals the central point of the As layer.
{
  G4double As_thickness = 0.137*mm; // 195 mg of 3.74 g/cm3 in a cylinder of r=1.1cm
  G4double As_radius = 11.0*mm;
  G4double cont_window_thickness = 0.5*mm;
  G4double cont_total_z = 5.5*mm;
  G4double cont_total_r = 12.0*mm;
  G4double ring_inner_r = 9.0*mm;
  G4double bottom = -cont_window_thickness - 0.5*As_thickness; // z coordinate

  // construct the container
  const int cont_num_planes = 4;
  const G4double cont_z_plane[] = {bottom+cont_total_z, -bottom, -bottom, bottom};
  const G4double cont_inner_r[] = {ring_inner_r, ring_inner_r, 0.0, 0.0};
  const G4double cont_outer_r[] = {cont_total_r, cont_total_r, cont_total_r, cont_total_r};
  G4Polycone* solidCont = new G4Polycone("solidCont", 0.0, 2*M_PI,
    cont_num_planes, cont_z_plane, cont_inner_r, cont_outer_r);
  G4LogicalVolume* logicCont = new G4LogicalVolume(
    solidCont, mat["polycarbonate"], "logicCont");
  G4VPhysicalVolume* physiCont = new G4PVPlacement(
    global_transform, logicCont, "physiCont", mother_logical, false, 0);

  // Place the As layer into the container
  G4Tubs* solidSample = new G4Tubs("solidSample",
    0, As_radius, As_thickness/2, 0, 2*M_PI);
  G4LogicalVolume* logicSample = new G4LogicalVolume(
    solidSample, mat["As2O3"], "logicSample");
  logicSample->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)));
  G4VPhysicalVolume* physiSample = new G4PVPlacement(
    G4Transform3D(), logicSample, "physiSample", logicCont, false, 0);
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
  // Construct optional parts including detectors and non-detector parts like samples
  //
  G4int generic_HPGe_index = 0;
  for (int i = 0; i < fConstructPart.size(); i++) {
    G4String name = fConstructPart.at(i);
    G4Transform3D trans = fTransformPart.at(i);
    if (name == "TU2") ConstructTU2Detector(logicWorld, trans);
    if (name == "TU4") ConstructTU4Detector(logicWorld, trans);
    if (name == "TU5") ConstructTU5Detector(logicWorld, trans);
    if (name == "Scint") ConstructScintillator(logicWorld, trans);
    if (name == "GenericHPGe") {
        ConstructGenericHPGe(logicWorld, trans, generic_HPGe_index);
        generic_HPGe_index++;
    }
    if (name == "sample1") ConstructAsContainer1(logicWorld, trans);
    if (name == "sample2") ConstructAsContainer2(logicWorld, trans);
  }

  //
  //always return the physical World
  //
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  for (G4String detectorName : vecDetectorNames) {
    HPGeSD* aHPGeSD = new HPGeSD(detectorName+"SD", detectorName+"HitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(aHPGeSD);
    SetSensitiveDetector("logic"+detectorName, aHPGeSD, true);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
