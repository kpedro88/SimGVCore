#ifndef SimGVCore_Calo_BeginRunWrapper
#define SimGVCore_Calo_BeginRunWrapper

namespace sim {
	template <class T>
	class BeginRunWrapper {
		public:
			BeginRunWrapper(const T tmp) = delete;
			int getRunID() const = delete;
	};
}

#endif
