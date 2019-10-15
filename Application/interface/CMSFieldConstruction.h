#ifndef SimGVCore_Application_CMSFieldConstruction
#define SimGVCore_Application_CMSFieldConstruction

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "Geant/UserFieldConstruction.h"

#include "SimGVCore/CaloGV/interface/CMSGridField.h"

class CMSFieldConstruction : public geant::cxx::UserFieldConstruction {
	public:
		CMSFieldConstruction(const MagneticField* field, const edm::ParameterSet& params);
		~CMSFieldConstruction();

		bool CreateFieldAndSolver(bool useRungeKutta, VVectorField **fieldPP = nullptr) override final;

	private:
		CMSGridField* fCMSfield;
};

#endif
