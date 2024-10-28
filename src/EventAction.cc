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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "HPGeSD.hh"
#include "HPGeHit.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(HistoManager* histo, DetectorConstruction* det)
    : G4UserEventAction(), 
    glPrintModulo(100000)
{
  //G4cout << "************************************************" << G4endl
  //       << "* DEBUG: EventAction::EventAction() is called! *" << G4endl
  //       << "************************************************" << G4endl;
  histoManager = histo;
  detectorConstruction = det;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
    //delete eventMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{
     //init the variables
     glEventID = evt->GetEventID();

     //printing survey
     if (glEventID%glPrintModulo == 0)
        G4cout << "\n---> Begin of Event: " << glEventID << G4endl;

     //safe survey
     if ((glEventID%100000 == 0))
         histoManager->SaveFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* evt)
{
    G4bool noHits = true;
    for (int iDet = 0; iDet < detectorConstruction->vecDetectorNames.size(); iDet++)
    {
      G4String name = detectorConstruction->vecDetectorNames.at(iDet);
      G4cout << "EndOfEventAction knows detector " << name << G4endl;

      G4String HCName = name + "HitsCollection";
      HPGeHitsCollection *HPGe_HC;
      G4int fHPGe_HCID = G4SDManager::GetSDMpointer()->GetCollectionID(HCName);
      // Get hits collection
      HPGe_HC = GetHitsCollection(fHPGe_HCID, evt);

      //get through all hits of this event
      G4int nofHits = HPGe_HC->entries();
      if (nofHits > 0) noHits = false;
      //G4cout << "* DEBUG: event " << evt->GetEventID() << ", HC " << HPGe_HC->GetName() << ": " << nofHits << " hits" << G4endl;
      G4double HPGeEdep = 0.0 * CLHEP::keV;
      for (G4int i = 0; i < nofHits; i++) {
        HPGeHit *hit = (*HPGe_HC)[i];
        //hit->Print();
        HPGeEdep += hit->GetEdep();
      }
      histoManager->SetEdep(iDet, HPGeEdep/CLHEP::keV);
    }
    if (noHits)
      return;
    // Fill Histogram
    histoManager->Fill();
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HPGeHitsCollection* EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
  HPGeHitsCollection* hitsCollection
    = static_cast<HPGeHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("EventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
