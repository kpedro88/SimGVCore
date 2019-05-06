#ifndef SimGVCore_Calo_VolumeWrapper
#define SimGVCore_Calo_VolumeWrapper

#include <map>
#include <vector>
#include <string>

namespace sim {
	template <class T>
	class VolumeWrapper {
		public:
			VolumeWrapper(const T* tmp) = delete;
			static std::map<const std::string, const T*> getVolumes() = delete;
			double dz() const = delete;
	};
}

#endif
