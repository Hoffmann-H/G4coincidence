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

#include "TGraph.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGenerator::PrimaryGenerator()
: G4VPrimaryGenerator()
{ 
    primaryMessenger = new PrimaryMessenger(this);

    // Primary particle standard values
    gammaEnergy = 0;
    ionZ = 33;
    ionA = 76;
    ECbranching = 0.0;
    daughterExcitEnergy = 0.0*keV;
    electronEnergy = -1;
    sourceVolumeRadius = 0;
    sourceVolumeThickness = 0;
    sourceVolumeXcenter = 0;
    sourceVolumeYcenter = 0;
    sourceVolumeZcenter = 0;
    betaCum = nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGenerator::~PrimaryGenerator()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGenerator::SetGammaEnergy(G4double gamma_energy)
{
  gammaEnergy = gamma_energy;
  G4cout << gamma_energy / CLHEP::keV << " keV gamma" << G4endl;
}
void PrimaryGenerator::SetIonZandA(G4int Z, G4int A)
{
  ionZ = Z;
  ionA = A;
  gammaEnergy = 0;
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
  G4double X = sourceVolumeXcenter + R*cos(Phi);
  G4double Y = sourceVolumeYcenter + R*sin(Phi);
  G4double Z = sourceVolumeZcenter - 0.5*sourceVolumeThickness + CLHEP::RandFlat::shoot(0.0, sourceVolumeThickness);
  G4ThreeVector position(X*CLHEP::mm, Y*CLHEP::mm, Z*CLHEP::mm);
  //G4cout << sourceVolumeZcenter << "\t" << sourceVolumeThickness << "\t" << sourceVolumeRadius << "\t" << Z << "\t" << R << G4endl;

  if (gammaEnergy > 0)
  /// Single gamma in uniform direction
  {
    G4PrimaryVertex* vertexA = new G4PrimaryVertex(position, 0.0*s);
    G4ParticleDefinition* theGamma = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    G4PrimaryParticle* particle1 = new G4PrimaryParticle(theGamma);
    // uniform direction
    G4double phi = CLHEP::RandFlat::shoot(0.0, 2.0*M_PI);     // phi uniform
    G4double theta = acos(CLHEP::RandFlat::shoot(-1.0, 1.0)); // theta sine distributed
    particle1->SetMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
    particle1->SetKineticEnergy(gammaEnergy);
    //
    vertexA->SetPrimary(particle1);
    event->AddPrimaryVertex(vertexA);
    return;
  }

  if (electronEnergy >= 0)
  /// electron in uniform direction
  {
    G4PrimaryVertex* vertexA = new G4PrimaryVertex(position, 0.0*s);
    G4ParticleDefinition* theElectron = G4ParticleTable::GetParticleTable()->FindParticle("e-"); // 11
    G4PrimaryParticle* particle1 = new G4PrimaryParticle(theElectron);
    // uniform direction
    G4double phi = CLHEP::RandFlat::shoot(0.0, 2.0*M_PI);     // phi uniform
    G4double theta = acos(CLHEP::RandFlat::shoot(-1.0, 1.0)); // theta sine distributed
    particle1->SetMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
    G4double thisElectronEnergy = electronEnergy;
    if (electronEnergy == 0) 
    /// dice an electron energy according to the beta spectrum
    {
      thisElectronEnergy = betaCum->Eval(CLHEP::RandFlat::shoot(0.0, 1.0)) * keV;
      //G4cout << thisElectronEnergy / keV << " keV" << G4endl;
    }
    particle1->SetKineticEnergy(thisElectronEnergy);
    //
    vertexA->SetPrimary(particle1);
    event->AddPrimaryVertex(vertexA);
    return;
  }

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
    G4double phi, theta;
    
    // Particles 2 and 3 at vertex B - EC X-rays and excited doughter nucleus
    // Add gamma by hand
    G4ParticleDefinition* photon
      = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
    //G4PrimaryParticle* particle2 = new G4PrimaryParticle(photon);
    //// random direction (isotropic)
    //phi = CLHEP::RandFlat::shoot(0.0, 2.0*M_PI);     // phi uniform
    //theta = acos(CLHEP::RandFlat::shoot(-1.0, 1.0)); // theta sine distributed
    //particle2->SetMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));    
    //particle2->SetKineticEnergy(daughterExcitEnergy);
    // Getting ion from ion table
    G4double ionCharge   = 0.*eplus;
    G4double excitEnergy = daughterExcitEnergy;
    G4ParticleDefinition* theIon
        = G4IonTable::GetIonTable()->GetIon(ionZ-1,ionA,excitEnergy);
    G4PrimaryParticle* particle2 = new G4PrimaryParticle(theIon);
    particle2->SetKineticEnergy(0.0);

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

void PrimaryGenerator::SetElec(G4double value)
{
  electronEnergy = value;
  if (value == 0) {
    betaCum = new TGraph();
    betaCum->AddPoint(0, 0);
    betaCum->AddPoint(0.000344632500802602, 0.9);
    betaCum->AddPoint(0.00069422217092036, 1.8);
    betaCum->AddPoint(0.00104639110060182, 2.7);
    betaCum->AddPoint(0.00139932353796063, 3.6);
    betaCum->AddPoint(0.001752228948499, 4.5);
    betaCum->AddPoint(0.00210486499172702, 5.4);
    betaCum->AddPoint(0.00245711725410483, 6.3);
    betaCum->AddPoint(0.00280894384406076, 7.2);
    betaCum->AddPoint(0.00316035016695889, 8.1);
    betaCum->AddPoint(0.00351136099738463, 9);
    betaCum->AddPoint(0.00386200426305242, 9.9);
    betaCum->AddPoint(0.00390094290434872, 10);
    betaCum->AddPoint(0.00429010159132463, 11);
    betaCum->AddPoint(0.00467887089188757, 12);
    betaCum->AddPoint(0.00506730886365183, 13);
    betaCum->AddPoint(0.00545547056125162, 14);
    betaCum->AddPoint(0.00584338801647412, 15);
    betaCum->AddPoint(0.00623108975762981, 16);
    betaCum->AddPoint(0.00661861932792938, 17);
    betaCum->AddPoint(0.00700602527555028, 18);
    betaCum->AddPoint(0.00739333712979633, 19);
    betaCum->AddPoint(0.00778057340904449, 20);
    betaCum->AddPoint(0.00816776414309524, 21);
    betaCum->AddPoint(0.00855494736969588, 22);
    betaCum->AddPoint(0.00894214911467348, 23);
    betaCum->AddPoint(0.00932938239094159, 24);
    betaCum->AddPoint(0.00971666821936055, 25);
    betaCum->AddPoint(0.0101040361292342, 26);
    betaCum->AddPoint(0.0104915066409261, 27);
    betaCum->AddPoint(0.0108790852598998, 28);
    betaCum->AddPoint(0.0112667844985721, 29);
    betaCum->AddPoint(0.0116546298822735, 30);
    betaCum->AddPoint(0.0120426364259041, 31);
    betaCum->AddPoint(0.012430807132444, 32);
    betaCum->AddPoint(0.0128191515113301, 33);
    betaCum->AddPoint(0.0132076885814359, 34);
    betaCum->AddPoint(0.0135964308551785, 35);
    betaCum->AddPoint(0.0139853798340476, 36);
    betaCum->AddPoint(0.0143745405230102, 37);
    betaCum->AddPoint(0.0147639274364698, 38);
    betaCum->AddPoint(0.0151535505843599, 39);
    betaCum->AddPoint(0.0155434094661838, 40);
    betaCum->AddPoint(0.0159335070849216, 41);
    betaCum->AddPoint(0.0163238554524934, 42);
    betaCum->AddPoint(0.0167144620763494, 43);
    betaCum->AddPoint(0.0171053249545029, 44);
    betaCum->AddPoint(0.0174964470899339, 45);
    betaCum->AddPoint(0.0178878379920792, 46);
    betaCum->AddPoint(0.0182795021654089, 47);
    betaCum->AddPoint(0.0186714376079364, 48);
    betaCum->AddPoint(0.0190636448201583, 49);
    betaCum->AddPoint(0.0194561318100214, 50);
    betaCum->AddPoint(0.019848902581499, 51);
    betaCum->AddPoint(0.0202419531306178, 52);
    betaCum->AddPoint(0.0206352834573779, 53);
    betaCum->AddPoint(0.0210289010692292, 54);
    betaCum->AddPoint(0.0214228084686552, 55);
    betaCum->AddPoint(0.0218169996496959, 56);
    betaCum->AddPoint(0.0222114736113577, 57);
    betaCum->AddPoint(0.0226062363596009, 58);
    betaCum->AddPoint(0.0230012893959155, 59);
    betaCum->AddPoint(0.0233966282158313, 60);
    betaCum->AddPoint(0.0237922523188518, 61);
    betaCum->AddPoint(0.0241881667099436, 62);
    betaCum->AddPoint(0.0245843723901001, 63);
    betaCum->AddPoint(0.0249808643543546, 64);
    betaCum->AddPoint(0.0253776401002237, 65);
    betaCum->AddPoint(0.025774703131184, 66);
    betaCum->AddPoint(0.0261720534472357, 67);
    betaCum->AddPoint(0.0265696865439087, 68);
    betaCum->AddPoint(0.0269676009197128, 69);
    betaCum->AddPoint(0.0273657995776283, 70);
    betaCum->AddPoint(0.0277642825176549, 71);
    betaCum->AddPoint(0.0281630452353228, 72);
    betaCum->AddPoint(0.0285620852281485, 73);
    betaCum->AddPoint(0.028961403997622, 74);
    betaCum->AddPoint(0.0293610010432467, 75);
    betaCum->AddPoint(0.0297608703590625, 76);
    betaCum->AddPoint(0.0301610114445726, 77);
    betaCum->AddPoint(0.0305614263017639, 78);
    betaCum->AddPoint(0.0309621124281529, 79);
    betaCum->AddPoint(0.0313630663202628, 80);
    betaCum->AddPoint(0.0317642859761071, 81);
    betaCum->AddPoint(0.0321657718961823, 82);
    betaCum->AddPoint(0.0325675225789985, 83);
    betaCum->AddPoint(0.0329695320185956, 84);
    betaCum->AddPoint(0.0333717992139801, 85);
    betaCum->AddPoint(0.0337743271681322, 86);
    betaCum->AddPoint(0.0341771118770784, 87);
    betaCum->AddPoint(0.0345801488363487, 88);
    betaCum->AddPoint(0.0349834380459431, 89);
    betaCum->AddPoint(0.0353869785048683, 90);
    betaCum->AddPoint(0.0357907672101441, 91);
    betaCum->AddPoint(0.0361948011587905, 92);
    betaCum->AddPoint(0.0365990788493176, 93);
    betaCum->AddPoint(0.0370035987802352, 94);
    betaCum->AddPoint(0.0374083589495567, 95);
    betaCum->AddPoint(0.0378133568547987, 96);
    betaCum->AddPoint(0.0382185899934779, 97);
    betaCum->AddPoint(0.0386240568641042, 98);
    betaCum->AddPoint(0.0390297559651875, 99);
    betaCum->AddPoint(0.0394356842937479, 100);
    betaCum->AddPoint(0.0430989040402511, 109);
    betaCum->AddPoint(0.0467790335674076, 118);
    betaCum->AddPoint(0.0504746044179739, 127);
    betaCum->AddPoint(0.0541842742598681, 136);
    betaCum->AddPoint(0.0579067953548801, 145);
    betaCum->AddPoint(0.061641059603372, 154);
    betaCum->AddPoint(0.0653861886336796, 163);
    betaCum->AddPoint(0.0691414662350613, 172);
    betaCum->AddPoint(0.0729063788979288, 181);
    betaCum->AddPoint(0.0766806833808982, 190);
    betaCum->AddPoint(0.0804643436482088, 199);
    betaCum->AddPoint(0.084257562401014, 208);
    betaCum->AddPoint(0.0880608126086714, 217);
    betaCum->AddPoint(0.0918748465176831, 226);
    betaCum->AddPoint(0.0957006821382857, 235);
    betaCum->AddPoint(0.09953961225339, 244);
    betaCum->AddPoint(0.103393249463282, 253);
    betaCum->AddPoint(0.107263503663273, 262);
    betaCum->AddPoint(0.111152586548168, 271);
    betaCum->AddPoint(0.115063034134619, 280);
    betaCum->AddPoint(0.118997706761121, 289);
    betaCum->AddPoint(0.122959523324283, 298);
    betaCum->AddPoint(0.126950839661951, 307);
    betaCum->AddPoint(0.130972682793303, 316);
    betaCum->AddPoint(0.135024737405434, 325);
    betaCum->AddPoint(0.139106273774191, 334);
    betaCum->AddPoint(0.143216584697773, 343);
    betaCum->AddPoint(0.14735499450567, 352);
    betaCum->AddPoint(0.151520890589951, 361);
    betaCum->AddPoint(0.155713687369508, 370);
    betaCum->AddPoint(0.15993283079452, 379);
    betaCum->AddPoint(0.16417781636434, 388);
    betaCum->AddPoint(0.168448193631961, 397);
    betaCum->AddPoint(0.172743543681665, 406);
    betaCum->AddPoint(0.177063497146907, 415);
    betaCum->AddPoint(0.181407738714783, 424);
    betaCum->AddPoint(0.185775993612616, 433);
    betaCum->AddPoint(0.190168050130314, 442);
    betaCum->AddPoint(0.194583741602484, 451);
    betaCum->AddPoint(0.199022932895025, 460);
    betaCum->AddPoint(0.203485524909594, 469);
    betaCum->AddPoint(0.20797147710596, 478);
    betaCum->AddPoint(0.212480812006475, 487);
    betaCum->AddPoint(0.217013592673718, 496);
    betaCum->AddPoint(0.221569949737323, 505);
    betaCum->AddPoint(0.226150067880562, 514);
    betaCum->AddPoint(0.230754185840349, 523);
    betaCum->AddPoint(0.23538261442512, 532);
    betaCum->AddPoint(0.240035371652753, 541);
    betaCum->AddPoint(0.244711912482372, 550);
    betaCum->AddPoint(0.249411453136182, 559);
    betaCum->AddPoint(0.254133214340861, 568);
    betaCum->AddPoint(0.258876407814147, 577);
    betaCum->AddPoint(0.263640249778247, 586);
    betaCum->AddPoint(0.268423965464308, 595);
    betaCum->AddPoint(0.273226780103478, 604);
    betaCum->AddPoint(0.278047918926905, 613);
    betaCum->AddPoint(0.282886607165735, 622);
    betaCum->AddPoint(0.287742083564526, 631);
    betaCum->AddPoint(0.292613582363367, 640);
    betaCum->AddPoint(0.297500337802344, 649);
    betaCum->AddPoint(0.302401602139426, 658);
    betaCum->AddPoint(0.307316623128111, 667);
    betaCum->AddPoint(0.312244648521895, 676);
    betaCum->AddPoint(0.317184935083218, 685);
    betaCum->AddPoint(0.322136748583457, 694);
    betaCum->AddPoint(0.327099354793991, 703);
    betaCum->AddPoint(0.332072023990668, 712);
    betaCum->AddPoint(0.337054035458275, 721);
    betaCum->AddPoint(0.342044663977132, 730);
    betaCum->AddPoint(0.347043197840967, 739);
    betaCum->AddPoint(0.352048938856918, 748);
    betaCum->AddPoint(0.357061184327655, 757);
    betaCum->AddPoint(0.362079240564785, 766);
    betaCum->AddPoint(0.367102427393329, 775);
    betaCum->AddPoint(0.372130060133834, 784);
    betaCum->AddPoint(0.37716145861132, 793);
    betaCum->AddPoint(0.382195951659746, 802);
    betaCum->AddPoint(0.387232881626482, 811);
    betaCum->AddPoint(0.392271599867836, 820);
    betaCum->AddPoint(0.397311457740119, 829);
    betaCum->AddPoint(0.40235181560858, 838);
    betaCum->AddPoint(0.407392038342939, 847);
    betaCum->AddPoint(0.412431504326326, 856);
    betaCum->AddPoint(0.41746959644634, 865);
    betaCum->AddPoint(0.422505702095053, 874);
    betaCum->AddPoint(0.427539222177943, 883);
    betaCum->AddPoint(0.432569566609432, 892);
    betaCum->AddPoint(0.437596149808411, 901);
    betaCum->AddPoint(0.442618395202709, 910);
    betaCum->AddPoint(0.447635744238038, 919);
    betaCum->AddPoint(0.452647638360108, 928);
    betaCum->AddPoint(0.457653523519102, 937);
    betaCum->AddPoint(0.46265286368308, 946);
    betaCum->AddPoint(0.467645127324573, 955);
    betaCum->AddPoint(0.472629791925054, 964);
    betaCum->AddPoint(0.477606348479403, 973);
    betaCum->AddPoint(0.482574296991443, 982);
    betaCum->AddPoint(0.487533150978407, 991);
    betaCum->AddPoint(0.492482428461995, 1000);
    betaCum->AddPoint(0.497421656472851, 1009);
    betaCum->AddPoint(0.502350366546086, 1018);
    betaCum->AddPoint(0.507268103730225, 1027);
    betaCum->AddPoint(0.512174435596139, 1036);
    betaCum->AddPoint(0.517068934219171, 1045);
    betaCum->AddPoint(0.521951180683606, 1054);
    betaCum->AddPoint(0.526820760578195, 1063);
    betaCum->AddPoint(0.531677277509574, 1072);
    betaCum->AddPoint(0.536520348597785, 1081);
    betaCum->AddPoint(0.541349590962872, 1090);
    betaCum->AddPoint(0.54616464424723, 1099);
    betaCum->AddPoint(0.550965161606663, 1108);
    betaCum->AddPoint(0.555750796196975, 1117);
    betaCum->AddPoint(0.560521219191851, 1126);
    betaCum->AddPoint(0.565276115278386, 1135);
    betaCum->AddPoint(0.570015173648146, 1144);
    betaCum->AddPoint(0.574738101510574, 1153);
    betaCum->AddPoint(0.579444615084057, 1162);
    betaCum->AddPoint(0.58413443959592, 1171);
    betaCum->AddPoint(0.588807291264548, 1180);
    betaCum->AddPoint(0.593462796218926, 1189);
    betaCum->AddPoint(0.598100522029927, 1198);
    betaCum->AddPoint(0.602720045277363, 1207);
    betaCum->AddPoint(0.607320947045518, 1216);
    betaCum->AddPoint(0.611902808418675, 1225);
    betaCum->AddPoint(0.616465219490058, 1234);
    betaCum->AddPoint(0.6210077838663, 1243);
    betaCum->AddPoint(0.625530109658505, 1252);
    betaCum->AddPoint(0.630031813986716, 1261);
    betaCum->AddPoint(0.634512518475448, 1270);
    betaCum->AddPoint(0.638971849253684, 1279);
    betaCum->AddPoint(0.643409441459348, 1288);
    betaCum->AddPoint(0.647824939239305, 1297);
    betaCum->AddPoint(0.65221799124489, 1306);
    betaCum->AddPoint(0.656588255136376, 1315);
    betaCum->AddPoint(0.660935402087448, 1324);
    betaCum->AddPoint(0.665259103271791, 1333);
    betaCum->AddPoint(0.669559034367559, 1342);
    betaCum->AddPoint(0.673834884566318, 1351);
    betaCum->AddPoint(0.678086352068573, 1360);
    betaCum->AddPoint(0.682313139579299, 1369);
    betaCum->AddPoint(0.686514958812411, 1378);
    betaCum->AddPoint(0.690691534995235, 1387);
    betaCum->AddPoint(0.694842593355097, 1396);
    betaCum->AddPoint(0.698967868128261, 1405);
    betaCum->AddPoint(0.703067111568875, 1414);
    betaCum->AddPoint(0.707140075931083, 1423);
    betaCum->AddPoint(0.711186522477973, 1432);
    betaCum->AddPoint(0.71520622598604, 1441);
    betaCum->AddPoint(0.719198965736252, 1450);
    betaCum->AddPoint(0.723164525514044, 1459);
    betaCum->AddPoint(0.727102707122733, 1468);
    betaCum->AddPoint(0.731013321374576, 1477);
    betaCum->AddPoint(0.734896179081831, 1486);
    betaCum->AddPoint(0.738751104570164, 1495);
    betaCum->AddPoint(0.742577935678653, 1504);
    betaCum->AddPoint(0.746376514750844, 1513);
    betaCum->AddPoint(0.750146697643697, 1522);
    betaCum->AddPoint(0.753888344718638, 1531);
    betaCum->AddPoint(0.757601325346036, 1540);
    betaCum->AddPoint(0.761285535923079, 1549);
    betaCum->AddPoint(0.764940863838014, 1558);
    betaCum->AddPoint(0.768567209992501, 1567);
    betaCum->AddPoint(0.772164493306078, 1576);
    betaCum->AddPoint(0.775732632698284, 1585);
    betaCum->AddPoint(0.779271560602067, 1594);
    betaCum->AddPoint(0.782781222963788, 1603);
    betaCum->AddPoint(0.786261574738744, 1612);
    betaCum->AddPoint(0.789712579891175, 1621);
    betaCum->AddPoint(0.793134220403202, 1630);
    betaCum->AddPoint(0.796526487265884, 1639);
    betaCum->AddPoint(0.799889371470281, 1648);
    betaCum->AddPoint(0.803222877520863, 1657);
    betaCum->AddPoint(0.80652701893104, 1666);
    betaCum->AddPoint(0.809801827232102, 1675);
    betaCum->AddPoint(0.813047342964281, 1684);
    betaCum->AddPoint(0.816263611172277, 1693);
    betaCum->AddPoint(0.819450694918672, 1702);
    betaCum->AddPoint(0.822608666274985, 1711);
    betaCum->AddPoint(0.825737610826149, 1720);
    betaCum->AddPoint(0.828837627670505, 1729);
    betaCum->AddPoint(0.831908824915336, 1738);
    betaCum->AddPoint(0.834951315172392, 1747);
    betaCum->AddPoint(0.837965120964024, 1756);
    betaCum->AddPoint(0.840950138687421, 1765);
    betaCum->AddPoint(0.84390623320848, 1774);
    betaCum->AddPoint(0.84683328290651, 1783);
    betaCum->AddPoint(0.849731166160818, 1792);
    betaCum->AddPoint(0.852599770359652, 1801);
    betaCum->AddPoint(0.855438996404671, 1810);
    betaCum->AddPoint(0.858248745197534, 1819);
    betaCum->AddPoint(0.861028926648839, 1828);
    betaCum->AddPoint(0.863779455173654, 1837);
    betaCum->AddPoint(0.866500249691518, 1846);
    betaCum->AddPoint(0.86919121110409, 1855);
    betaCum->AddPoint(0.871852231304088, 1864);
    betaCum->AddPoint(0.87448321119317, 1873);
    betaCum->AddPoint(0.877084056177465, 1882);
    betaCum->AddPoint(0.879654671663101, 1891);
    betaCum->AddPoint(0.882194976569617, 1900);
    betaCum->AddPoint(0.884704898825492, 1909);
    betaCum->AddPoint(0.887184361854734, 1918);
    betaCum->AddPoint(0.889633302594763, 1927);
    betaCum->AddPoint(0.892051662487467, 1936);
    betaCum->AddPoint(0.894439387479206, 1945);
    betaCum->AddPoint(0.896796437029748, 1954);
    betaCum->AddPoint(0.899122775103335, 1963);
    betaCum->AddPoint(0.901418370168674, 1972);
    betaCum->AddPoint(0.903683195198945, 1981);
    betaCum->AddPoint(0.905917236680739, 1990);
    betaCum->AddPoint(0.908120490109585, 1999);
    betaCum->AddPoint(0.910292946476543, 2008);
    betaCum->AddPoint(0.912434610286083, 2017);
    betaCum->AddPoint(0.914545495051615, 2026);
    betaCum->AddPoint(0.91662562329549, 2035);
    betaCum->AddPoint(0.918675026548999, 2044);
    betaCum->AddPoint(0.920693740847901, 2053);
    betaCum->AddPoint(0.922681815741367, 2062);
    betaCum->AddPoint(0.924639300778569, 2071);
    betaCum->AddPoint(0.926566254517617, 2080);
    betaCum->AddPoint(0.928462753534503, 2089);
    betaCum->AddPoint(0.930328878909688, 2098);
    betaCum->AddPoint(0.932164720732573, 2107);
    betaCum->AddPoint(0.93397037359703, 2116);
    betaCum->AddPoint(0.93574594561034, 2125);
    betaCum->AddPoint(0.937491558393196, 2134);
    betaCum->AddPoint(0.93920733356629, 2143);
    betaCum->AddPoint(0.940893410768193, 2152);
    betaCum->AddPoint(0.942549938646419, 2161);
    betaCum->AddPoint(0.944177070352949, 2170);
    betaCum->AddPoint(0.945774972553176, 2179);
    betaCum->AddPoint(0.947343825425904, 2188);
    betaCum->AddPoint(0.948883818158876, 2197);
    betaCum->AddPoint(0.950395148948773, 2206);
    betaCum->AddPoint(0.951878029505691, 2215);
    betaCum->AddPoint(0.953332680548662, 2224);
    betaCum->AddPoint(0.954759336310131, 2233);
    betaCum->AddPoint(0.956158244535951, 2242);
    betaCum->AddPoint(0.957529657476446, 2251);
    betaCum->AddPoint(0.958873845399821, 2260);
    betaCum->AddPoint(0.960191096592161, 2269);
    betaCum->AddPoint(0.961481699339549, 2278);
    betaCum->AddPoint(0.962745959945952, 2287);
    betaCum->AddPoint(0.963984202733214, 2296);
    betaCum->AddPoint(0.96519675202318, 2305);
    betaCum->AddPoint(0.966383959164517, 2314);
    betaCum->AddPoint(0.9675461890193, 2323);
    betaCum->AddPoint(0.968683810954075, 2332);
    betaCum->AddPoint(0.969797216857739, 2341);
    betaCum->AddPoint(0.970886807628127, 2350);
    betaCum->AddPoint(0.971952997676487, 2359);
    betaCum->AddPoint(0.972996219431945, 2368);
    betaCum->AddPoint(0.97401692334151, 2377);
    betaCum->AddPoint(0.975015573365598, 2386);
    betaCum->AddPoint(0.975992646978037, 2395);
    betaCum->AddPoint(0.976948626157125, 2404);
    betaCum->AddPoint(0.977883803693416, 2413);
    betaCum->AddPoint(0.978798206613732, 2422);
    betaCum->AddPoint(0.97969180203544, 2431);
    betaCum->AddPoint(0.980564569237978, 2440);
    betaCum->AddPoint(0.981416485698997, 2449);
    betaCum->AddPoint(0.982247539706873, 2458);
    betaCum->AddPoint(0.98305772765803, 2467);
    betaCum->AddPoint(0.983847053156045, 2476);
    betaCum->AddPoint(0.984615528362987, 2485);
    betaCum->AddPoint(0.985363173999418, 2494);
    betaCum->AddPoint(0.986090018893947, 2503);
    betaCum->AddPoint(0.986796101785016, 2512);
    betaCum->AddPoint(0.987481470420009, 2521);
    betaCum->AddPoint(0.988146181555247, 2530);
    betaCum->AddPoint(0.988790301406441, 2539);
    betaCum->AddPoint(0.98941390700003, 2548);
    betaCum->AddPoint(0.990017085272285, 2557);
    betaCum->AddPoint(0.99059993351976, 2566);
    betaCum->AddPoint(0.991162560300185, 2575);
    betaCum->AddPoint(0.991705084982015, 2584);
    betaCum->AddPoint(0.992227638194884, 2593);
    betaCum->AddPoint(0.992730362280045, 2602);
    betaCum->AddPoint(0.993213412191268, 2611);
    betaCum->AddPoint(0.993676954593948, 2620);
    betaCum->AddPoint(0.994121167865098, 2629);
    betaCum->AddPoint(0.994546244796037, 2638);
    betaCum->AddPoint(0.994952390790601, 2647);
    betaCum->AddPoint(0.995339823865142, 2656);
    betaCum->AddPoint(0.995708776900762, 2665);
    betaCum->AddPoint(0.996059496742421, 2674);
    betaCum->AddPoint(0.996392244649383, 2683);
    betaCum->AddPoint(0.996707296295217, 2692);
    betaCum->AddPoint(0.99700494266869, 2701);
    betaCum->AddPoint(0.997285490073767, 2710);
    betaCum->AddPoint(0.997549260580057, 2719);
    betaCum->AddPoint(0.997796592473263, 2728);
    betaCum->AddPoint(0.998027840705627, 2737);
    betaCum->AddPoint(0.998243376445483, 2746);
    betaCum->AddPoint(0.998443588428599, 2755);
    betaCum->AddPoint(0.998628883859068, 2764);
    betaCum->AddPoint(0.998799686157078, 2773);
    betaCum->AddPoint(0.998956437661589, 2782);
    betaCum->AddPoint(0.999099599630338, 2791);
    betaCum->AddPoint(0.99922965133894, 2800);
    betaCum->AddPoint(0.999347092333126, 2809);
    betaCum->AddPoint(0.999452442428742, 2818);
    betaCum->AddPoint(0.999546240900947, 2827);
    betaCum->AddPoint(0.999629047295011, 2836);
    betaCum->AddPoint(0.999701442372261, 2845);
    betaCum->AddPoint(0.999764028245213, 2854);
    betaCum->AddPoint(0.999817428828016, 2863);
    betaCum->AddPoint(0.999862290106723, 2872);
    betaCum->AddPoint(0.999899280589738, 2881);
    betaCum->AddPoint(0.99992909184835, 2890);
    betaCum->AddPoint(0.999952438832051, 2899);
    betaCum->AddPoint(0.999970060228886, 2908);
    betaCum->AddPoint(0.999982719051043, 2917);
    betaCum->AddPoint(0.999991202977183, 2926);
    betaCum->AddPoint(0.999996324775871, 2935);
    betaCum->AddPoint(0.999998922805561, 2944);
    betaCum->AddPoint(0.999999861406044, 2953);
    betaCum->AddPoint(1, 2960.6);
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
