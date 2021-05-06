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

class EcalUncalibRecHitPhase2AnalyticFitProducer: public edm::stream::EDProducer<>{


public:
  explicit EcalUncalibRecHitPhase2AnalyticFitProducer(const edm::ParameterSet& ps);//, edm::ConsumesCollector& c);
  void produce(edm::Event& evt, const edm::EventSetup& es) override;

private:
  
  float tRise_;
  float tFall_;  
  //int    time;
  double gainRatios[2]={10,1};  
  float gratio;

  edm::EDGetTokenT<EBDigiCollectionPh2> ebDigiCollectionToken_;
  std::string hitCollection_;

  //edm::ESGetToken<EcalCATIAGainRatios, EcalCATIAGainRatiosRcd> gainsToken_;

};

EcalUncalibRecHitPhase2AnalyticFitProducer::EcalUncalibRecHitPhase2AnalyticFitProducer(const edm::ParameterSet& ps){//, edm::ConsumesCollector& c){
    
  hitCollection_ = ps.getParameter<std::string>("EBhitCollection");
  produces<EBUncalibratedRecHitCollection>(hitCollection_);
  
  tRise_ = ps.getParameter<double>("tRise");
  tFall_ = ps.getParameter<double>("tFall");
  
  ebDigiCollectionToken_ = consumes<EBDigiCollectionPh2>(ps.getParameter<edm::InputTag>("BarrelDigis"));
  
  //gainsToken_= c.esConsumes<EcalCATIAGainRatios, EcalCATIAGainRatiosRcd>();
}

void EcalUncalibRecHitPhase2AnalyticFitProducer::produce(edm::Event& evt, const edm::EventSetup& es){
    
  //edm::ESHandle<EcalCATIAGainRatios> gains=  es.getHandle(gainsToken_);
         
  
  // from https://arxiv.org/pdf/1912.07682

  TF1 fitfunc("fitfunc","2*[0]* 1/(exp(- (x-[1])/[2]) +1 )  *  1/(exp( (x-[1])/[3]) ) +[4] ",0,20);
  
  fitfunc.SetParameter(0,100);     // amplitude
  fitfunc.SetParameter(1,6);       // t0
  fitfunc.SetParameter(2,tRise_);  // tRise   
  fitfunc.SetParLimits(2,tRise_,tRise_);
  fitfunc.SetParameter(3,tFall_);  // tFall  
  fitfunc.SetParLimits(4,tFall_,tFall_);  
  fitfunc.SetParameter(4,10);      // pedestal
  
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
    
    int nSamples = digi.size();
    
    for (int sample = 0; sample < nSamples; ++sample){
      
      EcalLiteDTUSample thisSample = dataFrame[sample];
      gratio = gainRatios[thisSample.gainId()];
      adctrace.push_back(thisSample.adc()*gratio);
      
      if (thisSample.gainId()==1) g1=true;
      
      timetrace.push_back(sample);
      
    }// loop on samples
    
    TGraph g(adctrace.size(), &timetrace[0], &adctrace[0]);//cannot create TGraph from vectors
    g.Fit("fitfunc","Q"); // this makes some strange outputs
    
    float amp  = fitfunc.GetParameter(0);
    float amp_e= fitfunc.GetParError(0);
    float t0   = fitfunc.GetParameter(1);
    float t0_e = fitfunc.GetParError(1);
    
    float ped  = fitfunc.GetParameter(4);
    float chi2 = fitfunc.GetChisquare()/fitfunc.GetNDF();   // should retrieve reduced chi2  
    
    uint32_t flags =0; // should mark gain switch
    
    EcalUncalibratedRecHit rhit(detId, amp, ped, t0, chi2, flags);
    rhit.setAmplitudeError(amp_e);
    rhit.setJitterError(t0_e);  
    if (g1) rhit.setFlagBit(EcalUncalibratedRecHit::kHasSwitchToGain1); 
    
    ebUncalibRechits->push_back(rhit); // should also sort by energy ?   
    
  }// loop on digis    
  
  
  evt.put(std::move(ebUncalibRechits), hitCollection_);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(EcalUncalibRecHitPhase2AnalyticFitProducer);