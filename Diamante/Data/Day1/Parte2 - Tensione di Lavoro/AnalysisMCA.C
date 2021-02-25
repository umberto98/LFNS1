#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TMath.h"

const double sig = 30.; //supponiamo che il segnale sia circa 20 intorno al valore massimo
const bool drawh = true; //metti true se vuoi 20 canvas di istogrammi con relativo fit

double calcmean (TH1D *h1, double a, double b);
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

    std::vector<double> rms; 
    std::vector<double> mean;
    std::vector<double> hv;

    int cntneg,cntpos;
    cntneg = 9; //parto a leggere da -500V fino a -50
    cntpos = 0; //poi passo a quelli positivi

    for(int i=-500;i<=500;i=i+50) {

        if (i==0) continue;
        hv.push_back(i); //hv salta 0V

        if (cntneg >= 0) {

            //---------
            //CENTROIDE 
            //---------

            int binmax = hneg[cntneg]->GetMaximumBin();
            double maxch = hneg[cntneg]->GetBinCenter(binmax);
            double centr = calcmean(hneg[cntneg],maxch-sig/2.,maxch+sig/2.);

            mean.push_back(centr);

            //-----------
            //RISOLUZIONE
            //-----------

            sprintf(fitname,"f_%d",cntf);
            farray[cntf] = new TF1(fitname,"gaus",maxch-sig/2.,maxch+sig/2.); //range di fit con gaus
            hneg[cntneg]->Fit(fitname,"RQ0");
            cout<<" Sigma: "<<2.355*farray[cntf]->GetParameter("Sigma")<<" e Centroide: "<<centr<<" @ HV = "<<i<<endl;
            rms.push_back(235.5*farray[cntf]->GetParameter("Sigma")/centr);
            cntf++;

            cntneg--;

        }

        else {

            //---------
            //CENTROIDE 
            //---------

            int binmax = hpos[cntpos]->GetMaximumBin();
            double maxch = hpos[cntpos]->GetBinCenter(binmax);
            double centr = calcmean(hpos[cntpos],maxch-sig/2.,maxch+sig/2.);
	    double consta = hpos[cntpos]->Integral(binmax-sig/2.,binmax+sig/2.);
            mean.push_back(centr);

            //-----------
            //RISOLUZIONE
            //-----------

            sprintf(fitname,"f_%d",cntf);
            farray[cntf] = new TF1(fitname,"gaus(0)+gaus(3)",maxch-sig/2.,maxch+sig/2.); //range di fit con gaus
	    farray[cntf]->SetParameters(consta,maxch,sig/2.,consta/10.,maxch-sig/2.,sig/2.);
	    farray[cntf]->SetParLimits(4,500,maxch-sig/8.);
            hpos[cntpos]->Fit(fitname,"RQ0");
            //rms.push_back(hpos[cntpos]->GetStdDev()/centr);
            cout<<" Sigma "<<2.355*farray[cntf]->GetParameter("Sigma")<<" e Centroide: "<<centr<<" @ HV = "<<i<<endl;
            rms.push_back(235.5*farray[cntf]->GetParameter("Sigma")/centr);
            
            cntf++;

            cntpos++;

        }

    }

    //-------------------
    //SCELTA HV DI LAVORO
    //-------------------

    int minindex = std::min_element(rms.begin(),rms.end()) - rms.begin();
    cout<<" Indice di Ris. Max.: "<<minindex<<endl;
    cout<<" Risoluzione Massima "<<rms[minindex]<<" @ HV WORK = "<<hv[minindex]<<endl;

    TGraph *gmean = new TGraph(20,&hv[0],&mean[0]);
    gmean->SetTitle(" CENTROIDE vs HV ");
    gmean->GetXaxis()->SetTitle(" HV [V] ");
    gmean->GetYaxis()->SetTitle(" CENTROIDE [CH] ");
    gmean->SetMarkerStyle(20);
    gmean->SetMarkerSize(0.9);
    gmean->SetMarkerColor(kRed);

    TCanvas *c1 = new TCanvas();
    gmean->Draw("AP");

    TGraph *grms = new TGraph(20,&hv[0],&rms[0]);
    grms->SetTitle(" RISOLUZIONE RELATIVA vs HV ");
    grms->GetXaxis()->SetTitle(" HV [V] ");
    grms->GetYaxis()->SetTitle(" RISOLUZIONE RELATIVA [%] ");
    grms->SetMarkerStyle(20);
    grms->SetMarkerSize(0.9);
    grms->SetMarkerColor(kBlue);

    TCanvas *c2 = new TCanvas();
    grms->Draw("AP");

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

void printvec (std::vector<double> v) {
    int sz = (int)v.size();
    cout<<" v = ( ";
    for(int i=0;i<sz;i++) {
        cout<<v[i]<<" ";
    }
    cout<<")\n";
}
