#ifndef fasernux_DetectorConstruction_hh
#define fasernux_DetectorConstruction_hh

#include "G4VUserDetectorConstruction.hh"
#include "G4GDMLParser.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction() = default;
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    void SetReadFile(const G4String& File);
    void SetWriteFile(const G4String& File);
    std::vector<G4VPhysicalVolume*> GetTargetPhysVols() const { return fTarget_phys; }

    G4int GetNlayers() const { return fTarget_phys.size(); }

  private:
    G4String fWriteFile = "fasernux.gdml";
    G4GDMLParser fParser;
    G4LogicalVolume* fPixelLV;

    std::vector<G4VPhysicalVolume*> fTarget_phys;
};


#endif // fasernux_DetectorConstruction_hh