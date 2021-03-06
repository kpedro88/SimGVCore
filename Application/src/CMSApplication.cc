#include "SimGVCore/Application/interface/CMSApplication.h"
#include "SimGVCore/Application/interface/CMSEvent.h"

#include "Geant/RunManager.h"
#include "Geant/TaskData.h"
#include "Geant/Track.h"
#include "Geant/Event.h"

CMSApplication::CMSApplication(geant::RunManager *runmgr, const edm::ParameterSet& p) :
	geant::UserApplication(runmgr), fParams(p), fInitialized(false), fNumBufferedEvents(-1), fDataHandler(nullptr)
{ }

CMSApplication::~CMSApplication() { }

bool CMSApplication::Initialize() {
	if(fInitialized) return true;
	fNumBufferedEvents = fRunMgr->GetConfig()->fNbuff;
	
	//register thread-local data
	fDataHandler = fRunMgr->GetTDManager()->RegisterUserData<CMSDataPerThread>("CMSDataPerThread");

	fInitialized = true;
	return true;
}

void CMSApplication::AttachUserData(geant::TaskData *td) {
	CMSDataPerThread *eventData = new CMSDataPerThread(fNumBufferedEvents,fParams);
	fDataHandler->AttachUserData(eventData, td);
}

void CMSApplication::BeginRun() {
	//loop over tasks
	auto tdManager = fRunMgr->GetTDManager();
	for(size_t i = 0; i < tdManager->GetNtaskData(); ++i){
		auto td = tdManager->GetTaskData(i);
		auto data = fDataHandler->GetUserData(td);
		data->BeginRun();
	}
}

void CMSApplication::FinishRun() {
	//loop over tasks
	auto tdManager = fRunMgr->GetTDManager();
	for(size_t i = 0; i < tdManager->GetNtaskData(); ++i){
		auto td = tdManager->GetTaskData(i);
		auto data = fDataHandler->GetUserData(td);
		data->FinishRun();
	}
}

void CMSApplication::BeginEvent(geant::Event* event) {
	//loop over tasks
	auto tdManager = fRunMgr->GetTDManager();
	for(size_t i = 0; i < tdManager->GetNtaskData(); ++i){
		auto td = tdManager->GetTaskData(i);
		auto data = fDataHandler->GetUserData(td);
		data->BeginEvent(event);
	}
}

void CMSApplication::FinishEvent(geant::Event* event) {
	//loop over tasks
	auto tdManager = fRunMgr->GetTDManager();
	for(size_t i = 0; i < tdManager->GetNtaskData(); ++i){
		auto td = tdManager->GetTaskData(i);
		auto data = fDataHandler->GetUserData(td);
		data->FinishEvent(event);
	}
	//merge and store thread-local data for this event
	auto base_data = fRunMgr->GetTDManager()->MergeUserData(event->GetSlot(), *fDataHandler);
	static_cast<CMSEvent*>(event)->Store(*(base_data->GetEventData(event->GetSlot())));
	//now clear out unneeded data
	base_data->Clear(event->GetSlot());
}

void CMSApplication::SteppingActions(geant::Track &track, geant::TaskData *td) {
	auto data = fDataHandler->GetUserData(td);
	data->SteppingActions(track);
}
