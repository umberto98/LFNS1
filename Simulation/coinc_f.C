#include <Riostream.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TF1.h>
#include <TGraphErrors.h>

void coinc_f(){

  TH1F* graph = new TH1F("graph","Simulazione curva di coincidenza",141,-90.5,50.5);
  TH2F* chi = new TH2F("chi","Distribuzione chi",44,4.75,10.25,36,4.75,9.25);	

  //data
  float x[29] = {0, 5, 10, 15, 20, 25, 30, 35, 37, 38, 39, 40, 42, 46, -5, -10, -15, -20, -25, -30, -35, -40, -45, -50, -52, -54, -56, -58,-60};

  float counts[29] = {745, 771, 779, 813, 758, 771, 631, 470, 484, 398, 307, 252, 243, 178, 780, 752, 749, 765, 735, 722, 726, 660, 622, 485, 527, 426, 270, 152, 88};

  float time[29] = {300, 300, 300, 300, 300, 300, 300, 300, 400, 400, 400, 400, 400, 400, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 400, 400, 400, 400, 400};
    
  float y[29];
  float y_err[29];
  float rate_m  = 0;  //mi serve per l'altezza del grafico
  float counter = 0;
  
  for(int i=0;i<29;i++){
    y[i] = counts[i]/time[i];
    y_err[i] = TMath::Sqrt(counts[i])/time[i];
    if(-30<x[i] & x[i]<26){
      rate_m+=y[i];
      counter++;
    }
  }
  rate_m = rate_m/counter;

  //simulzione
  double gS_width = 30.;   //è centrato in zero
  double g1_width = 29.;
  double rate = 1./(2.*352.81);
  double scarti_min = 10000;
  double scarti_l = 0;
  double scarti_h = 0;

  //Calcoliamo il chi quadro per ogni simulazione
  for(double h = 5.; h<10.; h+=0.125){
    for(double l = 5.; l<9.; l+=0.125){
      double g1_sigma = h;
      double adj = l;
      graph->Reset();
  
      //mi serve per l'altezza dell'histo
      for(int i=0; i<29; i++){
	for(int j=0; j<1800; j++){
	  double temp = gRandom->Gaus(x[i],g1_sigma);
	  if(temp<0 & temp+g1_width>=-gS_width+adj)
	    graph->Fill(x[i]);
	  if(temp>=0 & temp<=gS_width+adj)
	    graph->Fill(x[i]);
	}
      }
      graph->Scale(rate);
      //cout << "Altezza media punti: " << rate_m << "\nAltezza histo al centro: " << graph->GetMaximum() << endl;
      
      double scarti = 0;
      for(int i=0; i<29; i++)
	scarti += (y[i]-graph->GetBinContent(graph->GetXaxis()->FindBin(x[i])))*
	  (y[i]-graph->GetBinContent(graph->GetXaxis()->FindBin(x[i])))/(y_err[i]*y_err[i]);
      
      //cout << "chi quadro: " << scarti << endl;
      chi->Fill(h,l,scarti);
      if(scarti<scarti_min){
	scarti_l = l; 
	scarti_h = h;
	scarti_min = scarti;
      }
    }
  }

  cout << "chi quadro: " << scarti_min <<
    "\nValore che minimizza il chi di offset: " << scarti_l <<
    "\nValore che minimizza il chi di jitter: " << scarti_h << endl;
  
  TCanvas* c1 = new TCanvas("c1","c1",800,1000);
  chi->Draw("COLZ");
}
