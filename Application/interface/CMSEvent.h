#ifndef SimGVCore_Application_CMSEvent_h
#define SimGVCore_Application_CMSEvent_h

#include "FWCore/Concurrency/interface/WaitingTaskWithArenaHolder.h"

#include "Geant/Event.h"

//stream cache to keep track of GeantV event scoring output
struct EventCache {
	void clear() {
		sd_ = nullptr;
	}

	std::unique_ptr<CaloSteppingAction> sd_ = nullptr;
};

// an event with a callback
class CMSEvent : public geant::Event {
	public:
		CMSEvent(edm::WaitingTaskWithArenaHolder holder, EventCache* cache) : holder_(std::move(holder)), cache_(cache) {
			//clear cache values
			cache_->clear();
		}

		//set cache values
		void Store(const CaloSteppingAction& sd){
			cache_->sd_ = std::make_unique<CaloSteppingAction>(sd);
		}

		void FinalActions() override {
			edm::LogInfo("GeantVProducer") << "Callback for event " << this;
			std::exception_ptr exceptionPtr;
			holder_.doneWaiting(exceptionPtr);
		}

	private:
		edm::WaitingTaskWithArenaHolder holder_;
		EventCache* cache_;
};

#endif
