#ifndef SimGVCore_Application_CMSData_h
#define SimGVCore_Application_CMSData_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimGVCore/CaloGV/interface/CaloSteppingAction.h"

#include <vector>

//defines common interface for data classes
//similar to methods in geant::UserApplication, but not quite the same
class GVScoring {
	public:
		//mirrored application interface
		virtual void BeginRun() = 0;
		virtual void BeginEvent(geant::Event *event) = 0;
		virtual void SteppingActions(geant::Track &track) = 0;
		virtual void FinishEvent(geant::Event *event) = 0;
		virtual void FinishRun() = 0;
};

class CMSDataPerEvent : public GVScoring {
	public:
		CMSDataPerEvent();
		CMSDataPerEvent(const edm::ParameterSet& p);
		//specialized copy constructor
		CMSDataPerEvent(const CMSDataPerEvent& orig);
		~CMSDataPerEvent() {}
		CMSDataPerEvent& operator=(const CMSDataPerEvent& other);

		//common scoring interface
		void BeginRun() override;
		void BeginEvent(geant::Event *event) override;
		void SteppingActions(geant::Track &track) override;
		void FinishEvent(geant::Event *event) override;
		void FinishRun() override;

		//for final output
		bool Merge(const CMSDataPerEvent& other);
		CaloSteppingAction* GetData() { return fSD.get(); }
		void clear() { fSD->clear(); }

	private:
		//member variables
		std::unique_ptr<CaloSteppingAction> fSD;
		bool fInitialized;
};

class CMSDataPerThread : public GVScoring {
	public:
		CMSDataPerThread(int nevtbuffered, const edm::ParameterSet& p);
		~CMSDataPerThread() {}

		//required interface
		void Clear(int index) { fDataPerEvent[index].clear(); }
		bool Merge(int index, const CMSDataPerThread& other);

		//common scoring interface
		void BeginRun() override;
		void BeginEvent(geant::Event *event) override;
		void SteppingActions(geant::Track &track) override;
		void FinishEvent(geant::Event *event) override;
		void FinishRun() override;

		//for final output
		CaloSteppingAction* GetEventData(int slot) { return fDataPerEvent[slot].GetData(); }

	private:
		//member variables
		int fNumBufferedEvents;
		std::vector<CMSDataPerEvent> fDataPerEvent;
};

#endif
