#ifndef SimGVCore_CaloGV_StepWrapper
#define SimGVCore_CaloGV_StepWrapper

#include "SimGVCore/Calo/interface/StepWrapper.h"

//includes for GV
#include "Geant/Track.h"
#include "Geant/Particle.h"
#include "Geant/MaterialProperties.h"
#include "Geant/Typedefs.h"
#include "Geant/PhysicalConstants.h"
#include "Geant/SystemOfUnits.h"
#include "base/Transformation3D.h"

namespace sim {
	template <>
	class StepWrapper<geant::Track, vecgeom::LogicalVolume> {
		public:
			StepWrapper(const geant::Track* tmp) : track_(tmp) {}
			const vecgeom::LogicalVolume* getVolume() const { return track_->GetVolume(); }
			double getEnergyDeposit() const { return track_->Edep()/geant::units::MeV; }
			double addEnergy() const { return ((track_->Status() == geant::kExitingSetup) ? track_->Ekin()/geant::units::MeV : 0); }
			int getPDGId() const {
				const geantphysics::Particle *part = geantphysics::Particle::GetParticleByInternalCode(track_->GVcode());
				return part->GetPDGCode();
			}
			double getTime() const { return track_->GlobalTime()/geant::units::nanosecond; }
			int getTrackID() const { return track_->Particle(); }
			bool getEM() const { 
				const geantphysics::Particle *part = geantphysics::Particle::GetParticleByInternalCode(track_->GVcode());
				int pdgCode = part->GetPDGCode();
				return (pdgCode == 11 || pdgCode == -11 || pdgCode == 22); 
			}
			double getStepLength() const { return track_->GetStep()/geant::units::cm; }
			double getCharge() const { return track_->Charge()/geant::units::eplus; }
			double getDensity() const { return track_->GetMaterial()->GetDensity()/(geant::units::g/geant::units::cm3); }
			int getSize() const { return (2+track_->Path()->GetLevel()); }
			double getDz() const {
				auto const touch = track_->Path();
				using Vector3D = vecgeom::Vector3D<double>;
				Vector3D local;
				vecgeom::cxx::Transformation3D m;
				touch->TopMatrix(m);
				m.Transform(Vector3D(track_->X(),track_->Y(),track_->Z()),local);
				return local.z()/geant::units::mm;
			}
			double getRadlen() const { return track_->GetMaterial()->GetMaterialProperties()->GetRadiationLength()/geant::units::mm; }
			int getCopyNo(int level) const {
				auto const touch = track_->Path(); // returns vecgeom::NavigationState*
				int theSize = touch->GetLevel();
				return touch->At(theSize-level)->GetCopyNo();
			}
			uint32_t getCopy() const {
				auto const touch = track_->Path(); // returns vecgeom::NavigationState*
				int theSize = touch->GetLevel();
				return ((theSize<1) ? static_cast<uint32_t>(touch->At(theSize)->GetCopyNo()) : static_cast<uint32_t>(touch->At(theSize)->GetCopyNo()+1000*touch->At(theSize-1)->GetCopyNo()));
			}
			std::pair<int,int> getCopyNos() const {
				auto const touch = track_->Path(); // returns vecgeom::NavigationState*
				int theSize = touch->GetLevel();
				return std::make_pair(touch->At(theSize)->GetCopyNo(),touch->At(theSize-1)->GetCopyNo());
			}
			math::XYZVectorD getPosition(bool) const {
				//GeantV currently does not provide prestep info
				double xp = track_->X()/geant::units::mm;
				double yp = track_->Y()/geant::units::mm;
				double zp = track_->Z()/geant::units::mm;
				return math::XYZVectorD(xp,yp,zp);
			}
			math::XYZVectorD getMomentum() const {
				double px = track_->Px()/geant::units::MeV;
				double py = track_->Py()/geant::units::MeV;
				double pz = track_->Pz()/geant::units::MeV;
				return math::XYZVectorD(px,py,pz);
			}
			std::pair<std::string,int> getNameNumber(int level) const {
				auto const touch = track_->Path(); // returns vecgeom::NavigationState*
				int theSize = touch->GetLevel();
				auto const placed = touch->At(theSize-level);
				return std::make_pair(placed->GetName(), placed->GetCopyNo());
			}
			void setNameNumber(EcalBaseNumber & baseNumber) const {
				int size = getSize();
				if (baseNumber.getCapacity() < size ) baseNumber.setSize(size);
				auto const touch = track_->Path();
				int theSize = touch->GetLevel();
				for (int ii = 0; ii < size-1 ; ii++) {
					auto const placed = touch->At(theSize-ii);
					baseNumber.addLevel(placed->GetName(), placed->GetCopyNo());
				}
				baseNumber.addLevel("World", 0);
			}
			std::string getVolumeName() const {
				return track_->GetVolume()->GetLabel();
			}
			
		private:
			const geant::Track* track_;
	};
}

#endif

