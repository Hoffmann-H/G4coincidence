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
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGenerator::~PrimaryGenerator()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::GeneratePrimaryVertex(G4Event* event)
{
  //vertex position
  //
  G4ThreeVector position(0.0,0.0,0.0);
  G4double timeA = 0.0;
  // 
  G4PrimaryVertex* vertexA = new G4PrimaryVertex(1.0*cm,0,0, timeA);
  
    // Getting ion from ion table
    G4int Z = 33, A = 76;
    G4double ionCharge   = 0.*eplus;
    G4double excitEnergy = 0.*keV;
    G4ParticleDefinition* ionA
       = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
    // Output if ion decay table is ready... No.
    G4DecayTable* ionDecayTable = ionA->GetDecayTable();
    if (ionDecayTable) {
      G4int n = ionDecayTable->entries();
      G4cout << "ionDecayTable->entries():" << n << G4endl;
    } else {
      G4cout << "No ionDecayTable!" << G4endl;
    }

  //particle 1 at vertex A - ion for beta- decay
  //
  G4PrimaryParticle* particle1 = new G4PrimaryParticle(ionA);
  particle1->SetMomentumDirection(G4ThreeVector(1,0,0));
  particle1->SetKineticEnergy(0*MeV);
  //
  vertexA->SetPrimary(particle1);
  event->AddPrimaryVertex(vertexA);
  
  //vertex (B) position
  //
  G4double timeB = 0.0*s;
  // 
  G4PrimaryVertex* vertexB = new G4PrimaryVertex(position, timeB);
  
  //particles 2 and 3 at vertex B - EC X-rays and excited doughter nucleus
  //
    // Getting ion from ion table
    Z = 32, A = 76;
    ionCharge   = 0.*eplus;
    excitEnergy = 562.93*keV;
    G4ParticleDefinition* ionB
        = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
  G4PrimaryParticle* particle2 = new G4PrimaryParticle(ionB);
  particle2->SetKineticEnergy(0.0);
  //
  G4ParticleDefinition* photon
    = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  G4PrimaryParticle* particle3 = new G4PrimaryParticle(photon);
  particle3->SetMomentumDirection(G4ThreeVector(0,0,1));    
  particle3->SetKineticEnergy(9.8*keV);
  //
  vertexB->SetPrimary(particle2);
  vertexB->SetPrimary(particle3);  
  event->AddPrimaryVertex(vertexB);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
