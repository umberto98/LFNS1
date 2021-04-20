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

TH1D *hcalibr (TH1D *h1, int ch); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

void CesioScattNoScatt () {

    gStyle->SetOptStat(0);

    //ricalibro con i gain
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    TFile *hfile1 = new TFile("hcesio.root");
    TH1D *hold1 = (TH1D*)hfile1->Get("hce");
    TH1D *hnew1 = hcalibr(hold1,2048);

    offset=2.*offset;
    calibr=2.*calibr;

    TFile *hfile2 = new TFile("hcesionoscatt.root");
    TH1D *hold2 = (TH1D*)hfile2->Get("hcenoscatt");
    TH1D *hnew2 = hcalibr(hold2,1024);

    hnew1->SetTitle(" Cesio - Scatterless ");
    hnew2->SetTitle(" Cesio - No Scatterless ");

    hnew1->SetLineColor(kBlue);
    hnew2->SetLineColor(kGreen);

    hnew1->SetMarkerStyle(21);
   	hnew1->SetMarkerSize(0.6);
    hnew1->GetXaxis()->SetTitle(" E [MeV] ");
    hnew1->GetYaxis()->SetTitle("Counts");

    hnew2->SetMarkerStyle(21);
   	hnew2->SetMarkerSize(0.6);
    hnew2->GetXaxis()->SetTitle(" E [MeV] ");
    hnew2->GetYaxis()->SetTitle("Counts");

    hnew1->Rebin(2);

    double integ1 = hnew1->Integral();
    double integ2 = hnew2->Integral();

    hnew2->Scale(integ1/integ2);

    TCanvas *c = new TCanvas();
    hnew1->Draw("HIST");
    hnew2->Draw("HIST SAME");
    c->BuildLegend();

}

TH1D *hcalibr (TH1D *h1,int ch) {

    int nbins = h1->GetXaxis()->GetNbins();
    double enmin = offset;
    double enmax = offset+calibr*ch;
    TH1D *hnew = new TH1D("hnew"," Sorgente Cesio - Scatterless ",nbins,enmin,enmax); 
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
