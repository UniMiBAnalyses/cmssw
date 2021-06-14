#include "TCanvas.h"
#include "TTree.h"
#include "TROOT.h"

void averagePulse() {
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

    Float_t P = 0;
    // with countPulses.C 36 pulses were found
    Float_t pulses[36][17];

    // Setup default
    for (Int_t ixtal=0; ixtal<36; ixtal++) {
        for (Int_t isample=0; isample<17; isample++) {
            pulses[ixtal][isample]=-999;
        }
    }

    Int_t count = 0;

    // Loop on the events
    for (Int_t k=0; k<10; k++) {

        T->GetEvent(k);
        printf("\n Event %d\n",k);

        // Loop on eta and phi
        for (Int_t j=0; j<61200; j++) {

            if (adc[j][5] > 100 && gain[j][5] == 10) {

                // printf("\n\nCompute the pedestal.\n");
                
                // Compute the pedestal
                P = 0;
                for (Int_t p=12; p<16; p++) {
                    P += adc[j][p]*gain[j][p];
                }
                P /= 4;

                // printf("Fill the matrix.\n");
                
                // Fill the matrix
                for (Int_t i=0; i<16; i++) {
                    pulses[count][i] = adc[j][i]*gain[j][i] - P;
                    // printf("pulses[%d][%d] = %f, \n", count,i,pulses[count][i]);
                }
                
                // Adding the weight as last column
                pulses[count][16] = pulses[count][5];
    
                // Normalisation of the pulse
                for (Int_t i=0; i<16; i++) {
                    pulses[count][i] = pulses[count][i]/pulses[count][16];
                }

                count ++;
            }
        }
    }

    printf("\nThere are %d pulses.\n", count);
    
    // // Print the matrix
    // printf("\n\n MATRIX: \n\n");
    // for (Int_t ixtal=0; ixtal<36; ixtal++) {
    //     for (Int_t isample=0; isample<16; isample++) {
    //         printf("%f, ", pulses[ixtal][isample]);
    //     }
    //     printf("\n\n");
    // }

    // Now there is the pulses matrix with 36 normalised pulses and 36 weights
    // Let's compute the average pulse

    Float_t avg_pulse[16];

    // Setup default
    for (Int_t isample=0; isample<17; isample++) {
        avg_pulse[isample]=0;
    }


    Float_t sum_weights = 0;

    for (Int_t ixtal=0; ixtal<36; ixtal++) {
        sum_weights += pulses[ixtal][16];
        // printf("pulses[%d][16] = %f \n",ixtal,pulses[ixtal][16]);
    }

    // printf("sum_weights = %f\n", sum_weights);

    for (Int_t isample=0; isample<16; isample++) {
        for (Int_t ixtal=0; ixtal<36; ixtal++) {
            avg_pulse[isample] += pulses[ixtal][isample]*pulses[ixtal][16];
            // printf("pulses[%d][%d] = %f\n", ixtal, isample, pulses[ixtal][isample]);
            // printf("pulses[%d][16] = %f\n", ixtal, pulses[ixtal][16]);
        }
        // printf("\n\n");
        avg_pulse[isample] = avg_pulse[isample]/sum_weights;
    }

    // Print the average pulse
    printf("\n\nAVERAGE PULSE: ");
    for (Int_t isample=0; isample<16; isample++) {
        printf("%f, ", avg_pulse[isample]);
    }

    printf("\n\n");


}