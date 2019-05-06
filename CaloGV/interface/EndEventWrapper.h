#ifndef SimGVCore_CaloGV_EndEventWrapper
#define SimGVCore_CaloGV_EndEventWrapper

#include "SimGVCore/Calo/interface/EndEventWrapper.h"

#include "Geant/Event.h"

struct GVEndEvent : public geant::Event { };

namespace sim {
	template <>
	class EndEventWrapper<GVEndEvent> {
		public:
			EndEventWrapper(const GVEndEvent* tmp) : event_(tmp) {}
			int getEventID() const { return event_->GetEvent(); }
			
		private:
			const GVEndEvent* event_;
	};
}

#endif
