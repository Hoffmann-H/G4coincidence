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
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"
#include "HPGeSD.hh"
#include "G4VisAttributes.hh"
//#include "G4Colour.hh"
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

  // Optional hard-coded angles, just for visualization
  sphi = 0.0;
  dphi = 2.0*M_PI;
  housing_gray = G4Colour(0.5, 0.5, 0.5);
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
  mat["Ta"] = nist->FindOrBuildMaterial("G4_Ta");
  mat["concrete"] = nist->FindOrBuildMaterial("G4_CONCRETE");
  mat["PMMA"] = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
  mat["polycarbonate"] = nist->FindOrBuildMaterial("G4_POLYCARBONATE");
  G4cout << "G4_POLYCARBONATE density: " << mat["polycarbonate"]->GetDensity()/(g/cm3) << " g/cm3" << G4endl;
  G4Element* As = nist->FindOrBuildElement(33);
  G4Element* O = nist->FindOrBuildElement(8);
  mat["As2O3"] = new G4Material("As2O3", 3.74*g/cm3, 2, kStateSolid);
  mat["As2O3"]->AddElement(As, 2);
  mat["As2O3"]->AddElement(O, 3);
  G4Element* C = nist->FindOrBuildElement(6);
  G4Element* H = nist->FindOrBuildElement(1);
  mat["EJ200"] = new G4Material("EJ200", 1.023*g/cm3, 2, kStateSolid);
  mat["EJ200"]->AddElement(C, 9);
  mat["EJ200"]->AddElement(H, 10);
  mat["EJ260"] = new G4Material("EJ260", 1.023*g/cm3, 2, kStateSolid);
  mat["EJ260"]->AddElement(C, 9);
  mat["EJ260"]->AddElement(H, 11);
  mat["benzene"] = new G4Material("benzene", 0.8765*g/cm3, 2, kStateLiquid);
  mat["benzene"]->AddElement(C, 6);
  mat["benzene"]->AddElement(H, 6);
  mat["vacuum"] = nist->FindOrBuildMaterial("G4_Galactic");

  //G4Ele
  //TODO: peltier, tantalum collimator
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAsLayerThickness(G4double As_layer_thickness)
{
  AsLayerThickness = As_layer_thickness;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAsCoverThickness(G4double As_cover_thickness)
{
  AsCoverThickness = As_cover_thickness;
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
  G4LogicalVolume *sens_logical = new G4LogicalVolume(sens_solid, mat["Si"], "logicTU5");
  sens_logical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  new G4PVPlacement(sens_total_transform, sens_logical, "physiSDD", mother_logical, false, 0, checkOverlaps);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);

  G4Tubs* window_solid = new G4Tubs("solidBeWindow", window_rmin, window_rmax, 0.5*window_dz, window_sphi, window_dphi);
  G4LogicalVolume *window_logical = new G4LogicalVolume(window_solid, mat["Be"], "logicBeWindow");
  new G4PVPlacement(window_total_transform, window_logical, "physiBeWindow", mother_logical, false, 0, checkOverlaps);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructTU3Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the TU3 SDD X-ray detector.
// Placement reference point is the center of the Be entrance window front.
{
  // From technical drawing: 
  // Housing: outer diameter 21mm, radial thickness 0.27mm, front thickness 0.33mm, opening diameter 14mm, total length 9.2mm.
  // Vacuum: z 8.1mm
  // Collimator: z 0.42mm, inner diameter 10mm, outer diameter 15.1mm
  // Backing: diameter 15.0mm, z 0.45mm
  // Steffen 7.5.2022: Auf Nachfrage bei Ketek sagte man mir, dass der verbaute On-Chip Multilayer Kollimator zum Großteil aus Tantal bestünde (Siehe “Ketek_Kollimator”-Bild im Anhang)
  // Picture "KETEK multi-layer collimator": outside Al-Ti-Cr-Ta inside

  // Si crystal
  G4double sens_depth = 2.0*mm; // distance between front layer of enrance window and front layer of Si sensor
  G4double sens_dz = 0.450*mm;
  G4double sens_rmin = 0.0*mm;
  G4double sens_rmax = 7.5*mm;//5.890*mm; // area = 109mm^2. 
  G4double sens_sphi = sphi;
  G4double sens_dphi = dphi;
  G4Transform3D Si_internal_translate = G4Translate3D(0.0, 0.0, -sens_depth-0.5*sens_dz);
  // Si sensitive part - to be tuned
  // G4double sens_rsens = 5.0*mm; // collimator inner radius from technical drawing
  G4double sens_rsens = 5.24*mm;
  G4double sens_dead_front = 0.008*mm;
  G4double sens_dead_back = 0.001*mm;
  G4Transform3D sens_internal_translate = G4Translate3D(0.0, 0.0, 0.5*(sens_dead_back-sens_dead_front));
  
  // Be entrance window
  G4double window_dz = 0.025*mm;
  G4double window_rmin = sens_rmin;
  G4double window_rmax = 7.*mm;
  G4double window_sphi = sphi;
  G4double window_dphi = dphi;
  G4Transform3D window_internal_rotate = G4Rotate3D();
  G4Transform3D window_internal_translate = G4Translate3D(0.0, 0.0, -0.5*window_dz);
  G4Transform3D window_total_transform = global_transform*window_internal_translate*window_internal_rotate;
  
  // TODO: collimator.
  //G4double coll_rmin = 5.046*mm; // collimated area = 80mm^2 = pi*r^2 => r = 5.046mm
  //G4double coll_rmin = 1.7933617; // tuned to match experimental efficiency for 14keV gammas @50mm
  // G4double coll_rmin = 5.0*mm; // from technical drawing; consistent with nominal collimated area
  G4double coll_rmin = 5.24*mm;
  G4double coll_dz = 0.42*mm;
  G4double coll_rmax = 7.55*mm;
  G4double coll_sphi = sphi;
  G4double coll_dphi = dphi;
  G4Transform3D coll_internal_translate = G4Translate3D(0.0, 0.0, -sens_depth+0.5*coll_dz);

  // Housing
  G4double housing_length = 9.2*mm;
  G4double housing_front_dz = 0.3*mm;
  G4double housing_dr = 0.3*mm;
  G4double housing_outer_diameter = 21.0*mm;
  G4double housing_opening_diameter = 2*window_rmax;
  const int cap_num_planes = 4;
  const G4double cap_z_plane[cap_num_planes] = {0.0,
    -housing_front_dz,
    -housing_front_dz,
    -housing_length};
  const G4double cap_inner_r[cap_num_planes] = {
    housing_opening_diameter/2,
    housing_opening_diameter/2,
    housing_outer_diameter/2-housing_dr,
    housing_outer_diameter/2-housing_dr};
  const G4double cap_outer_r[cap_num_planes] = {
    housing_outer_diameter/2,
    housing_outer_diameter/2,
    housing_outer_diameter/2,
    housing_outer_diameter/2};
  G4Transform3D cap_total_transform = global_transform;
  G4Polycone* solidCap = new G4Polycone("solidCap", sphi, dphi,
    cap_num_planes, cap_z_plane, cap_inner_r, cap_outer_r);
  G4LogicalVolume* logicCap = new G4LogicalVolume(
    solidCap, mat["Al"], "logicCap");
  logicCap->SetVisAttributes(new G4VisAttributes(housing_gray));
  G4VPhysicalVolume* physiCap = new G4PVPlacement(
    cap_total_transform, logicCap, "physiCap", mother_logical, false, 0);

  // Vacuum
  G4double vac_z = 8.1*mm;
  const int vac_num_planes = 4;
  const G4double vac_z_plane[vac_num_planes] = {
    -window_dz,
    -housing_front_dz,
    -housing_front_dz,
    -vac_z};
  const G4double vac_inner_r[vac_num_planes] = {
    0, 0, 0, 0};
  const G4double vac_outer_r[vac_num_planes] = {
    housing_opening_diameter/2,
    housing_opening_diameter/2,
    housing_outer_diameter/2-housing_dr,
    housing_outer_diameter/2-housing_dr};
  G4Transform3D vac_total_transform = global_transform;
  G4Polycone* solidVac = new G4Polycone("solidVac", sphi, dphi,
    vac_num_planes, vac_z_plane, vac_inner_r, vac_outer_r);
  G4LogicalVolume* logicVac = new G4LogicalVolume(
    solidVac, mat["vacuum"], "logicVac");
  logicVac->SetVisAttributes(G4VisAttributes::GetInvisible());
  G4VPhysicalVolume* physiVac = new G4PVPlacement(
    vac_total_transform, logicVac, "physiVac", mother_logical, false, 0);

  // Nose (just for visualization)
  G4double nose_dz = 100*mm;
  G4double nose_inner_diameter = housing_outer_diameter;
  G4double nose_outer_diameter = 25*mm;
  G4Transform3D nose_internal_translate = G4Translate3D(0, 0, -8*mm - 0.5*nose_dz);
  G4Transform3D nose_global_transform = global_transform*nose_internal_translate;
  G4Tubs* nose_solid = new G4Tubs("solidNose", 0.5*nose_inner_diameter, 0.5*nose_outer_diameter, 0.5*nose_dz, sphi, dphi);
  G4LogicalVolume* nose_logical = new G4LogicalVolume(nose_solid, mat["air"], "logicNose");
  nose_logical->SetVisAttributes(new G4VisAttributes(housing_gray));
  new G4PVPlacement(nose_global_transform, nose_logical, "physiNose", mother_logical, false, 0, checkOverlaps);

  // construct Si crystal
  G4Tubs* Si_solid = new G4Tubs("solidSi", sens_rmin, sens_rmax, 0.5*sens_dz, sens_sphi, sens_dphi);
  G4LogicalVolume *Si_logical = new G4LogicalVolume(Si_solid, mat["Si"], "logicSi");
  //Si_logical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0))); // yellow
  Si_logical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0))); // red
  new G4PVPlacement(Si_internal_translate, Si_logical, "physiSi", logicVac, false, 0, checkOverlaps);
  // unique detector name
  G4String detectorName = "TU3";
  // construct sensitive part
  G4Tubs* sens_solid = new G4Tubs("solidSDD", sens_rmin, sens_rsens, 0.5*(sens_dz-sens_dead_front-sens_dead_back), sens_sphi, sens_dphi);
  G4LogicalVolume *sens_logical = new G4LogicalVolume(sens_solid, mat["Si"], "logic"+detectorName);
  sens_logical->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  new G4PVPlacement(sens_internal_translate, sens_logical, "physiSDD", Si_logical, false, 0, checkOverlaps);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);

  G4Tubs* window_solid = new G4Tubs("solidBeWindow", window_rmin, window_rmax, 0.5*window_dz, window_sphi, window_dphi);
  G4LogicalVolume *window_logical = new G4LogicalVolume(window_solid, mat["Be"], "logicBeWindow");
  window_logical->SetVisAttributes(new G4VisAttributes(housing_gray));
  new G4PVPlacement(window_total_transform, window_logical, "physiBeWindow", mother_logical, false, 0, checkOverlaps);

  G4Tubs* coll_solid = new G4Tubs("solidOnChipCollimator", coll_rmin, coll_rmax, 0.5*coll_dz, coll_sphi, coll_dphi);
  G4LogicalVolume* coll_logical = new G4LogicalVolume(coll_solid, mat["Ta"], "logicalOnChipCollimator");
  coll_logical->SetVisAttributes(new G4VisAttributes(housing_gray));
  new G4PVPlacement(coll_internal_translate, coll_logical, "physiOnChipCollimator", logicVac, false, 0, checkOverlaps);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
  G4double capDiameterOutside = 89 * mm;
  G4double capDiameterInside = 16 * mm;
  G4double capLength = 160 * mm;
  G4double capDepth = 40 * mm;
  G4double capThicknessOutside = 1.5 * mm;
  G4double capThicknessInside = 1.5 * mm;
  G4double distanceFromCapFront = 10 * mm;
  G4double crystalDiameter = 72 * mm;
  G4double crystalLength = 65 * mm;
  G4double boreHoleDiameter = 21 * mm;
  G4double boreHoleLength = 40 * mm;
  G4double deadLayerFront = 0.2 * mm;
  G4double deadLayerOutside = 0.2 * mm;
  G4double deadLayerInside = 0.2 * mm;
  G4double deadLayerBack = 0.2 * mm;

  // Empirical HPGe properties
  distanceFromCapFront = 8 * mm;
  deadLayerFront = 1.0 * mm;
  deadLayerOutside = 3.0 * mm;
  deadLayerInside = 0.1 * mm;
  deadLayerBack = 1.0 * mm;

  // cryostat
  const int cap_num_planes = 4;
  const G4double cap_z_plane[cap_num_planes] = {0.0,
    -capDepth,
    -capDepth,
    -capLength};
  const G4double cap_inner_r[cap_num_planes] = {
    capDiameterInside/2,
    capDiameterInside/2,
    0.0, 0.0};
  const G4double cap_outer_r[cap_num_planes] = {
    capDiameterOutside/2,
    capDiameterOutside/2,
    capDiameterOutside/2,
    capDiameterOutside/2};
  G4Polycone* solidCap = new G4Polycone("solidCap", sphi, dphi,
    cap_num_planes, cap_z_plane, cap_inner_r, cap_outer_r);
  G4LogicalVolume* logicCap = new G4LogicalVolume(
    solidCap, mat["Al"], "logicCap");
  logicCap->SetVisAttributes(new G4VisAttributes(housing_gray));
  G4VPhysicalVolume* physiCap = new G4PVPlacement(
    global_transform, logicCap, "physiCap", mother_logical, false, 0);

  // vacuum
  const int vac_num_planes = 4;
  const G4double vac_z_plane[vac_num_planes] = {-capThicknessOutside,
    -capDepth-capThicknessInside,
    -capDepth-capThicknessInside,
    -capLength};
  const G4double vac_inner_r[vac_num_planes] = {
    capDiameterInside/2+capThicknessInside,
    capDiameterInside/2+capThicknessInside,
    0.0, 0.0};
  const G4double vac_outer_r[vac_num_planes] = {
    capDiameterOutside/2-capThicknessOutside,
    capDiameterOutside/2-capThicknessOutside,
    capDiameterOutside/2-capThicknessOutside,
    capDiameterOutside/2-capThicknessOutside};
  G4Transform3D vac_internal_transform = G4Transform3D();
  G4Polycone* solidVac = new G4Polycone("solidVac", sphi, dphi,
    vac_num_planes, vac_z_plane, vac_inner_r, vac_outer_r);
  G4LogicalVolume* logicVac = new G4LogicalVolume(
    solidVac, mat["vacuum"], "logicVac");
  logicVac->SetVisAttributes(G4VisAttributes::GetInvisible());
  G4VPhysicalVolume* physiVac = new G4PVPlacement(
    vac_internal_transform, logicVac, "physiVac", logicCap, false, 0);

  // HPGe dead layer
  const int Ge_num_planes = 4;
  const G4double Ge_z_plane[Ge_num_planes] = {-distanceFromCapFront,
    -distanceFromCapFront-boreHoleLength,
    -distanceFromCapFront-boreHoleLength,
    -distanceFromCapFront-crystalLength};
  const G4double Ge_inner_r[Ge_num_planes] = {
    boreHoleDiameter/2,
    boreHoleDiameter/2,
    0.0, 0.0};
  const G4double Ge_outer_r[Ge_num_planes] = {
    crystalDiameter/2,
    crystalDiameter/2,
    crystalDiameter/2,
    crystalDiameter/2};
  G4Transform3D Ge_internal_transform = G4Transform3D();
  G4Polycone* solidGe = new G4Polycone("solidGe", sphi, dphi,
    Ge_num_planes, Ge_z_plane, Ge_inner_r, Ge_outer_r);
  G4LogicalVolume* logicGe = new G4LogicalVolume(
    solidGe, mat["Ge"], "logicGe");
  //logicGe->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 1.0, 0.0))); // yellow
  logicGe->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0))); // red
  G4VPhysicalVolume* physiGe = new G4PVPlacement(
    Ge_internal_transform, logicGe, "physiGe", logicVac, false, 0);

  // HPGe sensitive volume
  const int TU2_num_planes = 4;
  const G4double TU2_z_plane[TU2_num_planes] = {-distanceFromCapFront-deadLayerFront,
    -distanceFromCapFront-boreHoleLength-deadLayerInside,
    -distanceFromCapFront-boreHoleLength-deadLayerInside,
    -distanceFromCapFront-crystalLength+deadLayerBack};
  const G4double TU2_inner_r[TU2_num_planes] = {
    boreHoleDiameter/2+deadLayerInside,
    boreHoleDiameter/2+deadLayerInside,
    0.0, 0.0};
  const G4double TU2_outer_r[TU2_num_planes] = {
    crystalDiameter/2-deadLayerOutside,
    crystalDiameter/2-deadLayerOutside,
    crystalDiameter/2-deadLayerOutside,
    crystalDiameter/2-deadLayerOutside};
  G4Transform3D TU2_internal_transform = G4Transform3D();
  G4Polycone* solidTU2 = new G4Polycone("solidTU2", sphi, dphi,
    TU2_num_planes, TU2_z_plane, TU2_inner_r, TU2_outer_r);
  G4LogicalVolume* logicTU2 = new G4LogicalVolume(
    solidTU2, mat["Ge"], "logicTU2");
  logicTU2->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  G4VPhysicalVolume* physiTU2 = new G4PVPlacement(
    TU2_internal_transform, logicTU2, "physiTU2", logicGe, false, 0);

  // register the sensitive logic volume name for sensitive detector
  AddDetectorName("TU2");
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructGenericScint(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct a generic scintillator detector. 
// Shape: cylinder
// Dimensions so far hard coded. Could be implemented in commands.
{
  G4double scint_z = 5.0*mm;
  G4double scint_rmin = 0;
  G4double scint_rmax = 15.0*mm;

  // unique detector name
  G4String detectorName = "GenSc";
  // construct sensitive part
  G4Tubs* solidScint = new G4Tubs("solidGenSc",
    scint_rmin, scint_rmax, scint_z/2, 0, 2*M_PI);
    
  G4LogicalVolume* logicScint = new G4LogicalVolume(
    solidScint, mat["EJ200"], "logic"+detectorName);
  logicScint->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  G4VPhysicalVolume* physiScint = new G4PVPlacement(
    global_transform, logicScint, "physiScint", mother_logical, false, 0);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructVetoScint(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct a veto liquid scintillator detector. 
// Shape: rotation body 
// Dimensions so far hard coded. Could be implemented in commands.
{
  G4double scint_dz = 610.0*mm;
  G4double scint_rmin = 0;
  G4double scint_rmax = 610.0*mm;
  G4double hole0_r = 13.0*mm;
  G4double hole0_z = -7.0*mm;
  G4double hole1_r = 48.0*mm;
  G4double hole1_z = hole0_z + 105.0*mm;
  G4double cont_d = 1.001*mm;

  // unique detector name
  G4String detectorName = "VetoSc";
  // construct sensitive part
  const int scint_num_planes = 6;
  const G4double scint_z_plane[scint_num_planes] = {-scint_dz, hole0_z - cont_d, hole0_z - cont_d, hole1_z - cont_d, hole1_z - cont_d, scint_dz};
  const G4double scint_inner_r[scint_num_planes] = {0, 0, hole0_r + cont_d, hole0_r + cont_d, hole1_r + cont_d, hole1_r + cont_d};
  const G4double scint_outer_r[scint_num_planes] = {scint_dz, scint_dz, scint_dz, scint_dz, scint_dz, scint_dz};
  G4Polycone* solidScint = new G4Polycone("solidVetoSc", 0, 2*M_PI,
      scint_num_planes, scint_z_plane, scint_inner_r, scint_outer_r);    
  G4LogicalVolume* logicScint = new G4LogicalVolume(
    solidScint, mat["EJ200"], "logic"+detectorName);
  logicScint->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)));
  G4VPhysicalVolume* physiScint = new G4PVPlacement(
    global_transform, logicScint, "physiScint", mother_logical, false, 0);
  // register the sensitive logic volume name for sensitive detector
  AddDetectorName(detectorName);

  // Construct a container for sample and detector
  cont_d -= 0.001*mm;
  const int cont_num_planes = 8;
  const G4double cont_z_plane[cont_num_planes] = {hole0_z - cont_d, hole0_z, hole0_z, hole1_z - cont_d, hole1_z - cont_d, hole1_z, hole1_z, scint_dz};
  const G4double cont_inner_r[cont_num_planes] = {0, 0, hole0_r, hole0_r, hole0_r, hole0_r, hole1_r, hole1_r};
  const G4double cont_outer_r[cont_num_planes] = {hole0_r + cont_d, hole0_r + cont_d, hole0_r + cont_d, hole0_r + cont_d, hole1_r + cont_d, hole1_r + cont_d, hole1_r + cont_d, hole1_r + cont_d};
  G4Polycone* solidCont = new G4Polycone("solidCont", 0, 2*M_PI,
      cont_num_planes, cont_z_plane, cont_inner_r, cont_outer_r);    
  G4LogicalVolume* logicCont = new G4LogicalVolume(
    solidCont, mat["polycarbonate"], "logicCont");
  logicCont->SetVisAttributes(new G4VisAttributes(G4Colour(0.75, 0.75, 0.75)));
  G4VPhysicalVolume* physiCont = new G4PVPlacement(
    global_transform, logicCont, "physiCont", mother_logical, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
  G4Polycone* solidCont = new G4Polycone("solidCont", sphi, dphi,
    cont_num_planes, cont_z_plane, cont_inner_r, cont_outer_r);
  G4LogicalVolume* logicCont = new G4LogicalVolume(
    solidCont, mat["polycarbonate"], "logicCont");
  logicCont->SetVisAttributes(new G4VisAttributes(G4Colour(0.75, 0.75, 0.75)));
  G4VPhysicalVolume* physiCont = new G4PVPlacement(
    global_transform, logicCont, "physiCont", mother_logical, false, 0);

  // Place the As layer into the container
  G4Tubs* solidSample = new G4Tubs("solidSample",
    0, As_radius, As_thickness/2, sphi, dphi);
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

void DetectorConstruction::ConstructAsContainer3(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct a proposed 3rd As sample holder fitting into TU2's borehole.
// A layer of As2O3 surrounded by varying thickness of varying material.
// Measures editable by command: As layer thickness, holder thickness. Holder material?
// Central point equals the central point of the As layer.
{
  G4double As_radius = 6.5*mm;
  G4double bottom_window_thickness = 1.0*mm;
  G4double cont_total_z = bottom_window_thickness + AsLayerThickness + AsCoverThickness;
  G4double cont_total_r = 7.75*mm;

  // construct the container
  const int cont_num_planes = 4;
  G4Tubs* solidCont = new G4Tubs("solidCont", 0, cont_total_r, cont_total_z/2, 0.0, 2*M_PI);
  G4LogicalVolume* logicCont = new G4LogicalVolume(
    solidCont, mat["polycarbonate"], "logicCont");
  G4Transform3D cont_internal_translate = G4Translate3D(0.0, 0.0, AsCoverThickness/2 + AsLayerThickness/2);
  G4Transform3D cont_total_transform = global_transform*cont_internal_translate;
  G4VPhysicalVolume* physiCont = new G4PVPlacement(
    cont_total_transform, logicCont, "physiCont", mother_logical, false, 0);
  
  // Place the As layer into the container
  G4Tubs* solidSample = new G4Tubs("solidSample",
    0, As_radius, AsLayerThickness/2, 0, 2*M_PI);
  G4LogicalVolume* logicSample = new G4LogicalVolume(
    solidSample, mat["As2O3"], "logicSample");
  logicSample->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)));
  G4Transform3D As_internal_translate = G4Translate3D(0.0, 0.0, -AsCoverThickness/2 - AsLayerThickness/2);
  G4VPhysicalVolume* physiSample = new G4PVPlacement(
    As_internal_translate, logicSample, "physiSample", logicCont, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructPTBstandard(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct a PTB standard. 
// Two layers of (21.3 +- 1.8) mg/cm^2 polyethylene with 26 mm diameter.
// An Al ring with thickness 3 mm , inner diameter 26 mm and outer diameter 30 mm.
// Central point equals the geometrical center which is the central point of the radioactive substance.
{
  G4double PTBfoil_total_r = 13.0*mm;
  G4double PTBfoil_total_z = 2*0.021/0.94*mm; // PE density as implemented in G4 is 0.94 g/cm^3
  G4double PTBring_outer_r = 15.0*mm;
  G4double PTBring_total_z = 3.0*mm;

  // construct the PE foils
  G4Tubs* solidPTBfoil = new G4Tubs("solidPTBfoil", 0, PTBfoil_total_r, PTBfoil_total_z/2, sphi, dphi);
  G4LogicalVolume* logicPTBfoil = new G4LogicalVolume(
    solidPTBfoil, mat["polycarbonate"], "logicPTBfoil");
  G4VPhysicalVolume* physiPTBfoil = new G4PVPlacement(
    global_transform, logicPTBfoil, "physiPTBfoil", mother_logical, false, 0);
  
  // construct the Al ring
  G4Tubs* solidPTBring = new G4Tubs("solidPTBring", PTBfoil_total_r, PTBring_outer_r, PTBring_total_z/2, sphi, dphi);
  G4LogicalVolume* logicPTBring = new G4LogicalVolume(
    solidPTBring, mat["polycarbonate"], "logicPTBring");
  G4VPhysicalVolume* physiPTBring = new G4PVPlacement(
    global_transform, logicPTBring, "physiPTBring", mother_logical, false, 0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructCopperHolder(G4LogicalVolume* mother_logical, G4Transform3D& global_transform)
// Construct the Cu holder for PTB standards.
// Two holdes exis, one for 0mm and one for 3 and 7 cm.
// Both are simplified into one version omitting legs, screws etc.
// A cuboid (8cm)^2 x 5mm with the actual rotational notch.
// Central point equals the geometrical center which is the central point of the PTB standard.
{
  G4double CuHolder_total_xy = 80.0*mm;
  G4double CuHolder_total_z = 5*mm;
  G4double CuHolder_inner_r = 10.0*mm;
  G4double PTBring_outer_r = 15.0*mm;
  G4double PTBring_total_z = 3.0*mm;

  // Solid by subtraction of 2 cylinders from a cuboid
  G4Box* solidBox = new G4Box("solidBox", CuHolder_total_xy/2, CuHolder_total_xy/2, CuHolder_total_z/2);
  G4Tubs* solidTub1 = new G4Tubs("solidTub1", 0, PTBring_outer_r + 0.1*mm, CuHolder_total_z/2 + PTBring_total_z/2, 0.0, 2*M_PI);
  G4Tubs* solidTub2 = new G4Tubs("solidTub2", 0, CuHolder_inner_r, CuHolder_total_z, 0.0, 2*M_PI);
  G4SubtractionSolid* solidSub1 = new G4SubtractionSolid("solidSub1", solidBox, solidTub1, 0, G4ThreeVector(0, 0, CuHolder_total_z/2));
  G4SubtractionSolid* solidSub2 = new G4SubtractionSolid("solidSub2", solidSub1, solidTub2);

  // Logical volume and physical placement
  G4LogicalVolume* logicCuHolder = new G4LogicalVolume(
    solidSub2, mat["Cu"], "logicCuHolder");
  logicCuHolder->SetVisAttributes(new G4VisAttributes(G4Colour(0.67, 0.33, 0.0)));
  G4VPhysicalVolume* physiCuHolder = new G4PVPlacement(
    global_transform, logicCuHolder, "physiCuHolder", mother_logical, false, 0);
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
    if (name == "TU3") ConstructTU3Detector(logicWorld, trans);
    if (name == "TU4") ConstructTU4Detector(logicWorld, trans);
    if (name == "TU5") ConstructTU5Detector(logicWorld, trans);
    if (name == "Scint") ConstructScintillator(logicWorld, trans);
    if (name == "GenSc") ConstructGenericScint(logicWorld, trans);
    if (name == "VetoSc") ConstructVetoScint(logicWorld, trans);
    if (name == "GenericHPGe") {
        ConstructGenericHPGe(logicWorld, trans, generic_HPGe_index);
        generic_HPGe_index++;
    }
    if (name == "sample1") ConstructAsContainer1(logicWorld, trans);
    if (name == "sample2") ConstructAsContainer2(logicWorld, trans);
    if (name == "sample3") ConstructAsContainer3(logicWorld, trans);
    if (name == "PTBstandard") ConstructPTBstandard(logicWorld, trans);
    if (name == "CuHolder") ConstructCopperHolder(logicWorld, trans);
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
