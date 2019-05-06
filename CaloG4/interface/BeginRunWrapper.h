#ifndef SimGVCore_CaloG4_BeginRunWrapper
#define SimGVCore_CaloG4_BeginRunWrapper

#include "SimGVCore/Calo/interface/BeginRunWrapper.h"

#include "SimG4Core/Notification/interface/BeginOfRun.h"

namespace sim {
	template <>
	class BeginRunWrapper<BeginOfRun> {
		public:
			BeginRunWrapper(const BeginOfRun* tmp) : run_(tmp) {}
			int getRunID() const { return (*run_)()->GetRunID(); }
			
		private:
			const BeginOfRun* run_;
	};
}

#endif
