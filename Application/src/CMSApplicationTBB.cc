//===--- CMSApplicationTBB.cpp - Geant-V ------------------------*- C++ -*-===//
//
//                     Geant-V Prototype               
//
//===----------------------------------------------------------------------===//
/**
 * @file CMSApplicationTBB.cpp
 * @brief Implementation of simple scoring for CMS geometry 
 * @author Guilherme Lima, Andrei Gheata
 */
//===----------------------------------------------------------------------===//

// ported from https://gitlab.cern.ch/GeantV/geant/blob/master/examples/physics/cmsToyGV/TBBProcessingDemo/TBBTestModules/CMSApplicationTBB.cpp

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimGVCore/Application/interface/CMSApplicationTBB.h"
#include "Geant/Event.h"
#include "Geant/TaskData.h"
#include "Geant/Error.h"

using namespace geant;

namespace cmsapp {
CMSApplicationTBB::CMSApplicationTBB(geant::RunManager *runmgr, CMSParticleGun* gun)
  : CMSFullApp(runmgr, gun) 
{
  if(!gun){
    //make a dummy
    gun = new CMSParticleGun();
    gun->SetNumPrimaryPerEvt(0);
  }
}

CMSApplicationTBB::~CMSApplicationTBB() {}

//// User actions in terms of TBB tasks
void CMSApplicationTBB::SetEventContinuationTask(int ievt, tbb::task *pTask) {
  std::lock_guard<std::mutex> lock(fMapLock);
  fPostSimTaskMap.insert( std::pair<int,tbb::task*>(ievt, pTask) );
  edm::LogInfo("CMSApplicationTBB")<<"SetEvtContTask: ievt="<<ievt<<", pTask=<"<<pTask<<">, map.size="<<fPostSimTaskMap.size();
}


//______________________________________________________________________________
void CMSApplicationTBB::FinishEvent(geant::Event *event) {
  // find next tbb::task and decrement its ref count
  CMSFullApp::FinishEvent(event);
  std::lock_guard<std::mutex> lock(fMapLock);
  auto iter = fPostSimTaskMap.find(event->GetEvent());
  if (iter != fPostSimTaskMap.end()) {
    tbb::task* pTask = iter->second;
    edm::LogInfo("GeantVProducer")<<"FinishEvent("<<event->GetEvent()<<","<<event->GetSlot()<<"), iter=<"<<pTask<<">, map.size="<<fPostSimTaskMap.size();
    pTask->decrement_ref_count();
    edm::LogInfo("GeantVProducer")<<"FinishEvent: pTask ref count="<<pTask->ref_count();
  }
}

}
