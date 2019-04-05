#ifndef SimGVCore_Calo_BeginEventWrapper
#define SimGVCore_Calo_BeginEventWrapper

#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "Geant/Event.h"

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
	class BeginEventWrapper<geant::Event> {
		public:
			BeginEventWrapper(const geant::Event* tmp) : event_(tmp) {}
			int getEventID() const { return event_->GetEvent(); }
			
		private:
			const geant::Event* event_;
	};
}

#endif
