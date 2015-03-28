import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

## Run from RAW: 
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
## Get Cosmic Reconstruction
process.load('Configuration/StandardSequences/ReconstructionCosmics_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
## Raw to Digi
process.load('Configuration/StandardSequences/RawToDigi_Data_cff')
## HLT Filter Splash
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.spashesHltFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
throw = cms.bool(False),
HLTPaths = ['HLT_EG20*']
)
## Do you want to Pre-Scale
process.load("FWCore.Modules.preScaler_cfi")
process.preScaler.prescaleFactor = 1
## GlobalTag Conditions Related
##process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = 'GR_R_73_V3A::All'
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
## Process Digi To Raw Step
process.digiStep = cms.Sequence(process.ecalDigis + process.ecalPreshowerDigis)
## Perform Reco
#process.muonSequence = cms.Sequence(process.calolocalreco)
process.caloCosmics.remove(process.hbhereco)
process.caloCosmics.remove(process.hcalLocalRecoSequence)
process.caloCosmics.remove(process.hfreco)
process.caloCosmics.remove(process.horeco)
process.caloCosmics.remove(process.zdcreco)
process.caloCosmicOrSplashRECOSequence = cms.Sequence(process.caloCosmics )#+ process.egammaCosmics)

# Dump Some event Content
import FWCore.Modules.printContent_cfi
process.dumpEv = FWCore.Modules.printContent_cfi.printContent.clone()
### Print Out Some Messages
process.MessageLogger = cms.Service("MessageLogger",
cout = cms.untracked.PSet(
threshold = cms.untracked.string('WARNING')
),
categories = cms.untracked.vstring('ecalTimingCalibFromSplash'),
destinations = cms.untracked.vstring('cout')
)
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)
# OutPutFile
process.TFileService = cms.Service("TFileService",
fileName = cms.string("SplashEcalTimeCalibs.root"),
closeFileFast = cms.untracked.bool(True)
)
## Dumpevent Event Contents
process.dumpEvContent = cms.EDAnalyzer("EventContentAnalyzer")

# get RAW Input Files
process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(
     'file:Cosmic-Commissioning2014-Cosmics-RAW-v1-AC4963B3-54BE-E311-97F5-02163E00E6E3.root'
    # '/store/data/Commissioning2015/Cosmics/RAW/v1/000/232/881/00000/26ADAFFB-3FAB-E411-A313-02163E011DDC.root'
    ),
    # drop native rechits and clusters, to be sure only those locally made will be picked up
    inputCommands = cms.untracked.vstring('keep *'
    ,'drop EcalRecHitsSorted_*_*_RECO' # drop hfRecoEcalCandidate as remade in this process
    , 'drop recoSuperClusters_*_*_RECO' # drop hfRecoEcalCandidate as remade in this process
    , 'drop recoCaloClusters_*_*_RECO'
    )
)

## Events Analyzer Here!!
#process.SplashTiming = cms.EDAnalyzer('EcalTimingCalibFromSplash',
process.load("CalibCalorimetry.EcalTiming.ecalTimingCalibFromSplash_cfi")

process.SplashTiming.energycuttot = cms.untracked.double(1000.0) # Total Energy 
process.SplashTiming.energycutecal  = cms.untracked.double(700.0) ## Total Energy in ECAL
process.SplashTiming.energycuthcal  = cms.untracked.double(700.0) ## Tota; Energy in HCAL
process.SplashTiming.minEtEB = cms.untracked.double(2.0) ## 2GeV Transverse energy cut.
process.SplashTiming.minEtEE = cms.untracked.double(3.0)
process.SplashTiming.hbTreshold = cms.untracked.double(1.)
process.SplashTiming.IsSplash = cms.bool(True) #True= Splash Apply Correction, False=Cosmic/Other Do not apply corrections
process.SplashTiming.runNum = cms.untracked.int32(-1) 
#runNum = cms.int32(-1), 



### Process Full Path
process.p = cms.Path(process.spashesHltFilter
                     + process.preScaler
                     + process.digiStep
                     + process.caloCosmicOrSplashRECOSequence
                     + process.SplashTiming
                    )
### Schedule ###
process.schedule = cms.Schedule(process.p) 
