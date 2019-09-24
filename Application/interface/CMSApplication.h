#ifndef SimGVCore_Application_CMSApplication_h
#define SimGVCore_Application_CMSApplication_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geant/UserApplication.h"
#include "Geant/TaskData.h"

#include "CMSData.h"

#include <vector>

class CMSApplication : public geant::UserApplication {
	public:
		CMSApplication(geant::RunManager *runmgr, const edm::ParameterSet& p);
		virtual ~CMSApplication();

		//helpers
		bool Initialize() override;
		void AttachUserData(geant::TaskData *td) override;

		//methods for GeantV data access
		void BeginRun() override;
		void BeginEvent(geant::Event *event) override;
		void SteppingActions(geant::Track &track, geant::TaskData *td) override;
		void FinishEvent(geant::Event *event) override;
		void FinishRun() override;

		//methods for CMSSW data access
		CMSDataPerEvent& GetEventData(int slot);

	private:
		//member variables
		edm::ParameterSet fParams;
		bool fInitialized;
		int fNumBufferedEvents;
		geant::TaskDataHandle<CMSDataPerThread> *fDataHandler;
		std::vector<CMSDataPerEvent> fEventData;
};

#endif
