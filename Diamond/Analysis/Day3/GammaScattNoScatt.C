#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"

//CALIBRAZIONE
double calibr = 0.003289; //da moltiplicare nei canali
double offset = 0.007863;
const double gain = 1000.;
const double gaincal = 200.;

const double gammaen = 0.662; //MeV
const double masselteo = 0.511; //MeV
const double rhopoly = 1.420; //g/cm3
const double energyloss = 1.912; //MeV/g/cm2

TH1D *hcalibr (TH1D *h1); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

void GammaScattNoScatt () {

    gStyle->SetOptStat(0);

    //ricalibro con i gain
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    offset=2.*offset;
    calibr=2.*calibr;

    TFile *hfile1 = new TFile("hgamma.root");
    TH1D *hold1 = (TH1D*)hfile1->Get("hga");
    TH1D *hnew1 = hcalibr(hold1);

    TFile *hfile2 = new TFile("hgammnoscatt.root");
    TH1D *hold2 = (TH1D*)hfile2->Get("hganoscatt");
    TH1D *hnew2 = hcalibr(hold2);

    hnew1->SetTitle(" Cesio Coperto - Scatterless ");
    hnew2->SetTitle(" Cesio Coperto - No Scatterless ");

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

    double integ1 = hnew1->Integral();
    double integ2 = hnew2->Integral();

    hnew2->Scale(integ1/integ2);

    TCanvas *c = new TCanvas();
    hnew2->Draw("HIST");
    hnew1->Draw("HIST SAME");
    c->BuildLegend();

}

TH1D *hcalibr (TH1D *h1) {

    int nbins = h1->GetXaxis()->GetNbins();
    double enmin = offset;
    double enmax = offset+calibr*1024;
    TH1D *hnew = new TH1D("hnew"," Sorgente Cesio - Copertura ",nbins,enmin,enmax); 
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
