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

    TH1D* hpos[5]; //10 istogrammi a tensioni positive

    char hposname[50]; //nome degli istogrammi positivi

    for(int i=0;i<5;i++) {
        sprintf(hposname,"hpos_%d",i);
        hpos[i] = (TH1D*)hfile->Get(hposname);
    }


    TCanvas *c2 = new TCanvas();
    hpos[0]->Draw("hist");
    hpos[0]->Rebin(4);
    hpos[0]->SetLineColor(3);
    for(int i=1;i<5;i++)
	if(i!=3){
             hpos[i]->Draw("histsame");
	     hpos[i]->Rebin(4);
	     hpos[i]->SetLineColor(i);    
	}

    c2->BuildLegend();
    gStyle->SetOptFit(1);
}