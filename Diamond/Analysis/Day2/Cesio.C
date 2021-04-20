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

void Cesio () {

    //ricalibro con i gain
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    TFile *hfile = new TFile("hcesio.root");
    TH1D *hold = (TH1D*)hfile->Get("hce");
    TH1D *hnew = hcalibr(hold);

    TH1D *hbeta = new TH1D(*hnew);
    TH1D *hkshell = new TH1D(*hnew);
    TH1D *hlshell = new TH1D(*hnew);

    hnew->SetMarkerStyle(21);
   	hnew->SetMarkerSize(0.6);
    hnew->GetXaxis()->SetTitle(" E [MeV] ");
    hnew->GetXaxis()->SetRangeUser(0.07,0.8);
    hnew->GetYaxis()->SetTitle("Counts");

    hbeta->SetTitle(" CESIO - DISTRIBUZIONE #beta^{-} ");
    hbeta->SetMarkerStyle(21);
   	hbeta->SetMarkerSize(0.6);
    hbeta->GetXaxis()->SetTitle(" E [MeV] ");
    hbeta->GetXaxis()->SetRangeUser(0.07,0.47);
    hbeta->GetYaxis()->SetTitle("Counts");

    hkshell->SetTitle(" CESIO - KSHELL ");
    hkshell->SetMarkerStyle(21);
   	hkshell->SetMarkerSize(0.6);
    hkshell->GetXaxis()->SetTitle(" E [MeV] ");
    hkshell->GetXaxis()->SetRangeUser(0.5,0.7);
    hkshell->GetYaxis()->SetTitle("Counts");

    hlshell->SetTitle(" CESIO - LSHELL ");
    hlshell->SetMarkerStyle(21);
   	hlshell->SetMarkerSize(0.6);
    hlshell->GetXaxis()->SetTitle(" E [MeV] ");
    hlshell->GetXaxis()->SetRangeUser(0.5,0.7);
    hlshell->GetYaxis()->SetTitle("Counts");

    int polgrade = 3;
    char betatitle[50];
    sprintf(betatitle,"pol%d",polgrade);

    TF1 *fitbeta = new TF1("fitbeta",betatitle,0.07,0.5);
    TF1 *fitkshell = new TF1("fitkshell","crystalball",0.50,0.63);
    TF1 *fitlshell = new TF1("fitlshell","gaus",0.63,0.68);

    fitkshell->SetParameters(271,0.61,0.012,1.6,0.22);

    char ftitle[50];
    sprintf(ftitle,"crystalball(0) + pol%d(5)",polgrade);
    TF1 *fittotl = new TF1("fittotl",ftitle,0.07,0.55);
    sprintf(ftitle,"crystalball(0)+gaus(5)");
    TF1 *fittotr = new TF1("fittotr",ftitle,0.54,0.67);

    TF1 *beta = new TF1("beta",betatitle,0.07,0.8); //prolungamento analitico beta
    TF1 *kshell = new TF1("kshell","crystalball",0.54,0.7); //prolungamento analitico kshell
    TF1 *lshell = new TF1("lshell","gaus",0.6,0.7); //prolungamento analitico kshell

    beta->SetTitle(" Spettro #beta^{-} ");
    kshell->SetTitle(" K-Shell ");
    lshell->SetTitle(" L-Shell ");

    beta->SetLineColor(kGreen);
    kshell->SetLineColor(kOrange);
    lshell->SetLineColor(kMagenta);

    fitbeta->SetLineColor(kGreen);
    fitkshell->SetLineColor(kOrange);
    fitlshell->SetLineColor(kMagenta);

    fittotl->SetLineColor(kViolet);
    fittotr->SetLineColor(kRed);

    fitbeta->SetTitle(" Spettro #beta^{-} ");
    fittotr->SetTitle(" K-Shell + L-Shell ");

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);

    hbeta->Fit(fitbeta,"R0");
    hkshell->Fit(fitkshell,"R");
    hlshell->Fit(fitlshell,"R0");

    TCanvas *c1 = new TCanvas();
    hnew->Draw("E1");

    for(int i=0;i<5;i++) fittotl->SetParameter(i,fitkshell->GetParameter(i));
    for(int i=0;i<=polgrade;i++) fittotl->SetParameter(i+5,fitbeta->GetParameter(i));

    for(int i=0;i<5;i++) fittotl->SetParLimits(i,fitkshell->GetParameter(i)-0.1*fitkshell->GetParameter(i),fitkshell->GetParameter(i)+0.1*fitkshell->GetParameter(i));
    for(int i=0;i<=polgrade;i++) fittotl->SetParLimits(i+5,fitbeta->GetParameter(i)-0.1*fitbeta->GetParameter(i),fitbeta->GetParameter(i)+0.1*fitbeta->GetParameter(i));

    for(int i=0;i<5;i++) fittotr->SetParameter(i,fitkshell->GetParameter(i));
    for(int i=0;i<3;i++) fittotr->SetParameter(i+5,fitlshell->GetParameter(i));

    hnew->Fit(fittotl,"R0");
    hnew->Fit(fittotr,"R0");

    for(int i=0;i<=polgrade;i++) beta->FixParameter(i,fitbeta->GetParameter(i));
    for(int i=0;i<5;i++) kshell->FixParameter(i,fittotr->GetParameter(i));
    for(int i=0;i<3;i++) lshell->FixParameter(i,fittotr->GetParameter(i+5));

    TCanvas *c5 = new TCanvas();
    hnew->DrawCopy("E1");
    fittotr->Draw("same");
    kshell->Draw("same");
    lshell->Draw("same");
    c5->BuildLegend();

    double Qval = beta->GetX(0,0.5,0.55,1e-10);
    
    cout<<" \n --------------------------- \n Q-VALUE \n --------------------------- \n Q-VALUE = "<<Qval<<" MeV \n Q-VALUE-EXP = 0.514 MeV \n Z = ? \n";

    cout<<" \n --------------------------- \n K-SHELL \n --------------------------- \n K-SHELL = ( "<<fittotr->GetParameter(1)<<" +- "<<fittotr->GetParameter(2)<<" ) MeV \n K-SHELL-EXP = 0.62422 MeV \n Z = "<<(fittotr->GetParameter(1)-0.62422)/fittotr->GetParameter(2)<<" \n";

    cout<<" \n --------------------------- \n L-SHELL \n --------------------------- \n K-SHELL = ( "<<fittotr->GetParameter(6)<<" +- "<<fittotr->GetParameter(7)<<" ) MeV \n L-SHELL-EXP = 0.656 MeV \n Z =  "<<(fittotr->GetParameter(6)-0.656)/fittotr->GetParameter(7)<<" \n";

}

TH1D *hcalibr (TH1D *h1) {

    int nbins = h1->GetXaxis()->GetNbins();
    double enmin = offset;
    double enmax = offset+calibr*2048;

    cout<<(enmax-enmin)/nbins;

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
