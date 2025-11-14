#ifndef GPSGenerator_HH
#define GPSGenerator_HH

#include "generators/GeneratorBase.hh"

class G4GeneralParticleSource;
class G4Event;

class GPSGenerator : public GeneratorBase
{
  public:
    GPSGenerator();
    ~GPSGenerator();

    // override methods from common base class
    void LoadData() override;
    void GeneratePrimaries(G4Event *anEvent) override;

  private:
    G4GeneralParticleSource *fGPS;
};

#endif
