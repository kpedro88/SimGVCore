#ifndef SimGVCore_CaloG4_EndEventWrapper
#define SimGVCore_CaloG4_EndEventWrapper

#include "SimGVCore/Calo/interface/EndEventWrapper.h"

#include "SimG4Core/Notification/interface/EndOfEvent.h"

namespace sim {
	template <>
	class EndEventWrapper<EndOfEvent> {
		public:
			EndEventWrapper(const EndOfEvent* tmp) : event_(tmp) {}
			int getEventID() const { return (*event_)()->GetEventID(); }
			
		private:
			const EndOfEvent* event_;
	};
}

#endif
