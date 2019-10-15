#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "SimGVCore/CaloG4/interface/CMSGridField.h"

#include <cmath>
#include <cassert>

CMSGridField::CMSGridField(const MagneticField* field, const edm::ParameterSet& params) :
	Rmin_(params.getParameter<double>("Rmin")),
	Rmax_(params.getParameter<double>("Rmax")),
	Rstep_(params.getParameter<double>("Rstep")),
	RstepInv_(1./Rstep_),
	Rnum_((Rmax_-Rmin_)/Rstep_+1),
	Zmin_(params.getParameter<double>("Zmin")),
	Zmax_(params.getParameter<double>("Zmax")),
	Zstep_(params.getParameter<double>("Zstep")),
	ZstepInv_(1./Zstep_),
	Znum_((Zmax_-Zmin_)/Zstep_+1),
	ZnumHalf_(Znum_/2),
	Ainverse_(CLHEP::tesla/(Rstep_*Zstep_))
{
	const double num = Rnum_*Znum_;
	fRadius.reserve(num);
	fPhi.reserve(num);
	fZ.reserve(num);
	fBr.reserve(num);
	fBz.reserve(num);
	fBphi.reserve(num);
	//construct grid from field
	const double phi = 0.;
	for(auto R = Rmin_; R <= Rmax_; R += Rstep_){
		for(auto Z = Zmin_; Z <= Zmax_; Z += Zstep_){
			//evaluate magnetic field at each grid point from conditions
			GlobalPoint point(GlobalPoint::Cylindrical(R, phi, Z));
			fRadius.push_back(R);
			fZ.push_back(Z);
			fPhi.push_back(phi);
			const auto& fieldVal = field->inTesla(point);
			fBr.push_back(fieldVal.perp());
			fBz.push_back(fieldVal.z());
			fBphi.push_back(fieldVal.phi());
		}
	}
}

CMSGridField::CMSGridField(const CMSGridField& other) :
	Rmin_(other.Rmin_),
	Rmax_(other.Rmax_),
	Rstep_(other.Rstep_),
	RstepInv_(other.RstepInv_),
	Rnum_(other.Rnum_),
	Zmin_(other.Zmin_),
	Zmax_(other.Zmax_),
	Zstep_(other.Zstep_),
	ZstepInv_(other.ZstepInv_),
	Znum_(other.Znum_),
	ZnumHalf_(other.ZnumHalf_),
	Ainverse_(other.Ainverse_),
	fRadius(other.fRadius),
	fPhi(other.fPhi),
	fZ(other.fZ),
	fBr(other.fBr),
	fBz(other.fBz),
	fBphi(other.fBphi)
{

}

void CMSGridField::GetFieldValueRZ(double r, double Zin, G4ThreeVector &rzField) const {
	//take care that radius and z for out of limit values take values at end points
	double radius = std::min(r, Rmax_);
	double z      = std::max(std::min(Zin, Zmax_), Zmin_);
	
	// to make sense of the indices, consider any particular instance e.g. (25,-200)
	int rFloor   = std::min( (int) floor(radius * RstepInv_), Rnum_ - 2 );
	int rIndLow  = rFloor * Znum_;
	int rIndHigh = rIndLow + Znum_;
	
	// if we use z-z0 in place of two loops for Z<0 and Z>0
	// z-z0 = [0,32000]
	// so indices 0 to 160 : total 161 indices for (z-z0)/200
	
	int zInd = std::min( (int) std::floor(z * ZstepInv_) + ZnumHalf_, Znum_ - 2);
	
	// need i1,i2,i3,i4 for 4 required indices
	int i1            = rIndLow + zInd;
	int i2            = i1 + 1;
	int i3            = rIndHigh + zInd;
	int i4            = i3 + 1;
	double zLow       = (zInd - ZnumHalf_) * Zstep_; // 80 because it's the middle index in 0 to 160
	double zHigh      = zLow + Zstep_;
	double radiusLow  = rFloor * Rstep_;
	double radiusHigh = radiusLow + Rstep_;
	
	// now write function
	double a1 = (radiusHigh - radius) * (zHigh - z); // area to be multiplied with i1
	double a2 = (radiusHigh - radius) * (z - zLow);
	double a3 = (radius - radiusLow) * (zHigh - z);
	double a4 = (radius - radiusLow) * (z - zLow);
	
	unsigned long minSzUL = std::min( std::min( fBr.size(), fBphi.size()) , fBz.size() );
	
	assert( 0 <= i1 );
	assert( i4 <= (int) minSzUL ) ;
	assert( 0. <= a1  && a1 * Ainverse_ <= 1.0 );
	assert( 0. <= a2  && a2 * Ainverse_ <= 1.0 );
	assert( 0. <= a3  && a3 * Ainverse_ <= 1.0 );
	assert( 0. <= a4  && a4 * Ainverse_ <= 1.0 );  
	
	double BR   = (fBr[i1] * a1 + fBr[i2] * a2 + fBr[i3] * a3 + fBr[i4] * a4) * Ainverse_;
	double BZ   = (fBz[i1] * a1 + fBz[i2] * a2 + fBz[i3] * a3 + fBz[i4] * a4) * Ainverse_;
	double BPhi = (fBphi[i1] * a1 + fBphi[i2] * a2 + fBphi[i3] * a3 + fBphi[i4] * a4) * Ainverse_;
	
	rzField = G4ThreeVector( BR, BPhi, BZ );
}

void CMSGridField::GetFieldValueXYZ(const G4ThreeVector& pos, G4ThreeVector& xyzField) const {
	double cyl[2];
	CartesianToCylindrical(pos, cyl);
	G4ThreeVector rzField;
	GetFieldValueRZ(cyl[0], cyl[1], rzField); // cyl[2] =[r,z]
	
	double sinTheta = 0.0, cosTheta = 1.0; // initialize as theta=0
	// To take care of r =0 case
	if (cyl[0] != 0.0) {
		double rInv = 1 / cyl[0];
		sinTheta    = pos.y() * rInv;
		cosTheta    = pos.x() * rInv;
	}
	
	CylindricalToCartesian(rzField, sinTheta, cosTheta, xyzField);
}
