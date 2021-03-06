//
//  GeantVProducer.cpp
//
//  Ported from GeantV by Kevin Pedro on 2018/03
//  (https://gitlab.cern.ch/GeantV/geant/blob/master/examples/physics/cmsToyGV/TBBProcessingDemo/TBBTestModules/GeantVProducer.cpp)

#include <vector>
#include <memory>
#include <atomic>
#include <exception>

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

// for ExternalWork
#include "FWCore/Concurrency/interface/WaitingTaskWithArenaHolder.h"
#include "tbb/task.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "Fireworks/Geometry/interface/DisplayGeomRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "Geant/Config.h"
#include "Geant/RunManager.h"
#include "Geant/EventSet.h"
#include "Geant/Event.h"
#include "Geant/Particle.h"

#include "Geant/PhysicsProcessHandler.h"
#include "Geant/PhysicsListManager.h"

#include "SimGVCore/CaloGV/interface/CaloSteppingAction.h"
#include "SimGVCore/Application/interface/CMSEvent.h"
#include "SimGVCore/Application/interface/CMSData.h"
#include "SimGVCore/Application/interface/CMSDetectorConstruction.h"
#include "SimGVCore/Application/interface/CMSFieldConstruction.h"
#include "SimGVCore/Application/interface/CMSApplication.h"
#include "SimGVCore/Application/interface/CMSPhysicsListX.h"
#include "Geant/UserFieldConstruction.h"
#include "TGeoManager.h"

using namespace geant;
using namespace cmsapp;

// modified to handle lambdas w/ unique_ptrs
namespace {
  template<typename F>
  class FunctorTask : public tbb::task {
  public:
    explicit FunctorTask( F f): func_(std::move(f)) {}
    
    task* execute() override {
      func_();
      return nullptr;
    };
    
  private:
    F func_;
  };
  
  template< typename ALLOC, typename F>
  FunctorTask<F>* make_functor_task( ALLOC&& iAlloc, F f) {
    return new (iAlloc) FunctorTask<F>(std::move(f));
  }
}

//dummy run cache to get access to global begin run
class GeantVProducer : public edm::global::EDProducer<edm::ExternalWork,edm::RunCache<int>,edm::StreamCache<EventCache>> {
  public:
    GeantVProducer(edm::ParameterSet const&);
    ~GeantVProducer() override;

	std::unique_ptr<EventCache> beginStream(edm::StreamID) const override;

    void acquire(edm::StreamID, edm::Event const&, edm::EventSetup const&, edm::WaitingTaskWithArenaHolder) const override;

    void produce(edm::StreamID, edm::Event&, edm::EventSetup const&) const override;

  private:
    void preallocate(edm::PreallocationConfiguration const&) override;
    std::shared_ptr<int> globalBeginRun(edm::Run const&, edm::EventSetup const&) const override;
    void globalEndRun(edm::Run const&, edm::EventSetup const&) const override;

    /** Functions using new GeantV interface */
    void initialize(const MagneticField* magfield=nullptr) const;

    /** @brief Generate an event set to be processed by a single task.
    Not required as application functionality, the event reading or generation
    can in the external event loop.
    */
    std::unique_ptr<geant::EventSet> GenerateEventSet(const HepMC::GenEvent * evt, long long event_index, edm::WaitingTaskWithArenaHolder iHolder, EventCache* cache, TaskData *td) const;

	edm::ParameterSet scoringParams, gridParams;
    double zFieldInTesla;
    edm::EDGetTokenT<edm::HepMCProduct> m_InToken;
    int n_threads;
    std::unique_ptr<GeantConfig> fConfig;
    // cheating because run manager's functions modify its internal state
    // hope it handles locking internally
    mutable RunManager* fRunMgr;
};

GeantVProducer::GeantVProducer(edm::ParameterSet const& iConfig) :
	scoringParams(iConfig.getParameter<edm::ParameterSet>("Scoring")),
	gridParams(iConfig.getParameter<edm::ParameterSet>("Grid")),
    zFieldInTesla(iConfig.getParameter<double>("ZFieldInTesla")),
    m_InToken(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("HepMCProductLabel"))),
    n_threads(0),
    fConfig(std::make_unique<GeantConfig>()),
	fRunMgr(nullptr)
{
    //fill the config here
    fConfig->fRunMode = GeantConfig::kExternalLoop;
    fConfig->fUseV3 = true;
    fConfig->fSingleTrackMode      = iConfig.getParameter<bool>("singleTrackMode");
    fConfig->fNminReuse            = iConfig.getParameter<int>("NminReuse");
    fConfig->fNminThreshold        = iConfig.getParameter<int>("NminThreshold");
    fConfig->fNperBasket           = iConfig.getParameter<int>("NperBasket");
    fConfig->fUseVectorizedGeom    = iConfig.getParameter<bool>("UseVectorizedGeom");
    fConfig->fUseVectorizedPhysics = iConfig.getParameter<bool>("UseVectorizedPhysics");
    fConfig->fNvecPHY              = iConfig.getParameter<int>("NvecPHY");
    fConfig->fUseVectorizedMSC     = iConfig.getParameter<bool>("UseVectorizedMSC");
    fConfig->fNvecMSC              = iConfig.getParameter<int>("NvecMSC");
    fConfig->fUseVectorizedField   = iConfig.getParameter<bool>("UseVectorizedField");
    fConfig->fNvecFLD              = iConfig.getParameter<int>("NvecFLD");
    fConfig->fUseRungeKutta        = iConfig.getParameter<bool>("UseRungeKutta");
    fConfig->fEpsilonRK            = iConfig.getParameter<double>("EpsilonRK");
    fConfig->fGeomFileName         = iConfig.getParameter<std::string>("geometry");

	//make a temporary instance of SD class to get the product list
	//products will actually come from other instances of SD class downstream
	CaloSteppingAction sd(scoringParams);
	sd.registerProducts(*this);
}

GeantVProducer::~GeantVProducer() {
    fConfig.release();
    // this will delete fConfig also
    delete fRunMgr;
}

std::unique_ptr<EventCache> GeantVProducer::beginStream(edm::StreamID) const {
	return std::make_unique<EventCache>();
}

void GeantVProducer::preallocate(edm::PreallocationConfiguration const& iPrealloc) {
    n_threads = iPrealloc.numberOfThreads();
}

std::shared_ptr<int> GeantVProducer::globalBeginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) const {
    if(fConfig->fGeomFileName.empty()){
        // obtain the geometry
        edm::ESTransientHandle<TGeoManager> geoh;
        iSetup.get<DisplayGeomRecord>().get(geoh);
        // this fills gGeoManager used by Geant classes
        (void) geoh.product();
        std::stringstream message;
        message << " produce(): gGeoManager = " << gGeoManager;
        if(gGeoManager) message << ", " << gGeoManager->GetName() << ", " << gGeoManager->GetTitle();
        edm::LogInfo("GeantVProducer") << message.str();
    }

    const MagneticField* magfield = nullptr;
    if(zFieldInTesla < 0){
        edm::ESHandle<MagneticField> pMF;
        iSetup.get<IdealMagneticFieldRecord>().get(pMF);
        magfield = pMF.product();
    }

    // initialize manager
    // avoid CMSSW exception from kWarning issued by Geant::RunManager
    edm::Service<edm::RootHandlers> rootHandler;
    rootHandler->ignoreWarningsWhileDoing(
        [this,magfield] { this->initialize(magfield); },
        edm::RootHandlers::SeverityLevel::kError
    );

    //dummy return
    return std::shared_ptr<int>();
}

//invoke user FinishRun method (must be done manually in external loop mode)
void GeantVProducer::globalEndRun(edm::Run const&, edm::EventSetup const&) const {
	auto cmsApp = static_cast<CMSApplication*>(fRunMgr->GetUserApplication());
	cmsApp->FinishRun();
}

void GeantVProducer::initialize(const MagneticField* magfield) const {
    //fill in remaining config settings that need post-constructor info
    fConfig->fNtotal = 9999; //need to get nevents from somewhere
    fConfig->fNbuff = n_threads;
    fConfig->fNminThreshold *= n_threads;

     // Create run manager
    int n_propagators = 1;
    edm::LogInfo("GeantVProducer") <<"*** RunManager: instantiating with "<< n_propagators <<" propagators and "<< n_threads <<" threads.";
    fRunMgr = new RunManager(n_propagators, n_threads, fConfig.get());

    // create the real physics main manager/interface object and set it in the RunManager
    edm::LogInfo("GeantVProducer") <<"*** RunManager: setting physics process...";
    fRunMgr->SetPhysicsInterface(new geantphysics::PhysicsProcessHandler(*fRunMgr->GetConfig()));

    // Create user defined physics list for the full CMS application
    geantphysics::PhysicsListManager::Instance().RegisterPhysicsList(new cmsapp::CMSPhysicsListX(*fRunMgr->GetConfig()));

    // Detector construction
    auto detector_construction = new CMSDetectorConstruction(fRunMgr);
    detector_construction->SetGDMLFile(fRunMgr->GetConfig()->fGeomFileName);
    fRunMgr->SetDetectorConstruction( detector_construction );

    // field map option
    if(magfield){
        auto field_construction = new CMSFieldConstruction(magfield,gridParams);
        fRunMgr->SetUserFieldConstruction(field_construction);
    }
    // use a constant field
    else {
        float fieldVec[3] = {0.0,0.0,float(zFieldInTesla)};
        auto field_construction = new geant::UserFieldConstruction();
        field_construction->UseConstantMagField(fieldVec,"tesla");
        fRunMgr->SetUserFieldConstruction(field_construction);
    }

    CMSApplication* cmsApp = new CMSApplication(fRunMgr, scoringParams);
    edm::LogInfo("GeantVProducer") <<"*** RunManager: setting up CMSApplication...";
    fRunMgr->SetUserApplication( cmsApp );

    // Start simulation for all propagators
    edm::LogInfo("GeantVProducer") <<"*** RunManager: initializing...";
    fRunMgr->Initialize();

    edm::LogInfo("GeantVProducer") << "= GeantV initialized using maximum " << fRunMgr->GetNthreads() << " worker threads";
}

void GeantVProducer::acquire(edm::StreamID iStream, edm::Event const& iEvent, edm::EventSetup const& iSetup, edm::WaitingTaskWithArenaHolder iHolder) const
{
    edm::Handle<edm::HepMCProduct> HepMCEvt;
    iEvent.getByToken(m_InToken, HepMCEvt);

    edm::LogInfo("GeantVProducer") << " acquire(): *** Run GeantV simulation task ***";

    const HepMC::GenEvent * evt = HepMCEvt->GetEvent();
    long long event_index = iEvent.eventAuxiliary().event();

    // First book a transport task from GeantV run manager
    TaskData *td = fRunMgr->BookTransportTask();
    edm::LogInfo("GeantVProducer") <<" acquire(): td= "<< td <<", EventID="<<event_index;
    if (!td) {
        std::exception_ptr exceptionPtr;
        iHolder.doneWaiting(exceptionPtr);
        return;
    }

    // ... then create the event set
	auto cache = streamCache(iStream);
    auto evset = GenerateEventSet(evt, event_index, iHolder, cache, td);

    edm::Service<edm::RootHandlers> rootHandler;
    auto rootHandlerPtr = &(*rootHandler);

    // spawn a separate task: non-blocking!
    auto task = make_functor_task(
        tbb::task::allocate_root(),
        [this,evset=std::move(evset),td,rootHandlerPtr]() mutable {
            rootHandlerPtr->ignoreWarningsWhileDoing([this,evset=std::move(evset),td]() mutable {
                // ... finally invoke the GeantV transport task
                bool transported = this->fRunMgr->RunSimulationTask(evset.release(), td);

                // Now we could run some post-transport task
                edm::LogInfo("GeantVProducer")<<" RunTransportTask: task "<< td->fTid <<" : transported="<< transported;
            }, edm::RootHandlers::SeverityLevel::kError);
        }
    );
    tbb::task::spawn(*task);
}

void GeantVProducer::produce(edm::StreamID iStream, edm::Event& iEvent, edm::EventSetup const& iSetup) const
{
    edm::LogInfo("GeantVProducer") <<" at "<< this <<": adding to event...";

	//get the event data back from GeantV
	//the "final" DataPerEvent object after merging puts the products into the event
	auto cache = streamCache(iStream);
	cache->sd_->produce(iEvent,iSetup);
    //don't hang on to memory
	cache->clear();

    edm::LogInfo("GeantVProducer") <<" at "<< this <<": done!";
}

// eventually this can become more like SimG4Core/Generators/interface/Generator.h
std::unique_ptr<geant::EventSet> GeantVProducer::GenerateEventSet(const HepMC::GenEvent * evt, long long event_index, edm::WaitingTaskWithArenaHolder iHolder, EventCache* cache, geant::TaskData *td) const
{
    using EventSet = geant::EventSet;
    using Event = geant::Event;
    using Track = geant::Track;

    auto evset = std::make_unique<EventSet>(1);
    // keep track of the callback
    auto event = std::make_unique<CMSEvent>(iHolder,cache);

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
        track.SetGVcode(pParticle->GetInternalCode());
        track.SetCharge(pParticle->GetPDGCharge());
        track.SetMass(pParticle->GetPDGMass());
        track.SetDirection(mom.px(),mom.py(),mom.pz());
        track.SetPosition(pos.x(),pos.py(),pos.pz());
        track.SetE(mom.e());
        track.SetP(mom.perp());
        
        ++counter;
    }    

    evset->AddEvent(event.release());
    return evset;
}

DEFINE_FWK_MODULE(GeantVProducer);
