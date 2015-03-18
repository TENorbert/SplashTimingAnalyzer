// -*- C++ -*-
//
// Package:    DemoAnalyzer/EcalTimingCalibFromSplash
// Class:      EcalTimingCalibFromSplash
// 
/**\class EcalTimingCalibFromSplash EcalTimingCalibFromSplash.cc DemoAnalyzer/EcalTimingCalibFromSplash/plugins/EcalTimingCalibFromSplash.cc

 Description: [one line class summary]

 Timing Calibration using proton Beam Splash Triggered events
 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Tambe Ebai Norbert & Peter Hansen
//         Created:  Wed, 18 Mar 2015 18:12:02 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//
// class declaration
//

class EcalTimingCalibFromSplash : public edm::EDAnalyzer {
   public:
      explicit EcalTimingCalibFromSplash(const edm::ParameterSet&);
      ~EcalTimingCalibFromSplash();

      double SplashTimeCorr(const CaloSubdetectorGeometry *geometry_p, DetId id);
      double myTheta(const CaloSubdetectorGeometry *geometry_p, DetId id);
        //Trigger Selection
      bool L1TriggerSelection( const edm::Event& iEvent, const edm::EventSetup& iSetup ) ;
      void TriggerTagging( edm::Handle<edm::TriggerResults> triggers, const edm::TriggerNames& trgNameList, int RunID, vector<int>& firedTrig ) ;
      bool TriggerSelection( edm::Handle<edm::TriggerResults> triggers, vector<int> firedTrig ) ;
      
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
EcalTimingCalibFromSplash::EcalTimingCalibFromSplash(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

}


EcalTimingCalibFromSplash::~EcalTimingCalibFromSplash()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
EcalTimingCalibFromSplash::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   using namespace cms;
    // get calibration service
      // IC's
    iSetup.get<EcalIntercalibConstantsRcd>().get(ical);
    // ADCtoGeV
    iSetup.get<EcalADCToGeVConstantRcd>().get(agc);
    // transp corrections
    iSetup.get<EcalLaserDbRecord>().get(laser);
   
 
     //Get Event infos 
     EventTime = iEvent.time() ;
     BX = iEvent.bunchCrossing();
     LumiSection = iEvent.id().luminosityBlock();
     Orbit = iEvent.orbitNumber();
     EventRunId = iEvent.id().run() ;
     EventId = iEvent.id().event() ;

    // Now get Collection infos
#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
      //Extracting Raw Data
    edm::Handle<EcalRawDataCollection> DCCHeaders;
    iEvent.getByLabel(digiProducer_, DCCHeaders);
    if (!DCCHeaders.isValid()) {
    edm::LogError("EcalTiminCalibFromSplash") << "can't get the product for EcalRawDataCollection";
    }
   
     //Geometry information
    edm::ESHandle<CaloGeometry> pGeometry;
    iSetup.get<CaloGeometryRecord> ().get (pGeometry) ;
    theGeometry = pGeometry.product() ;
    const CaloSubdetectorGeometry *pgeometryEB = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalBarrel);
    const CaloSubdetectorGeometry *pgeometryEE = pGeometry->getSubdetectorGeometry(DetId::Ecal, EcalEndcap);
   
    // Extract Splash Timing
    // Fill Histograms Unclorrected Timing

    // Correct Splash Timing
    double SplashTravTimeEB = SlashTimeCorr(pgeometryEB, DId) 
    double SplashTravTimeEE = SlashTimeCorr(pgeometryEE, DId) 
    // Fill Histograms with Corrected Timing
  
   
    // Get triggered events
    // L1 Trigger Selection
    L1TrigEvent = L1TriggerSelection( iEvent, iSetup ) ;
    // HLT trigger analysis
    Handle<edm::TriggerResults> triggers;
    iEvent.getByLabel( trigSource, triggers );
    const edm::TriggerNames& trgNameList = iEvent.triggerNames( *triggers ) ;
    TriggerTagging( triggers, trgNameList, run_id, firedTrig ) ;
    HLTTrigEvent = TriggerSelection( triggers, firedTrig ) ;
    // Using L1 or HLT to select events ?!
    bool passTrigger = ( L1Select ) ? L1TrigEvent : HLTTrigEvent ;
   
   //Only Allowed Triggerd events
    if (!passTrigger) continue;



}


// ------------ method called once each job just before starting event loop  ------------
void 
EcalTimingCalibFromSplash::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
EcalTimingCalibFromSplash::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
EcalTimingCalibFromSplash::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
EcalTimingCalibFromSplash::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
EcalTimingCalibFromSplash::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
EcalTimingCalibFromSplash::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

double EcalTimingCalibFromSplash::SplashTimeCorr(const CaloSubdetectorGeometry *geometry_p, DetId id) {




}




bool EcalTimingCalibFromSplash::L1TriggerSelection( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
// Get L1 Trigger menu
 ESHandle<L1GtTriggerMenu> menuRcd;
 iSetup.get<L1GtTriggerMenuRcd>().get(menuRcd) ;
 const L1GtTriggerMenu* menu = menuRcd.product();
 // Get L1 Trigger record
 Handle< L1GlobalTriggerReadoutRecord > gtRecord;
 iEvent.getByLabel( edm::InputTag("gtDigis"), gtRecord);
 // Get dWord after masking disabled bits
 const DecisionWord dWord = gtRecord->decisionWord();
 bool l1_accepted = menu->gtAlgorithmResult( l1GTSource , dWord);
// //int passL1 = ( l1SingleEG22 ) ? 1 : 0 ;
// //cout<<" pass L1 EG22 ? "<< passL1 <<endl ;
 if ( l1_accepted ) leaves.L1a = 1 ;
 return l1_accepted ;
 }

void EcalTimingCalibFromSplash::TriggerTagging( Handle<edm::TriggerResults> triggers, const edm::TriggerNames& trgNameList, int RunId, vector<int>& firedTrig ) {
 if ( runID_ != RunId ) {
    for (size_t j=0; j< triggerPatent.size(); j++ ) firedTrig[j] = -1;
         // loop through trigger menu
    for ( size_t i =0 ; i < trgNameList.size(); i++ ) {
         string tName = trgNameList.triggerName( i );
          // loop through desired triggers
          for ( size_t j=0; j< triggerPatent.size(); j++ ) {
               if ( strncmp( tName.c_str(), triggerPatent[j].c_str(), triggerPatent[j].size() ) ==0 ) {
                    firedTrig[j] = i;
                     //cout<<" Trigger Found ("<<j <<"): "<<tName ;
                     //cout<<" Idx: "<< i <<" triggers "<<endl;
                   }
              }
        }
    runID_ = RunId ;
       }
 }

bool EcalTimingCalibFromSplash::L1TriggerSelection( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
     // Get L1 Trigger menu
   ESHandle<L1GtTriggerMenu> menuRcd;
   iSetup.get<L1GtTriggerMenuRcd>().get(menuRcd) ;
   const L1GtTriggerMenu* menu = menuRcd.product();
    // Get L1 Trigger record
   Handle< L1GlobalTriggerReadoutRecord > gtRecord;
   iEvent.getByLabel( edm::InputTag("gtDigis"), gtRecord);
   // Get dWord after masking disabled bits
   const DecisionWord dWord = gtRecord->decisionWord();
   bool l1_accepted = menu->gtAlgorithmResult( l1GTSource , dWord);
   //int passL1 = ( l1SingleEG22 ) ? 1 : 0 ;
   //cout<<" pass L1 EG22 ? "<< passL1 <<endl ;
   if ( l1_accepted ) leaves.L1a = 1 ;
   return l1_accepted ;
 }



// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
EcalTimingCalibFromSplash::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(EcalTimingCalibFromSplash);
