#ifndef SimGVCore_Calo_EndEventWrapper
#define SimGVCore_Calo_EndEventWrapper

#include "SimG4Core/Notification/interface/EndOfEvent.h"
#include "Geant/Event.h"

namespace sim {
	template <class T>
	class EndEventWrapper {
		public:
			EndEventWrapper(const T tmp) = delete;
			int getEventID() const = delete;
	};
	
	template <>
	class EndEventWrapper<EndOfEvent> {
		public:
			EndEventWrapper(const EndOfEvent* tmp) : event_(tmp) {}
			int getEventID() const { return (*event_)()->GetEventID(); }
			
		private:
			const EndOfEvent* event_;
	};
	
	template <>
	class EndEventWrapper<geant::Event> {
		public:
			EndEventWrapper(const geant::Event* tmp) : event_(tmp) {}
			int getEventID() const { return event_->GetEvent(); }
			
		private:
			const geant::Event* event_;
	};
}

#endif
