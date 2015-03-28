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
#include "CalibCalorimetry/EcalTiming/plugins/EcalTimingCalibFromSplash.h"
//
// class declaration
//
//
#define BarrelLimit 1.479
#define EndcapLimit 3.0
#define ADCtoGeVEB 0.039
#define ADCtoGeVEE 0.063
#define lightSpeed 299792458


EcalTimingCalibFromSplash::EcalTimingCalibFromSplash(const edm::ParameterSet& iConfig):
     //Set Configuration on initialization as needed
      EBRecHitCollection_  (iConfig.getParameter<edm::InputTag>("ebrechitcollection")),
      EERecHitCollection_  ( iConfig.getParameter<edm::InputTag>("eerechitcollection")),
      HBHERecHitCollection_ ( iConfig.getParameter<edm::InputTag>("hbherechitcollection")),
      UncalibEBRecHitCollection_  ( iConfig.getParameter<edm::InputTag>("barrelEcalUncalibratedRecHitCollection")),
      UncalibEERecHitCollection_ ( iConfig.getParameter<edm::InputTag>("endcapEcalUncalibratedRecHitCollection")),
      digiProducer_ ( iConfig.getParameter<std::string>("digiProducer")), 

      minEtCutEB_  ( iConfig.getUntrackedParameter<double>("minEtEB")),
      minEtCutEE_  ( iConfig.getUntrackedParameter<double>("minEtEE")),
      HBEtCut_  (iConfig.getUntrackedParameter<double>("hbTreshold")),
      runNumber_ (iConfig.getUntrackedParameter<int>("runNum")),
      IsSplash_ (iConfig.getParameter<bool>("IsSplash")),

      EnergyCutTot_  ( iConfig.getUntrackedParameter<double>("energycuttot")),
      EnergyCutEcal_ (  iConfig.getUntrackedParameter<double>("energycutecal")),
      EnergyCutHcal_ ( iConfig.getUntrackedParameter<double>("energycuthcal"))
{

      
      
      
//hbheToken_   =   consumes<HBHERecHitCollection>(iConfig.getParameter<edm::InputTag>("hbheInput")),
edm::Service<TFileService> fileService_;
/*
 * TimingHistsEB ebhists;
TimingHistsEEM eeMhists;
TimingHistsEEP eePhists;
*/
// Initialise Histograms
 /*
 initEBHists(ebhists,fileService_);
 initEEMHists(eeMhists,fileService_);
 initEEPHists(eePhists,fileService_);
*/
 initEBHists(fileService_);
 initEEMHists(fileService_);
 initEEPHists(fileService_);
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
     EventId = iEvent.id().event() ;
     LumiSection = iEvent.id().luminosityBlock();
     Orbit = iEvent.orbitNumber();
     EventRunId = iEvent.id().run() ;

    // Now get Collection infos

      //Extracting EcalRawDataCollection 
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
  
  /*
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
    */
    // Get Good RecHits

    bool accepted = false;
    bool acceptedtot = false;
    bool acceptedEcal = false;
    bool acceptedHcal = false;
    double totene=0;
    double ecalene=0;
    double hcalene=0;
    
    Handle< EBRecHitCollection > pEBRecHits;
    Handle< EERecHitCollection > pEERecHits;
    Handle< HBHERecHitCollection > pHBHERecHits;
    
    const EBRecHitCollection* EBRecHits = 0;
    const EERecHitCollection* EERecHits = 0;
    const HBHERecHitCollection* HBHERecHits = 0;
    // EB Rechits
    if ( EBRecHitCollection_.label() != "" && EBRecHitCollection_.instance() != "" )
    {
        iEvent.getByLabel( EBRecHitCollection_, pEBRecHits);
       if ( pEBRecHits.isValid() )
          {
           EBRecHits = pEBRecHits.product(); // get a ptr to the product
          }
    else
    {
       edm::LogError("EcalRecHitError") << "Error! can't get the product " << EBRecHitCollection_.label() ;
    }
    }
    //EE Rechits
    if ( EERecHitCollection_.label() != "" && EERecHitCollection_.instance() != "" )
    {
    iEvent.getByLabel( EERecHitCollection_, pEERecHits);
    if ( pEERecHits.isValid() )
    {
    EERecHits = pEERecHits.product(); // get a ptr to the product
    }
    else
    {
    edm::LogError("EcalRecHitError") << "Error! can't get the product " << EERecHitCollection_.label() ;
    }
    }
    //HBHE Rechits
    if (HBHERecHitCollection_.label() != "" )
     {
      iEvent.getByLabel( HBHERecHitCollection_, pHBHERecHits);
    if ( pHBHERecHits.isValid() )
        {
          HBHERecHits = pHBHERecHits.product(); // get a ptr to the product
       }
    else
    {
    edm::LogError("HcalRecHitError") << "Error! can't get the product " << HBHERecHitCollection_.label() ;
    }
   
    }

  // Now loop Over RecHits and Get Energetic Beamsplash events;
  if (EBRecHits)
    {
     for(EBRecHitCollection::const_iterator it = EBRecHits->begin(); it != EBRecHits->end(); ++it)
       {
         totene+=it->energy();
         ecalene+=it->energy();
       }
    }
  if (EERecHits)
    {
     for(EERecHitCollection::const_iterator it = EERecHits->begin(); it != EERecHits->end(); ++it)
       {
         totene+=it->energy();
         ecalene+=it->energy();
       }
    }
  if (HBHERecHits)
    {
     for(HBHERecHitCollection::const_iterator it = HBHERecHits->begin(); it != HBHERecHits->end(); ++it)
       {
         totene+=it->energy();
         hcalene+=it->energy();
       }
    }
  
   if(totene>EnergyCutTot_) acceptedtot=true;
   if(ecalene>EnergyCutEcal_) acceptedEcal=true;
   if(hcalene>EnergyCutHcal_) acceptedHcal=true;
   accepted = acceptedtot|acceptedEcal|acceptedHcal;
  
  // Now use Accepted Splash Events!
  if (accepted) 
  {
  edm::LogVerbatim("EcalTimingCalibFromSplash") << "!!!!!!!BeamSplash!!!!!!!: run:" << EventRunId << " event:" << EventId << " LumiSec:"<< LumiSection << " bx= " << BX  <<" totene=" << totene << " ecalene=" << ecalene << " hcalene=" << hcalene ;
  std::cout << "!!!!!!!BeamSplash!!!!!!!: run:" << EventRunId << " event:" << EventId << " ls:"<< LumiSection << " bx= " << BX <<" totene=" <<totene << " ecalene=" << ecalene << " hcalene=" << hcalene << std::endl;
  
    
  // Perform Analysis on only Valid Splash Events
  
    // Extract Splash Timing
 for(EBRecHitCollection::const_iterator thisit = EBRecHits->begin(); thisit != EBRecHits->end(); ++thisit)
    {

    EcalRecHit myhit = (*thisit);
	     
   // skip if not good
   if( !( myhit.checkFlag(EcalRecHit::kGood) ||  myhit.checkFlag(EcalRecHit::kOutOfTime) || myhit.checkFlag(EcalRecHit::kPoorCalib)) ) continue;
   // Fill Histograms Unclorrected Timing
   FillRecHitEB(myhit); 
   // Correct Splash Timing EB
   //double SplashTravTimeEB = SlashTimeCorr(pgeometryEB, DId);
 }
      
    // Correct Splash Timing EB
    // double SplashTravTimeEE = SlashTimeCorr(pgeometryEE, DId);

    if(IsSplash_){ /*apply Time of Flight correction*/ }

   // Fill Histograms with Corrected Timing
  } 
  
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

//Fill Histograms With Timing//
//EB
void EcalTimingCalibFromSplash::FillRecHitEB(EcalRecHit myhit)
{
   //const EcalIntercalibConstantMap& icalMap = ical->getMap();
   //float adcToGeV = float(agc->getEBValue());
   // thisamp is the EB amplitude of the current rechit
   // double thisamp = myhit.energy () ;
    double thistime = myhit.time ();
   // double thisChi2 = myhit.chi2 ();
    //double thisOutOfTimeChi2 = myhit.outOfTimeChi2 ();
   /*
    double thisOutOfTimeChi2 = myhit.chi2 ();
    int  numXtalsinCluster; 
    double secondMin = 0. ;
    double secondTime = -1000.;
    double thistimeErr = 0;
  
    EEDetId maxDet ;
    EEDetId secDet ;
 
   EcalIntercalibConstantMap::const_iterator icalit = icalMap.find(detitr->first);
   EcalIntercalibConstant icalconst = 1;
   if( icalit!=icalMap.end() ) {
   icalconst = (*icalit);
   } else {
   edm::LogError("EcaltimingCalibFromSplash") << "No intercalib const found for xtal "
   << (detitr->first).rawId();
   }
   // get laser coefficient
      float lasercalib = 1.;
      lasercalib = laser->getLaserCorrection( detitr->first, iEvent.time());
    
    // discard rechits with A/sigma < 12
    if ( thisamp/(icalconst*lasercalib*adcToGeVEB) < (1.1*12) ) continue;
    if (thisamp > 0.027) //cut on energy->number of crystals in cluster above 3sigma noise; gf: desirable?
        { numXtalsinCluster++ ;  }
    if (thisamp > secondMin)
        {
         secondMin = thisamp ;
         secondTime = myhit.time () ;
         secDet = (EBDetId) (detitr -> first) ;
        }
    if (secondMin > ampli)
        {
         std::swap (ampli, secondMin) ;
         std::swap (time, secondTime) ;
         std::swap (maxDet, secDet) ;
        }
     */
    //if(myhit.isTimeErrorValid()) thistimeErr =  myhit.timeError();
    // else thistimeErr = -9999999;

     //Fill Histograms:
    calibHistEB_ ->Fill(thistime);
}

//EEM
void EcalTimingCalibFromSplash::FillRecHitEEM(EcalRecHit myhit)
{

}
//EEP
void EcalTimingCalibFromSplash::FillRecHitEEP(EcalRecHit myhit)
{
}

//TimeOfFlight Corrector//
double EcalTimingCalibFromSplash::SplashTimeCorr(const CaloSubdetectorGeometry *geometry_p, DetId id) {

 return 0;
}

// Convert Int to String
std::string EcalTimingCalibFromSplash::intToString(int num)
{
  using namespace std;
  ostringstream myStream;
  myStream << num << flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

// Initilise Histograms//
void EcalTimingCalibFromSplash::initEBHists( edm::Service<TFileService>& fileService_)
{
 calibHistEB_ = fileService_->make<TH1F>("timingCalibDiffEB","timingCalib diff EB [ns]",400,-10,10);
 calibHistEB_->Sumw2();


}
void EcalTimingCalibFromSplash::initEEMHists( edm::Service<TFileService>& fileService_)
{
 calibHistEEM_ = fileService_->make<TH1F>("timingCalibDiffsEEM","timingCalibDiffs EEM [ns]",400,-10,10);
 calibHistEEM_->Sumw2();
}

void EcalTimingCalibFromSplash::initEEPHists( edm::Service<TFileService>& fileService_)
{

 calibHistEEP_ = fileService_->make<TH1F>("timingCalibDiffsEEP","timingCalibDiffs EEP [ns]",400,-10,10);
 calibHistEEP_->Sumw2();
}




/*
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
*/
//define this as a plug-in
DEFINE_FWK_MODULE(EcalTimingCalibFromSplash);
