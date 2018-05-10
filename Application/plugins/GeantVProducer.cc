//
//  GeantVProducer.cpp
//
//  Ported from GeantV by Kevin Pedro on 2018/03
//  (https://gitlab.cern.ch/GeantV/geant/blob/master/examples/physics/cmsToyGV/TBBProcessingDemo/TBBTestModules/GeantVProducer.cpp)

#include <vector>
#include <memory>
#include <atomic>

// framework
#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/src/PreallocationConfiguration.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/RootHandlers.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "Fireworks/Geometry/interface/DisplayGeomRecord.h"

#include "Geant/Config.h"
#include "Geant/RunManager.h"
#include "Geant/EventSet.h"
#include "Geant/Particle.h"

#include "Geant/PhysicsProcessHandler.h"
#include "Geant/PhysicsListManager.h"

#include "SimGVCore/Application/interface/CMSApplicationTBB.h"
#include "Geant/example/CMSPhysicsList.h"
#include "Geant/example/CMSDetectorConstruction.h"
#include "TGeoManager.h"

using namespace geant;
using namespace cmsapp;

class GeantVProducer : public edm::global::EDProducer<> {
  public:
    GeantVProducer(edm::ParameterSet const&);
    ~GeantVProducer() override;

    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

  private:
    void preallocate(edm::PreallocationConfiguration const&) override;

    RunManager* initialize() const;

    /** Functions using new GeantV interface */
    void RunTransportTask(const HepMC::GenEvent * evt, long long event_index) const;

    /** @brief Generate an event set to be processed by a single task.
    Not required as application functionality, the event reading or generation
    can in the external event loop.
    */
    geant::EventSet* GenerateEventSet(const HepMC::GenEvent * evt, long long event_index, TaskData *td) const;

    // e.g. cms2015.root, cms2018.gdml, ExN03.root
    std::string cms_geometry_filename;
    edm::EDGetTokenT<edm::HepMCProduct> m_InToken;
    int n_threads;
    // cheating because run manager's functions modify its internal state
    // hope it handles locking internally
    mutable std::atomic<RunManager*> fRunMgr;
};

GeantVProducer::GeantVProducer(edm::ParameterSet const& iConfig) :
    cms_geometry_filename(iConfig.getParameter<std::string>("geometry")),
    m_InToken(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("HepMCProductLabel"))),
    n_threads(0),
	fRunMgr(nullptr)
{
    produces<long long>();
}

GeantVProducer::~GeantVProducer() {
    // this will delete fConfig also
    delete fRunMgr.load();
}

void GeantVProducer::preallocate(edm::PreallocationConfiguration const& iPrealloc) {
    n_threads = iPrealloc.numberOfThreads();
}

RunManager* GeantVProducer::initialize() const {
    int n_propagators = 1;
    int n_track_max = 500;
    int n_reuse = 100000;
    bool usev3 = true, usenuma = false;
    bool performance = true;

    // instantiate configuration helper
    GeantConfig* fConfig = new GeantConfig();

    fConfig->fRunMode = GeantConfig::kExternalLoop;

    fConfig->fGeomFileName = cms_geometry_filename;
    fConfig->fNtotal = 9999; //need to get nevents from somewhere
    fConfig->fNbuff = n_threads;

    // V3 options
    fConfig->fNstackLanes = 10;
    fConfig->fNmaxBuffSpill = 128;  // New configuration parameter!!!
    fConfig->fUseV3 = usev3;

    fConfig->fUseRungeKutta = false;  // Enable use of RK integration in field for charged particles
    // prop->fEpsilonRK = 0.001;      // Revised / reduced accuracy - vs. 0.0003 default

    fConfig->fUseNuma = usenuma;
    fConfig->fNminThreshold = 5 * n_threads;
    fConfig->fNaverage = 50;

    // Initial vector size, this is no longer an important model parameter,
    // because it gets dynamically modified to accommodate the track flow
    fConfig->fNperBasket = 16; // Initial vector size (tunable)

    // This is now the most important parameter for memory considerations
    fConfig->fMaxPerBasket = n_track_max;  // Maximum vector size (tunable)

    // This is temporarily used as gun energy
    fConfig->fEmax = 10;   // [GeV] used for now to select particle gun energy

    // Activate I/O
    fConfig->fFillTree = false;

    // Set threshold for tracks to be reused in the same volume
    fConfig->fNminReuse = n_reuse;

    // Activate standard scoring   
    fConfig->fUseStdScoring = false;
    if (performance) fConfig->fUseStdScoring = false;

    // Activate vectorized geometry (for now does not work properly with MT)
    fConfig->fUseVectorizedGeom = false;

     // Create run manager
    edm::LogInfo("GeantVProducer") <<"*** RunManager: instantiating with "<< n_propagators <<" propagators and "<< n_threads <<" threads.";
    RunManager* runMgr = new RunManager(n_propagators, n_threads, fConfig);

    // create the real physics main manager/interface object and set it in the RunManager
    edm::LogInfo("GeantVProducer") <<"*** RunManager: setting physics process...";
    runMgr->SetPhysicsInterface(new geantphysics::PhysicsProcessHandler());

    // Create user defined physics list for the full CMS application
    geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(new cmsapp::CMSPhysicsList());

    // Detector construction
    auto detector_construction = new CMSDetectorConstruction(runMgr);
    detector_construction->SetGDMLFile(cms_geometry_filename); 
    runMgr->SetDetectorConstruction( detector_construction );

    CMSApplicationTBB *cmsApp = new CMSApplicationTBB(runMgr, nullptr);
    cmsApp->SetPerformanceMode(performance);
    edm::LogInfo("GeantVProducer") <<"*** RunManager: setting up CMSApplicationTBB...";
    runMgr->SetUserApplication( cmsApp );

    // Start simulation for all propagators
    edm::LogInfo("GeantVProducer") <<"*** RunManager: initializing...";
    runMgr->Initialize();

    edm::LogInfo("GeantVProducer") << "= GeantV initialized using maximum " << runMgr->GetNthreads() << " worker threads";

    return runMgr;
}

void GeantVProducer::produce(edm::StreamID, edm::Event& iEvent, edm::EventSetup const& iSetup) const
{
    // first check for initialization
    if(!fRunMgr.load(std::memory_order_acquire)){
        // obtain the geometry
        edm::ESTransientHandle<TGeoManager> geoh;
        iSetup.get<DisplayGeomRecord>().get(geoh);
        // this fills gGeoManager used by Geant classes
        const TGeoManager *geom = geoh.product();
        edm::LogInfo("GeantVProducer") << " produce(): gGeoManager = " << gGeoManager;

        // initialize manager
        // avoid CMSSW exception from kWarning issued by Geant::RunManager
        edm::Service<edm::RootHandlers> rootHandler;
        rootHandler->ignoreWarningsWhileDoing(
            [this] {
                RunManager* ptr = this->initialize();
                RunManager* expect = nullptr;
                bool exchanged = this->fRunMgr.compare_exchange_strong(expect, ptr, std::memory_order_acq_rel);
                if(!exchanged) delete ptr;
            },
            edm::RootHandlers::SeverityLevel::kError
        );
    }

    edm::Handle<edm::HepMCProduct> HepMCEvt;
    iEvent.getByToken(m_InToken, HepMCEvt);

    // this will block the thread until completed
    edm::LogInfo("GeantVProducer") << " produce(): *** Run GeantV simulation task ***";
    RunTransportTask(HepMCEvt->GetEvent(), iEvent.eventAuxiliary().event());

    edm::LogInfo("GeantVProducer") <<" at "<< this <<": adding to event...";
    iEvent.put(std::move(std::make_unique<long long>(iEvent.eventAuxiliary().event())));
    edm::LogInfo("GeantVProducer") <<" at "<< this <<": done!";
}

// This is the entry point for the user code to transport as a task a set of events
void GeantVProducer::RunTransportTask(const HepMC::GenEvent * evt, long long event_index) const
{
    // First book a transport task from GeantV run manager
    TaskData *td = fRunMgr.load()->BookTransportTask();
    edm::LogInfo("GeantVProducer") <<" RunTransportTask: td= "<< td <<", EventID="<<event_index;
    if (!td) return;

    // ... then create the event set
    geant::EventSet *evset = GenerateEventSet(evt, event_index, td);

    edm::Service<edm::RootHandlers> rootHandler;
    rootHandler->ignoreWarningsWhileDoing([this,evset,td] {
        // ... finally invoke the GeantV transport task
        bool transported = this->fRunMgr.load()->RunSimulationTask(evset, td);

        // Now we could run some post-transport task
        edm::LogInfo("GeantVProducer")<<" RunTransportTask: task "<< td->fTid <<" : transported="<< transported;
    }, edm::RootHandlers::SeverityLevel::kError);
}

// eventually this can become more like SimG4Core/Generators/interface/Generator.h
geant::EventSet* GeantVProducer::GenerateEventSet(const HepMC::GenEvent * evt, long long event_index, geant::TaskData *td) const
{
    using EventSet = geant::EventSet;
    using Event = geant::Event;
    using Track = geant::Track;

    EventSet *evset = new EventSet(1);
    Event *event = new Event();

    // convert from HepMC to GeantV format
    //event->SetEvent(evt->event_number());
    event->SetEvent(event_index);
    event->SetNprimaries(evt->particles_size());
    const auto& vpos = (*(evt->vertices_begin()))->position();
    event->SetVertex(vpos.x(),vpos.y(),vpos.z());
    unsigned counter = 0;
    for(auto i_part = evt->particles_begin(); i_part != evt->particles_end(); ++i_part){
        Track &track = td->GetNewTrack();
        int trackIndex = event->AddPrimary(&track);
        track.SetParticle(trackIndex);
        track.SetPrimaryParticleIndex(counter);

        // get geantv version of particle
        const geantphysics::Particle *pParticle = geantphysics::Particle::GetParticleByPDGCode((*i_part)->pdg_id());
        const auto& mom = (*i_part)->momentum();
        const auto& pos = (*i_part)->production_vertex()->position();

        // set track properties
        track.SetPDG(pParticle->GetPDGCode());
        track.SetGVcode(pParticle->GetInternalCode());
        track.SetCharge(pParticle->GetPDGCharge());
        track.SetMass(pParticle->GetPDGMass());
        track.SetDirection(mom.px(),mom.py(),mom.pz());
        track.SetPosition(pos.x(),pos.py(),pos.pz());
        track.SetE(mom.e());
        track.SetP(mom.perp());
        
        ++counter;
    }    

    evset->AddEvent(event);
    return evset;
}

DEFINE_FWK_MODULE(GeantVProducer);
