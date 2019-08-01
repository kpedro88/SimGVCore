#ifndef SimGVCore_CaloG4_StepWrapper
#define SimGVCore_CaloG4_StepWrapper

#include "SimGVCore/Calo/interface/StepWrapper.h"
#include "SimG4Core/Notification/interface/G4TrackToParticleID.h"

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
			double getEnergyDeposit() const { return step_->GetTotalEnergyDeposit()/CLHEP::MeV; }
			double addEnergy() const { return (((step_->GetPostStepPoint() == nullptr) || (step_->GetTrack()->GetNextVolume() == nullptr)) && (step_->IsLastStepInVolume())) ? (step_->GetPreStepPoint()->GetKineticEnergy()/CLHEP::MeV) : 0.0; }
			double getTime() const { return step_->GetTrack()->GetGlobalTime()/CLHEP::nanosecond; }
			int getTrackID() const { return step_->GetTrack()->GetTrackID(); }
			bool getEM() const { return G4TrackToParticleID::isGammaElectronPositron(step_->GetTrack()); }
			double getStepLength() const { return step_->GetStepLength()/CLHEP::cm; }
			double getCharge() const { return step_->GetPreStepPoint()->GetCharge(); }
			double getDensity() const { return step_->GetPreStepPoint()->GetMaterial()->GetDensity()/(CLHEP::g/CLHEP::cm3); }
			int getSize() const { return (1+step_->GetPreStepPoint()->GetTouchable()->GetHistoryDepth()); }
			double getDz() const {
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
				auto const& hitPoint= step_->GetPreStepPoint()->GetPosition();
				auto local = touch->GetHistory()->GetTopTransform().TransformPoint(hitPoint);
				return (local.z()/CLHEP::mm);
			}
			double getRadlen() const { return (step_->GetPreStepPoint()->GetMaterial()->GetRadlen()/CLHEP::mm); }
			int getCopyNo(int level) const { return step_->GetPreStepPoint()->GetTouchable()->GetReplicaNumber(level); }
			uint32_t getCopy() const { 
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
				return (touch->GetHistoryDepth() < 1) ? static_cast<uint32_t>(touch->GetReplicaNumber(0)) : static_cast<uint32_t>(touch->GetReplicaNumber(0) + 1000*touch->GetReplicaNumber(1));
			}
			std::pair<int,int> getCopyNos() const { 
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
			        return std::make_pair(touch->GetReplicaNumber(0),touch->GetReplicaNumber(1));
			}
			math::XYZVectorD getPosition(bool trk) const {
				auto pos = trk ? step_->GetTrack()->GetPosition() : step_->GetPreStepPoint()->GetPosition();
				double xp = pos.x()/CLHEP::mm;
				double yp = pos.y()/CLHEP::mm;
				double zp = pos.z()/CLHEP::mm;
				return math::XYZVectorD(xp,yp,zp);
			}
			math::XYZVectorD getMomentum() const {
				auto mom = step_->GetTrack()->GetMomentum();
				double px = mom.x()/CLHEP::MeV;
				double py = mom.y()/CLHEP::MeV;
				double pz = mom.z()/CLHEP::MeV;
				return math::XYZVectorD(px,py,pz);
			}
			std::pair<std::string,int> getNameNumber(int level) const {
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
				return std::make_pair(touch->GetVolume(level)->GetName(), touch->GetReplicaNumber(level));
			}
			void setNameNumber(EcalBaseNumber & baseNumber) const {
			        int size = (1+step_->GetPreStepPoint()->GetTouchable()->GetHistoryDepth());
				if (baseNumber.getCapacity() < size ) baseNumber.setSize(size);
				auto const touch = step_->GetPreStepPoint()->GetTouchable();
				for (int ii = 0; ii < size ; ii++)
				  baseNumber.addLevel(touch->GetVolume(ii)->GetName(), touch->GetReplicaNumber(ii));
			}
			std::string getVolumeName() const {
				return step_->GetPreStepPoint()->GetPhysicalVolume()->GetName();
			}
			
		private:
			const G4Step* step_;
	};
}

#endif

