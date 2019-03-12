#ifndef SimGVCore_Calo_VolumeWrapper
#define SimGVCore_Calo_VolumeWrapper

#include <map>
#include <vector>
#include <string>

//includes for G4
#include "G4LogicalVolume.hh"

//includes for GV
#include "volumes/LogicalVolume.h"

namespace sim {
	template <class T>
	class VolumeWrapper {
		public:
			VolumeWrapper(const T tmp) = delete;
			static std::map<std::string, const T> getVolumes() = delete;
			double dz() const = delete;
	};
	
	template <>
	class VolumeWrapper<G4LogicalVolume*> {
		public:
			VolumeWrapper(const G4LogicalVolume* tmp) : wrapped(tmp) {}
			static std::map<std::string, const G4LogicalVolume*> getVolumes(){
				std::map<const std::string, const G4LogicalVolume*> nameMap;
				const G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
				if(lvs){
					for(auto itr = lvs->begin(); itr != lvs->end(); ++itr){
						nameMap.emplace((*lvi)->GetName(), *lvi);
					}
				}
				return nameMap;
			}
			double dz() const {
				G4Trap* solid = static_cast<G4Trap*>(wrapped->GetSolid());
				return 2*solid->GetZHalfLength();
			}
			
		private:
			const G4LogicalVolume* wrapped;
	};
	
	template <>
	class VolumeWrapper<vecgeom::LogicalVolume> {
		public:
			VolumeWrapper(const vecgeom::LogicalVolume tmp) : wrapped(tmp) {}
			static std::map<std::string, const vecgeom::LogicalVolume> getVolumes(){
				std::map<const std::string, const vecgeom::LogicalVolume> nameMap;
				std::vector<vecgeom::LogicalVolume> theLogicalVolumes;
				vecgeom::GeoManager::Instance().GetAllLogicalVolumes(theLogicalVolumes);
				for (const auto& volume : theLogicalVolumes)
					nameMap.emplace(volume->GetLabel(), volume);
				return nameMap;
			}
			double dz() const {
				vecgeom::UnplacedTrapezoid *solid = static_cast<vecgeom::UnplacedTrapezoid*>(wrapped.GetUnplacedVolume());
				return solid->GetDz()/mm; // should one multiply by 2 here?
			}
			
		private:
			const vecgeom::LogicalVolume& wrapped;
	};
}

#endif