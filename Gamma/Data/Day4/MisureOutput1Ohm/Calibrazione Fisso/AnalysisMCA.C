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

    TFile *hfile = new TFile("day2.root");

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

      /////////////////SODIO//////////////////////////////////////
      TCanvas *canv[2]; 
      
	canv[0] = new TCanvas();
	hgamma[1]->SetMarkerStyle(21);
	hgamma[1]->SetMarkerSize(0.7);
	hgamma[1]->GetXaxis()->SetTitle("E [A.U.]");
	hgamma[1]->GetYaxis()->SetTitle("Counts");
	hgamma[1]->Draw("e1");

	///Background sigmoide
	//TF1* f = new TF1("func","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",300,440);
	//TF1* f1 = new TF1("func1","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",800,960);

	//Background retta
	TF1* f = new TF1("func","gaus(0)+pol3(3)",300,440);
	TF1* f1 = new TF1("func1","gaus(0)+pol3(3)",800,960);
	f->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","a","b","c","d");
	f1->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","a","b","c","d");
	//f->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
	// Set mameningful initial parameters

	//Parametri retta
	f->SetParameter (0,4250);
	f->SetParLimits (0,4000,10000);
	f->SetParameter(1,360);
	f->SetParLimits (1,350,400);
	f->SetParameter (2,10);
	f->SetParLimits (2,5,15);
	f->SetParameter (3,0);
	//f->SetParLimits (3,300,400);
	f->SetParameter (4,100);
	//f->SetParLimits (4,0,10000);
	
	f1->SetParameter (0,2100);
	f1->SetParLimits (0,0,1000);
	f1->SetParameter (1,877);
	f1->SetParLimits (1,840,920);
	f1->SetParameter (2,15);
	f1->SetParLimits (2,5,30);
	f1->SetParameter (3,-850);
	f1->SetParameter (4,10);
	//f1->SetParLimits (4,7,100);

	//Parametri aggiuntivi sigmoide
	f->SetParameter (5,100);
	//f->SetParLimits (5,100,160);
	f->SetParameter (6,0);
	//f->SetParLimits (6,100,300);
	
	f1->SetParameter (5,0);
	f1->SetParLimits (5,-1,1);
	f1->SetParameter (6,0);
	f1->SetParLimits (6,-1,1);
hgamma[1]->Fit(f1,"ERM+");
	///Fit
	hgamma[1]->Fit(f,"ERM+");
	canv[0]->Update();
	
	
	// Draw individual peaks separately, using the parameters from the fit
	TF1* g[4];
	g[0] = new TF1("g0", "gaus", 300,440);
	g[0]->SetLineColor(kMagenta);
	g[1] = new TF1("g1", "gaus + [3]", 800,960);
	g[1]->SetLineColor(kBlue);
	//sigmoide
	//g[2] = new TF1("g2", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 300,440);
	//g[3] = new TF1("g3", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 800,960);
	//retta
	g[2] = new TF1("g2", "pol3", 300,440);
	g[3] = new TF1("g2", "pol3", 800,960);	
	g[2]->SetLineColor(kGreen);	
	g[3]->SetLineColor(kGreen);

	
	double g_int[2];
	for (int j=0; j<3; ++j) {
	  g[0]->SetParameter(j,f->GetParameter(j));
	  g[1]->SetParameter(j,f1->GetParameter(j));
	}
	//Sigmoide
	for (int j=3; j<7; ++j) {
	  g[2]->SetParameter(j-3,f->GetParameter(j));
	  g[3]->SetParameter(j-3,f1->GetParameter(j));
	}
	//retta
	for (int j=3; j<5; ++j) {
	  //g[2]->SetParameter(j-3,f->GetParameter(j));
	  //g[3]->SetParameter(j-3,f1->GetParameter(j));
	}
	g[0]->SetLineWidth(2);
	//g[0]->Draw("same");
	g[2]->SetLineWidth(2);
	g[2]->Draw("same");
	g[1]->SetLineWidth(2);
	//g[1]->Draw("same");
	g[3]->SetLineWidth(2);
	g[3]->Draw("same");



	/////////////////COBALTO//////////////////////////////////////////////////
      
	canv[1] = new TCanvas();
	hgamma[0]->SetMarkerStyle(21);
	hgamma[0]->SetMarkerSize(0.7);
	hgamma[0]->GetXaxis()->SetTitle("E [A.U.]");
	hgamma[0]->GetYaxis()->SetTitle("Counts");
	hgamma[0]->Draw("e1");

	///Background sigmoide
	//TF1* C = new TF1("funcC","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",320,440);

	//Background retta
	//TF1* C = new TF1("funcC","gaus(0)+gaus(3)+[6]/(1+ TMath::Exp((x-[7])/[8]))+[9]",600,1000);
	//TF1* C = new TF1("funcC","gaus(0)+gaus(3)+[6]*TMath::Erfc((x-[7])/[8])+[9]+[10]*TMath::Erfc((x-[11])/[12])",600,1000);
TF1* C = new TF1("funcC","gaus(0)+gaus(3)+[6]*TMath::Erfc((x-[7])/(TMath::Sqrt(2)*[8]))+[9]",700,1000);
	C->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}","Norm_{3}","E_{c}","#sigma_{3}","c");
	// Set mameningful initial parameters

	//Parametri retta
	C->SetParameter (0,450);
	C->SetParLimits (0,0,1000);
	C->SetParameter(1,850);
	C->SetParLimits (1,750,850);
	C->SetParameter (2,10);
	C->SetParLimits (2,5,20);
	C->SetParameter (3,450);
	C->SetParLimits (3,0,1000);
	C->SetParameter (4,900);
	C->SetParLimits (4,850,950);
	C->SetParameter (5,10);
	C->SetParLimits (5,5,25);
	C->SetParameter (6,30);
	C->SetParLimits (6,10,50);
	C->SetParameter (7,780);
	C->SetParLimits (7,700,850);
	//Parametri aggiuntivi sigmoide
	C->SetParameter (8,10);
	C->SetParLimits (8,0,70);
	C->SetParameter (9,5);
	C->SetParLimits (9,0,20);
	//C->SetParameter (10,60);
	//C->SetParLimits (10,10,60);
	//C->SetParameter (11,780);
	//C->SetParLimits (11,600,700);
	//Parametri aggiuntivi sigmoide
	//C->SetParameter (12,10);
	//C->SetParLimits (12,0,70);

	///Fit
	hgamma[0]->Fit(C,"ERM+");
	canv[1]->Update();
	
	// Draw individual peaks separately, using the parameters from the fit
	TF1* c[4];
	c[0] = new TF1("c0", "gaus", 600,1000);
	c[0]->SetLineColor(kMagenta);
	c[1] = new TF1("c1", "gaus", 600,1000);
	c[1]->SetLineColor(kBlue);
	//sigmoide
	c[2] = new TF1("c2", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))+[3]", 700,1000);	
	c[2]->SetLineColor(kGreen);
	c[3] = new TF1("c2", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))", 700,1000);	
	c[3]->SetLineColor(kOrange);

	for (int j=0; j<3; ++j) {
	  c[0]->SetParameter(j,C->GetParameter(j));
	  c[1]->SetParameter(j,C->GetParameter(j+3));
	}
	//Sigmoide
	for (int j=3; j<7; ++j) {
	  //c[2]->SetParameter(j-3,C->GetParameter(j));
	}
	//retta
	for (int j=6; j<10; ++j) {
	  c[2]->SetParameter(j-6,C->GetParameter(j));
	  //c[3]->SetParameter(j-6,C->GetParameter(j+4));
	}
	c[2]->SetParameter(3,C->GetParameter(9));
	c[0]->SetLineWidth(2);
	//c[0]->Draw("same");
	c[2]->SetLineWidth(2);
	c[2]->Draw("same");
	c[1]->SetLineWidth(2);
	//c[1]->Draw("same");
	c[3]->SetLineWidth(2);
	//c[3]->Draw("same");
	
    }
    

}
