// plot_generator.C

#include <cstring>
#include <string>
#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>
#include <TString.h>
#include <TKey.h>
#include <iostream>

void plot_overlayed_histograms(const std::string filename) {
    // Open the input file

    TString fin(filename);
    TFile *inputFile = TFile::Open(fin);

    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
    TH1F *hm12dat = (TH1F*)inputFile->Get("hm12dat");
    TH1F *hm12acc = (TH1F*)inputFile->Get("hm12acc");
    TH1F *hm12acc1 = (TH1F*)inputFile->Get("hm12acc1");
    TH1F *hm12acc2 = (TH1F*)inputFile->Get("hm12acc2");
    TH1F *hm12gen = (TH1F*)inputFile->Get("hm12gen");
    TH1F *hm12gen1 = (TH1F*)inputFile->Get("hm12gen1");
    TH1F *hm12gen2 = (TH1F*)inputFile->Get("hm12gen2");
    hm12dat -> Draw("e");
    hm12acc -> Draw("hist,same");
    hm12acc1 -> Draw("hist,same");
    hm12acc2 -> Draw("hist,same");
    TString fout1(filename + ".acc.png");
    c1->SaveAs(fout1);
    hm12gen -> Draw("hist");
    hm12gen1 -> Draw("hist,same");
    hm12gen2 -> Draw("hist,same");
    TString fout2(filename + ".gen.png");
    c1->SaveAs(fout2);

    delete c1;
    inputFile->Close();
}