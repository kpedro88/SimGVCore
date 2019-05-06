#ifndef SimGVCore_CaloGV_StepWrapper
#define SimGVCore_CaloGV_StepWrapper

#include "SimGVCore/Calo/interface/StepWrapper.h"

//includes for GV
#include "Geant/Track.h"
#include "Geant/Particle.h"
#include "Geant/MaterialProperties.h"
#include "Geant/Typedefs.h"
#include "Geant/SystemOfUnits.h"
#include "base/Transformation3D.h"

namespace sim {
	template <>
	class StepWrapper<geant::Track, vecgeom::LogicalVolume> {
		public:
			StepWrapper(const geant::Track* tmp) : track_(tmp) {}
			const vecgeom::LogicalVolume* getVolume() const { return track_->GetVolume(); }
			double getEnergyDeposit() const { return track_->Edep()/geant::units::MeV; }
			double getTime() const { return track_->Time()/geant::units::nanosecond; }
			int getTrackID() const { return track_->Particle(); }
			bool getEM() const { 
				const geantphysics::Particle *part = geantphysics::Particle::GetParticleByInternalCode(track_->GVcode());
				int pdgCode = part->GetPDGCode();
				return (pdgCode == 11 || pdgCode == -11 || pdgCode == 21); 
			}
			double getStepLength() const { return track_->GetStep()/geant::units::mm; }
			double getCharge() const { return track_->Charge()/geant::units::eplus; }
			double getDensity() const { return track_->GetMaterial()->GetDensity(); }
			int getSize() const { return (1+track_->Path()->GetLevel()); }
			double getDz() const {
				auto const touch = track_->Path();
				using Vector3D = vecgeom::Vector3D<double>;
				Vector3D local;
				vecgeom::cxx::Transformation3D m;
				touch->TopMatrix(m);
				m.Transform(Vector3D(track_->X(),track_->Y(),track_->Z()),local);
				return local.z();
			}
			double getRadlen() const { return track_->GetMaterial()->GetMaterialProperties()->GetRadiationLength(); }
			int getCopyNo(int level) const {
				auto const touch = track_->Path(); // returns vecgeom::NavigationState*
				int theSize = touch->GetLevel();
				return touch->ToPlacedVolume(theSize-level)->GetCopyNo();
			}
			math::XYZVectorD getPosition(bool) const {
				//GeantV currently does not provide prestep info
				double xp = track_->X()/geant::units::mm;
				double yp = track_->Y()/geant::units::mm;
				double zp = track_->Z()/geant::units::mm;
				return math::XYZVectorD(xp,yp,zp);
			}
			math::XYZVectorD getMomentum() const {
				double px = track_->Px()/mm;
				double py = track_->Py()/mm;
				double pz = track_->Pz()/mm;
				return math::XYZVectorD(px,py,pz);
			}
			std::pair<std::string,int> getNameNumber(int level) const {
				auto const touch = track_->Path(); // returns vecgeom::NavigationState*
				int theSize = touch->GetLevel();
				return std::make_pair(touch->ToPlacedVolume(theSize-level)->GetName(), touch->ToPlacedVolume(theSize-level)->GetCopyNo());
			}
			std::string getVolumeName() const {
				return track_->GetVolume()->GetLabel();
			}
	
		private:
			const geant::Track* track_;
	};
}

#endif

