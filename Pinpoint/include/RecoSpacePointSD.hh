#ifndef RecoSpacePointSD_HH
#define RecoSpacePointSD_HH

#include "SCTModuleHit.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4ThreeVector.hh"
#include  "G4LorentzVector.hh"
#include "DetectorConstruction.hh"

/// Dummy sensitive detector to hold spacepoints for visualisation
class RecoSpacePointSD : public G4VSensitiveDetector {
public:
    RecoSpacePointSD(const G4String& name) : G4VSensitiveDetector(name) {
        collectionName.insert("RecoSpacePoints");
    }

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) override {
        return false; // no actual hit processing
    }
};

#endif