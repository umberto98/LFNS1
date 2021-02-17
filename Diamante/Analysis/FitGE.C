#include <TF1.h>
#include <TH1F.h>
#include <TStyle.h>
#include <iostream>

using namespace std;


// First example: a Gaussian signal over an exponential background.
void fitGE() {
  gStyle->SetOptStat(10);
  gStyle->SetOptFit(111);


  // Generate a random distribution of a Gaussian signal on top of an
  // exponential background
  TH1F* hDistr = new TH1F("hDistr","Gaussian over background",50,0,100);
  {
    TF1* genSig = new TF1("genSig","gaus",0,100);
    genSig->SetParameters(1,50,5);
    hDistr->FillRandom("genSig",1500);
    TF1* genBkg = new TF1("genBkg","expo",0,100);
    genBkg->SetParameters(1,-0.03);
    hDistr->FillRandom("genBkg",8500);
    delete genSig;
    delete genBkg;
  }



  hDistr->SetMarkerStyle(21);
  hDistr->SetMarkerSize(0.7);
  hDistr->GetXaxis()->SetTitle("E [A.U.]");
  hDistr->GetYaxis()->SetTitle("Counts");
  hDistr->Draw("e1");

  // Fit with the sum of a Gaussian and an exponential
  TF1* f = new TF1("func","[0]*exp(-[1]*x) + gaus(2)",0,100);
  f->SetParNames("Norm_exp","slope","Norm","#mu","#sigma");
  // Set mameningful initial parameters
  f->SetParameter (0,500);
  f->SetParameter (1,0.1);
  f->SetParameter (2,200);
  f->SetParameter (3,50);
  f->SetParameter (4,3);

  hDistr->Fit(f);

  // Draw signal and background separately, using the parameters from the fit
  TF1* fGaus = new TF1("Signal", "gaus", 0,100);
  for (int j=0; j<3; ++j) {
    fGaus->SetParameter(j,f->GetParameter(2+j));
  }
  fGaus->SetLineColor(kBlue);
  fGaus->SetLineWidth(2);
  fGaus->Draw("same");

  TF1* fBkg = new TF1("Background", "[0]*exp(-[1]*x)", 0,100);
  fBkg->SetParameter(0,f->GetParameter(0));
  fBkg->SetParameter(1,f->GetParameter(1));
  fBkg->SetLineColor(kMagenta);
  fBkg->SetLineWidth(2);
  fBkg->Draw("same");
  f->Draw("same");

}
