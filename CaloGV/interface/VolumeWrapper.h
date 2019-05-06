#ifndef SimGVCore_CaloGV_VolumeWrapper
#define SimGVCore_CaloGV_VolumeWrapper

#include "SimGVCore/Calo/interface/VolumeWrapper.h"

//includes for GV
#include "volumes/LogicalVolume.h"
#include "volumes/UnplacedTrapezoid.h"

namespace sim {
	template <>
	class VolumeWrapper<vecgeom::LogicalVolume> {
		public:
			VolumeWrapper(const vecgeom::LogicalVolume* tmp) : wrapped_(tmp) {}
			static std::map<const std::string, const vecgeom::LogicalVolume*> getVolumes(){
				std::map<const std::string, const vecgeom::LogicalVolume*> nameMap;
				std::vector<const vecgeom::LogicalVolume*> theLogicalVolumes;
				vecgeom::GeoManager::Instance().GetAllLogicalVolumes(theLogicalVolumes);
				for (const auto volume : theLogicalVolumes)
					nameMap.emplace(volume->GetLabel(), volume);
				return nameMap;
			}
			double dz() const {
				auto *solid = static_cast<const vecgeom::UnplacedTrapezoid*>(wrapped_->GetUnplacedVolume());
				return solid->GetDz()/mm; // should one multiply by 2 here?
			}
			
		private:
			const vecgeom::LogicalVolume* wrapped_;
	};
}

#endif
