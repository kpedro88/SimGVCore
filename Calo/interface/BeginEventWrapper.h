#ifndef SimGVCore_Calo_BeginEventWrapper
#define SimGVCore_Calo_BeginEventWrapper

#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "Geant/Event.h"

struct GVBeginEvent : public geant::Event { };

namespace sim {
	template <class T>
	class BeginEventWrapper {
		public:
			BeginEventWrapper(const T tmp) = delete;
			int getEventID() const = delete;
	};
	
	template <>
	class BeginEventWrapper<BeginOfEvent> {
		public:
			BeginEventWrapper(const BeginOfEvent* tmp) : event_(tmp) {}
			int getEventID() const { return (*event_)()->GetEventID(); }
			
		private:
			const BeginOfEvent* event_;
	};
	
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
