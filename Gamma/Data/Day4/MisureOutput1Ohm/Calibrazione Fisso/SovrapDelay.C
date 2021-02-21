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

void SovrapDelay () {

    TFile *hfile = new TFile("day2.root");

    TH1D* hgamma[2]; //2 istogrammi a tensioni

    char hposname[50]; //nome degli istogrammi positivi

    TF1 *farray[2]; //array di fit
    char fitname[50];
    int cntf = 0; //contatore per fit

    for(int i=1;i<3;i++) {
        if (i==1) sprintf(hposname,"hday1_%d",i);
	if (i==2) sprintf(hposname,"hday1_%d",i);
	
        hgamma[i-1] = (TH1D*)hfile->Get(hposname);
    }

     //--------
    //DRAW FIT
    //--------
    gStyle->SetOptStat(10);
    gStyle->SetOptFit(111);

    
    if (drawh) {

      /////////////////SODIO con delay//////////////////////////////////////
      TCanvas *canv[3]; 
      
	canv[0] = new TCanvas();
	hgamma[0]->SetMarkerStyle(21);
	hgamma[0]->SetMarkerSize(0.7);
	hgamma[0]->GetXaxis()->SetTitle("E [A.U.]");
	hgamma[0]->GetYaxis()->SetTitle("Counts");
	hgamma[0]->Draw("e1");

	///Background sigmoide
	//TF1* f = new TF1("func","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",320,440);
	//TF1* f1 = new TF1("func1","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",800,960);

	//Background retta
	TF1* f = new TF1("func","gaus(0)+[3]*x+[4]",300,410);
	TF1* f1 = new TF1("func1","gaus(0)+[3]*x+[4]",800,960);
	
	//f->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
	// Set mameningful initial parameters

	//Parametri retta
	f->SetParameter (0,4250);
	//f->SetParLimits (0,4000,4500);
	f->SetParameter(1,360);
	f->SetParLimits (1,350,400);
	f->SetParameter (2,10);
	f->SetParLimits (2,5,15);
	f->SetParameter (3,-350);
	//f->SetParLimits (3,300,400);
	f->SetParameter (4,100);
	//f->SetParLimits (4,0,10000);
	
	f1->SetParameter (0,2100);
	f1->SetParameter (1,877);
	f1->SetParLimits (1,840,920);
	f1->SetParameter (2,15);
	//f1->SetParLimits (2,5,30);
	f1->SetParameter (3,-850);
	f1->SetParameter (4,10);
	//f1->SetParLimits (4,7,100);

	//Parametri aggiuntivi sigmoide
	//f->SetParameter (5,100);
	//f->SetParLimits (5,100,160);
	//f->SetParameter (6,150);
	//f->SetParLimits (6,100,300);
	
	//f1->SetParameter (5,10);
	//f1->SetParLimits (5,0,25);
	//f1->SetParameter (6,20);
	//f1->SetParLimits (6,0,100);

	///Fit
	hgamma[0]->Fit(f,"RM+");
	canv[0]->Update();
	hgamma[0]->Fit(f1,"RM+");
	
	// Draw individual peaks separately, using the parameters from the fit
	TF1* g[4];
	g[0] = new TF1("g0", "gaus", 300,410);
	g[0]->SetLineColor(kMagenta);
	g[1] = new TF1("g1", "gaus + [3]", 800,960);
	g[1]->SetLineColor(kBlue);
	//sigmoide
	//g[2] = new TF1("g2", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 290,430);
	//g[3] = new TF1("g3", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 780,960);
	//retta
	g[2] = new TF1("g2", "[0]*x+[1]", 300,410);
	g[3] = new TF1("g2", "[0]*x+[1]", 800,960);	
	g[2]->SetLineColor(kGreen);	
	g[3]->SetLineColor(kGreen);

	
	double g_int[2];
	for (int j=0; j<3; ++j) {
	  g[0]->SetParameter(j,f->GetParameter(j));
	  g[1]->SetParameter(j,f1->GetParameter(j));
	}
	//Sigmoide
	for (int j=3; j<7; ++j) {
	  //g[2]->SetParameter(j-3,f->GetParameter(j));
	  //g[3]->SetParameter(j-3,f1->GetParameter(j));
	}
	//retta
	for (int j=3; j<5; ++j) {
	  g[2]->SetParameter(j-3,f->GetParameter(j));
	  g[3]->SetParameter(j-3,f1->GetParameter(j));
	}
	g[0]->SetLineWidth(2);
	g[0]->Draw("same");
	g[2]->SetLineWidth(2);
	g[2]->Draw("same");
	g[1]->SetLineWidth(2);
	g[1]->Draw("same");
	g[3]->SetLineWidth(2);
	g[3]->Draw("same");


	/////////////////SODIO senza delay/////////////////////////////////////      
	canv[1] = new TCanvas();
	hgamma[1]->SetMarkerStyle(21);
	hgamma[1]->SetMarkerSize(0.7);
	hgamma[1]->GetXaxis()->SetTitle("E [A.U.]");
	hgamma[1]->GetYaxis()->SetTitle("Counts");
	hgamma[1]->Draw("e1");

	///Background sigmoide
	//TF1* c = new TF1("funcc","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",320,440);
	//TF1* c1 = new TF1("funcc1","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",800,960);

	//Background retta
	TF1* c = new TF1("funcc","gaus(0)+[3]*x+[4]",320,440);
	TF1* c1 = new TF1("funcc1","gaus(0)+[3]*x+[4]",840,1000);
	
	//c->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
	// Set mameningful initial parameters

	//Parametri retta
	c->SetParameter (0,4250);
	//c->SetParLimits (0,4000,4500);
	c->SetParameter(1,380);
	c->SetParLimits (1,350,420);
	c->SetParameter (2,10);
	c->SetParLimits (2,5,15);
	c->SetParameter (3,-350);
	//c->SetParLimits (3,300,400);
	c->SetParameter (4,100);
	//c->SetParLimits (4,0,10000);
	
	c1->SetParameter (0,2100);
	c1->SetParameter (1,920);
	c1->SetParLimits (1,900,1000);
	c1->SetParameter (2,15);
	//c1->SetParLimits (2,5,30);
	c1->SetParameter (3,-850);
	c1->SetParameter (4,10);
	//c1->SetParLimits (4,7,100);

	//Parametri aggiuntivi sigmoide
	//c->SetParameter (5,100);
	//c->SetParLimits (5,100,160);
	//c->SetParameter (6,150);
	//c->SetParLimits (6,100,300);
	
	//c1->SetParameter (5,10);
	//c1->SetParLimits (5,0,25);
	//c1->SetParameter (6,20);
	//c1->SetParLimits (6,0,100);

	///Fit
	hgamma[1]->Fit(c,"RM+");
	canv[1]->Update();
	hgamma[1]->Fit(c1,"RM+");
	
	// Draw individual peaks separately, using the parameters from the fit
	TF1* gc[4];
	gc[0] = new TF1("gc0", "gaus", 320,440);
	gc[0]->SetLineColor(kMagenta);
	gc[1] = new TF1("gc1", "gaus + [3]", 840,1000);
	gc[1]->SetLineColor(kBlue);
	//sigmoide
	//gc[2] = new TF1("gc2", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 290,430);
	//gc[3] = new TF1("gc3", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 780,960);
	//retta
	gc[2] = new TF1("gc2", "[0]*x+[1]", 320,440);
	gc[3] = new TF1("gc2", "[0]*x+[1]", 840,1000);	
	gc[2]->SetLineColor(kGreen);	
	gc[3]->SetLineColor(kGreen);

	
	for (int j=0; j<3; ++j) {
	  gc[0]->SetParameter(j,c->GetParameter(j));
	  gc[1]->SetParameter(j,c1->GetParameter(j));
	}
	//Sigmoide
	for (int j=3; j<7; ++j) {
	  //gc[2]->SetParameter(j-3,c->GetParameter(j));
	  //gc[3]->SetParameter(j-3,c1->GetParameter(j));
	}
	//retta
	for (int j=3; j<5; ++j) {
	  gc[2]->SetParameter(j-3,c->GetParameter(j));
	  gc[3]->SetParameter(j-3,c1->GetParameter(j));
	}
	gc[0]->SetLineWidth(2);
	gc[0]->Draw("same");
	gc[2]->SetLineWidth(2);
	gc[2]->Draw("same");
	gc[1]->SetLineWidth(2);
	gc[1]->Draw("same");
	gc[3]->SetLineWidth(2);
	gc[3]->Draw("same");


	//Sovrapposizione spettri NA delay e senza
	canv[2] = new TCanvas();
	hgamma[0]->GetXaxis()->SetTitle("E [A.U.]");
	hgamma[0]->GetYaxis()->SetTitle("Counts");
	hgamma[0]->SetLineWidth(2);
	hgamma[1]->SetLineWidth(2);
	hgamma[0]->SetLineColor(kBlack);
	hgamma[1]->SetLineColor(kBlue);
	hgamma[0]->Draw("histo");
	hgamma[1]->Draw("histosame");


	TF1* fit1[4];
	//delay
	fit1[0] = new TF1("gc0", "gaus(0)+[3]*x+[4]", 300,410);
	fit1[0]->SetLineColor(kRed);
	fit1[1] = new TF1("gc1", "gaus(0)+[3]*x+[4]", 800,960);
	fit1[1]->SetLineColor(kRed);

	//senza delay
	fit1[2] = new TF1("gc2", "gaus(0)+[3]*x+[4]", 320,440);
	fit1[3] = new TF1("gc2", "gaus(0)+[3]*x+[4]", 840,1000);	
	fit1[2]->SetLineColor(kGreen);	
	fit1[3]->SetLineColor(kGreen);

	
	for (int j=0; j<5; ++j) {
	  fit1[0]->SetParameter(j,f->GetParameter(j));
	  fit1[1]->SetParameter(j,f1->GetParameter(j));
	  fit1[2]->SetParameter(j,c->GetParameter(j));
	  fit1[3]->SetParameter(j,c1->GetParameter(j));
	}
	//Sigmoide
	for (int j=3; j<7; ++j) {
	  //gc[2]->SetParameter(j-3,c->GetParameter(j));
	  //gc[3]->SetParameter(j-3,c1->GetParameter(j));
	}

	fit1[0]->SetLineWidth(2);
	fit1[0]->Draw("same");
	fit1[2]->SetLineWidth(2);
	fit1[2]->Draw("same");
	fit1[1]->SetLineWidth(2);
	fit1[1]->Draw("same");
	fit1[3]->SetLineWidth(2);
	fit1[3]->Draw("same");

	
    }
}
