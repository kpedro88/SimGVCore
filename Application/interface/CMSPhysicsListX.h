
#ifndef SimGVCore_Application_CMSPHYSICSLISTX_H
#define SimGVCore_Application_CMSPHYSICSLISTX_H

#include "Geant/GeantConfig.h"
#include "Geant/PhysicsList.h"

#include <string>

namespace cmsapp {

class CMSPhysicsListX : public geantphysics::PhysicsList {
public:
  // CTR
  CMSPhysicsListX(const geant::GeantConfig &config, const std::string &name = "CMS-PhysicsList",
                 bool useSamplingTables = false);
  // DTR
  virtual ~CMSPhysicsListX();
  // interface method to assigne physics-process to particles
  virtual void Initialize();
  // Setter for global basketized mode
  void SetBasketizing(bool flag = true) { fVectorized = flag; }

private:
  bool fUseSamplingTables = false;
  bool fVectorized        = false;
  bool fVectorizedMSC     = false;
};

} // namespace cmsapp

#endif // SimGVCore_Application_CMSPHYSICSLISTX_H
