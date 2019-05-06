#ifndef SimGVCore_CaloG4_StepWrapper
#define SimGVCore_CaloG4_StepWrapper

#include "SimGVCore/Calo/interface/StepWrapper.h"

//includes for G4
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4ParticleTable.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4NavigationHistory.hh"

namespace sim {
	template <>
	class StepWrapper<G4Step, G4LogicalVolume> {
		public:
			StepWrapper(const G4Step* tmp) : step_(tmp) {}
			const G4LogicalVolume* getVolume() const { return step_->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume(); }
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
}

#endif

