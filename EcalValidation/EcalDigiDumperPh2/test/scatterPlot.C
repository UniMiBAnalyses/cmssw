// .x readRoot.C
// --ieta=-26 --iphi=322 --event=8

#include "TCanvas.h"
#include "TTree.h"
#include "TGraph.h"
#include "TROOT.h"

void scatterPlot() {
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

    Float_t max_adc = 0;
    Float_t P = 0;

    for (Int_t j=0; j<61200; j++){
        P = adc[j][15]*gain[j][15];
        for (Int_t i=0; i < 16; i++) {
            if (adc[j][i]*gain[j][i] > max_adc) {
                max_adc = adc[j][i]*gain[j][i];
            }
        }
        gr->SetPoint(j,amplitude[j], max_adc-P);
        max_adc = 0;
    }

    TCanvas *c1 = new TCanvas();

    gr->SetMarkerColor(kBlue);
    gr->SetLineColor(kBlue);
    gr->SetMarkerSize(3);
    gr->SetMarkerStyle(20);
    gr->GetXaxis()->SetTitle("Amplitude");
    gr->GetYaxis()->SetTitle("Max ADC");


    gr->Draw("ap"); 

    
    c1->SaveAs("scatter.root");

}