#ifndef SimGVCore_Calo_StepWrapper
#define SimGVCore_Calo_StepWrapper

#include <map>
#include <vector>
#include <string>
#include <utility>

//includes for G4
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4ParticleTable.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//includes for GV
#include "Geant/Track.h"
#include "Geant/Particle.h"
#include "Geant/MaterialProperties.h"
#include "Geant/Typedefs.h"
#include "Geant/SystemOfUnits.h"

namespace sim {
	template <class T, class V>
	class StepWrapper {
		public:
			StepWrapper(const T tmp) = delete;
			V getVolume() const = delete;
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

	template <>
	class StepWrapper<G4Step, G4LogicalVolume*> {
		public:
			StepWrapper(const G4Step* tmp) : step_(tmp) {}
			G4LogicalVolume* getVolume() const { return step_->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume(); }
			double getEnergyDeposit() const { return step_->GetTotalEnergyDeposit()/MeV; }
			double getTime() const { return step_->GetTrack()->GetGlobalTime()/nanosecond; }
			int getTrackID() const { return step_->GetTrack()->GetTrackID(); }
			bool getEM() const { return G4TrackToParticleID::isGammaElectronPositron(step_->GetTrack()); }
			double getStepLength() const { return step_->GetStepLength()/mm; }
			double getCharge() const { return step_->GetPreStepPoint()->GetCharge(); }
			double getDensity() const { return step_->GetPreStepPoint()->GetMaterial()->GetDensity(); }
			int getSize() const { return (1+step_->GetPreStepPoint()->GetTouchable()->GetHistoryDepth()); }
			double getDz() const {
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
				auto const& hitPoint= step_->GetPreStepPoint()->GetPosition();
				auto local = touch->GetHistory()->GetTopTransform().TransformPoint(hitPoint);
				return local.z();
			}
			double getRadlen() const { return step_->GetPreStepPoint()->GetMaterial()->GetRadlen(); }
			int getCopyNo(int level) const { return step_->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(level); }
			math::XYZVectorD getPosition(bool trk) const {
				auto pos = trk ? step_->GetTrack()->GetPosition() : step_->GetPreStepPoint()->GetPosition();
				double xp = pos.x()/mm;
				double yp = pos.y()/mm;
				double zp = pos.z()/mm;
				return math::XYZVectorD(xp,yp,zp);
			}
			math::XYZVectorD getMomentum() const {
				auto mom = step_->GetTrack()->GetMomentum();
				double px = mom.x();
				double py = mom.y();
				double pz = mom.z();
				return math::XYZVectorD(px,py,pz);
			}
			std::pair<std::string,int> getNameNumber(int level) const {
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
				return std::make_pair(touch->GetVolume(level)->GetName(), touch->GetReplicaNumber(level));
			}
			std::string getVolumeName() const {
				return step_->GetPreStepPoint()->GetPhysicalVolume()->GetName();
			}
			
		private:
			const G4Step* step_;
	};
	
	template <>
	class StepWrapper<geant::Track, vecgeom::LogicalVolume> {
		public:
			StepWrapper(const geant::Track* tmp) : track_(tmp) {}
			vecgeom::LogicalVolume getVolume() const { return step_->GetVolume(); }
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
				touch->TopMatrix()->Transform(Vector3D(track_->X(),track_->Y(),track_->Z()),local);
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

