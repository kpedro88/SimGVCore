#ifndef SimG4Core_PhysicsLists_DummyGVPhysics_H
#define SimG4Core_PhysicsLists_DummyGVPhysics_H

#include "SimG4Core/Physics/interface/PhysicsList.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class DummyGVPhysics : public PhysicsList {

public:
  DummyGVPhysics(const edm::ParameterSet &);
  ~DummyGVPhysics() override = default;
};
 
#endif
