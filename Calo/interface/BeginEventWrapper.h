#ifndef SimGVCore_Calo_BeginEventWrapper
#define SimGVCore_Calo_BeginEventWrapper

namespace sim {
	template <class T>
	class BeginEventWrapper {
		public:
			BeginEventWrapper(const T tmp) = delete;
			int getEventID() const = delete;
	};

#ifdef USEGEANT4
#include "SimG4Core/Notification/interface/BeginOfEvent.h"

	template <>
	class BeginEventWrapper<BeginOfEvent> {
		public:
			BeginEventWrapper(const BeginOfEvent* tmp) : event_(tmp) {}
			int getEventID() const { return (*event_)()->GetEventID(); }
			
		private:
			const BeginOfEvent* event_;
	};

#endif

#ifdef USEGEANTV
#include "Geant/Event.h"

	struct GVBeginEvent : public geant::Event { };

	template <>
	class BeginEventWrapper<GVBeginEvent> {
		public:
			BeginEventWrapper(const GVBeginEvent* tmp) : event_(tmp) {}
			int getEventID() const { return event_->GetEvent(); }
			
		private:
			const GVBeginEvent* event_;
	};

#endif

}

#endif
