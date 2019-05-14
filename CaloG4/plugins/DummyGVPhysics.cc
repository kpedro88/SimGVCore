#include "DummyGVPhysics.h"
#include "SimGVCore/CaloG4/interface/DummyGVEMPhysics.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "G4DecayPhysics.hh"

DummyGVPhysics::DummyGVPhysics(const edm::ParameterSet & p) 
  : PhysicsList(p) {

  bool emPhys  = p.getUntrackedParameter<bool>("EMPhysics",true);
  int  ver     = p.getUntrackedParameter<int>("Verbosity",0);
  if (emPhys) {
    RegisterPhysics(new DummyGVEMPhysics(ver));
  }
  RegisterPhysics(new G4DecayPhysics(ver));
  edm::LogInfo("PhysicsList") << "DummyGVPhysics constructed with EM Physics "
			      << emPhys << " and Decay";
}

#include "SimG4Core/Physics/interface/PhysicsListFactory.h"

DEFINE_PHYSICSLIST(DummyGVPhysics);

