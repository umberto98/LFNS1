#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"

const double rangeCm[] = {1735,1765,1750,1780}; //range fit dei due picchi di Cumio
const double rangeAm[] = {1640,1670,1655,1680}; //range fit dei due picchi di Americio
const double rangeNp[] = {1430,1470}; //solo un picco per Np

const double EnCm[] = {5.76,5.81}; //MeV corrispondenti
const double EnAm[] = {5.44,5.49}; //MeV picchi Americio
const double EnNp[] = {4.7688}; //Unico MeV pesato con B.R. del Nettunio

void DiamCalibr () {

    TFile *hfile = new TFile("hcal.root");

    TH1D *h1 = (TH1D*)hfile->Get("htriple");
    TH1D *hCm = new TH1D(*h1);
    TH1D *hAm = new TH1D(*h1);
    TH1D *hNp = new TH1D(*h1);

    h1->SetTitle(" Sorgente Tripla - Nettunio/Americio/Curio ");

    TF1 *fitCm = new TF1("fitCm","gaus(0)+gaus(3)",rangeCm[0],rangeCm[3]);
    TF1 *fitAm = new TF1("fitAm","gaus(0)+gaus(3)",rangeAm[0],rangeAm[3]);
    TF1 *fitNp = new TF1("fitNp","gaus(0)",rangeNp[0],rangeNp[1]);

    cout<<"\n --------------------------------- \n FIT TRE SORGENTI \n ---------------------------------\n";

    std::vector<double> energy; 
    std::vector<double> channel;
    std::vector<double> channelerr;

    fitCm->SetParameters(1000,1770,2,500,1750,5);
    fitAm->SetParameters(1000,1665,2,500,1655,10); 
    fitNp->SetParameters(1000,1450,2);

    fitCm->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
    fitAm->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
    fitNp->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}");

    //limiti prima gaus Americio
    fitAm->SetParLimits(0,0,2000);
    fitAm->SetParLimits(1,rangeAm[2],rangeAm[3]);
    fitAm->SetParLimits(2,0,10);

    //seconda gaus Americio
    fitAm->SetParLimits(3,0,1000);
    fitAm->SetParLimits(4,1655,1660);
    fitAm->SetParLimits(5,0,50);

    h1->SetMarkerStyle(21);
   	h1->SetMarkerSize(0.6);
    h1->GetXaxis()->SetTitle("CH");
    h1->GetYaxis()->SetTitle("Counts");

    hAm->SetTitle(" Americio ");
    hAm->SetMarkerStyle(21);
   	hAm->SetMarkerSize(0.6);
    hAm->GetXaxis()->SetTitle("CH");
    hAm->GetXaxis()->SetRangeUser(rangeAm[0]-10,rangeAm[3]+10);
    hAm->GetYaxis()->SetTitle("Counts");

    hCm->SetTitle(" Curio ");
    hCm->SetMarkerStyle(21);
   	hCm->SetMarkerSize(0.6);
    hCm->GetXaxis()->SetTitle("CH");
    hCm->GetXaxis()->SetRangeUser(rangeCm[0]-10,rangeCm[3]+10);
    hCm->GetYaxis()->SetTitle("Counts");

    hNp->SetTitle(" Nettunio ");
    hNp->SetMarkerStyle(21);
   	hNp->SetMarkerSize(0.6);
    hNp->GetXaxis()->SetTitle("CH");
    hNp->GetXaxis()->SetRangeUser(rangeNp[0]-10,rangeNp[1]+10);
    hNp->GetYaxis()->SetTitle("Counts");

    TCanvas *c1 = new TCanvas();
    h1->DrawCopy("hist");
    h1->Fit(fitCm,"R0");
    c1->Update();
    h1->Fit(fitAm,"R0");
    c1->Update();
    h1->Fit(fitNp,"R0");

    gStyle->SetOptStat(10); //solo entries
    gStyle->SetOptFit(1111); //parametri, prob e chi quadro ridotto

    TF1 *g[4]; 
  	g[0] = new TF1("g1","gaus",rangeCm[0],rangeCm[1]);
    g[1] = new TF1("g2","gaus",rangeCm[2],rangeCm[3]);
    g[2] = new TF1("g3","gaus",rangeAm[0],rangeAm[1]);
    g[3] = new TF1("g4","gaus",rangeAm[2],rangeAm[3]);

    TCanvas *c2 = new TCanvas();
    hCm->Draw("E1");
    hCm->Fit(fitCm,"R");
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
    hAm->Fit(fitAm,"R");
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
    hNp->Fit(fitNp,"R");

    cout<<" --------------------------------- \n RETTA (INVERTITA) DI CALIBRAZIONE \n ---------------------------------\n";

    //-------------------------------------------
    //RETTA CALIBRAZIONE - FIT CH(E) e INVERSIONE
    //-------------------------------------------

    channel.push_back(fitNp->GetParameter(1)); //Nettunio
    channelerr.push_back(fitNp->GetParameter(2));
    energy.push_back(EnNp[0]);

    channel.push_back(fitAm->GetParameter(1)); //Secondo picco Americio
    channelerr.push_back(fitAm->GetParameter(2));
    energy.push_back(EnAm[1]);

    channel.push_back(fitCm->GetParameter(1)); //Secondo picco Curio
    channelerr.push_back(fitCm->GetParameter(2));
    energy.push_back(EnCm[1]);

    TGraphErrors *cal = new TGraphErrors(3,&energy[0],&channel[0],0,&channelerr[0]);
    cal->SetTitle(" Retta di Calibrazione Diamante ");
    cal->GetXaxis()->SetTitle(" E [MeV] ");
    cal->GetYaxis()->SetTitle(" CH ");

    TF1 *calfit = new TF1("calfit","[0]+[1]*x",0.,6.);

    TCanvas *c5 = new TCanvas();
    cal->Draw("AP");
    cal->Fit(calfit,"R");

    //Si assume matrice di covarianza diagonale

    double calib = calfit->GetParameter(1);
    double calibinv = 1./calib;
    double calibsig = calfit->GetParError(1);
    double caliberr = calibsig/calib/calib;

    double offset=calfit->GetParameter(0);
    double offsetinv = -offset/calib;
    double offsetsig = calfit->GetParError(0);
    double offseterr = offsetsig*offsetsig/calib/calib + caliberr*caliberr*offset*offset;
    offseterr = TMath::Sqrt(offseterr);

    cout<<"\n --------------------------------- \n FATTORE DI CALIBRAZIONE \n ---------------------------------\n E(MeV) = ( "<<fixed<<setprecision(6)<<calibinv<<" +- "<<caliberr<<" ) * CH + ( "<<offsetinv<<" +- "<<offseterr<<" ) \n";

    //METODO INVERSO - FIT E(CH) DIRETTO

    double energyerr[] = {1e-7,1e-7,1e-7};

    TGraphErrors *calnew = new TGraphErrors(3,&channel[0],&energy[0],&channelerr[0],energyerr);
    calnew->SetTitle(" Retta di Calibrazione Diamante ");
    calnew->GetXaxis()->SetTitle(" CH ");
    calnew->GetYaxis()->SetTitle(" E [MeV] ");

    TF1 *newfit = new TF1("newfit","[0]+[1]*x",0.,2048);

    TCanvas *c6 = new TCanvas();
    calnew->Draw("AP");
    calnew->Fit(newfit,"R");

    cout<<"\n --------------------------------- \n NUOVO FATTORE DI CALIBRAZIONE \n ---------------------------------\n E(MeV) = ( "<<fixed<<setprecision(6)<<newfit->GetParameter(1)<<" +- "<<newfit->GetParError(1)<<" ) * CH + ( "<<newfit->GetParameter(0)<<" +- "<<newfit->GetParError(0)<<" ) \n";

}
