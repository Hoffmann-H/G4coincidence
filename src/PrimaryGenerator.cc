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
/// \file PrimaryGenerator.cc
/// \brief Implementation of the PrimaryGenerator1 class
//
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGenerator.hh"
#include "PrimaryMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4IonTable.hh"
#include "G4DecayTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGenerator::PrimaryGenerator()
: G4VPrimaryGenerator()
{ 
    primaryMessenger = new PrimaryMessenger(this);

    // Primary particle standard values
    ionZ = 33;
    ionA = 76;
    ECbranching = 0.0;
    daughterExcitEnergy = 0.0*keV;
    sourceVolumeRadius = 0;
    sourceVolumeThickness = 0;
    sourceVolumeXcenter = 0;
    sourceVolumeYcenter = 0;
    sourceVolumeZcenter = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGenerator::~PrimaryGenerator()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetIonZandA(G4int Z, G4int A)
{
  ionZ = Z;
  ionA = A;
}
void PrimaryGenerator::SetECbranching(G4double branching)
{
  ECbranching = branching;
}
void PrimaryGenerator::SetDaughterExcitEnergy(G4double excitEnergy)
{
  daughterExcitEnergy = excitEnergy;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::GeneratePrimaryVertex(G4Event* event)
{
  //vertex position
  G4double Phi = CLHEP::RandFlat::shoot(0.0, 2.0*M_PI);     // phi uniform
  G4double R = sqrt(CLHEP::RandFlat::shoot(0.0, 1.0)) * sourceVolumeRadius; // radius linear
  G4double Z = sourceVolumeZcenter - 0.5*sourceVolumeThickness + CLHEP::RandFlat::shoot(0.0, sourceVolumeThickness);
  G4ThreeVector position(R*cos(Phi)*CLHEP::mm, R*sin(Phi)*CLHEP::mm, Z*CLHEP::mm);
  //G4cout << sourceVolumeZcenter << "\t" << sourceVolumeThickness << "\t" << sourceVolumeRadius << "\t" << Z << "\t" << R << G4endl;
  
  // randomly choose original ion or hand-made EC according to branching ratio
  G4double random_number = CLHEP::RandFlat::shoot(0.0, 1.0);
  if (random_number > ECbranching)
  // Ordinary G4 ion decay
  {
    // Vertex at position and time
    G4double timeA = 0.0*s;
    G4PrimaryVertex* vertexA = new G4PrimaryVertex(position, timeA);
    // Getting ion from ion table
    G4double ionCharge   = 0.*eplus;
    G4double excitEnergy = 0.0*keV;
    G4ParticleDefinition* theIon
        = G4IonTable::GetIonTable()->GetIon(ionZ,ionA,excitEnergy);

    //particle 1 at vertex A - ion for beta- decay
    //
    G4PrimaryParticle* particle1 = new G4PrimaryParticle(theIon);
    particle1->SetMomentumDirection(G4ThreeVector(1,0,0));
    particle1->SetKineticEnergy(0*MeV);
    //
    vertexA->SetPrimary(particle1);
    event->AddPrimaryVertex(vertexA);
  } else
  // hand-made EC
  {
    // Vertex position
    G4double timeB = 0.0*s;
    G4PrimaryVertex* vertexB = new G4PrimaryVertex(position, timeB);
    
    // Particles 2 and 3 at vertex B - EC X-rays and excited doughter nucleus
    // Add gamma by hand
    G4ParticleDefinition* photon
      = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    G4PrimaryParticle* particle2 = new G4PrimaryParticle(photon);
    // random direction (isotropic)
    G4double phi = CLHEP::RandFlat::shoot(0.0, 2.0*M_PI);     // phi uniform
    G4double theta = acos(CLHEP::RandFlat::shoot(-1.0, 1.0)); // theta sine distributed
    particle2->SetMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));    
    particle2->SetKineticEnergy(daughterExcitEnergy);
    // Getting ion from ion table
    //G4double ionCharge   = 0.*eplus;
    //G4double excitEnergy = daughterExcitEnergy;
    //G4ParticleDefinition* theIon
    //    = G4IonTable::GetIonTable()->GetIon(ionZ-1,ionA,excitEnergy);
    //G4PrimaryParticle* particle2 = new G4PrimaryParticle(theIon);
    //particle2->SetKineticEnergy(0.0);

    // Add X-ray by hand
    G4PrimaryParticle* particle3 = new G4PrimaryParticle(photon);
    // random direction (isotropic)
    phi = CLHEP::RandFlat::shoot(0.0, 2.0*M_PI);     // phi uniform
    theta = acos(CLHEP::RandFlat::shoot(-1.0, 1.0)); // theta sine distributed
    particle3->SetMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));    
    particle3->SetKineticEnergy(9.8*keV);
    //
    vertexB->SetPrimary(particle2);
    vertexB->SetPrimary(particle3);
    event->AddPrimaryVertex(vertexB);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetSourceVolumeRadius(G4double value)
{
  sourceVolumeRadius = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetSourceVolumeThickness(G4double value)
{
  sourceVolumeThickness = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetSourceVolumeXcenter(G4double value)
{
  sourceVolumeXcenter = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetSourceVolumeYcenter(G4double value)
{
  sourceVolumeYcenter = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetSourceVolumeZcenter(G4double value)
{
  sourceVolumeZcenter = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
