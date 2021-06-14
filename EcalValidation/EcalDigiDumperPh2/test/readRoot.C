// .x readRoot.C
// --ieta=-26 --iphi=322 --event=8

#include "TCanvas.h"
#include "TTree.h"
#include "TGraph.h"
#include "TROOT.h"

void readRoot() {
    TFile *f = new TFile("test.root");
    printf("> File opened.\n");
    TTree *T = (TTree*)f->Get("EcalDigiDumperPh2/tree");
    printf("> TTree opened.\n");

    Float_t           adc[61200][16];
    Float_t           gain[61200][16];
    Float_t           amplitude[61200];

    printf("> Variables defined.\n");

    T->SetBranchAddress("adc",adc);
    T->SetBranchAddress("gain",gain);
    T->SetBranchAddress("amplitude",amplitude);

    printf("> Branches defined.\n");

    T->GetEvent(9);
    printf("\n Event 9\n");

    TGraph* gr = new TGraph();

    Int_t j = 21561;

    // Compute the pedestal
    Float_t P = 0.;

    for (Int_t p=12; p<16; p++) {
        P = P + adc[j][p]*gain[j][p];
    }
    P = P/4;

    printf("pedestal = %f\n", P);

    for (Int_t i=0; i < 16; i++) {
        Float_t point = adc[j][i]*gain[j][i] - P;
        gr->SetPoint(i,i*6.25, point);
        // printf("%f, ",adc[j][i]*gain[j][i]);
        // printf("gain = %f \n",gain[j][i]);
        // printf("adc = %f \n",adc[j][i]);
    }

    printf("\nAmplitude = %f\n",amplitude[j]);

    TCanvas *c1 = new TCanvas();

    gr->SetMarkerColor(kBlue);
    gr->SetLineColor(kBlue);
    gr->SetMarkerSize(3);
    gr->SetMarkerStyle(20);
    gr->GetXaxis()->SetTitle("time [ns]");
    gr->GetYaxis()->SetTitle("ADC count");


    gr->Draw("apl"); 

    
    c1->SaveAs("picture.root");

}