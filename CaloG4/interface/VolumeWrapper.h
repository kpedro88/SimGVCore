#ifndef SimGVCore_CaloG4_VolumeWrapper
#define SimGVCore_CaloG4_VolumeWrapper

#include "SimGVCore/Calo/interface/VolumeWrapper.h"

//includes for G4
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Trap.hh"

namespace sim {
	template <>
	class VolumeWrapper<G4LogicalVolume> {
		public:
			VolumeWrapper(const G4LogicalVolume* tmp) : wrapped_(tmp) {}
			static std::map<const std::string, const G4LogicalVolume*> getVolumes(){
				std::map<const std::string, const G4LogicalVolume*> nameMap;
				const G4LogicalVolumeStore* lvs = G4LogicalVolumeStore::GetInstance();
				if(lvs){
					for(auto itr = lvs->begin(); itr != lvs->end(); ++itr){
						nameMap.emplace((*itr)->GetName(), *itr);
					}
				}
				return nameMap;
			}
			double dz() const {
				G4Trap* solid = static_cast<G4Trap*>(wrapped_->GetSolid());
				return 2*solid->GetZHalfLength();
			}
			
		private:
			const G4LogicalVolume* wrapped_;
	};
}

#endif
