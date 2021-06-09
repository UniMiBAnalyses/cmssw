// -*- C++ -*-
//
// Package:    EcalValidation/EcalDigiDumperPh2
// Class:      EcalDigiDumperPh2
//
/**\class EcalDigiDumperPh2 EcalDigiDumperPh2.cc EcalValidation/EcalDigiDumperPh2/plugins/EcalDigiDumperPh2.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Bianca Sofia Pinolini
//         Created:  Tue, 08 Jun 2021 16:05:22 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

// ECAL specific
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

// #include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
// #include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"
#include "CondFormats/EcalObjects/interface/EcalADCToGeVConstant.h"
#include "CondFormats/DataRecord/interface/EcalADCToGeVConstantRcd.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstants.h"
#include "CondFormats/EcalObjects/interface/EcalIntercalibConstantsMC.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsRcd.h"
#include "CondFormats/DataRecord/interface/EcalIntercalibConstantsMCRcd.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "CondFormats/EcalObjects/interface/EcalMGPAGainRatio.h"
#include "CondFormats/EcalObjects/interface/EcalGainRatios.h"
#include "CondFormats/DataRecord/interface/EcalGainRatiosRcd.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondFormats/EcalObjects/interface/EcalCATIAGainRatios.h"
#include "CondFormats/DataRecord/interface/EcalCATIAGainRatiosRcd.h"

//---- EcalUncalibratedRecHit
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"

//---- pfrechit
#include "DataFormats/ParticleFlowReco/interface/PFRecHitFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"

//--- pfcluster
#include "DataFormats/ParticleFlowReco/interface/PFClusterFwd.h"
#include "DataFormats/ParticleFlowReco/interface/PFCluster.h"

//---- for TP
#include "CondFormats/EcalObjects/interface/EcalTPGLutIdMap.h"
#include "CondFormats/EcalObjects/interface/EcalTPGLutGroup.h"
#include "CondFormats/EcalObjects/interface/EcalTPGPhysicsConst.h"
#include "CondFormats/DataRecord/interface/EcalTPGLutIdMapRcd.h"
#include "CondFormats/DataRecord/interface/EcalTPGLutGroupRcd.h"
#include "CondFormats/DataRecord/interface/EcalTPGPhysicsConstRcd.h"
#include "Geometry/CaloTopology/interface/EcalTrigTowerConstituentsMap.h"

//---- for Laser Correction 
// #include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbService.h"
// #include "CalibCalorimetry/EcalLaserCorrection/interface/EcalLaserDbRecord.h"

#include "TTree.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class EcalDigiDumperPh2 : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit EcalDigiDumperPh2(const edm::ParameterSet&);
  ~EcalDigiDumperPh2();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  // ----------member data ---------------------------
  double gainRatios[2]={10,1};

  edm::EDGetTokenT<EcalUncalibratedRecHitCollection> _token_rechits;  
  edm::EDGetTokenT<EBDigiCollectionPh2> _token_digi;
    
  TTree *_outTree;
  
  float _amplitude[61200];
  int   _ieta[61200];
  int   _iphi[61200];
  float _adc[61200][16];
  float _gain[61200][16];
};

EcalDigiDumperPh2::EcalDigiDumperPh2(const edm::ParameterSet& iConfig) {
    
  //now do what ever initialization is needed
  usesResource("TFileService");
  edm::Service<TFileService> fs;
  
  _token_rechits = consumes<EcalUncalibratedRecHitCollection>(iConfig.getParameter<edm::InputTag>("EcalUncalibRecHitsEBCollection"));
  _token_digi = consumes<EBDigiCollectionPh2>(iConfig.getParameter<edm::InputTag>("EBDigiCollectionPh2"));
  
  _outTree = fs->make<TTree>("tree","tree");
  
  _outTree->Branch("amplitude",        _amplitude,        "amplitude[61200]/F");
  _outTree->Branch("ieta",                _ieta,                "ieta[61200]/I");
  _outTree->Branch("iphi",                _iphi,                "iphi[61200]/I");
  
  _outTree->Branch("adc",             _adc,                  "adc[61200][16]/I"); // CONTROLLA
  _outTree->Branch("gain",             _gain,                  "gain[61200][16]/I");
  

}

EcalDigiDumperPh2::~EcalDigiDumperPh2() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called for each event  ------------
void EcalDigiDumperPh2::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  //---- uncalibrated rechits
  edm::Handle<EcalUncalibratedRecHitCollection> rechithandle;
  const EcalUncalibratedRecHitCollection *rechits = NULL;
  
  iEvent.getByToken(_token_rechits,rechithandle);
  rechits = rechithandle.product();

  //---- digis
  edm::Handle<EBDigiCollectionPh2> digihandle;
  const EBDigiCollectionPh2 *digis = NULL;  
  iEvent.getByToken(_token_digi,digihandle);
  digis = digihandle.product();

  //---- setup default
  for (int ixtal=0; ixtal < 61200; ixtal++) {
    _amplitude[ixtal] = -999;
    _ieta[ixtal] = -999;
    _iphi[ixtal] = -999;

    for (int isample=0; isample < 16; isample++) {
      _adc[ixtal][isample]=-999;
      _gain[ixtal][isample]=-999;
    }
  }
  
  //---- filling adc and gain
  int index = 0;
  
  for (auto itdg = digis->begin(); itdg != digis->end(); ++itdg) {

    index = std::distance(digis->begin(), itdg);
    EcalDataFrame_Ph2 dataFrame(*itdg);

    for (int sample = 0; sample < 16; ++sample){
      EcalLiteDTUSample thisSample = dataFrame[sample];
      _adc[index][sample] = thisSample.adc();
      _gain[index][sample] = gainRatios[thisSample.gainId()];
    }
  }

  //---- filling amplitude and geometry
  for (EcalUncalibratedRecHitCollection::const_iterator itrechit = rechits->begin(); itrechit != rechits->end(); itrechit++ ) {
    _amplitude[EBDetId(itrechit->id()).hashedIndex()] =  itrechit->amplitude();
    _ieta[EBDetId(itrechit->id()).hashedIndex()] = EBDetId(itrechit->id()).ieta();
    _iphi[EBDetId(itrechit->id()).hashedIndex()] = EBDetId(itrechit->id()).iphi();
  }
  
  _outTree->Fill();
  
}




// ------------ method called once each job just before starting event loop  ------------
void EcalDigiDumperPh2::beginJob() {
  // please remove this method if not needed
}

// ------------ method called once each job just after ending the event loop  ------------
void EcalDigiDumperPh2::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void EcalDigiDumperPh2::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(EcalDigiDumperPh2);
