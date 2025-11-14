#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4ThreeVector.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"

#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "DetectorParameters.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* manager) 
  : det(manager) {
    detDir = new G4UIdirectory("/det/");
    detDir->SetGuidance("detector control");
 
    // GENERAL OPTIONS  
    // detGdmlCmd = new G4UIcmdWithABool("/det/saveGdml", this);
    // detGdmlCmd->SetParameterName("saveGdml", true);
    // detGdmlCmd->SetDefaultValue(false);
    
    
    // magnetFieldCmd = new G4UIcmdWithADoubleAndUnit("/det/magnetField", this);
    // magnetFieldCmd->SetUnitCategory("Magnetic flux density");
    // magnetFieldCmd->SetDefaultUnit("tesla");
    // magnetFieldCmd->SetUnitCandidates("tesla kG G");
    // magnetFieldCmd->SetDefaultValue(1.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionMessenger::~DetectorConstructionMessenger() {
//   delete detGdmlCmd;
  // delete magnetFieldCmd;
  delete detDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValues) {
  
  // flare
//   if (command == detGdmlCmd) det->SaveGDML(detGdmlCmd->GetNewBoolValue(newValues));
    // if (command == magnetFieldCmd) { 
    //   G4cout << "Changing magnetic field!!!" << G4endl;
    //   GeometricalParameters::Get()->SetSpectrometerMagnetField(magnetFieldCmd->ConvertToDimensionedDouble(newValues)); 
    //   G4RunManager::GetRunManager()->ReinitializeGeometry();
    //   }; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......