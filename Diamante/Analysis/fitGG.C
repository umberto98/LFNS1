#include <TF1.h>
#include <TH1F.h>
#include <TStyle.h>
#include <iostream>

using namespace std;

// Second example: fitting a spectrum with multiple Gaussian peaks
void fitGG() {
  gStyle->SetOptStat(10);
  gStyle->SetOptFit(111);


  // Generate a random distribution with two different Gaussian contributions
  TH1F* hDistr = new TH1F("hDistr","Sum of two Gaussians",50,0,100);
  double n[] = {7500, 1850};
  {
    TF1* genSig1 = new TF1("genSig1","gaus",0,100);
    genSig1->SetParameters(1,58,5);
    hDistr->FillRandom("genSig1",n[0]);
    TF1* genSig2 = new TF1("genSig2","gaus",0,100);
    genSig2->SetParameters(1,42,4);
    hDistr->FillRandom("genSig2",n[1]);
    delete genSig1;
    delete genSig2;
  }


  hDistr->SetMarkerStyle(21);
  hDistr->SetMarkerSize(0.7);
  hDistr->GetXaxis()->SetTitle("E [A.U.]");
  hDistr->GetYaxis()->SetTitle("Counts");
  hDistr->Draw("e1");


  // Fit with the sum of three Gaussians
  TF1* f = new TF1("func","gaus(0) + gaus(3)",0,100);
  f->SetParNames("Norm_{1}","#mu_{1}","#sigma_{1}","Norm_{2}","#mu_{2}","#sigma_{2}");
  // Set mameningful initial parameters
  f->SetParameter (0,1000);
  f->SetParameter (1,60);
  f->SetParameter (2,3);
  f->SetParameter (3,400);
  f->SetParameter (4,40);
  f->SetParameter (5,3);
  hDistr->Fit(f);

  // Draw individual peaks separately, using the parameters from the fit
  TF1* g[2];
  g[0] = new TF1("g0", "gaus", 0,100);
  g[0]->SetLineColor(kMagenta);
  g[1] = new TF1("g1", "gaus", 0,100);
  g[1]->SetLineColor(kBlue);
  double g_int[2];
  double int_total=0.;
  for (int i=0; i<2; ++i) {
    for (int j=0; j<3; ++j) {
      g[i]->SetParameter(j,f->GetParameter(3*i+j));
      g_int[i] = g[i]->Integral(0,100);
      int_total += g_int[i];
    }
    g[i]->SetLineWidth(2);
    g[i]->Draw("same");
  }
  f->Draw("same");

  // Print the observed BRs
  cout << endl << "BRs:" << endl;
  cout << "        Measured     | True" << endl;
  for (int i=0; i<2; ++i) {
    double fr = g_int[i]/int_total;
    cout << std::setprecision(3) << std::fixed;
    cout << "Peak " << i << ": "  << fr << "+-" << sqrt(fr*(1-fr)/int_total)
	 << " | "  << n[i]/(n[0]+n[1]) << endl;
  }
    cout << endl;
}
