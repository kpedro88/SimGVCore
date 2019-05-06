#ifndef SimGVCore_Calo_GVTraits
#define SimGVCore_Calo_GVTraits

#include "SimGVCore/CaloGV/interface/BeginRunWrapper.h"
#include "SimGVCore/CaloGV/interface/BeginEventWrapper.h"
#include "SimGVCore/CaloGV/interface/EndEventWrapper.h"
#include "SimGVCore/CaloGV/interface/StepWrapper.h"
#include "SimGVCore/CaloGV/interface/VolumeWrapper.h"

#include "volumes/LogicalVolume.h"
#include "Geant/Track.h"
#include "Geant/Event.h"

struct GVTraits {
	typedef GVRun BeginRun;
	typedef GVBeginEvent BeginEvent;
	typedef GVEndEvent EndEvent;
	typedef geant::Track Step;
	typedef vecgeom::LogicalVolume Volume;
	
	typedef sim::BeginRunWrapper<BeginRun> BeginRunWrapper;
	typedef sim::BeginEventWrapper<BeginEvent> BeginEventWrapper;
	typedef sim::EndEventWrapper<EndEvent> EndEventWrapper;
	typedef sim::StepWrapper<Step,Volume> StepWrapper;
	typedef sim::VolumeWrapper<Volume> VolumeWrapper;
};

#endif
