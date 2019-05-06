#ifndef SimGVCore_Calo_EndEventWrapper
#define SimGVCore_Calo_EndEventWrapper

namespace sim {
	template <class T>
	class EndEventWrapper {
		public:
			EndEventWrapper(const T tmp) = delete;
			int getEventID() const = delete;
	};
}

#endif
