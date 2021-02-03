#include <Riostream.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TF1.h>

void sim(){

  TH1F* graph = new TH1F("graph","Simulazione curva di coincidenza",71,-90.5,50.5);

  double gS_width = 30.;   //è centrato in zero
  double g1_width = 30.;
  double g1_sigma = 10.;
  double rate = 1/300.;

  for(int i=-45; i<26; i++){
    for(int j=0; j<900; j++){
      double temp = gRandom->Gaus(2*i,g1_sigma);
      if(temp<0 & temp+g1_width>=-gS_width)
	graph->Fill(2*i);
      if(temp>=0 & temp<=gS_width)
	graph->Fill(2*i);      
    }
  }
  graph->Scale(rate);
    
  TCanvas* c = new TCanvas("c","c",800,1000);
  graph->Draw("histo");
}
