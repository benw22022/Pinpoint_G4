#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "DetectorConstruction.hh"
#include "PixelSD.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"
#include "G4SDManager.hh"
#include <fstream>
#include "G4VisAttributes.hh"


G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Geometry parameters
  // https://iopscience.iop.org/article/10.1088/1748-0221/20/02/C02015
  G4int nLayers = 100;
  G4double tungstenThickness = 5 * mm;
  G4double boxThickness = 5 * mm - 50 * um;
  G4double siliconThickness = 50 * um;
  G4double detectorSizeX = 26.6 * cm;
  G4double detectorSizeY = 19.6 * cm;
  G4bool checkOverlaps = true;
  
  G4int nPixelsX = 12788; // 20.8um pixel pitch
  G4int nPixelsY = 8596;  // 22.8um pixel pitch
  G4double pixelSizeX = detectorSizeX / nPixelsX;
  G4double pixelSizeY = detectorSizeY / nPixelsY;
  G4cout << "Creating " << nPixelsX << " x " << nPixelsY << " pixels per silicon layer" << G4endl;
  G4cout << "Pixel size: " << pixelSizeX/micrometer << " x " << pixelSizeY/micrometer << " μm" << G4endl;

  auto layerThickness = tungstenThickness + boxThickness + siliconThickness;
  auto detectorThickness = nLayers * layerThickness;
  auto worldSizeX = 1.2 * detectorSizeX;
  auto worldSizeY = 1.2 * detectorSizeY;
  auto worldSizeZ = 1.2 * detectorThickness;

  // Get materials
  G4NistManager* nist = G4NistManager::Instance();
  G4Material* worldMaterial = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* tungstenMaterial = nist->FindOrBuildMaterial("G4_W");
  G4Material* siliconMaterial = nist->FindOrBuildMaterial("G4_Si");

  // World
  G4Box* worldS = new G4Box("World", 0.5 * worldSizeX, 0.5 * worldSizeY, worldSizeZ);
  G4LogicalVolume* worldLV = new G4LogicalVolume(worldS, worldMaterial, "World");
  G4VPhysicalVolume* worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World", nullptr, false, 0, checkOverlaps);

  G4VisAttributes* experimentalHallVisAtt = new G4VisAttributes(G4Colour(1.,1.,1.));
  experimentalHallVisAtt->SetForceWireframe(true);
  worldLV->SetVisAttributes(experimentalHallVisAtt);

  // Detector
  auto detectorS = new G4Box("Detector", 0.5 * detectorSizeX, 0.5 * detectorSizeY, 0.5 * detectorThickness);
  auto detectorLV = new G4LogicalVolume(detectorS, worldMaterial, "Detector");
  // Position detector so that first layer starts at z=0 (shift by half detector thickness)
  new G4PVPlacement(0, G4ThreeVector(0., 0., 0.5 * detectorThickness), detectorLV, "Detector", worldLV, false, 0, checkOverlaps);

  // Layer
  auto layerS = new G4Box("Layer", 0.5 * detectorSizeX, 0.5 * detectorSizeY, layerThickness / 2);
  auto layerLV = new G4LogicalVolume(layerS, worldMaterial, "Layer");
  new G4PVReplica("Layer", layerLV, detectorLV, kZAxis, nLayers, layerThickness);

  // Tungsten
  auto tungstenS = new G4Box("Tungsten", 0.5 * detectorSizeX, 0.5 * detectorSizeY, 0.5 * tungstenThickness);
  auto tungstenLV = new G4LogicalVolume(tungstenS, tungstenMaterial, "Tungsten");
  fTarget_phys.push_back(new G4PVPlacement(0, G4ThreeVector(0., 0., -0.5 * layerThickness + 0.5 * tungstenThickness), tungstenLV, "Tungsten", layerLV, false, 0, checkOverlaps));
  
  G4VisAttributes* TargetVisAtt =  new G4VisAttributes(G4Colour::Red());
  TargetVisAtt->SetForceWireframe(true);
  tungstenLV->SetVisAttributes(TargetVisAtt);


  // Silicon layer (will contain pixels)
  G4VisAttributes* invisAtrrib = new G4VisAttributes();
  invisAtrrib->SetVisibility(false);
  invisAtrrib->SetForceSolid(false);

  G4VisAttributes* LayerAtrrib = new G4VisAttributes(G4Colour::Green());
  LayerAtrrib->SetVisibility(true);
  LayerAtrrib->SetForceSolid(true);

  auto siliconLayerS = new G4Box("SiliconLayer", 0.5 * detectorSizeX, 0.5 * detectorSizeY, 0.5 * siliconThickness);
  auto siliconLayerLV = new G4LogicalVolume(siliconLayerS, siliconMaterial, "SiliconLayer");  // Changed to siliconMaterial
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -0.5 * layerThickness + tungstenThickness + 0.5 * siliconThickness), siliconLayerLV, "SiliconLayer", layerLV, false, 0, checkOverlaps);
  siliconLayerLV->SetVisAttributes(LayerAtrrib);

  // Create pixel row (Y direction)
  auto pixelRowS = new G4Box("SiliconPixelRow", 0.5 * detectorSizeX, 0.5 * pixelSizeY, 0.5 * siliconThickness);
  auto pixelRowLV = new G4LogicalVolume(pixelRowS, siliconMaterial, "SiliconPixelRow");  // Changed to siliconMaterial
  new G4PVReplica("SiliconPixelRow", pixelRowLV, siliconLayerLV, kYAxis, nPixelsY, pixelSizeY);
  pixelRowLV->SetVisAttributes(invisAtrrib);

  // Create individual pixels (X direction)
  auto pixelS = new G4Box("SiliconPixel", 0.5 * pixelSizeX, 0.5 * pixelSizeY, 0.5 * siliconThickness);
  fPixelLV = new G4LogicalVolume(pixelS, siliconMaterial, "SiliconPixel");
  new G4PVReplica("SiliconPixel", fPixelLV, pixelRowLV, kXAxis, nPixelsX, pixelSizeX);
  fPixelLV->SetVisAttributes(invisAtrrib);

  // // Box
  // auto boxS = new G4Box("Box", 0.5 * detectorSizeX, 0.5 * detectorSizeY, 0.5 * boxThickness);
  // auto boxLV = new G4LogicalVolume(boxS, worldMaterial, "Box");
  // new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -0.5 * layerThickness + tungstenThickness + siliconThickness + 0.5 * boxThickness), boxLV, "Box", layerLV, false, 0, checkOverlaps);

  // G4cout << "Detector consists of " << nLayers << " layers of: [ " << tungstenThickness / mm << "mm of " << tungstenMaterial->GetName() << " + " << siliconThickness / mm << "mm of "
  //        << siliconMaterial->GetName() <<  " + " << boxThickness / mm << "mm of " << worldMaterial->GetName() << " ] " << G4endl;

  // // Write GDML file if it doesn't exist
  // std::ifstream file(fWriteFile);
  // if (!file.good()) {
  //   fParser.Write(fWriteFile, worldPV);
  // }
  // file.close();
  
  return worldPV;
}

void DetectorConstruction::ConstructSDandField()
{
  if (fPixelLV) {
    auto pixelSD = new PixelSD("PixelDetector", "PixelHitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(pixelSD);
    fPixelLV->SetSensitiveDetector(pixelSD);
  }
}