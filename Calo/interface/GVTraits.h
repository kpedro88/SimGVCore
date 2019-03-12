#ifndef SimGVCore_Calo_GVTraits
#define SimGVCore_Calo_GVTraits

#include "SimGVCore/Calo/interface/BeginJobWrapper.h"
#include "SimGVCore/Calo/interface/BeginRunWrapper.h"
#include "SimGVCore/Calo/interface/BeginEventWrapper.h"
#include "SimGVCore/Calo/interface/EndEventWrapper.h"
#include "SimGVCore/Calo/interface/StepWrapper.h"
#include "SimGVCore/Calo/interface/VolumeWrapper.h"

#include "SimG4Core/Notification/interface/BeginOfJob.h"
#include "SimG4Core/Notification/interface/BeginOfRun.h"
#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "SimG4Core/Notification/interface/EndOfEvent.h"
#include "volumes/LogicalVolume.h"
#include "Geant/Track.h"

struct G4Traits {
	typedef BeginOfJob BeginJob;
	typedef BeginOfRun BeginRun;
	typedef BeginOfEvent BeginEvent;
	typedef EndOfEvent EndEvent;
	typedef geant::Track Step;
	typedef vecgeom::LogicalVolume Volume;
	
	typedef sim::BeginJobWrapper<BeginJob> BeginJobWrapper;
	typedef sim::BeginRunWrapper<BeginRun> BeginRunWrapper;
	typedef sim::BeginEventWrapper<BeginEvent> BeginEventWrapper;
	typedef sim::EndEventWrapper<EndEvent> EndEventWrapper;
	typedef sim::StepWrapper<Step,Volume> StepWrapper;
	typedef sim::VolumeWrapper<Volume> VolumeWrapper;
};

#endif