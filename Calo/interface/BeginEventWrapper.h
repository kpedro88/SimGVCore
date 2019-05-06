#ifndef SimGVCore_Calo_BeginEventWrapper
#define SimGVCore_Calo_BeginEventWrapper

namespace sim {
	template <class T>
	class BeginEventWrapper {
		public:
			BeginEventWrapper(const T tmp) = delete;
			int getEventID() const = delete;
	};
}

#endif
