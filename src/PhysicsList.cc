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
/// \file B3PhysicsList.cc
/// \brief Implementation of the B3PhysicsList class

#include "PhysicsList.hh"

#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"

#include "G4DecayTable.hh"
#include "G4ParticleTypes.hh" // provides GenericIon
#include "G4VDecayChannel.hh"
#include "G4GeneralPhaseSpaceDecay.hh"
#include "G4BetaMinusDecay.hh"
#include "G4ECDecay.hh"
#include "G4Ions.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() 
: G4VModularPhysicsList(){
  SetVerboseLevel(0);

  G4cout << "Begin of PhysicsList::PhysicsList()" << G4endl;

  // Default physics
  RegisterPhysics(new G4DecayPhysics());

  // EM physics
  RegisterPhysics(new G4EmStandardPhysics());

  // Radioactive decay
  RegisterPhysics(new G4RadioactiveDecayPhysics());

  G4cout << "End of PhysicsList::PhysicsList()" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructParticle()
{
    G4cout << "Begin of PhysicsList::ConstructParticle()" << G4endl;

    G4VModularPhysicsList::ConstructParticle();

    // inspired by exapmles/extended/field/field04 F04PhysicsList

    G4GenericIon::GenericIonDefinition();

    //G4int Z = 11, A = 24;
    //G4double excitEnergy = 0.*CLHEP::keV;
    //G4ParticleDefinition* ion
    //   = G4IonTable::GetIonTable()->GetIon(Z,A,excitEnergy);
    //G4DecayTable* ionDecayTable = ion->GetDecayTable();
    //G4DecayTable* ionDecayTable = new G4DecayTable();
    //G4BetaMinusDecay* beta = new G4BetaMinusDecay(ion, 1.0, 1263.*CLHEP::keV, 0.0, G4Ions::G4FloatLevelBase::no_Float, G4BetaDecayType::allowed);
    //ionDecayTable -> Insert(beta);
    //G4ECDecay* ec = new G4ECDecay(ion, 1.0, 921.5*CLHEP::keV, 0.0, G4Ions::G4FloatLevelBase::no_Float, G4RadioactiveDecayMode::KshellEC);
    //ionDecayTable -> Insert(ec);
    //ion -> SetDecayTable(ionDecayTable);

    G4Electron::ElectronDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
    G4cout << "Begin of PhysicsList::ConstructProcess()" << G4endl;

    G4VModularPhysicsList::ConstructProcess();
/*
    fStepMaxProcess = new F04StepMax();
    G4AutoDelete::Register(fStepMaxProcess);
    
    G4DecayWithSpin* decayWithSpin = new G4DecayWithSpin();

    G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();

    G4VProcess* decay;
    decay = processTable->FindProcess("Decay",G4MuonPlus::MuonPlus());

    G4ProcessManager* pmanager;
    pmanager = G4MuonPlus::MuonPlus()->GetProcessManager();

    if (pmanager) {
      if (decay) pmanager->RemoveProcess(decay);
      pmanager->AddProcess(decayWithSpin);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(decayWithSpin, idxPostStep);
      pmanager ->SetProcessOrdering(decayWithSpin, idxAtRest);
    }

    decay = processTable->FindProcess("Decay",G4MuonMinus::MuonMinus());

    pmanager = G4MuonMinus::MuonMinus()->GetProcessManager();

    if (pmanager) {
      if (decay) pmanager->RemoveProcess(decay);
      pmanager->AddProcess(decayWithSpin);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(decayWithSpin, idxPostStep);
      pmanager ->SetProcessOrdering(decayWithSpin, idxAtRest);
    }

    G4VProcess* process = processTable->
         FindProcess("muMinusCaptureAtRest",G4MuonMinus::MuonMinus());

    if (pmanager) {
       if (process) pmanager->RemoveProcess(process);
       process = new G4MuonMinusCapture(new G4MuMinusCapturePrecompound());
       pmanager->AddRestProcess(process);
    }

    G4PionDecayMakeSpin* poldecay = new G4PionDecayMakeSpin();

    decay = processTable->FindProcess("Decay",G4PionPlus::PionPlus());

    pmanager = G4PionPlus::PionPlus()->GetProcessManager();

    if (pmanager) {
      if (decay) pmanager->RemoveProcess(decay);
      pmanager->AddProcess(poldecay);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(poldecay, idxPostStep);
      pmanager ->SetProcessOrdering(poldecay, idxAtRest);
    }

    decay = processTable->FindProcess("Decay",G4PionMinus::PionMinus());

    pmanager = G4PionMinus::PionMinus()->GetProcessManager();

    if (pmanager) {
      if (decay) pmanager->RemoveProcess(decay);
      pmanager->AddProcess(poldecay);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(poldecay, idxPostStep);
      pmanager ->SetProcessOrdering(poldecay, idxAtRest);
    }

    AddStepMax();
    //*/
}
