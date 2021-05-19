#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"

//CALIBRAZIONE
double calibr = 0.003289; //da moltiplicare nei canali
double offset = 0.007863; //MeV
const double gain = 1000.;
const double gaincal = 200.;

const double gammaen = 0.546; //MeV
const double masselteo = 0.511; //MeV

TH1D *hcalibr (TH1D *h1); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

void Sodio () {

    //ricalibro con i gain
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    TFile *hfile = new TFile("hsodio.root");
    TH1D *hold = (TH1D*)hfile->Get("hna");
    TH1D *hnew = hcalibr(hold);

    hnew->SetMarkerStyle(21);
   	hnew->SetMarkerSize(0.6);
    hnew->GetXaxis()->SetTitle(" E [MeV] ");
    hnew->GetYaxis()->SetTitle("Counts");

    TF1 *fit = new TF1("fit","[0]*TMath::Erfc((x-[1])/[2])+[3] + pol4(4)",0.1,0.5);
    TF1 *comptfit = new TF1("comptfit","[0]*TMath::Erfc((x-[1])/[2])+[3]",0.1,0.5);
    TF1 *bkg = new TF1("bkg","pol4",0.1,0.5);

    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    comptfit->SetLineColor(kGreen);
    comptfit->SetTitle(" Compton ");

    fit->SetLineColor(kRed);
    fit->SetTitle(" Fit Totale ");

    bkg->SetLineColor(kOrange);
    bkg->SetTitle(" Background ");

    double parcompt[] = {200,0.35,0.05,17};

    for(int i=0;i<4;i++) {
        fit->SetParameter(i,parcompt[i]);
        fit->SetParLimits(i,parcompt[i]-0.2*parcompt[i],parcompt[i]+0.2*parcompt[i]);
    }

    fit->SetParLimits(2,0,0.2);

    hnew->Fit(fit,"R0");

    for(int i=0;i<4;i++) comptfit->FixParameter(i,fit->GetParameter(i));
    for(int i=0;i<5;i++) bkg->FixParameter(i,fit->GetParameter(i+4));

    TCanvas *c = new TCanvas();
    hnew->Draw("e1");
    fit->Draw("same");
    comptfit->Draw("same");
    bkg->Draw("same");
    c->BuildLegend();

    //-----------------------
    //CALCOLO MASSA ELETTRONE
    //-----------------------

    double compten = fit->GetParameter(1);
    double scompten = fit->GetParError(1);

    double massel = gammaen/compten -1.;
    massel = massel*2.*gammaen;

    double smassel = fit->GetParameter(2)/TMath::Sqrt(2.);

    double zcalc = TMath::Abs(massel-masselteo);
    zcalc = zcalc/smassel;

    cout<<"\n ------------------------------ \n STIMA MASSA ELETTRONE \n ------------------------------ \n MASSA (e) = ( "<<massel<<" +- "<<smassel<<" ) MeV \n MASSA TEO (e) = "<<masselteo<<" MeV \n Z = "<<zcalc<<endl;


}

TH1D *hcalibr (TH1D *h1) {

    int nbins = h1->GetXaxis()->GetNbins();
    double enmin = offset;
    double enmax = offset+calibr*2048;
    TH1D *hnew = new TH1D("hnew"," Sorgente Sodio ",nbins,enmin,enmax); 
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
