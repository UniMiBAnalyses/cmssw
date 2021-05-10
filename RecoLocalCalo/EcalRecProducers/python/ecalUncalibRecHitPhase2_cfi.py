import FWCore.ParameterSet.Config as cms

ecalUncalibRecHitPhase2 = cms.EDProducer('EcalUncalibRecHitPhase2AnalyticFitProducer',
    EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
    tRise = cms.double(0.2),
    tFall = cms.double(2.),
    BarrelDigis = cms.InputTag("simEcalUnsuppressedDigis","")#,"DIGI")
)


### Ph1
#   ebDigiCollectionToken_ = consumes<EBDigiCollection>(ps.getParameter<edm::InputTag>("EBdigiCollection"));
#   EBdigiCollection = cms.InputTag("ecalDigis","ebDigis"),
#   EBDigiCollection                      "ecalDigis"                 "ebDigis"         "PulseTree"   


### Ph2
#   ebDigiCollectionToken_ = consumes<EBDigiCollectionPh2>(ps.getParameter<edm::InputTag>("BarrelDigis"));
#   BarrelDigis = cms.InputTag("simEcalUnsuppressedDigis","","DIGI")
#   EBDigiCollectionPh2                   "simEcalUnsuppressedDigis"   ""                "DIGI"
