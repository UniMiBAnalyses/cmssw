#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "CondFormats/EcalObjects/interface/EcalCATIAGainRatios.h"
#include "CondFormats/DataRecord/interface/EcalCATIAGainRatiosRcd.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"

#include <TGraph.h>
#include <TF1.h>

class EcalUncalibRecHitPhase2WeightsProducer: public edm::stream::EDProducer<>{


public:
  explicit EcalUncalibRecHitPhase2WeightsProducer(const edm::ParameterSet& ps);//, edm::ConsumesCollector& c);
  void produce(edm::Event& evt, const edm::EventSetup& es) override;

private:
  
  float tRise_;
  float tFall_;  
  double gainRatios[2]={10,1};  
  float gratio;

  edm::EDGetTokenT<EBDigiCollectionPh2> ebDigiCollectionToken_;
  std::string hitCollection_;

};

EcalUncalibRecHitPhase2WeightsProducer::EcalUncalibRecHitPhase2WeightsProducer(const edm::ParameterSet& ps){//, edm::ConsumesCollector& c){
    
  hitCollection_ = ps.getParameter<std::string>("EBhitCollection");
  produces<EBUncalibratedRecHitCollection>(hitCollection_);
  
  tRise_ = ps.getParameter<double>("tRise");
  tFall_ = ps.getParameter<double>("tFall");
  
  ebDigiCollectionToken_ = consumes<EBDigiCollectionPh2>(ps.getParameter<edm::InputTag>("BarrelDigis"));
}

void EcalUncalibRecHitPhase2WeightsProducer::produce(edm::Event& evt, const edm::EventSetup& es){
  
  // retrieve digis
  edm::Handle<EBDigiCollectionPh2> pdigis_;
  
  evt.getByToken(ebDigiCollectionToken_, pdigis_);
  
  const EBDigiCollectionPh2* pdigis = nullptr;
  pdigis = pdigis_.product();
  
  // prepare output
  auto ebUncalibRechits = std::make_unique<EBUncalibratedRecHitCollection>();
  
  for (auto itdg = pdigis->begin(); itdg != pdigis->end(); ++itdg) {
    
    EBDataFrame digi(*itdg);
    EcalDataFrame_Ph2 dataFrame(*itdg);
    DetId detId(digi.id());
    
    bool g1 = false; 
    std::vector<float> timetrace;
    std::vector<float> adctrace;
    // std::vector<int> weights (16,1);
    std::vector<double> weights = {0.00403987,0.00538649, 0.00628424, 0.0300746,0.270671, 0.452465, 0.360895, 0.190772, 0.0758598, 0.0260347, 0.0121196, 0.00897749, 0.00763086, 0.00673312, 0.00718199, 0.00583537};
    
    int nSamples = digi.size();
    
    float amp = 0;

    //std::cout << " digi.size = " << nSamples << std::endl;
    
    for (int sample = 0; sample < nSamples; ++sample){
      
      EcalLiteDTUSample thisSample = dataFrame[sample];
      //gratio = 1;
      gratio = gainRatios[thisSample.gainId()];
      adctrace.push_back(thisSample.adc()*gratio);

      amp = amp + adctrace[sample]*weights[sample];
      
      if (thisSample.gainId()==1) g1=true;
      
      timetrace.push_back(sample);
      
    }// loop on samples
     
    // float amp  = adctrace[5]; // qua metto come test il massimo degli adctrace
    
    float amp_e= 1;
    float t0   = 0;
    float t0_e = 0;
    float ped  = 0;
    float chi2 = 0;
    uint32_t flags =0;
    
    EcalUncalibratedRecHit rhit(detId, amp, ped, t0, chi2, flags);
    rhit.setAmplitudeError(amp_e);
    rhit.setJitterError(t0_e);  
    if (g1) rhit.setFlagBit(EcalUncalibratedRecHit::kHasSwitchToGain1); 
    
    ebUncalibRechits->push_back(rhit);
    
  }// loop on digis    
  
  
  evt.put(std::move(ebUncalibRechits), hitCollection_);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(EcalUncalibRecHitPhase2WeightsProducer);