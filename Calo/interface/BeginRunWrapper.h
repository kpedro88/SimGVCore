#ifndef SimGVCore_Calo_BeginRunWrapper
#define SimGVCore_Calo_BeginRunWrapper

#include "SimG4Core/Notification/interface/BeginOfRun.h"

//GeantV does not provide a Run object
struct GVRun { };

namespace sim {
	template <class T>
	class BeginRunWrapper {
		public:
			BeginRunWrapper(const T tmp) = delete;
			int getRunID() const = delete;
	};
	
	template <>
	class BeginRunWrapper<BeginOfRun> {
		public:
			BeginRunWrapper(const BeginOfRun* tmp) : run_(tmp) {}
			int getRunID() const { return (*run_)()->GetRunID(); }
			
		private:
			const BeginOfRun* run_;
	};
	
	template <>
	class BeginRunWrapper<GVRun> {
		public:
			BeginRunWrapper(const GVRun* tmp) : run_(tmp) {}
			int getRunID() const { return 1; }
			
		private:
			const GVRun* run_;
	};
}

#endif
