#ifndef SimGVCore_Calo_CaloMapT_H
#define SimGVCore_Calo_CaloMapT_H

#include <vector>
#include <map>
#include <string>

template <class Traits>
class CaloMapT {
	public:
		typedef typename Traits::Volume Volume;
		typedef typename Traits::VolumeWrapper VolumeWrapper;

		//constructor
		CaloMapT(const std::vector<std::string>& nameEBSD, const std::vector<std::string>& nameEESD, const std::vector<std::string>& nameHCSD) {
			const auto& nameMap = VolumeWrapper::getVolumes();
			for (auto const& name : nameEBSD) {
				for (const auto& itr : nameMap) {
					const std::string &lvname = itr.first;
					if (lvname.find(name) != std::string::npos) {
						volEBSD_.emplace_back(itr.second);
						int type =	(lvname.find("refl") == std::string::npos) ? -1 : 1;
						double dz = VolumeWrapper(itr.second).dz();
						xtalMap_.emplace(itr.second,dz*type);
					}
				}
			}
			for (auto const& name : nameEESD) {
				for (const auto& itr : nameMap) {
					const std::string &lvname = itr.first;
					if (lvname.find(name) != std::string::npos)	{
						volEESD_.emplace_back(itr.second);
						int type =	(lvname.find("refl") == std::string::npos) ? 1 : -1;
						double dz = VolumeWrapper(itr.second).dz();
						xtalMap_.emplace(itr.second,dz*type);
					}
				}
			}
			for (auto const& name : nameHCSD) {
				for (const auto& itr : nameMap) {
					const std::string &lvname = itr.first;
					if (lvname.find(name) != std::string::npos) 
						volHCSD_.emplace_back(itr.second);
				}
			}
		}

		//const accessors
		const std::vector<const Volume*>& volEBSD() const { return volEBSD_; }
		const std::vector<const Volume*>& volEESD() const { return volEESD_; }
		const std::vector<const Volume*>& volHCSD() const { return volHCSD_; }
		const std::map<const Volume*,double>& xtalMap() const { return xtalMap_; }

	private:
		//members
		std::vector<const Volume*> volEBSD_, volEESD_, volHCSD_;
		std::map<const Volume*,double> xtalMap_;
};

#endif
