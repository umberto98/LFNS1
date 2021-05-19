#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"

//CALIBRAZIONE
double calibr = 0.003289; //da moltiplicare nei canali
double offset = 0.007863;
const double gain = 1000.;
const double gaincal = 200.;

TH1D *hcalibr (TH1D *h1); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

void Background () {

    //ricalibro con i gain
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    TFile *hfile = new TFile("hbackground.root");
    TH1D *hold = (TH1D*)hfile->Get("hbkg");
    TH1D *hnew = hcalibr(hold);

    hnew->SetMarkerStyle(21);
   	hnew->SetMarkerSize(0.6);
    hnew->GetXaxis()->SetTitle(" E [MeV] ");
    hnew->GetYaxis()->SetTitle("Counts");

    hnew->Draw("E1");

}

TH1D *hcalibr (TH1D *h1) {

    int nbins = h1->GetXaxis()->GetNbins();
    double enmin = offset;
    double enmax = offset+calibr*2048;
    TH1D *hnew = new TH1D("hnew"," Background ",nbins,enmin,enmax); 
    for (int i=1;i<=nbins;i++) { 
        double y = h1->GetBinContent(i); 
        double x = h1->GetXaxis()->GetBinCenter(i); 
        double yerr = h1->GetBinError(i);
        double xnew = offset+calibr*x;
        hnew->Fill(xnew,y); 
        hnew->SetBinError(i,yerr);
    }

    return hnew;

}
