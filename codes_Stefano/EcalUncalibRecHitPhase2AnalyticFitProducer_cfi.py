import FWCore.ParameterSet.Config as cms

EcalUncalibRecHitPhase2AnalyticFitProducer = cms.EDProducer('EcalUncalibRecHitPhase2AnalyticFitProducer',
                                                            EBhitCollection = cms.string('EcalUncalibRecHitsEB'),
                                                            tRise = cms.double(0.2),
                                                            tFall = cms.double(2.)
)
