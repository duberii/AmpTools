// plot_generator.C

#include <cstring>
#include <string>
#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>
#include <TString.h>
#include <TKey.h>
#include <iostream>

void plot_all_histograms(const std::string filename) {
    // Open the input file

    TString fin(filename);
    TFile *inputFile = TFile::Open(fin);

    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);

    TIter nextkey(inputFile->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)nextkey())) {
                    TString keyname = key->GetName();
                    TClass* classtype = gROOT -> GetClass(key -> GetClassName());
                    if (!classtype) {
                        continue;
                    }
                    if (classtype -> InheritsFrom("TH1")) {
                        TH1* hist = (TH1*)key->ReadObj();
                        if (!hist) {
                            continue;
                        }
                        hist-> Draw("COLZ");
                        TString fout(filename + "." + keyname.Data()+ ".png");
                        c1->SaveAs(fout);
                    }
    }

    delete c1;
    inputFile->Close();
}