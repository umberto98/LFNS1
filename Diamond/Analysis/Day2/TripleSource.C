#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"

//CALIBRAZIONE
const double calibr = 0.003289; //da moltiplicare nei canali
const double offset = 0.007863; //MeV

TH1D *hcalibr (TH1D *h1, double calibr, double offset); //ricalibra l'istogramma
double calibration (double x) {return offset+calibr*x;}

const double rangeCm[] = {calibration(1735),calibration(1765),calibration(1750),calibration(1780)}; //range fit dei due picchi di Cumio
const double rangeAm[] = {calibration(1640),calibration(1670),calibration(1655),calibration(1680)}; //range fit dei due picchi di Americio
const double rangeNp[] = {calibration(1430),calibration(1470)}; //solo un picco per Np

void TripleSource () {

    TFile *hfile = new TFile("hcal.root");

    TH1D *hch = (TH1D*)hfile->Get("htriple");
    TH1D *h1 = hcalibr(hch,calibr,offset);
    TH1D *hCm = new TH1D(*h1);
    TH1D *hAm = new TH1D(*h1);
    TH1D *hNp = new TH1D(*h1);

    //Picchi ignoti a sinistra e a destra del Nettunio
    TH1D *hNpl = new TH1D(*h1);
    TH1D *hNpr = new TH1D(*h1);

    TF1 *fitCm = new TF1("fitCm","gaus(0)+gaus(3)",rangeCm[0],rangeCm[3]);
    TF1 *fitAm = new TF1("fitAm","gaus(0)+gaus(3)",rangeAm[0],rangeAm[3]);
    TF1 *fitNp = new TF1("fitNp","gaus(0)",rangeNp[0],rangeNp[1]);

    TF1 *fitNpl = new TF1("fitNpl","gaus(0)",4.6,4.7);
    TF1 *fitNpr = new TF1("fitNpr","gaus(0)",4.84,4.89);

    cout<<"\n --------------------------------- \n FIT TRE SORGENTI \n ---------------------------------\n";

    fitCm->SetParameters(1000,calibration(1770),calibration(2),500,calibration(1750),calibration(5));
    fitAm->SetParameters(1000,calibration(1665),calibration(2),500,calibration(1655),calibration(10)); 
    fitNp->SetParameters(1000,calibration(1450),calibration(2));

    fitNpl->SetParameters(6,4.65,0);
    fitNpr->SetParameters(6,4.87,0);

    fitCm->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
    fitAm->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
    fitNp->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}");

    fitNpl->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}");
    fitNpl->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}");

    //limiti prima gaus Americio
    fitAm->SetParLimits(0,0,2000);
    fitAm->SetParLimits(1,rangeAm[2],rangeAm[3]);
    fitAm->SetParLimits(2,calibration(1),calibration(10));

    //seconda gaus Americio
    fitAm->SetParLimits(3,0,1000);
    fitAm->SetParLimits(4,calibration(1655),calibration(1660));
    fitAm->SetParLimits(5,calibration(1),calibration(50));

    h1->SetMarkerStyle(21);
   	h1->SetMarkerSize(0.6);
    h1->GetXaxis()->SetTitle(" E [MeV] ");
    h1->GetYaxis()->SetTitle("Counts");

    hAm->SetTitle(" Americio ");
    hAm->SetMarkerStyle(21);
   	hAm->SetMarkerSize(0.6);
    hAm->GetXaxis()->SetTitle(" E [MeV] ");
    hAm->GetXaxis()->SetRangeUser(rangeAm[0],rangeAm[3]);
    hAm->GetYaxis()->SetTitle("Counts");

    hCm->SetTitle(" Curio ");
    hCm->SetMarkerStyle(21);
   	hCm->SetMarkerSize(0.6);
    hCm->GetXaxis()->SetTitle(" E [MeV] ");
    hCm->GetXaxis()->SetRangeUser(rangeCm[0],rangeCm[3]);
    hCm->GetYaxis()->SetTitle("Counts");

    hNp->SetTitle(" Nettunio ");
    hNp->SetMarkerStyle(21);
   	hNp->SetMarkerSize(0.6);
    hNp->GetXaxis()->SetTitle(" E [MeV] ");
    hNp->GetXaxis()->SetRangeUser(rangeNp[0],rangeNp[1]);
    hNp->GetYaxis()->SetTitle("Counts");

    hNpl->SetTitle(" Nettunio - Picco Sinistro ");
    hNpl->SetMarkerStyle(21);
   	hNpl->SetMarkerSize(0.6);
    hNpl->GetXaxis()->SetTitle(" E [MeV] ");
    hNpl->GetXaxis()->SetRangeUser(4.6,4.7);
    hNpl->GetYaxis()->SetTitle("Counts");
    
    hNpr->SetTitle(" Nettunio - Picco Destro ");
    hNpr->SetMarkerStyle(21);
   	hNpr->SetMarkerSize(0.6);
    hNpr->GetXaxis()->SetTitle(" E [MeV] ");
    hNpr->GetXaxis()->SetRangeUser(4.84,4.89);
    hNpr->GetYaxis()->SetTitle("Counts");

    TCanvas *c1 = new TCanvas();
    h1->DrawCopy("hist");
    h1->Fit(fitCm,"RQ+");
    c1->Update();
    h1->Fit(fitAm,"RQ+");
    c1->Update();
    h1->Fit(fitNp,"RQ+");

    gStyle->SetOptStat(10); //solo entries
    gStyle->SetOptFit(1111); //parametri, prob e chi quadro ridotto

    TF1 *g[4]; 
  	g[0] = new TF1("g1","gaus",rangeCm[0],rangeCm[1]);
    g[1] = new TF1("g2","gaus",rangeCm[2],rangeCm[3]);
    g[2] = new TF1("g3","gaus",rangeAm[0],rangeAm[1]);
    g[3] = new TF1("g4","gaus",rangeAm[2],rangeAm[3]);

    TCanvas *c2 = new TCanvas();
    hCm->Draw("E1");
    hCm->Fit(fitCm,"RQ");
    for(int i=0;i<3;i++) {
        g[0]->SetParameter(i,fitCm->GetParameter(i+3));
        g[1]->SetParameter(i,fitCm->GetParameter(i));
    }
    
    g[0]->SetLineColor(kGreen);
    g[1]->SetLineColor(kOrange);
    g[0]->Draw("same");
    g[1]->Draw("same");

    TCanvas *c3 = new TCanvas();
    hAm->Draw("E1");
    hAm->Fit(fitAm,"RQ");
    for(int i=0;i<3;i++) {
        g[2]->SetParameter(i,fitAm->GetParameter(i+3));
        g[3]->SetParameter(i,fitAm->GetParameter(i));
    }
    g[2]->SetLineColor(kGreen);
    g[3]->SetLineColor(kOrange);
    g[2]->Draw("same");
    g[3]->Draw("same");

    TCanvas *c4 = new TCanvas();
    hNp->Draw("E1");
    hNp->Fit(fitNp,"RQ");

    TCanvas *c5 = new TCanvas();
    hNpl->Draw("E1");
    hNpl->Fit(fitNpl,"RQ");

    TCanvas *c6 = new TCanvas();
    hNpr->Draw("E1");
    hNpr->Fit(fitNpr,"RQ");

}

TH1D *hcalibr (TH1D *h1, double calibr, double offset) {

    int nbins = h1->GetXaxis()->GetNbins(); 
    double enmin = offset;
    double enmax = offset+calibr*2048;
    TH1D *hnew = new TH1D("hnew"," Sorgente Tripla - Americio/Curio/Nettunio ",nbins,enmin,enmax); 
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
