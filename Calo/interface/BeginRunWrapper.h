#ifndef SimGVCore_Calo_BeginRunWrapper
#define SimGVCore_Calo_BeginRunWrapper


namespace sim {
	template <class T>
	class BeginRunWrapper {
		public:
			BeginRunWrapper(const T tmp) = delete;
			int getRunID() const = delete;
	};

#ifdef USEGEANT4
#include "SimG4Core/Notification/interface/BeginOfRun.h"

	template <>
	class BeginRunWrapper<BeginOfRun> {
		public:
			BeginRunWrapper(const BeginOfRun* tmp) : run_(tmp) {}
			int getRunID() const { return (*run_)()->GetRunID(); }
			
		private:
			const BeginOfRun* run_;
	};

#endif

#ifdef USEGEANTV
	//GeantV does not provide a Run object
	struct GVRun { };

	template <>
	class BeginRunWrapper<GVRun> {
		public:
			BeginRunWrapper(const GVRun* tmp) : run_(tmp) {}
			int getRunID() const { return 1; }
			
		private:
			const GVRun* run_;
	};

#endif

}

#endif
