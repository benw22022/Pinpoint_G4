#ifndef DetectorParameters_hh
#define DetectorParameters_hh

#include <set>

#include "G4String.hh"
#include "G4ThreeVector.hh"

/** This class stores Detector parameters for quick access
 *  It can be called form any other class to retrieve 
 *  configuration options or parameters whenever needed
**/

class DetectorParameters  {
  private:
    DetectorParameters();
  public:
    static DetectorParameters* Get();
    virtual ~DetectorParameters() {};

  private:
    //the singleton
    static DetectorParameters *me;
    
  public: //? make private
    G4double fexpHall_x;
    G4double fexpHall_y;
    G4double fexpHall_z;

    G4double ftungstenThickness;
    G4double fSCTThickness;
    
    G4double fnumSCTLayers;
    G4double flayerShift;
    
    G4double ftargetStartPosZ;
    G4double fdetWidth;
    G4double fdetHeight;

    G4double fstripWidth;
    G4double fstripsPerSide;
    G4double fstripStereoAngle;
    G4double fmoduleWidth; 
    G4double fmoduleHeight;

    G4double fSCTSideThickness;
    G4double fsideSeparation;
    G4double fsideThickness;

};

#endif 