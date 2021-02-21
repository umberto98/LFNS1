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

    TFile *hfile = new TFile("day2conv.root");

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
	//TF1* f = new TF1("func","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",320,440);
	//TF1* f1 = new TF1("func1","gaus(0)+[6]/(1+ TMath::Exp((x-[3])/[4]))+[5]",800,960);

	//Background retta
	TF1* f = new TF1("func",
	//"gaus(0)+0.5*([3]*(x*x+[5]*[5])+[7]*x+[8])*TMath::Erfc((x-[4])/(TMath::Sqrt(2)*[5]))+(-[5]*[3]*(x-[4])/TMath::Sqrt(2*TMath::Pi())+[7])*TMath::Exp(-0.5*(x-[4])/[5]*(x-[4])/[5])+[6]",270,640);
	"gaus(0)+[3]*TMath::Erfc((x-[4])/(TMath::Sqrt(2)*[5]))+[6]",280,600);
	//"gaus(0)+[3]/(1+ TMath::Exp((x-[4])/[5]))+[6]",260,600);
	TF1* f1 = new TF1("func1","[0]/(1+ TMath::Exp((x-[1])/[2]))+[3]",260,400);
	//f->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
	// Set mameningful initial parameters

	//Parametri retta
	f->SetParameter (0,4250);
	//f->SetParLimits (0,4000,4500);
	f->SetParameter(1,500);
	f->SetParLimits (1,460,560);
	f->SetParameter (2,10);
	f->SetParLimits (2,5,50);
	f->SetParameter (3,350);
	//f->SetParLimits (3,250,450);
	f->SetParameter (4,300);
	f->SetParLimits (4,300,400);
	f->SetParameter (5,10);
	//f->SetParLimits (5,0,100);
	f->SetParameter (6,10);
	//f->SetParameter (7,10);
	//f->SetParLimits (6,100,300);
	//f->SetParameter (8,0);
	//f->SetParLimits (8,400,600);
	//f->SetParameter (9,0);	
	//f->SetParLimits (9,18,200);
	f1->SetParameter (0,4250);
	//f->SetParLimits (0,4000,4500);
	f1->SetParameter(1,500);
	f1->SetParLimits (1,300,400);
	f1->SetParameter (2,10);
	//f1->SetParLimits (2,5,50);
	f1->SetParameter (3,350);
	///Fit
	hgamma[1]->Fit(f,"RM+");
hgamma[1]->Fit(f1,"RM+");
	canv[0]->Update();
	
	// Draw individual peaks separately, using the parameters from the fit
	TF1* g[4];
	g[0] = new TF1("g0", "gaus", 290,600);
	g[0]->SetLineColor(kMagenta);
	g[1] = new TF1("g1", "[0]*TMath::Erfc((x-[1])/(TMath::Sqrt(2)*[2]))", 290,600);
	g[1]->SetLineColor(kBlue);
	//sigmoide
	//g[2] = new TF1("g2", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 290,430);
	//g[3] = new TF1("g3", "[3]/(1+ TMath::Exp((x-[0])/[1]))+[2]", 780,960);
	//retta
	//g[2] = new TF1("g2", "[0]+[1]*TMath::Erfc((x-[2])/(TMath::Sqrt(2)*[3]))", 290,600);
	//g[3] = new TF1("g2", "[0]*x+[1]", 800,960);	
	//g[2]->SetLineColor(kGreen);	
	//g[3]->SetLineColor(kGreen);

	
	double g_int[2];
	for (int j=0; j<3; ++j) {
	  g[0]->SetParameter(j,f->GetParameter(j));
	  g[1]->SetParameter(j,f->GetParameter(j+3));
	}
	//retta
	for (int j=6; j<10; ++j) {
	  //g[2]->SetParameter(j-6,f->GetParameter(j));
	}
	g[0]->SetLineWidth(2);
	g[0]->Draw("same");
	//g[2]->SetLineWidth(2);
	//g[2]->Draw("same");
	g[1]->SetLineWidth(2);
	g[1]->Draw("same");
	//g[3]->SetLineWidth(2);
	//g[3]->Draw("same");

	cout<<"Massa elettrone: "<< 2*511*(511-f->GetParameter(4))/(f->GetParameter(4)) << endl;
    	cout<<"Errore: " << 2*511*511/(f->GetParameter(4)*f->GetParameter(4))*f->GetParError(4)<<endl;
    }
    

}
