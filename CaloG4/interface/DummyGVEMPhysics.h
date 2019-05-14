#ifndef SimG4Core_PhysicsLists_DummyGVEMPhysics_h
#define SimG4Core_PhysicsLists_DummyGVEMPhysics_h

// Physics List equivalent to GeantV 

#include "G4VPhysicsConstructor.hh"

class DummyGVEMPhysics : public G4VPhysicsConstructor {

public:
  DummyGVEMPhysics(G4int verb);
  ~DummyGVEMPhysics() override = default;
  void ConstructParticle() override;
  void ConstructProcess() override;

private:

  G4int verbose;
};

#endif

