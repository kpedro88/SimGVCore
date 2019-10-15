#ifndef SimGVCore_CaloGV_CMSGridField
#define SimGVCore_CaloGV_CMSGridField

//based on https://gitlab.cern.ch/GeantV/geant/blob/master/examples/physics/FullCMS/GeantV/inc/CMSmagField.h
//originally developed by John Apostolakis and Ananya

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "base/Vector3D.h"
#include "base/Global.h"
#include "Geant/SystemOfUnits.h"
#include "Geant/VectorTypes.h"

#define FORCE_INLINE 1

#include "Geant/VVectorField.h"

class CMSGridField : public VVectorField {
  using Double_v = geant::Double_v;
  using Float_v  = geant::Float_v;

  template <typename T>
  using Vector3D = vecgeom::Vector3D<T>;

public:
  CMSGridField(const MagneticField* field, const edm::ParameterSet& params);
  CMSGridField(const CMSGridField& other);

  //Scalar interface for field retrieval
  void ObtainFieldValue(const Vector3D<double> &position, Vector3D<double> &fieldValue) override final {
    EstimateFieldValues<double>(position, fieldValue);
  }

  //Vector interface for field retrieval
  void ObtainFieldValueSIMD(const Vector3D<Double_v> &position, Vector3D<Double_v> &fieldValue) override final {
    EstimateFieldValues<Double_v>(position, fieldValue);
  }

  //Scalar interface for field retrieval
  void ObtainFieldValue(const Vector3D<float> &position, Vector3D<float> &fieldValue) {
    EstimateFieldValues<float>(position, fieldValue);
  }
   
  //Templated field interface
  template <typename Real_v>
  void EstimateFieldValues(const Vector3D<Real_v> &position, Vector3D<Real_v> &fieldValue);

  ~CMSGridField();

public:
  //  Invariants -- parameters of the field
  static constexpr float tesla      = geant::units::tesla;
  static constexpr float kilogauss  = geant::units::kilogauss;
  static constexpr float millimeter = geant::units::millimeter;

  static constexpr int gNumFieldComponents  = 3;
  static constexpr bool gFieldChangesEnergy = false;

  // For (R,Z) pairs : gives field in cylindrical coordinates in rzfield
  template <typename Real_v>
  void GetFieldValueRZ(const Real_v &radius, const Real_v &z, Vector3D<Real_v> &rzField);

protected:
  // Used to convert cartesian coordinates to cylindrical coordinates R-Z-phi
  // Does not calculate phi
  template <typename Real_v>
  GEANT_FORCE_INLINE void CartesianToCylindrical(const Vector3D<Real_v> &cart, Real_v cyl[2]);

  // Converts cylindrical magnetic field to field in cartesian coordinates
  template <typename Real_v>
  GEANT_FORCE_INLINE void CylindricalToCartesian(const Vector3D<Real_v> &rzField, const Real_v &sinTheta, const Real_v &cosTheta, Vector3D<Real_v> &xyzField);

  // Gets the field array pointer in the appropriate form
  template <typename Real_v>
  GEANT_FORCE_INLINE const typename vecCore::Scalar<Real_v> *GetFieldArray() const;

  // Takes care of indexing into multiple places in AOS.
  template <typename Real_v>
  GEANT_FORCE_INLINE void Gather2(const vecCore::Index<Real_v> index, Real_v B1[3], Real_v B2[3]);

public:
  // Methods for Multi-threading - needed for ScalarMagFieldEquation / GUFieldPropagator
  CMSGridField* CloneOrSafeSelf( bool* pSafe );
  VVectorField* Clone() const override;

  enum kIndexRPhiZ { kIndR = 0, kIndPhi = 1, kIndZ = 2 };

private:
  const double Rmin_, Rmax_, Rstep_, RstepInv_;
  const int Rnum_;
  const double Zmin_, Zmax_, Zstep_, ZstepInv_;
  const int Znum_, ZnumHalf_, RZnum_, Vnum_;
  const double Ainverse_;
  float* fMagLinArray;
  double* fMagLinArrayD;
};

CMSGridField::CMSGridField(const MagneticField* field, const edm::ParameterSet& params) : VVectorField(gNumFieldComponents, gFieldChangesEnergy),
  Rmin_(params.getParameter<double>("Rmin")*millimeter),
  Rmax_(params.getParameter<double>("Rmax")*millimeter),
  Rstep_(params.getParameter<double>("Rstep")),
  RstepInv_(1./Rstep_),
  Rnum_((Rmax_-Rmin_)/Rstep_+1),
  Zmin_(params.getParameter<double>("Zmin")*millimeter),
  Zmax_(params.getParameter<double>("Zmax")*millimeter),
  Zstep_(params.getParameter<double>("Zstep")),
  ZstepInv_(1./Zstep_),
  Znum_((Zmax_-Zmin_)/Zstep_+1),
  ZnumHalf_(Znum_/2),
  RZnum_(Rnum_*Znum_),
  Vnum_(RZnum_*gNumFieldComponents),
  Ainverse_(CLHEP::tesla/(Rstep_*Zstep_))
{
  fMagLinArray = new float[Vnum_];
  fMagLinArrayD = new double[Vnum_];  

  //construct grid from field
  const double phi = 0.;
  unsigned ind = 0;
  for(auto R = Rmin_; R <= Rmax_; R += Rstep_){
    for(auto Z = Zmin_; Z <= Zmax_; Z += Zstep_){
      //evaluate magnetic field at each grid point from conditions
      GlobalPoint point(GlobalPoint::Cylindrical(R, phi, Z));
      const auto& fieldVal = field->inTesla(point);
      fMagLinArray[ind + kIndR] = fieldVal.perp();
      fMagLinArray[ind + kIndPhi] = fieldVal.phi();
      fMagLinArray[ind + kIndZ] = fieldVal.z();
      ind += gNumFieldComponents;
    }
  }
  //copy to double version
  for(int i = 0; i < Vnum_; ++i){
    fMagLinArrayD[i] = fMagLinArray[i];
  }
}

CMSGridField::CMSGridField(const CMSGridField& other) :
  VVectorField(gNumFieldComponents, gFieldChangesEnergy),
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
  RZnum_(other.RZnum_),
  Vnum_(other.Vnum_),
  Ainverse_(other.Ainverse_),
  fMagLinArray(other.fMagLinArray),
  fMagLinArrayD(other.fMagLinArrayD)
{
}

CMSGridField::~CMSGridField()
{
  delete[] fMagLinArray;
  delete[] fMagLinArrayD;
}

template <typename Real_v>
GEANT_FORCE_INLINE void CMSGridField::CartesianToCylindrical(const Vector3D<Real_v> &cart, Real_v cyl[2])
{
  cyl[0] = cart.Perp(); // calling sqrt at every call...
  cyl[1] = cart.z();
}

template <typename Real_v>
GEANT_FORCE_INLINE void CMSGridField::CylindricalToCartesian(const Vector3D<Real_v> &rzField, const Real_v &sinTheta, const Real_v &cosTheta, Vector3D<Real_v> &xyzField)
{
  xyzField.x() = rzField.x() * cosTheta - rzField.y() * sinTheta; // Bx= Br cos(theta) - Bphi sin(theta)
  xyzField.y() = rzField.x() * sinTheta + rzField.y() * cosTheta; // By = Br sin(theta) + Bphi cos(theta)
  xyzField.z() = rzField.z();                                     // Bz = Bz
}

template <typename Real_v>
GEANT_FORCE_INLINE const typename vecCore::Scalar<Real_v> *CMSGridField::GetFieldArray() const
{
  return nullptr;
}

template <>
GEANT_FORCE_INLINE const float *CMSGridField::GetFieldArray<geant::Float_v>() const
{
  return fMagLinArray;
}

template <>
GEANT_FORCE_INLINE const float *CMSGridField::GetFieldArray<float>() const
{
  return fMagLinArray;
}

template <>
GEANT_FORCE_INLINE const double *CMSGridField::GetFieldArray<geant::Double_v>() const
{
  return fMagLinArrayD;
}

template <typename Real_v>
GEANT_FORCE_INLINE void CMSGridField::Gather2(const vecCore::Index<Real_v> index, Real_v B1[3], Real_v B2[3])
{
  using namespace vecCore::math;   
  using Index_v = vecCore::Index<Real_v>;
  using Real_s  = vecCore::Scalar<Real_v>;

  const Index_v ind1 = gNumFieldComponents * index; // 3 components per 'location'
  const Index_v ind2 = gNumFieldComponents * Min( index + Znum_, Index_v(RZnum_) );
  
  Real_s const *addr = GetFieldArray<Real_v>();

  // Fetch one component of each point first, then the rest.
  B1[0] = vecCore::Gather<Real_v>(addr, ind1);
  B2[0] = vecCore::Gather<Real_v>(addr, ind2);

  const Index_v ind1phi = ind1 + kIndPhi;
  const Index_v ind2phi = ind2 + kIndPhi;
  B1[1]                 = vecCore::Gather<Real_v>(addr, ind1phi);
  B2[1]                 = vecCore::Gather<Real_v>(addr, ind2phi);

  const Index_v ind1z = ind1 + kIndZ;
  const Index_v ind2z = ind2 + kIndZ;
  B1[2]               = vecCore::Gather<Real_v>(addr, ind1z);
  B2[2]               = vecCore::Gather<Real_v>(addr, ind2z);
}

// Scalar specialization
template <>
GEANT_FORCE_INLINE void CMSGridField::Gather2<double>(const vecCore::Index<double> index, double B1[3], double B2[3])
{
  const int ind1 = gNumFieldComponents * int(index);
  const int indexPlus = index + Znum_;
  const int ind2 = gNumFieldComponents * std::min( indexPlus, Vnum_ );
  
  // Fetch one component of each point first, then the rest.
  B1[0] = fMagLinArrayD[ind1 + kIndR];
  B2[0] = fMagLinArrayD[ind2 + kIndR];

  B1[1] = fMagLinArrayD[ind1 + kIndPhi];
  B2[1] = fMagLinArrayD[ind2 + kIndPhi];

  B1[2] = fMagLinArrayD[ind1 + kIndZ];
  B2[2] = fMagLinArrayD[ind2 + kIndZ];
}

template <typename Real_v>
GEANT_FORCE_INLINE void CMSGridField::GetFieldValueRZ(const Real_v &rIn, const Real_v &zIn, Vector3D<Real_v> &rzField)
{
  // Convention for return value:  x -> R,  y-> Phi, z->Z
  
  using namespace vecCore::math;
  using namespace geant;
  using Index_v = vecCore::Index<Real_v>;

  // Limit radius and z:  outside take values at limit points
  const Real_v radius = Min(rIn, Real_v(Rmax_));
  const Real_v z      = Max(Min(zIn, Real_v(Zmax_)), Real_v(Zmin_));

  // to make sense of the indices, consider any particular instance e.g. (25,-200)
  const Real_v rFloor  = Floor(radius * RstepInv_);
  const Real_v rIndLow = rFloor * Real_v(Znum_);

  const Real_v zInd = Floor((z - Real_v(Zmin_)) * Real_v(ZstepInv_));
  const Index_v i1 = vecCore::Convert<Index_v>(rIndLow + zInd);
  const Index_v i2 = i1 + 1;

  Real_v B1[3], B2[3], B3[3], B4[3];

  Gather2<Real_v>(i1, B1, B3);

  const Real_v zLow  = (zInd - Real_v(ZnumHalf_)) * Real_v(Zstep_); // z=0. corresponds to index kHalfZValues
  const Real_v zHigh = zLow + Real_v(Zstep_);
  const Real_v radiusLow  = rFloor * Real_v(Rstep_);
  const Real_v radiusHigh = radiusLow + Real_v(Rstep_);

  Gather2<Real_v>(i2, B2, B4);

  const Real_v a1 = (radiusHigh - radius) * (zHigh - z); // area to be multiplied with i1
  const Real_v a2 = (radiusHigh - radius) * (z - zLow);
  const Real_v a3 = (radius - radiusLow) * (zHigh - z);
  const Real_v a4 = (radius - radiusLow) * (z - zLow);

  rzField.x() = B1[0] * a1 + B2[0] * a2 + B3[0] * a3 + B4[0] * a4; // BR
  rzField.y() = B1[1] * a1 + B2[1] * a2 + B3[1] * a3 + B4[1] * a4; // BPhi
  rzField.z() = B1[2] * a1 + B2[2] * a2 + B3[2] * a3 + B4[2] * a4; // BZ
}

template <typename Real_v>
GEANT_FORCE_INLINE void CMSGridField::EstimateFieldValues(const Vector3D<Real_v> &pos, Vector3D<Real_v> &xyzField)
{
  // Sidenote: For theta =0; xyzField = rzField.
  // theta =0 corresponds to y=0
  Real_v cyl[2];
  CartesianToCylindrical<Real_v>(pos, cyl);
  vecgeom::Vector3D<Real_v> rzField;
  GetFieldValueRZ<Real_v>(cyl[0], cyl[1], rzField); // cyl[2] =[r,z]

  using vecCore::Mask_v;

  Mask_v<Real_v> nonZero = (cyl[0] != Real_v(0.));
  Real_v rInv            = vecCore::Blend(nonZero, Real_v(1.) / cyl[0], Real_v(0.));
  Real_v sinTheta        = pos.y() * rInv;
  Real_v cosTheta        = vecCore::Blend(nonZero, pos.x() * rInv, Real_v(1.));

  CylindricalToCartesian<Real_v>(rzField, sinTheta, cosTheta, xyzField);
}

VVectorField* CMSGridField::Clone() const
{
  return new CMSGridField(*this);
}

// This class is thread safe.  So other threads can use the same instance
CMSGridField* CMSGridField::CloneOrSafeSelf(bool* pSafe)
{
  if (pSafe) *pSafe = true;
  return this;
}

#endif
