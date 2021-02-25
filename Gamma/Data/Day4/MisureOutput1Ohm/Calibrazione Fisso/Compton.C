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

void Compton () {

    TFile *hfile = new TFile("day2conv.root");

    TH1D* hgamma[2]; //2 istogrammi a tensioni

    char hposname[50]; //nome degli istogrammi positivi

    TF1 *farray[2]; //array di fit
    char fitname[50];
    int cntf = 0; //contatore per fit

    for(int i=0;i<2;i++) {
        if (i==1) sprintf(hposname,"hday1_%d",0);
	if (i==0) sprintf(hposname,"hday1_%d",1);
	
        hgamma[i] = (TH1D*)hfile->Get(hposname);
    }

     //--------
    //DRAW FIT
    //--------
    gStyle->SetOptStat(10);
    gStyle->SetOptFit(111);

    
    if (drawh) {

      /////////////////SODIO//////////////////////////////////////
      TCanvas *canv[3]; 
      
	canv[0] = new TCanvas();
	hgamma[0]->SetMarkerStyle(21);
	hgamma[0]->SetMarkerSize(0.7);
	hgamma[0]->SetTitle("SPETTRO MCA Na - Fisso");
	hgamma[0]->GetXaxis()->SetTitle("E [KeV]");
	hgamma[0]->GetYaxis()->SetTitle("Counts");
	hgamma[0]->Draw("e1");

	//Background retta
	TF1* f = new TF1("func","[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))+[3]",280,370);
	TF1* f1 = new TF1("func1","[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))+[3]",970,1150);
	TF1* f2 = new TF1("func1","gaus(0)+pol1(3)+gaus(5)",30,90);
	f->SetParNames("Norm_{1}","E_{c}","#sigma_{1}","c");
	f1->SetParNames("Norm_{1}","E_{c}","#sigma_{1}","c");
	f2->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","a","b","Norm_{2}","#mu_{2}","#sigma_{2}","c","Norm_{c2}");
	// Set mameningful initial parameters

	//Parametri retta
	f->SetParameter (0,4250);
	//f->SetParLimits (0,4000,4500);
	f->SetParameter(1,300);
	//f->SetParLimits (1,290,400);
	f->SetParameter (2,10);
	//f->SetParLimits (2,5,15);
	f->SetParameter (3,-350);
	//f->SetParLimits (3,300,400);
	
	f1->SetParameter (0,1000);
	//f1->SetParLimits (0,100,3000);
	f1->SetParameter (1,877);
	//f1->SetParLimits (1,840,900);
	f1->SetParameter (2,15);
	//f1->SetParLimits (2,5,30);
	f1->SetParameter (3,-850);

	f2->SetParameter (0,1000);
	f2->SetParLimits (0,1000,3000);
	f2->SetParameter (1,877);
	f2->SetParLimits (1,50,60);
	f2->SetParameter (2,15);
	f2->SetParLimits (2,0,9);
	f2->SetParameter (3,1500);
	f2->SetParLimits (3,1000,2000);
	f2->SetParameter (4,1000);
	f2->SetParLimits (4,0,10);
	//f2->SetParLimits (0,55,75);
	f2->SetParameter (5,200);
	//f1->SetParLimits (1,840,900);
	f2->SetParameter (6,60);
	//f1->SetParLimits (2,5,30);
	f2->SetParameter (7,15);
	
	hgamma[0]->Fit(f2,"RM+");
	hgamma[0]->Fit(f1,"RM+");
	///Fit
	hgamma[0]->Fit(f,"RM+");
	canv[0]->Update();


	
	
	// Draw individual peaks separately, using the parameters from the fit
	TF1* g[5];
	g[0] = new TF1("g0", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))+[3]",280,370);
	g[0]->SetLineColor(kMagenta);
	g[1] = new TF1("g1", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))+[3]",970,1150);
	g[1]->SetLineColor(kBlue);
	g[2] = new TF1("g2", "gaus(0)+pol1(3)", 30,90);
	g[2]->SetLineColor(kMagenta);	
	g[3] = new TF1("g2", "gaus(0)+pol1(3)", 30,90);
	g[3]->SetLineColor(kBlue);	
	g[4] = new TF1("g2", "pol1", 30,90);
	g[4]->SetLineColor(kGreen);

	double g_int[2];
	for (int j=0; j<4; ++j) {
	  g[0]->SetParameter(j,f->GetParameter(j));
	  g[1]->SetParameter(j,f1->GetParameter(j));
	}
	for (int j=0; j<3; ++j) {
	  g[2]->SetParameter(j,f2->GetParameter(j));
	  g[3]->SetParameter(j,f2->GetParameter(j+5));
	}
	for (int j=3; j<5; ++j) {
	  g[4]->SetParameter(j-3,f2->GetParameter(j));
	  g[2]->SetParameter(j,f2->GetParameter(j));
	  g[3]->SetParameter(j,f2->GetParameter(j));
	}

	g[0]->SetLineWidth(2);
	g[0]->Draw("same");
	g[2]->SetLineWidth(2);
	g[2]->Draw("same");
	g[1]->SetLineWidth(2);
	g[1]->Draw("same");
	g[3]->SetLineWidth(2);
	g[3]->Draw("same");
	g[4]->SetLineWidth(2);
	g[4]->Draw("same");



	/////////////////Cobalto/////////////////////////////////////      
	canv[1] = new TCanvas();
	hgamma[1]->SetMarkerStyle(21);
	hgamma[1]->SetMarkerSize(0.7);
	hgamma[1]->GetXaxis()->SetTitle("E [KeV]");
	hgamma[1]->GetYaxis()->SetTitle("Counts");
	hgamma[1]->SetTitle("SPETTRO MCA Co - Fisso");
	hgamma[1]->Draw("e1");


	///Background sigmoide
	TF1* c = new TF1("funcC","gaus(0)+gaus(3)+[6]*TMath::Erfc((x-[7])/(TMath::Sqrt(2)*[8]))+[9]*TMath::Erfc((x-[10])/(TMath::Sqrt(2)*[11]))+[12]",880,1500);
	//Background retta
	TF1* c1 = new TF1("funcc1","gaus(0)+[3]",0,100);
	c->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}","Norm_{c1}","E_{c1}","#sigma_{c1}","Norm_{c2}","E_{c2}");
	c->SetParName(11,"#sigma_{c2}");
	c->SetParName(12,"c");
	// Set mameningful initial parameters
	TF1* c2 = new TF1("func1","gaus(0)+pol1(3)+gaus(5)",30,90);
	c2->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","a","b","Norm_{2}","#mu_{2}","#sigma_{2}","c","Norm_{c2}");
	
	//Parametri retta
	c->SetParameter (0,450);
	c->SetParLimits (0,300,500);
	c->SetParameter(1,1150);
	c->SetParLimits (1,1100,1200);
	c->SetParameter (2,10);
	c->SetParLimits (2,5,30);
	c->SetParameter (3,250);
	c->SetParLimits (3,200,300);
	c->SetParameter (4,1350);
	c->SetParLimits (4,1300,1400);
//Parametri aggiuntivi sigmoide
	c->SetParameter (5,100);
	c->SetParLimits (5,0,160);
	c->SetParameter (6,150);
	//c->SetParLimits (6,100,300);
	c->SetParameter(7,950);
	c->SetParLimits (7,900,1100);
	//Parametri aggiuntivi sigmoide
	c->SetParameter (8,10);
	c->SetParLimits (8,0,70);
	c->SetParameter (9,60);
	//c->SetParLimits (10,10,60);
	c->SetParameter (10,1100);
	c->SetParLimits (10,1000,1200);
	//arametri aggiuntivi sigmoide
	c->SetParameter (11,10);
	c->SetParLimits (11,0,40);
	c->SetParameter (12,5);
	c->SetParLimits (12,0,20);
	
	c1->SetParameter (0,1000);
	c1->SetParLimits (0,500,700);
	c1->SetParameter (1,920);
	c1->SetParLimits (1,900,960);
	c1->SetParameter (2,15);
	c1->SetParLimits (2,5,100);
	c1->SetParameter (3,0);

	c2->SetParameter (0,150);
	c2->SetParLimits (0,100,500);
	c2->SetParameter (1,55);
	c2->SetParLimits (1,50,60);
	c2->SetParameter (2,15);
	c2->SetParLimits (2,0,9);
	c2->SetParameter (3,150);
	//c2->SetParLimits (3,100,200);
	c2->SetParameter (4,5);
	c2->SetParLimits (4,0,20);
	//f2->SetParLimits (0,50,70);
	c2->SetParameter (5,100);
	c2->SetParLimits (5,0,500);
	c2->SetParameter (6,60);
	c2->SetParLimits (6,55,70);
	c2->SetParameter (7,15);
	c2->SetParLimits (7,0,10);
	
	

hgamma[1]->Fit(c2,"RM+");
	///Fit
	hgamma[1]->Fit(c,"RM+");
	canv[1]->Update();
	//hgamma[1]->Fit(c1,"RM+");
	

	// Draw individual peaks separately, using the parameters from the fit
	TF1* gc[7];
	gc[0] = new TF1("gc0", "gaus", 1000,1300);
	gc[0]->SetLineColor(kMagenta);
	gc[1] = new TF1("gc1", "gaus", 1200,1500);
	gc[1]->SetLineColor(kBlue);
	gc[2] = new TF1("c2", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))+[3]", 880,1150);	
	gc[2]->SetLineColor(kGreen);
	gc[3] = new TF1("c2", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))", 880,1250);	
	gc[3]->SetLineColor(kOrange);
	gc[4] = new TF1("c2", "gaus(0)+pol1(3)", 30,90);
	gc[4]->SetLineColor(kMagenta);	
	gc[5] = new TF1("c2", "gaus(0)+pol1(3)", 30,90);
	gc[5]->SetLineColor(kBlue);	
	gc[6] = new TF1("c2", "pol1", 30,90);
	gc[6]->SetLineColor(kGreen);
	
	for (int j=0; j<3; ++j) {
	  gc[0]->SetParameter(j,c->GetParameter(j));
	  gc[1]->SetParameter(j,c->GetParameter(j+3));
	}

	for (int j=6; j<9; ++j) {
	  gc[2]->SetParameter(j-6,c->GetParameter(j));
	  gc[3]->SetParameter(j-6,c->GetParameter(j+3));
	}
	gc[2]->SetParameter(3,c->GetParameter(12));

	for (int j=0; j<3; ++j) {
	  gc[4]->SetParameter(j,c2->GetParameter(j));
	  gc[5]->SetParameter(j,c2->GetParameter(j+5));
	}
	for (int j=3; j<5; ++j) {
	  gc[6]->SetParameter(j-3,c2->GetParameter(j));
	  gc[4]->SetParameter(j,c2->GetParameter(j));
	  gc[5]->SetParameter(j,c2->GetParameter(j));
	}

	gc[0]->SetLineWidth(2);
	gc[0]->Draw("same");
	gc[2]->SetLineWidth(2);
	gc[2]->Draw("same");
	gc[1]->SetLineWidth(2);
	gc[1]->Draw("same");
	gc[3]->SetLineWidth(2);
	gc[3]->Draw("same");
	gc[5]->SetLineWidth(2);
	gc[5]->Draw("same");
	gc[6]->SetLineWidth(2);
	gc[6]->Draw("same");
	gc[4]->SetLineWidth(2);
	gc[4]->Draw("same");



	
    }
}
