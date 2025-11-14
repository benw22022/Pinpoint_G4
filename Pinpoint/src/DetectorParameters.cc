#include "DetectorParameters.hh"
#include "G4SystemOfUnits.hh"

DetectorParameters *DetectorParameters::me = 0;

DetectorParameters::DetectorParameters()
{
    fexpHall_x = 1*m;
    fexpHall_y = 1*m;
    fexpHall_z = 3*m;
    
    // fnumSCTLayers = 50;
    fnumSCTLayers = 100;
    ftargetStartPosZ = 0*mm;
    fdetWidth = 300*mm;
    // fdetHeight = 300*2*mm;
    fdetHeight = 300*mm;
    flayerShift = 3*mm; // amount to translate layers up/down left/right to plug gaps

    // ftungstenThickness = 13.92*mm;
    ftungstenThickness = 0.9*mm;
}

DetectorParameters* DetectorParameters::Get()
{
  if (!me)
    me = new DetectorParameters();
  return me; 
}
