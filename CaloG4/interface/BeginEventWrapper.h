#ifndef SimGVCore_CaloG4_BeginEventWrapper
#define SimGVCore_CaloG4_BeginEventWrapper

#include "SimGVCore/Calo/interface/BeginEventWrapper.h"

#include "SimG4Core/Notification/interface/BeginOfEvent.h"

namespace sim {
	template <>
	class BeginEventWrapper<BeginOfEvent> {
		public:
			BeginEventWrapper(const BeginOfEvent* tmp) : event_(tmp) {}
			int getEventID() const { return (*event_)()->GetEventID(); }
			
		private:
			const BeginOfEvent* event_;
	};
}

#endif
