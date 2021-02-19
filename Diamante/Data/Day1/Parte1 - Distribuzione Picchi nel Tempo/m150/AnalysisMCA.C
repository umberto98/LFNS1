#include "Riostream.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TStyle.h"
#include "TMath.h"

//-----------
//INIZIO MAIN
//-----------

void AnalysisMCA () {

    TFile *hfile = new TFile("time.root");

    TH1D* hneg[7]; //10 istogrammi a tensioni negative

    char hnegname[50]; //nome degli istogrammi negativi

    for(int i=0;i<7;i++) {
        sprintf(hnegname,"hneg_%d",i);
        hneg[i] = (TH1D*)hfile->Get(hnegname);
    }

    TCanvas *c2 = new TCanvas();
    //grms->Draw("AP");
    hneg[0]->Draw("hist");
    hneg[0]->Rebin(1);
    hneg[0]->SetLineColor(1);
    hneg[0]->SetLineWidth(2);
    for(int i=1;i<7;i++)
	if((i)==6){
             hneg[i]->Draw("histsame");
	     hneg[i]->Rebin(1);
	     hneg[i]->SetLineColor(i); 
	     hneg[i]->SetLineWidth(2);   
	}

    c2->BuildLegend();
    gStyle->SetOptFit(1);
}


//------------------------
//IMPLEMENTAZIONE FUNZIONI
//------------------------
