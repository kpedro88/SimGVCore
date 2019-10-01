#ifndef SimGVCore_Application_CMSDetectorConstruction_h
#define SimGVCore_Application_CMSDetectorConstruction_h

#include "Geant/UserDetectorConstruction.h"
#include "Geant/Typedefs.h"
#include "Geant/Config.h"
#include "Geant/Fwd.h"

#include <string>

namespace geant {
  inline namespace GEANT_IMPL_NAMESPACE {
    class RunManager;
  }
}

class CMSDetectorConstruction : public geant::UserDetectorConstruction {
public:
  CMSDetectorConstruction(geant::RunManager *runmgr);

  virtual ~CMSDetectorConstruction();

  // interface method to define the geometry for the application
  void CreateGeometry() override;
  void SetGDMLFile(const std::string &gdml) { fGDMLFileName = gdml; }

private:
  std::string fGDMLFileName;
};

#endif
