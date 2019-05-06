#ifndef SimGVCore_CaloGV_BeginRunWrapper
#define SimGVCore_CaloGV_BeginRunWrapper

#include "SimGVCore/Calo/interface/BeginRunWrapper.h"

//GeantV does not provide a Run object
struct GVRun { };

namespace sim {
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
