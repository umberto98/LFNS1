#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"

void SubPed () {

    TFile *pfile = new TFile("hped.root"); //lettura file piedistallo
    TH1D *hped10 = (TH1D*)pfile->Get("hadc10");
    TH1D *hped11 = (TH1D*)pfile->Get("hadc11");
    hped10->SetDirectory(0);
    hped11->SetDirectory(0);
    pfile->Close();

    TFile *dfile = new TFile("hdat.root"); //lettura file dati
    TH1D *hdat10 = (TH1D*)dfile->Get("hadc10");
    TH1D *hdat11 = (TH1D*)dfile->Get("hadc11");
    TH1D *hadc11flag = (TH1D*)dfile->Get("hadc11flag");
    TH1D *htdc = (TH1D*)dfile->Get("htdc");
    TH1D *hclk = (TH1D*)dfile->Get("hclk");
    hdat10->SetDirectory(0);
    hdat11->SetDirectory(0);
    hadc11flag->SetDirectory(0);
    htdc->SetDirectory(0);
    hclk->SetDirectory(0);
    dfile->Close();

    //tolgo il piedistallo per l'analisi
    hdat10->Add(hped10,-1.); //stesso binning!
    hdat11->Add(hdat10,-1.);

    TFile *cfile = new TFile("hanal.root","recreate"); //scrittura dei file corretti senza piedistallo -> working dir
    hdat10->Write();
    hdat11->Write();
    hadc11flag->Write();
    htdc->Write();
    hclk->Write();

}