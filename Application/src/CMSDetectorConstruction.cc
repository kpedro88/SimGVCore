
#include "SimGVCore/Application/interface/CMSDetectorConstruction.h"

#include "Geant/RunManager.h"

CMSDetectorConstruction::CMSDetectorConstruction(geant::RunManager *runmgr)
    : geant::UserDetectorConstruction(runmgr), fGDMLFileName("cms2018.gdml")
{
}

CMSDetectorConstruction::~CMSDetectorConstruction() {}

void CMSDetectorConstruction::CreateGeometry()
{
  LoadGeometry(fGDMLFileName.c_str());
}
