#include "SimGVCore/Application/interface/CMSFieldConstruction.h"

CMSFieldConstruction::CMSFieldConstruction(const MagneticField* field, const edm::ParameterSet& params) : fCMSfield(new CMSGridField(field,params)) {}

CMSFieldConstruction::~CMSFieldConstruction(){
	delete fCMSfield;
}

bool CMSFieldConstruction::CreateFieldAndSolver(bool useRungeKutta, VVectorField **fieldPP) {
	//DormandPrince (?)
	const int stepperTypeId = 6;

	geant::FieldConfig *fieldConfig = new geant::FieldConfig(fCMSfield, false);
	geant::UserFieldConstruction::CreateSolverForField<CMSGridField>(fCMSfield, stepperTypeId);
	geant::FieldLookup::SetFieldConfig(fieldConfig);
	if(fieldPP) *fieldPP = fCMSfield;

	return true;	
}
