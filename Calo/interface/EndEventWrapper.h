#ifndef SimGVCore_Calo_EndEventWrapper
#define SimGVCore_Calo_EndEventWrapper

namespace sim {
	template <class T>
	class EndEventWrapper {
		public:
			EndEventWrapper(const T tmp) = delete;
			int getEventID() const = delete;
	};

#ifdef USEGEANT4	
#include "SimG4Core/Notification/interface/EndOfEvent.h"

	template <>
	class EndEventWrapper<EndOfEvent> {
		public:
			EndEventWrapper(const EndOfEvent* tmp) : event_(tmp) {}
			int getEventID() const { return (*event_)()->GetEventID(); }
			
		private:
			const EndOfEvent* event_;
	};

#endif

#ifdef USEGEANTV
#include "Geant/Event.h"

	struct GVEndEvent : public geant::Event { };

	template <>
	class EndEventWrapper<GVEndEvent> {
		public:
			EndEventWrapper(const GVEndEvent* tmp) : event_(tmp) {}
			int getEventID() const { return event_->GetEvent(); }
			
		private:
			const GVEndEvent* event_;
	};

#endif

}

#endif
