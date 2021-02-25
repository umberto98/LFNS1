#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TMath.h"

const bool drawh = true; //metti true se vuoi 20 canvas di istogrammi con relativo fit


//-----------
//INIZIO MAIN
//-----------

void AnalysisMCA () {

    TFile *hfile = new TFile("day1conv.root");

    TH1D* hgamma[2]; //2 istogrammi a tensioni

    char hposname[50]; //nome degli istogrammi positivi

    TF1 *farray[2]; //array di fit
    char fitname[50];
    int cntf = 0; //contatore per fit

    for(int i=0;i<2;i++) {
        if (i==0) sprintf(hposname,"hday1_%d",i);
	if (i==1) sprintf(hposname,"hday1_%d",i);
        hgamma[i] = (TH1D*)hfile->Get(hposname);
    }

    //--------
    //DRAW FIT
    //--------
    gStyle->SetOptStat(10);
    gStyle->SetOptFit(111);

    
    if (drawh) {
        TCanvas *c[3]; 
        for(int i=0;i<2;i++) {
		c[i] = new TCanvas();
    		hgamma[i]->SetMarkerStyle(21);
   		hgamma[i]->SetMarkerSize(0.7);
    		hgamma[i]->GetXaxis()->SetTitle("E [A.U.]");
    		hgamma[i]->GetYaxis()->SetTitle("Counts");
    		hgamma[i]->Draw("histo");
		//TF1* backdx = new TF1("func","[0]",420,580);
		//backdx->SetParameter (0,120);
		//hgamma[i]->Fit(backdx,"RM+");
		//backdx->Draw("same");
		TF1* f = new TF1("func","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",290,430);
		TF1* f1 = new TF1("func1","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",780,960);
  		//f->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
  		// Set mameningful initial parameters
  		f->SetParameter (0,4250);
		f->SetParLimits (0,4000,4500);
		f->SetParameter(1,360);
  		f->SetParLimits (1,350,370);
  		f->SetParameter (2,10);
		f->SetParLimits (2,5,15);
		f->SetParameter (3,350);
		//f->SetParLimits (3,300,400);
		f->SetParameter (4,10);
		f->SetParLimits (4,5,100);
		f->SetParameter (5,100);
		f->SetParLimits (5,100,130);
		f->SetParameter (6,150);
		f->SetParLimits (6,100,300);
  		f1->SetParameter (0,2100);
  		f1->SetParameter (1,877);
		f1->SetParLimits (1,820,920);
  		f1->SetParameter (2,15);
		f1->SetParameter (3,850);
		f1->SetParameter (4,10);
		f1->SetParLimits (4,7,100);
		f1->SetParameter (5,10);
		f1->SetParLimits (5,0,25);
		f1->SetParameter (6,20);
		f1->SetParLimits (6,0,50);
  		//hgamma[i]->Fit(f,"RM+");
		c[i]->Update();
		//hgamma[i]->Fit(f1,"RM+");
		
  		// Draw individual peaks separately, using the parameters from the fit
  		TF1* g[4];
  		g[0] = new TF1("g0", "gaus", 290,430);
  		g[0]->SetLineColor(kMagenta);
		g[2] = new TF1("g2", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 290,430);
  		g[2]->SetLineColor(kGreen);
  		g[1] = new TF1("g1", "gaus + [3]", 780,960);
  		g[1]->SetLineColor(kBlue);
		g[3] = new TF1("g3", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 780,960);
  		g[3]->SetLineColor(kGreen);
  		double g_int[2];
    		for (int j=0; j<3; ++j) {
      			g[0]->SetParameter(j,f->GetParameter(j));
			g[1]->SetParameter(j,f1->GetParameter(j));
    		}
		for (int j=3; j<7; ++j) {
      			g[2]->SetParameter(j-3,f->GetParameter(j));
			g[3]->SetParameter(j-3,f1->GetParameter(j));
    		}
    		g[0]->SetLineWidth(2);
    		//g[0]->Draw("same");
		g[2]->SetLineWidth(2);
    		//g[2]->Draw("same");
		g[1]->SetLineWidth(2);
    		//g[1]->Draw("same");
		g[3]->SetLineWidth(2);
    		//g[3]->Draw("same");

  		
  		//f->Draw("same");
		//f1->Draw("same");

        }
		c[2] = new TCanvas();
		hgamma[0]->SetLineColor(kRed);
		hgamma[0]->Draw("histo");
		hgamma[1]->SetLineColor(kBlue);
		hgamma[1]->Draw("histosame");
    }

}
