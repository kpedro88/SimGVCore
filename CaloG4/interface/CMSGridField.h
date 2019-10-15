#ifndef SimGVCore_CaloG4_CMSGridField
#define SimGVCore_CaloG4_CMSGridField

//based on https://gitlab.cern.ch/GeantV/geant/blob/master/examples/physics/FullCMS/Geant4/include/G4ScalarRZMagFieldFromMap.hh
//originally developed by John Apostolakis and Ananya

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "G4SystemOfUnits.hh"
#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"

#include <vector>

class CMSGridField : public G4MagneticField {
	public:
		//constructors
		CMSGridField(const MagneticField* field, const edm::ParameterSet& params);
		CMSGridField(const CMSGridField& other);
		//destructor
		~CMSGridField() {}

		//accessors
		//takes as input x,y,z; gives output Bx,By,Bz
		void GetFieldValueXYZ(const G4ThreeVector& pos, G4ThreeVector& xyzField) const;
		//evaluate field value at input position
		void GetFieldValue(const double posArr[4], double fieldArr[3] ) const override final {
			const G4ThreeVector position( posArr[0], posArr[1], posArr[2] );
			G4ThreeVector field;
			GetFieldValueXYZ(position, field);
			fieldArr[0] = field.x();
			fieldArr[1] = field.y();
			fieldArr[2] = field.z();
		}

	private:
		//helpers
		//for (R,Z) pairs : gives field in cylindrical coordinates in rzfield
		void GetFieldValueRZ(double r, double Zin, G4ThreeVector &rzField) const;
		//used to convert cartesian coordinates to cylindrical coordinates R-Z-phi (does not calculate phi)
		inline void CartesianToCylindrical(const G4ThreeVector &cart, double cyl[2]) const {
			cyl[0] = sqrt(cart.x() * cart.x() + cart.y() * cart.y());
			cyl[1] = cart.z();
		}
		//converts cylindrical magnetic field to field in cartesian coordinates
		inline void CylindricalToCartesian(const G4ThreeVector &rzField, const double sinTheta, const double cosTheta, G4ThreeVector &xyzField) const {
			G4double fldX = rzField.x() * cosTheta - rzField.y() * sinTheta;
			G4double fldY = rzField.x() * sinTheta + rzField.y() * cosTheta;
			xyzField = G4ThreeVector( fldX, fldY, rzField.z() );
		}

		//members
		const double Rmin_, Rmax_, Rstep_, RstepInv_;
		const int Rnum_;
		const double Zmin_, Zmax_, Zstep_, ZstepInv_;
		const int Znum_, ZnumHalf_;
		const double Ainverse_;
		std::vector<double> fRadius, fPhi, fZ, fBr, fBz, fBphi;
};

#endif

