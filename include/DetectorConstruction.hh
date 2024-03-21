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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...... 

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Transform3D.hh"
#include <map>

class DetectorMessenger;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
    DetectorConstruction();
   ~DetectorConstruction();

    virtual     
    G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    void SetConstructPart(G4int part_index, G4String axis, G4double rot, G4double x, G4double y, G4double z);
    
  private:
    DetectorMessenger *fDetectorMessenger;
    void BuildMaterials();
    void ConstructTU5Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform);
    void ConstructTU4Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform);
    void ConstructTU2Detector(G4LogicalVolume* mother_logical, G4Transform3D& global_transform);
    void ConstructAsContainer(G4LogicalVolume* mother_logical, G4Transform3D& global_transform);

    G4bool fDoConstruct[4]; // TU2, TU4, TU5, sample
    G4Transform3D fTransformPart[4];
  
    G4bool checkOverlaps;
    G4double fWorldSize;
    std::map<G4String,G4Material*> mat;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

