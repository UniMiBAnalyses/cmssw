#include "TCanvas.h"
#include "TTree.h"
#include "TROOT.h"

void countPulses() {
    TFile *f = new TFile("test.root");
    printf("> File opened.\n");
    TTree *T = (TTree*)f->Get("EcalDigiDumperPh2/tree");
    printf("> TTree opened.\n");

    Float_t           adc[61200][16];
    Float_t           gain[61200][16];

    printf("> Variables defined.\n");

    T->SetBranchAddress("adc",adc);
    T->SetBranchAddress("gain",gain);

    printf("> Branches defined.\n");

    Int_t count = 0;

    // Loop on the events
    for (Int_t k=0; k<10; k++) {

        T->GetEvent(k);
        printf("\n Event %d\n",k);

        // Loop on eta and phi
        for (Int_t j=0; j<61200; j++) {
            
            if (adc[j][5] > 100 && gain[j][5] == 10) {
                printf("MAXadc = %f\n", adc[j][5]);
                count ++;
            }
        }
    }

    printf("\n ########## There are %d pulses with maxADC > 1000 and gain = 10.\n", count);
}