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

    TH1D* hpos[10]; //10 istogrammi a tensioni positive

    char hposname[50]; //nome degli istogrammi positivi

    for(int i=0;i<10;i++) {
        sprintf(hposname,"hpos_%d",i);
        hpos[i] = (TH1D*)hfile->Get(hposname);
    }


    TCanvas *c2 = new TCanvas();
    hpos[0]->Draw("hist");
    hpos[0]->Rebin(4);
    hpos[0]->SetLineColor(1);
    hpos[0]->SetLineWidth(2);
    for(int i=1;i<10;i++)
	if(i!=8 & i%3==0){
             hpos[i]->Draw("histsame");
	     hpos[i]->Rebin(4);
	     hpos[i]->SetLineColor(i);   
    	     hpos[i]->SetLineWidth(2); 
	}
    
    c2->BuildLegend();
    gStyle->SetOptFit(1);
}