#ifndef SimGVCore_CaloSteppingAction_H
#define SimGVCore_CaloSteppingAction_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/Point3D.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/PluginManager/interface/ModuleDef.h"

#include "SimG4Core/Notification/interface/Observer.h"
#include "SimG4Core/Watcher/interface/SimProducer.h"
#include "SimG4Core/Watcher/interface/SimWatcherFactory.h"

#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"
#include "SimDataFormats/SimHitMaker/interface/CaloSlaveSD.h"

#include "SimG4CMS/Calo/interface/HcalNumberingScheme.h"
#include "SimGVCore/Calo/interface/CaloGVHit.h"
#include "SimGVCore/Calo/interface/HcalNumberingFromPS.h"

#include "Geometry/EcalCommonData/interface/EcalBarrelNumberingScheme.h"
#include "Geometry/EcalCommonData/interface/EcalBaseNumber.h"
#include "Geometry/EcalCommonData/interface/EcalEndcapNumberingScheme.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

template <class Traits>
class CaloSteppingAction : public SimProducer,
                           public Observer<const Traits::BeginJob *>
                           public Observer<const Traits::BeginRun *>, 
                           public Observer<const Traits::BeginEvent *>, 
                           public Observer<const Traits::EndEvent *>, 
                           public Observer<const Traits::Step *> {

public:
  CaloSteppingAction(const edm::ParameterSet &p);
  ~CaloSteppingAction() override;

  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  void fillHits(edm::PCaloHitContainer& cc, int type);
  // observer classes
  void update(const Traits::BeginJob * job)   override { update(BeginJobWrapper(job)); }
  void update(const Traits::BeginRun * run)   override { update(BeginRunWrapper(run)); }
  void update(const Traits::BeginEvent * evt) override { update(BeginEventWrapper(evt)); }
  void update(const Traits::Step * step)      override { update(StepWrapper(step)); }
  void update(const Traits::EndEvent * evt)   override { update(EndEventWrapper(evt)); }

  // subordinate functions with unified interfaces
  void update(const Traits::BeginJobWrapper& job);
  void update(const Traits::BeginRunWrapper& run);
  void update(const Traits::BeginEventWrapper& evt);
  void update(const Traits::StepWrapper& step);
  void update(const Traits::EndEventWrapper& evt);

  void NaNTrap(const Traits::StepWrapper&) const;
  uint32_t getDetIDHC(int det, int lay, int depth,
		      const math::XYZVectorD& pos) const;
  void fillHit(uint32_t id, double dE, double time, int primID, 
	       uint16_t depth, double em, int flag);
  uint16_t getDepth(bool flag, double crystalDepth, double radl) const;
  double   curve_LY(double crystalLength, double crystalDepth) const;
  double   getBirkL3(double dE, double step, double chg, double dens) const;
  double   getBirkHC(double dE, double step, double chg, double dens) const;
  void     saveHits(int flag);

  static const int                      nSD_= 3;
  std::unique_ptr<EcalBarrelNumberingScheme> ebNumberingScheme_;
  std::unique_ptr<EcalEndcapNumberingScheme> eeNumberingScheme_;
  std::unique_ptr<HcalNumberingFromPS>       hcNumberingPS_;
  std::unique_ptr<HcalNumberingScheme>       hcNumberingScheme_;
  std::unique_ptr<CaloSlaveSD>               slave_[nSD_];

  std::vector<std::string>              nameEBSD_, nameEESD_, nameHCSD_;
  std::vector<std::string>              nameHitC_;
  std::vector<const Traits::Volume>   volEBSD_, volEESD_, volHCSD_;
  std::map<const Traits::Volume,double> xtalMap_;
  int                                   count_, eventID_;
  double                                slopeLY_, birkC1EC_, birkSlopeEC_;
  double                                birkCutEC_, birkC1HC_, birkC2HC_;
  double                                birkC3HC_;
  std::map<std::pair<int,CaloHitID>,CaloGVHit> hitMap_[nSD_];
};

#endif
