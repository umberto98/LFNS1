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
double offset = 0.007863; //MeV
const double gain = 1000.;
const double gaincal = 200.;

const bool halfchan = true; //ci sono 1024 canali

const double gammaen = 0.662; //MeV
const double masselteo = 0.511; //MeV
const double rhopoly = 1.420; //g/cm3
const double energyloss = 1.912; //MeV/g/cm2

TH1D *hcalibr (TH1D *h1); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

void Gamma () {

    //GAIN
    offset = offset*gaincal/gain;
    calibr = calibr*gaincal/gain;

    if (halfchan) {
        offset=2.*offset;
        calibr=2.*calibr;
    }

    TFile *hfile = new TFile("hgamma.root");
    TH1D *hold = (TH1D*)hfile->Get("hga");
    TH1D *hnew = hcalibr(hold);

    hnew->SetMarkerStyle(21);
   	hnew->SetMarkerSize(0.6);
    hnew->GetXaxis()->SetTitle(" E [MeV] ");
    hnew->GetXaxis()->SetRangeUser(0.07,1.);
    hnew->GetYaxis()->SetTitle("Counts");

    //------------
    //FUNZIONI FIT
    //------------

    TF1 *compt = new TF1("compt","[0]*TMath::Erfc((x-[1])/[2])+[3]",0.44,0.8);
    TF1 *bkg = new TF1("bkg","expo(0)+[2]",0.07,0.44);
    TF1 *fit = new TF1("fit","[0]*TMath::Erfc((x-[1])/[2])+[3]+expo(4)",0.07,1.2);

    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    compt->SetParameters(30,0.5,0.2,5);
    compt->SetParLimits(1,0.4,0.6);
    compt->SetParLimits(3,0,10);

    hnew->Fit(compt,"RQ0");
    cout<<" Prob (compt) "<<compt->GetProb()<<endl;

    bkg->SetParameter(2,compt->GetParameter(0)+compt->GetParameter(3));
    hnew->Fit(bkg,"RQ0");
    cout<<" Prob (bkg) "<<bkg->GetProb()<<endl;

    for(int i=0;i<4;i++) fit->SetParameter(i,compt->GetParameter(i));
    for(int i=0;i<2;i++) fit->SetParameter(i+4,bkg->GetParameter(i));

    hnew->Fit(fit,"R0");

    TF1 *bkgdraw = new TF1("bkgdraw","expo",0.07,1.2);
    TF1 *comptdraw = new TF1("comptdraw","[0]*TMath::Erfc((x-[1])/[2])+[3]",0.07,1.2);

    for(int i=0;i<4;i++) comptdraw->FixParameter(i,fit->GetParameter(i));
    for(int i=0;i<2;i++) bkgdraw->FixParameter(i,fit->GetParameter(i+4));

    bkgdraw->SetLineColor(kGreen);
    comptdraw->SetLineColor(kMagenta);
    fit->SetLineColor(kRed);

    bkgdraw->SetTitle(" Bkg Esponenziale ");
    comptdraw->SetTitle(" Compton ");
    fit->SetTitle(" Fit Totale ");

    TCanvas *c = new TCanvas ();
    hnew->Draw("E1");
    fit->Draw("same");
    bkgdraw->Draw("same");
    comptdraw->Draw("same");
    c->BuildLegend();

    //-----------------------
    //CALCOLO MASSA ELETTRONE
    //-----------------------

    double compten = fit->GetParameter(1);
    double scompten = fit->GetParError(1);

    double massel = gammaen/compten -1.;
    massel = massel*2.*gammaen;

    double smassel = gammaen*scompten/compten/compten;
    smassel = smassel*2.*gammaen;
    smassel = fit->GetParameter(2)/TMath::Sqrt(2.);

    double zcalc = TMath::Abs(massel-masselteo);
    zcalc = zcalc/smassel;

    cout<<"\n ------------------------------ \n STIMA MASSA ELETTRONE \n ------------------------------ \n MASSA (e) = ( "<<massel<<" +- "<<smassel<<" ) MeV \n MASSA TEO (e) = "<<masselteo<<" MeV \n Z = "<<zcalc<<endl;

    //---------------------------
    //CALCOLO SPESSORE POLIAMMIDE
    //---------------------------

    cout<<"\n ------------------------------ \n STIMA SPESSORE POLIAMMIDE \n ------------------------------ \n";

    double truecompten = 1. + masselteo/2./gammaen;
    truecompten = gammaen/truecompten;

    double deltacompt = compten - truecompten;
    double deltaxtil;
    double deltax;

    if (deltacompt<0) {

        deltacompt = TMath::Abs(deltacompt);
        deltaxtil = deltacompt/energyloss;
        deltax = deltaxtil/rhopoly;

        cout<<" Gli elettroni perdono energia nello spessore di poliammide \n";
        cout<<" Compton Expected: "<<truecompten<<" MeV \n";
        cout<<" Compton Stimato: "<<compten<<" MeV \n";
        cout<<" Perdita d'energia (NIST): "<<energyloss<<" MeV*cm*cm/g \n";
        cout<<" Spessore = ( "<<deltax*10000.<<" +- "<<10000.*scompten/energyloss/rhopoly<<" )  um \n";


    }  
    
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
