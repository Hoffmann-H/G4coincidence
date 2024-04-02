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
/// \file HistoManager.cc
/// \brief Implementation of the HistoManager class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
#include "HistoMessenger.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
{
  fResFilename = "results/out.root";
  fDoWriteHisto = false;
  fDoWriteTree = true;
  HPGeEdep = 0;
  Det1Edep = 0;
  histoMessenger = new HistoMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
  //delete G4AnalysisManager::Instance();
  delete pH1Edep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
  G4cout << "Create void histograms" << G4endl;
  G4String hist_name = "H1Edep";
  G4String hist_title = "Energy deposition; E / keV; N";
  pH1Edep = new TH1F(hist_name.c_str(), hist_title.c_str(), 100, 0, 100);

  pEvtTree = new TTree("tEvt", "event tree");
  pEvtTree->Branch("det0", &HPGeEdep);
  pEvtTree->Branch("det1", &Det1Edep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::SetEdep(G4int det, G4double edep)
{
  // Edeps to be changed into an array
  if (det == 0)
    HPGeEdep = edep;
  else if (det == 1)
    Det1Edep = edep;
  else G4cout << "Unknown detector id: " << det << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Fill()
{
  if (fDoWriteHisto) pH1Edep->Fill(HPGeEdep);///CLHEP::keV);
  if (fDoWriteTree) pEvtTree->Fill();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::SaveFile()
{
  TFile* f = TFile::Open(fResFilename.c_str(), "UPDATE");
  if (fDoWriteHisto) pH1Edep->Write("", TObject::kOverwrite);
  if (fDoWriteTree) pEvtTree->Write("", TObject::kOverwrite);
  f->Save();
  f->Close();
  G4cout << "Save file" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::CloseFile()
{
  SaveFile();
  //delete pH1Edep;
  G4cout << "Close file" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::SetResultsFile(G4String filename)
{
    fResFilename = filename;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
