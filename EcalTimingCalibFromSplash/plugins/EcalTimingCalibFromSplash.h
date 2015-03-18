
#ifndef EcalTimingCalibFromSplash_H
#define EcalTimingCalibFromSplash_H


// -*- C++ -*-
//
// Package:    CalibCalorimetry/EcalTimingCalibFromSplash
// Class:      EcalTimingCalibFromSplash
// 
/**\class EcalTimingCalibFromSplash EcalTimingCalibFromSplash.cc CalibCalorimetry/EcalTiming/plugins/EcalTimingCalibFromSplash.cc

 Description: [one line class summary]

 Timing Calibration using Proton beams splash dataset
 Implementation:
     [Notes on implementation]
*/
//
// Original Authors: Tambe Ebai Norbert & Peter Hansen
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
// Make Histograms the way!!
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
// Get Data formats
#include "DataFormats/Common/interface/OwnVector.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalDetId/interface/EcalTrigTowerDetId.h"

// Digi and Rechits
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/DetId/interface/DetId.h"
//Geometrty Mapping
#include "Geometry/EcalMapping/interface/EcalElectronicsMapping.h"
#include "Geometry/EcalMapping/interface/EcalMappingRcd.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/EcalAlgo/interface/EcalBarrelGeometry.h"
#include "RecoCaloTools/Navigation/interface/CaloNavigator.h"
#include "RecoLocalCalo/EcalRecAlgos/interface/EcalSeverityLevelAlgo.h"

// L1 Trigger
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetupFwd.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GtPsbWord.h"

// Calibration services
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
#include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalTools.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

#include "CondFormats/EcalObjects/interface/EcalTimeCalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalTimeCalibErrors.h"
#include "CondFormats/EcalObjects/interface/EcalTimeOffsetConstant.h"
//#include "DQM/EcalCommon/interface/Numbers.h"
#include "CalibCalorimetry/EcalTiming/interface/Numbers.h"


// For ECAL CLustering
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/BasicClusterFwd.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterTools.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"


// Get some C/Root Goodies
#include <string>
#include <vector>
#include <algorithm>
#include <assert>
#include <TMath.h>
#include <Math/VectorUtil.h>

#include "TProfile.h"
#include "TProfile2D.h"

#include "TGraphErrors.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH3F.h"
#include "TTree.h"

// Might be useful
//#include<fstream>
//#include<map>
//#include<stl_pair>
//
// class declaration
//

using namespace  std;

class EcalTimingCalibFromSplash : public edm::EDAnalyzer {
   public:
      explicit EcalTimingCalibFromSplash(const edm::ParameterSet&);
      ~EcalTimingCalibFromSplash();


//
// member functions
//
      double SplashTimeCorr(const CaloSubdetectorGeometry *geometry_p, DetId id);
      double myTheta(const CaloSubdetectorGeometry *geometry_p, DetId id);
      // Trigger Selection
      bool L1TriggerSelection( const edm::Event& iEvent, const edm::EventSetup& iSetup ) ;
      void TriggerTagging( edm::Handle<edm::TriggerResults> triggers, const edm::TriggerNames& trgNameList, int RunID, vector<int>& firedTrig ) ;
      bool TriggerSelection( edm::Handle<edm::TriggerResults> triggers, vector<int> firedTrig ) ;
     // static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      // Input tags
     edm::InputTag trigSource;
     edm::InputTag trigEvent;
     edm::ESHandle<EcalIntercalibConstants> ical;
     edm::ESHandle<EcalADCToGeVConstant> agc;
     edm::ESHandle<EcalLaserDbService> laser;
     //edm::ESHandle<CaloGeometry> pGeometryEB ;
     //edm::ESHandle<CaloGeometry> pGeometryEE ;
     const CaloGeometry * theGeometry ;

     bool L1TrigEvent;
     bool HLTTrigEvent;
     int runID_;
     std::vector<int> firedTrig ;
     int targetTrig ;
     int EventId;
     int BX;
     int LumiSection;
     int Orbit;
     int EventRunId;
     float EventTime;

   
     
     std::string rootfile_;
     std::string hitCollection_;
     std::string hitCollectionEE_;
     std::string hitProducer_;
     std::string hitProducerEE_;
     std::string rhitCollection_;
     std::string rhitCollectionEE_;
     std::string rhitProducer_;
     std::string rhitProducerEE_;
     std::string digiProducer_;
     std::string gtRecordCollectionTag_;
     float ampl_thr_;
     float ampl_thrEE_;
     double mintime_;
     double maxtime_;
     int min_num_ev_;
     int max_num_ev_;
     int sm_;
     std::string txtFileName_;
     std::string txtFileForChGroups_;
     //std::string pndiodeProducer_;
     std::vector<double> sMAves_;
     std::vector<double> sMCorr_;
      
     TProfile* amplProfileConv_[54][4];
     TProfile* absoluteTimingConv_[54][4];

     TProfile* amplProfileAll_[54][4];
      TProfile* absoluteTimingAll_[54][4];
      
      TProfile* Chi2ProfileConv_[54][4];
      TH1F* timeCry[54][4];
      
      TProfile* relativeTimingBlueConv_[54];

      TGraphErrors* ttTiming_[54];
      TGraphErrors* ttTimingAll_;
      TGraphErrors* ttTimingRel_[54];
      TGraphErrors* ttTimingAllRel_;
      TGraphErrors* ttTimingAllSMChng_;
      
      TGraph* lasershiftVsTime_[54];
      TH2F* lasershiftVsTimehist_[54];
      TH1F* lasershiftLM_[54];
      TH1F* lasershift_;
      
      TProfile2D* ttTimingEtaPhi_;
      TProfile2D* chTimingEtaPhi_;
	    
      TProfile* ttTimingEta_;
      TProfile* chTimingEta_;
	  
      TProfile* ttTimingEtaEEP_;
	  
      TProfile* ttTimingEtaEEM_;
	  
      TProfile2D* chTimingEtaPhiEEP_;
      TProfile2D* chTimingEtaPhiEEM_;
      
      TProfile2D* ttTimingEtaPhiEEP_;
      TProfile2D* ttTimingEtaPhiEEM_;
      
      TH1F* timeCry1[54]; 
      TH1F* timeCry2[54]; 
      TH1F* timeRelCry1[54]; 
      TH1F* timeRelCry2[54]; 
      
      TH1F* aveRelXtalTime_;
      TH1F* aveRelXtalTimebyDCC_[54];
      TH2F* aveRelXtalTimeVsAbsTime_;
      
      TProfile2D* fullAmpProfileEB_;
      TProfile2D* fullAmpProfileEEP_;
      TProfile2D* fullAmpProfileEEM_;
      
      double timerunstart_;
      double timerunlength_;
	  
      TH1F* lasersPerEvt;

      const EcalElectronicsMapping* ecalElectronicsMap_;
 
      int ievt_;
      int numGoodEvtsPerSM_[54];
	  
      static const int numXtals = 15480;
  
      //Allows for running the job on a file
      bool fromfile_;
      std::string fromfilename_;   
	  
	  //Correct for Timing 
      bool corrtimeEcal;
      bool corrtimeBH;
      bool bhplus_;
      double EBradius_;
      bool splash09cor_;
      TTree* eventTimingInfoTree_;
	  
      struct TTreeMembers {
	    int numEBcrys_;
	    int numEEcrys_;
	    int cryHashesEB_[61200];
	    int cryHashesEE_[14648];
	    float cryTimesEB_[61200];
	    float cryTimesEE_[14648];
	    float cryUTimesEB_[61200];
	    float cryUTimesEE_[14648];
	    float cryTimeErrorsEB_[61200];
	    float cryTimeErrorsEE_[14648];
	    float cryAmpsEB_[61200];
	    float cryAmpsEE_[14648];
	    float cryETEB_[61200];
	    float cryETEE_[14648];
            float kswisskEB_[61200];
	    int numTriggers_;
	    int numTechTriggers_;
	    int triggers_[200];
	    int techtriggers_[200];
	    float absTime_;
	    int lumiSection_;
	    int bx_;
	    int orbit_;
	    int run_;
	    float correctionToSample5EB_;
	    float correctionToSample5EEP_;
	    float correctionToSample5EEM_;
      } TTreeMembers_; 
      double allave_;
      double allshift_;
      double timeerrthr_; 
      int minxtals_;
      bool writetxtfiles_;
      bool timingTree_;	  
      bool correctAVE_;
};

#endif
