#ifndef SimGVCore_Calo_G4Traits
#define SimGVCore_Calo_G4Traits

#include "SimGVCore/Calo/interface/BeginRunWrapper.h"
#include "SimGVCore/Calo/interface/BeginEventWrapper.h"
#include "SimGVCore/Calo/interface/EndEventWrapper.h"
#include "SimGVCore/Calo/interface/StepWrapper.h"
#include "SimGVCore/Calo/interface/VolumeWrapper.h"

#include "SimG4Core/Notification/interface/BeginOfRun.h"
#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "SimG4Core/Notification/interface/EndOfEvent.h"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"

struct G4Traits {
	typedef BeginOfRun BeginRun;
	typedef BeginOfEvent BeginEvent;
	typedef EndOfEvent EndEvent;
	typedef G4Step Step;
	typedef G4LogicalVolume* Volume;
	
	typedef sim::BeginRunWrapper<BeginRun> BeginRunWrapper;
	typedef sim::BeginEventWrapper<BeginEvent> BeginEventWrapper;
	typedef sim::EndEventWrapper<EndEvent> EndEventWrapper;
	typedef sim::StepWrapper<Step,Volume> StepWrapper;
	typedef sim::VolumeWrapper<Volume> VolumeWrapper;
};

#endif
