#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TMath.h"

const double sig = 70.; //supponiamo che il segnale sia circa 20 intorno al valore massimo
const bool drawh = true; //metti true se vuoi 20 canvas di istogrammi con relativo fit

double calcmean (TH1D *h1, double a, double b);
void printvec(std::vector<double> v);

//-----------
//INIZIO MAIN
//-----------

void AnalysisMCA () {

    TFile *hfile = new TFile("hmca.root");

    TH1D* hpos[6]; //10 istogrammi a tensioni positive

    char hposname[50]; //nome degli istogrammi positivi
    char hnegname[50]; //nome degli istogrammi negativi

    TF1 *farray[6]; //array di fit
    char fitname[50];
    int cntf = 0; //contatore per fit

    for(int i=0;i<6;i++) {
        sprintf(hposname,"hpos_%d",i);
        hpos[i] = (TH1D*)hfile->Get(hposname);
    }

    std::vector<double> rms; 
    std::vector<double> mean;
    std::vector<double> hv;
    std::vector<double> hverr;
    std::vector<double> rmserr;

    int cntneg,cntpos;
 
    cntpos = 0; //poi passo a quelli positivi

    for(int i=0;i<=10;i=i+2) {

       	hv.push_back(i); //hv salta 0V
	hverr.push_back(1e-7);
            //---------
            //CENTROIDE 
            //---------

            int binmax = hpos[cntpos]->GetMaximumBin();
            double maxch = hpos[cntpos]->GetBinCenter(binmax);
            double centr = calcmean(hpos[cntpos],maxch-sig/2.,maxch+sig/2.);

            //mean.push_back(centr);

            //-----------
            //RISOLUZIONE
            //-----------

            sprintf(fitname,"f_%d",cntf);
            farray[cntf] = new TF1(fitname,"gaus(0)+[3]",maxch-sig/2.,maxch+sig/2.); //range di fit con gaus
            farray[cntf]->SetParameter(0,0);
	    farray[cntf]->SetParameter(1,maxch);
	    farray[cntf]->SetParameter(2,sig);
	    farray[cntf]->SetParameter(3,0);
	    hpos[cntpos]->Fit(fitname,"RM+");
	    mean.push_back(farray[cntf]->GetParameter(1));
            rms.push_back(farray[cntf]->GetParameter(2));
	    rmserr.push_back(farray[cntf]->GetParError(2));
            cout<<" Sigma "<<farray[cntf]->GetParameter("2")<<" e Centroide: "<<centr<<" @ HV = "<<i<<endl;
            //rms.push_back(farray[cntf]->GetParameter(2));
            
            cntf++;

            cntpos++;

        

    }

    //-------------------
    //SCELTA HV DI LAVORO
    //-------------------

    int minindex = std::min_element(rms.begin(),rms.end()) - rms.begin();
    cout<<" Indice di Ris. Max.: "<<minindex<<endl;
    cout<<" Risoluzione Massima "<<rms[minindex]<<" @ HV WORK = "<<hv[minindex]<<endl;

    TGraphErrors *gmean = new TGraphErrors(6, &mean[0],&hv[0], &rms[0], &hverr[0]);
    //TGraph *gmean = new TGraph(6,&hv[0],&mean[0]);
    gmean->SetTitle(" CENTROIDE vs DELAY ");
    gmean->GetXaxis()->SetTitle(" HV [V] ");
    gmean->GetYaxis()->SetTitle(" CENTROIDE [CH] ");
    gmean->SetMarkerSize(0);
    gmean->SetMarkerStyle(1);
    gmean->GetXaxis()->SetLimits(0, 1024);
    gmean->SetMarkerColor(kRed);

    TF1 *funz1 = new TF1("funz1","[0]*x + [1]", 0, 1024);
    funz1->SetParNames("k", "p");
    cout << "\n\n --- Ipotesi:  " << funz1->GetTitle() << "  ---\n" << endl;
    funz1->SetLineColor(4);
    gmean->Fit("funz1","RM+");
    TCanvas *c1 = new TCanvas();
    gmean->Draw("AP");

    TGraphErrors *grms = new TGraphErrors(6, &hv[0], &rms[0], &hverr[0], &rmserr[0]);
    //TGraph *grms = new TGraph(6,&hv[0],&rms[0]);
    grms->SetTitle(" RISOLUZIONE RELATIVA vs HV ");
    grms->GetXaxis()->SetTitle(" HV [V] ");
    grms->GetYaxis()->SetTitle(" RISOLUZIONE ns");
    grms->SetMarkerStyle(20);
    grms->SetMarkerSize(0.9);
    grms->GetXaxis()->SetLimits(-0.5, 14);
    grms->SetMarkerColor(kBlue);

    TCanvas *c2 = new TCanvas();
    grms->Draw("AP");

    //--------
    //DRAW FIT
    //--------

    gStyle->SetOptFit(1);

    if (drawh) {
        
        TCanvas *c[6]; 
        for(int i=0;i<6;i++) {
            c[i] = new TCanvas();
            hpos[i]->Draw("hist");
            farray[i]->Draw("same");
        }

	TCanvas *all = new TCanvas(); 
        for(int i=0;i<6;i++) {
            hpos[i]->Draw("histsame");
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