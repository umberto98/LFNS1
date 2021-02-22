#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TMath.h"
#include "TLatex.h"
#include "TGraphErrors.h"

const double sig = 60.; //supponiamo che il segnale sia circa 20 intorno al valore massimo
const bool drawh = false; //metti true se vuoi 20 canvas di istogrammi con relativo fit

double calcmean (TH1D *h1, double a, double b);
double calcfwhm (TH1D *h1);
double calcriserror (TH1D *h1, double centr);
void printvec(std::vector<double> v);

//-----------
//INIZIO MAIN
//-----------

void AnalysisMCA () {

    TFile *hfile = new TFile("hmca.root");

    TH1D* hpos[10]; //10 istogrammi a tensioni positive
    TH1D* hneg[10]; //10 istogrammi a tensioni negative

    char hposname[50]; //nome degli istogrammi positivi
    char hnegname[50]; //nome degli istogrammi negativi

    TF1 *farray[20]; //array di fit
    char fitname[50];
    int cntf = 0; //contatore per fit

    for(int i=0;i<10;i++) {
        sprintf(hposname,"hpos_%d",i);
        sprintf(hnegname,"hneg_%d",i);
        hpos[i] = (TH1D*)hfile->Get(hposname);
        hneg[i] = (TH1D*)hfile->Get(hnegname);
    }

    std::vector<double> ris;
    std::vector<double> riserror;  
    std::vector<double> mean;
    std::vector<double> meanerror;
    std::vector<double> hv;

    int cntneg,cntpos;
    cntneg = 9; //parto a leggere da -500V fino a -50
    cntpos = 0; //poi passo a quelli positivi

    for(int i=-500;i<=500;i=i+50) {

        if (i==0) continue;
        hv.push_back(i); //hv salta 0V

        if (cntneg >= 0) {

            //----------------
            //CENTROIDE + FWHM 
            //----------------

            int binmax = hneg[cntneg]->GetMaximumBin();
            double maxch = hneg[cntneg]->GetBinCenter(binmax);
            double centr = calcmean(hneg[cntneg],maxch-sig/2.,maxch+sig/2.);
            double resfwhm = calcfwhm(hneg[cntneg]); //calcola FWHM

            mean.push_back(centr);
            ris.push_back(resfwhm/centr);
            riserror.push_back(calcriserror(hneg[cntneg],centr));
            meanerror.push_back(resfwhm);

            cout<<" Sigma: "<<resfwhm<<" e Centroide: "<<centr<<" @ HV = "<<i<<endl;

            //--------
            //FIT GAUS
            //--------

            sprintf(fitname,"f_%d",cntf);
            farray[cntf] = new TF1(fitname,"gaus",maxch-sig/2.,maxch+sig/2.); //range di fit con gaus
            hneg[cntneg]->Fit(fitname,"RQ0");
            cntf++;

            cntneg--;

        }

        else {

            //----------------
            //CENTROIDE + FWHM 
            //----------------

            int binmax = hpos[cntpos]->GetMaximumBin();
            double maxch = hpos[cntpos]->GetBinCenter(binmax);
            double centr = calcmean(hpos[cntpos],maxch-sig/2.,maxch+sig/2.);
            double resfwhm = calcfwhm(hpos[cntpos]); //calcola FWHM

            mean.push_back(centr);
            ris.push_back(resfwhm/centr);
            riserror.push_back(calcriserror(hpos[cntpos],centr));
            meanerror.push_back(resfwhm);

            cout<<" Sigma: "<<resfwhm<<" e Centroide: "<<centr<<" @ HV = "<<i<<endl;

            //--------
            //FIT GAUS
            //--------

            sprintf(fitname,"f_%d",cntf);
            farray[cntf] = new TF1(fitname,"gaus",maxch-sig/2.,maxch+sig/2.); //range di fit con gaus
            hpos[cntpos]->Fit(fitname,"RQ0");            
            cntf++;

            cntpos++;

        }

    }

    //-------------------
    //SCELTA HV DI LAVORO
    //-------------------

    int minindex = std::min_element(ris.begin(),ris.end()) - ris.begin();
    cout<<" Indice di Ris. Max.: "<<minindex<<endl;
    cout<<" Risoluzione Massima "<<ris[minindex]<<" @ HV WORK = "<<hv[minindex]<<endl;

    TGraphErrors *gmean = new TGraphErrors(20,&hv[0],&mean[0],0,&meanerror[0]);
    gmean->SetTitle(" CENTROIDE vs HV ");
    gmean->GetXaxis()->SetTitle(" HV [V] ");
    gmean->GetYaxis()->SetTitle(" CENTROIDE #mu [CH] ");
    gmean->SetMarkerStyle(20);
    gmean->SetMarkerSize(0.9);
    gmean->SetMarkerColor(kRed);

    TCanvas *c1 = new TCanvas();
    gmean->Draw("AP");

    TGraphErrors *gris = new TGraphErrors(20,&hv[0],&ris[0],0,&riserror[0]);
    gris->SetTitle(" RISOLUZIONE RELATIVA vs HV ");
    gris->GetXaxis()->SetTitle(" HV [V] ");
    gris->GetYaxis()->SetTitle(" #frac{#sigma_{FWHM}}{#mu} ");
    gris->SetMarkerStyle(20);
    gris->SetMarkerSize(0.9);
    gris->SetMarkerColor(kBlue);

    TCanvas *c2 = new TCanvas();
    gris->Draw("AP");

    //--------
    //DRAW FIT
    //--------

    gStyle->SetOptFit(1);

    if (drawh) {
        
        TCanvas *c[20]; 
        for(int i=0;i<20;i++) {
            c[i] = new TCanvas();
            if (i<10) hneg[9-i]->Draw("hist");
            else hpos[i-10]->Draw("hist");
            farray[i]->Draw("same");
        }
    }

}

//------------------------
//IMPLEMENTAZIONE FUNZIONI
//------------------------

double calcmean (TH1D *h1, double a, double b) { //media pesata

    int bins = h1->GetNbinsX();
    double s = 0.;
    double cnt = 0.;

    for (int i=1;i<bins+1;i++) {
        if (h1->GetBinCenter(i)>a && h1->GetBinCenter(i)<b) {
            cnt = cnt+h1->GetBinContent(i);
            s = s + h1->GetBinContent(i)*h1->GetBinCenter(i);
        }
    }

    if (cnt==0) return -50;
    else return s/cnt;

}

double calcfwhm (TH1D *h1) { //FWHM da h1
    int bin1 = h1->FindFirstBinAbove(h1->GetMaximum()/2.);
    int bin2 = h1->FindLastBinAbove(h1->GetMaximum()/2.);
    double fwhm = h1->GetBinCenter(bin2) - h1->GetBinCenter(bin1);
    return fwhm;
}

double calcriserror (TH1D *h1, double centr) {

    int bin1 = h1->FindFirstBinAbove(h1->GetMaximum()/2.);
    int bin2 = h1->FindLastBinAbove(h1->GetMaximum()/2.);
    double fwhm = h1->GetBinCenter(bin2) - h1->GetBinCenter(bin1);
    double fwhmerror = TMath::Sqrt(2.);
    double ristot = fwhmerror*fwhmerror/centr/centr;
    ristot = ristot + fwhm*fwhm*fwhm*fwhm/centr/centr/centr/centr;
    return TMath::Sqrt(ristot);

}

void printvec (std::vector<double> v) {
    int sz = (int)v.size();
    cout<<" v = ( ";
    for(int i=0;i<sz;i++) {
        cout<<v[i]<<" ";
    }
    cout<<")\n";
}