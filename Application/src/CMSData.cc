#include "SimGVCore/Application/interface/CMSData.h"
#include "SimGVCore/CaloGV/interface/GVTraits.h"

#include "Geant/Event.h"
#include "Geant/Track.h"

//------------------------------------------------------------------------------------------
//implementation of per-event data

CMSDataPerEvent::CMSDataPerEvent() : fSD(nullptr), fInitialized(false) { }

CMSDataPerEvent::CMSDataPerEvent(const edm::ParameterSet& p) : fSD(std::make_unique<CaloSteppingAction>(p)), fInitialized(false) { }

//SD class needs custom copy via Clone() function (contains unique_ptrs)
CMSDataPerEvent::CMSDataPerEvent(const CMSDataPerEvent& orig) : fSD(orig.fSD ? std::make_unique<CaloSteppingAction>(orig.fSD->GetParams()) : nullptr), fInitialized(false) { }

//pass calls through to SD class

void CMSDataPerEvent::BeginRun() {
	if(fInitialized) return;
	GVRun dummyRun;
	fSD->update(&dummyRun);
	fInitialized = true;
}

void CMSDataPerEvent::BeginEvent(geant::Event* event) {
	fSD->update((GVBeginEvent*)event);
}

void CMSDataPerEvent::SteppingActions(geant::Track& track) {
	fSD->update(&track);
}

void CMSDataPerEvent::FinishEvent(geant::Event* event) {
	fSD->update((GVEndEvent*)event);
}

void CMSDataPerEvent::FinishRun() { }

bool CMSDataPerEvent::Merge(const CMSDataPerEvent& other) {
	return fSD->Merge(*other.fSD);
}

void CMSDataPerEvent::produce(edm::Event& e, const edm::EventSetup& s) {
	fSD->produce(e, s);
}

//------------------------------------------------------------------------------------------
//implementation of per-thread data

CMSDataPerThread::CMSDataPerThread(int nevtbuffered, const edm::ParameterSet& p) :
	fNumBufferedEvents(nevtbuffered),
	fDataPerEvent(fNumBufferedEvents,CMSDataPerEvent(p))
{ }

void CMSDataPerThread::BeginRun() {
	for(auto& dataPerEvent : fDataPerEvent){
		dataPerEvent.BeginRun();
	}
}

void CMSDataPerThread::BeginEvent(geant::Event* event) {
	fDataPerEvent[event->GetSlot()].BeginEvent(event);
}

void CMSDataPerThread::SteppingActions(geant::Track& track) {
	fDataPerEvent[track.EventSlot()].SteppingActions(track);
}

void CMSDataPerThread::FinishEvent(geant::Event* event) {
	fDataPerEvent[event->GetSlot()].FinishEvent(event);
}

void CMSDataPerThread::FinishRun() {
	for(auto& dataPerEvent : fDataPerEvent){
		dataPerEvent.FinishRun();
	}
}

bool CMSDataPerThread::Merge(int index, const CMSDataPerThread& other){
	return fDataPerEvent[index].Merge(other.fDataPerEvent[index]);
}

CMSDataPerEvent* CMSDataPerThread::GetEventData(int slot) {
	return &fDataPerEvent[slot];
}

