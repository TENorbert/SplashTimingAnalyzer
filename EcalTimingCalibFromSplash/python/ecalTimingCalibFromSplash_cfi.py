import FWCore.ParameterSet.Config as cms

SplashTiming = cms.EDAnalyzer('EcalTimingCalibFromSplash',
# General Rechit
ebrechitcollection = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
eerechitcollection = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
hbherechitcollection = cms.InputTag("hbhereco"),
digiProducer = cms.string('ecalDigis'),
IsSplash = cms.bool(True), ## True= Splash Apply Correction, False= Cosmic Or something else, Don't apply corrections
## Uncalib Rechit
barrelEcalUncalibratedRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEB"),
endcapEcalUncalibratedRecHitCollection = cms.InputTag("ecalRatioUncalibRecHit","EcalUncalibRecHitsEE"),
# ThresholdCuts
minEtEB = cms.untracked.double(2.0),
minEtEE = cms.untracked.double(2.0),
hbTreshold = cms.untracked.double(1.),
runNum = cms.untracked.int32(-1),
energycuttot = cms.untracked.double(1000.0),
energycutecal = cms.untracked.double(700.0),
energycuthcal = cms.untracked.double(700.0),

)
