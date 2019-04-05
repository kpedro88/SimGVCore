#ifndef SimGVCore_Calo_GVTraits
#define SimGVCore_Calo_GVTraits

#include "SimGVCore/Calo/interface/BeginRunWrapper.h"
#include "SimGVCore/Calo/interface/BeginEventWrapper.h"
#include "SimGVCore/Calo/interface/EndEventWrapper.h"
#include "SimGVCore/Calo/interface/StepWrapper.h"
#include "SimGVCore/Calo/interface/VolumeWrapper.h"

#include "volumes/LogicalVolume.h"
#include "Geant/Track.h"
#include "Geant/Event.h"

struct GVTraits {
	typedef GVRun BeginRun;
	typedef geant::Event BeginEvent;
	typedef geant::Event EndEvent;
	typedef geant::Track Step;
	typedef vecgeom::LogicalVolume Volume;
	
	typedef sim::BeginRunWrapper<BeginRun> BeginRunWrapper;
	typedef sim::BeginEventWrapper<BeginEvent> BeginEventWrapper;
	typedef sim::EndEventWrapper<EndEvent> EndEventWrapper;
	typedef sim::StepWrapper<Step,Volume> StepWrapper;
	typedef sim::VolumeWrapper<Volume> VolumeWrapper;
};

#endif
