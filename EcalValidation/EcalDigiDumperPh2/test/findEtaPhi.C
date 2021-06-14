// root .x findEtaPhi.C
// --ieta=-26 --iphi=322 --event=8

#include "TTree.h"
#include "TROOT.h"

void findEtaPhi() {
    TFile *f = new TFile("test.root");
    printf("> File opened.\n");
    TTree *T = (TTree*)f->Get("EcalDigiDumperPh2/tree");
    printf("> TTree opened.\n");

    Int_t           eta[61200];
    Int_t           phi[61200];

    printf("> Variables defined.\n");

    T->SetBranchAddress("iphi",phi);
    T->SetBranchAddress("ieta",eta);

    printf("> Branches defined.\n");

    T->GetEvent(8);
    printf("\n Event 8\n");

    TGraph* gr = new TGraph();

    for (Int_t j=0; j<61200; j++) {
        if (phi[j]==322 && eta[j]==-26) {
            printf("j = %d\n", j);
        }
    }
}