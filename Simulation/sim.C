#include <Riostream.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TF1.h>
#include <TGraphErrors.h>

void sim(){

  TH1F* graph = new TH1F("graph","Simulazione curva di coincidenza",71,-90.5,50.5);

  double gS_width = 30.;   //è centrato in zero
  double g1_width = 29.;
  double g1_sigma = 9.;
  double rate = 1/350.;
  double adj = 7;
  float x[29] = {0, 5, 10, 15, 20, 25, 30, 35, 37, 38, 39, 40, 42, 46, -5, -10, -15, -20, -25, -30, -35, -40, -45, -50, -52, -54, -56, -58,-60};
  float y[29] = {2.5, 2.6, 2.6, 2.7, 2.5, 2.6, 2.1, 1.6, 1.2, 1.0, 0.8, 0.6, 0.6, 0.4, 2.6, 2.5, 2.5, 2.6, 2.5, 2.4, 2.4, 2.2, 2.1, 1.6, 1.3, 1.1, 0.7, 0.4, 0.2};
  float y_err[29] = {0.090982294, 0.092556289, 0.093035238, 0.09504385, 0.091772666, 0.092556289, 0.083732378, 0.072264945, 0.055, 0.049874843, 0.043803539, 0.03968627, 0.038971143, 0.03335416, 0.093094934, 0.091408728, 0.091226215, 0.092195445, 0.090369611, 0.089566859, 0.089814624, 0.085634884, 0.083133093, 0.073409052, 0.057391201, 0.051599419, 0.041079192, 0.03082207, 0.023452079};
  
  for(int i=-60; i<30; i++){
    for(int j=0; j<900; j++){
      double temp = gRandom->Gaus(2*i,g1_sigma);
      if(temp<0 & temp+g1_width>=-gS_width+adj)
	graph->Fill(2*i);
      if(temp>=0 & temp<=gS_width+adj)
	graph->Fill(2*i);      
    }
  }
  graph->Scale(rate);
    
  TCanvas* c = new TCanvas("c","c",800,1000);
  TGraphErrors *graph1 = new TGraphErrors(29, x, y, 0, y_err);
  graph1->SetMarkerSize(0);
  graph1->SetMarkerStyle(1);
  graph1->Draw("AP");
  graph->Draw("histosame");
}
