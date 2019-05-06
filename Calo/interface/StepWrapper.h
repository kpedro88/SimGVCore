#ifndef SimGVCore_Calo_StepWrapper
#define SimGVCore_Calo_StepWrapper

#include <map>
#include <vector>
#include <string>
#include <utility>

namespace sim {
	template <class T, class V>
	class StepWrapper {
		public:
			StepWrapper(const T tmp) = delete;
			const V* getVolume() const = delete;
			double getEnergyDeposit() const = delete;
			double getTime() const = delete;
			int getTrackID() const = delete;
			bool getEM() const = delete;
			double getStepLength() const = delete;
			double getCharge() const = delete;
			double getDensity() const = delete;
			int getSize() const = delete;
			double getDz() const = delete;
			double getRadlen() const = delete;
			int getCopyNo(int level) const = delete;
			math::XYZVectorD getPosition(bool) const = delete;
			math::XYZVectorD getMomentum() const = delete;
			std::pair<std::string,int> getNameNumber(int level) const = delete;
			std::string getVolumeName() const = delete;
	};
}

#endif
