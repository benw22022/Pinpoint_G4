#ifndef fasernux_PixelSD_hh
#define fasernux_PixelSD_hh

#include "PixelHit.hh"
#include "G4VSensitiveDetector.hh"
#include <set>
#include <tuple>
#include <vector>

class G4Step;
class G4HCofThisEvent;

class PixelSD : public G4VSensitiveDetector
{
public:
  PixelSD(const G4String& name, const G4String& hitsCollectionName);
  ~PixelSD() override = default;

  void Initialize(G4HCofThisEvent* hitCollection) override;
  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
  void EndOfEvent(G4HCofThisEvent* hitCollection) override;

  // Static methods to track if particles come from muons
  static void RecordMuonDescendant(G4int trackID, G4bool fromMuon);
  static G4bool IsFromMuon(G4int trackID);
  static void ClearMuonHistory();

  // Static method to track descendants of primary lepton (trackId 1)
  // static void RecordTrackParent(G4int trackID, G4int parentID);
  // static G4bool IsFromPrimaryTrack(G4int trackID);
  // static void ClearTrackHistory();

private:
  PixelHitsCollection* fHitsCollection = nullptr;
  // Static set to track all descendants of the primary lepton (trackId 1)
  static std::set<G4int> sPrimaryDescendants;
  // Static set to track particles that have already hit each layer: (trackID, layerID)
  static std::set<std::pair<G4int, G4int>> sHitParticles;
  // Static set to track which particles come from muons
  static std::set<G4int> sMuonDescendants;

  G4long fCurrentHitId = 0;
};

#endif
