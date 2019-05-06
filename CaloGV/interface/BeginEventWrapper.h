#ifndef SimGVCore_CaloGV_BeginEventWrapper
#define SimGVCore_CaloGV_BeginEventWrapper

#include "SimGVCore/Calo/interface/BeginEventWrapper.h"

#include "Geant/Event.h"

struct GVBeginEvent : public geant::Event { };

namespace sim {
	template <>
	class BeginEventWrapper<GVBeginEvent> {
		public:
			BeginEventWrapper(const GVBeginEvent* tmp) : event_(tmp) {}
			int getEventID() const { return event_->GetEvent(); }
			
		private:
			const GVBeginEvent* event_;
	};
}

#endif
