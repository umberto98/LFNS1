\\Reading the TTree created from .txt file
#include "TTree.h"
#include "TBranch.h"
#include "Riostream.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"
#include "TMath.h"
#include "TPaveLabel.h"

//SENSIBILITA
const double adcsens = 0.25; //-0.25 pC per canale -> prendiamo in modulo
const double tdcsens = 2.5; //2.5 ns per canale
const double clksens = 100; //100 ns per canale

//DYNAMIC RANGE
const int adcdyn = 1980; //max num canali adc
const int tdcdyn = 2048; //max num canali tdc
const int clkdyn = 100; //sappiamo solo il full scale range 

//FULL SCALE RANGE
const double adcrng = adcsens*adcdyn;
const double tdcrng = tdcsens*tdcdyn; 
const double clkrng = clksens*clkdyn; //10 microsec

//CONVERSIONI
double clkscal (int ch) {return clksens*ch;} 
double tdcconv (int ch) {return tdcsens*ch;}
double adc (int ch) {return adcsens*ch;}
Double_t doublegauss(Double_t *x, Double_t *par){

  Double_t gau1=TMath::Landau(x[0],par[0],par[1],kFALSE);
  Double_t gau2=TMath::Landau(x[0],par[2],par[3],kFALSE);
  return (par[4]*gau1+par[5]*gau2);
}

Double_t singlelandau(Double_t *x, Double_t *par){

  Double_t lan=TMath::Landau(x[0],par[0],par[1],kFALSE);
  return (par[2]*lan);
}

Double_t explandau(Double_t *x, Double_t *par){
  Double_t lan=TMath::Landau(x[0],par[0],par[1],kFALSE);
  Double_t exp=TMath::Exp(x[0]*par[2]);
  return (par[3]*lan+par[4]*exp);
}
//PIEDISTALLO
const int ped10 = 320; //channel
const int ped11 = 270; //channel

void ReadTree () {
    
    typedef struct {
        int clk, scal, inhscal, tdc, adc10, adc11, pu; //clk scaler, scaler, scaler inibito, tdc, adc ch10, adc ch11 e pattern unit
    } Ev;

    static Ev event;

    TFile tfile("MyTree.root"); //in lettura
    TTree *tree = (TTree*)tfile.Get("T");

    TFile *wfile = new TFile("hplot.root","recreate");
    TBranch *b1 = tree->GetBranch("Data");
    b1->SetAddress(&event.clk); 

    TH1D *hadc11PU = new TH1D("adc11PU1"," ADC - SG - PU=1",adcdyn,0.,adcrng);//sg   
    TH1D *hclkPU = new TH1D("hclkPU"," CLK SCALER - PU=1",clkdyn,0.,clkrng);//clk scaler
    
    TH1D *hadc11 = new TH1D("adc11"," ADC - SG - PU=0",adcdyn,0.,adcrng);//sg
    TH1D *hadc11fkst = new TH1D("adc11fkst"," ADC - SG - PU=0 - Fake Stop",adcdyn,0.,adcrng);//sg
    TH1D *hadc11el = new TH1D("adc11el"," ADC - SG - PU=0 - Electron",adcdyn,0.,adcrng);//sg
    TH1D *hadc11cap = new TH1D("adc11cap"," ADC - SG - PU=0 - Capture",adcdyn,0.,adcrng);//sg

    TH1D *hadc10 = new TH1D("hadc10"," ADC - S1 ",adcdyn,0.,adcrng);//s1
    TH1D *hadc10_e = new TH1D("hadc10_e"," ADC - S1 - electron ",adcdyn,0.,adcrng);//s1
    TH1D *hclk = new TH1D("hclk"," CLK SCALER ",clkdyn,0.,100/*clkrng*/);//clk scaler
    TH1D *htdc = new TH1D("htdc"," TDC ",tdcdyn,0.,tdcrng);//tdc

    TH2D *hcorr11 = new TH2D("hcorr11"," Correlation ADC (SG) - TDC ",adcdyn,0,adcrng,clkdyn,0,clkrng);

    TF1 *fit1=new TF1("fitdoublelandau",singlelandau,75,300.,3);
    fit1->SetParameters(0,10,0);
    fit1->SetParLimits(0,0,200);
    fit1->SetParLimits(1,10,100);
    fit1->SetParLimits(2,0,100000000);
    //fit1->SetParLimits(3,0,1000);


    TF1 *fit2=new TF1("fitlandauPU",singlelandau,150,280.,3);
    fit2->SetParameters(136,0,0);
    fit2->SetParLimits(0,136,360);
    fit2->SetParLimits(1,0,100);
    fit2->SetParLimits(2,0,1000000);
    //fit2->SetParLimits(3,0,10000);
    

    TF1 *fit3=new TF1("fitlandauexp",doublegauss,20,270.,6);
    fit3->SetParameters(40,0,140,0,0,10000,10000);
    fit3->SetParLimits(0,40,80);
    fit3->SetParLimits(1,0,100);
    fit3->SetParLimits(2,140,200);
    fit3->SetParLimits(3,0,100);
    fit3->SetParLimits(4,10000,100000);
    fit3->SetParLimits(5,10000,100000);
    //fit3->SetParLimits(6,0,1000);

    TF1 *fit4=new TF1("fitS1",singlelandau,40,200.,4);
    fit4->SetParameters(45,0,0,0);
    fit4->SetParLimits(0,40,100);
    fit4->SetParLimits(1,0,100);
    fit4->SetParLimits(2,0,1000000);
    fit4->SetParLimits(3,0,1000);

    TF1 *fit5=new TF1("fitS1all",singlelandau,48,200.,4);
    fit5->SetParameters(55,25,0,0);
    fit5->SetParLimits(0,40,80);
    fit5->SetParLimits(1,0,30);
    fit5->SetParLimits(2,0,10000000000);

    
    hclk->GetXaxis()->SetTitle(" Time [ns] ");
    hclk->GetYaxis()->SetTitle(" Entries ");
    htdc->GetXaxis()->SetTitle(" Time [ns] ");
    htdc->GetYaxis()->SetTitle(" Entries ");

    for(int ev=0;ev<tree->GetEntriesFast();ev++) {
        tree->GetEvent(ev);
        if (ev%10000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
	
	if (event.pu==1 /*& clkscal(event.clk)>6200 &tdcconv(event.tdc)<500 */ ) {
	  if(event.adc11<1980 & adc(event.adc11-ped11)>0){
	    hadc11PU->Fill(adc(event.adc11-ped11));
	    hclkPU->Fill(clkscal(event.clk));
	  }
	}else{
	  if(event.adc11<1980 & event.adc11-ped11>0){
	    hadc11->Fill(adc(event.adc11-ped11));
	    hcorr11->Fill(adc(event.adc11-ped11),clkscal(event.clk));
	    if(event.clk>=62){
	      hadc11fkst->Fill(adc(event.adc11-ped11));
	    }else{
	      if(event.clk>=8)
		hadc11el->Fill(adc(event.adc11-ped11));
	      if(event.clk<8)
		hadc11cap->Fill(adc(event.adc11-ped11));
	    }
	    
	  }
	}
	if(event.adc10<1980 & event.adc10-ped10>0){
	   //togliamo i canali del piedistallo
	  if(adc(event.adc11-ped11)<110 & adc(event.adc11-ped11)>0 & event.clk>=62)
	    hadc10_e->Fill(adc(event.adc10-ped10));
	  else
	    hadc10->Fill(adc(event.adc10-ped10));
	}
	if(adc(event.adc11)>100 & adc(event.adc11-ped11)>0)
	  hclk->Fill(event.clk);
	htdc->Fill(tdcconv(event.tdc));
	//hclk->Fill(clkscal(event.clk));


	
    }

    for(int i = 0;i<adcdyn;i++){
      double inte=hcorr11->Integral(i,i,0,tdcdyn);
      if(inte>0)
      for(int j=0;j<clkdyn;j++)
	hcorr11->SetBinContent(i,j,hcorr11->GetBinContent(i,j)/inte);
    }

    hadc10->Rebin(18);
    hadc10_e->Rebin(18);
    hadc11PU->Rebin(32);
    hclkPU->Rebin(2);

    hadc11->Rebin(30);
    hadc11fkst->Rebin(30);
    hadc11el->Rebin(30);
    hadc11cap->Rebin(30);
    htdc->Rebin(16);

    hcorr11->RebinX(30);
    hcorr11->RebinY(2);
    
     
    
     
    /*
    TCanvas *c1 = new TCanvas("c1","c1");
    hadc11PU->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11PU->GetYaxis()->SetTitle(" Entries ");
    hadc11PU->Fit("fitlandauPU","R");
    hadc11PU->Draw("e1");
    cout << "Counts casual PU=1 SG: " << hadc11PU->Integral(0,16) << endl;
  
    TCanvas *c2 = new TCanvas("c2","c2");
    hclkPU->GetXaxis()->SetTitle(" Time [ns] ");
    hclkPU->GetYaxis()->SetTitle(" Entries ");
    c2->SetLogy(1);
    hclkPU->Draw("histo");
    
    TCanvas *c3 = new TCanvas("c3","c3");
    hadc11->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11->GetYaxis()->SetTitle(" Entries ");
    hadc11->Fit("fitlandauPU","R");
    hadc11->Draw("e1");
    
    TCanvas *c4 = new TCanvas("c4","c4");
    hadc11fkst->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11fkst->GetYaxis()->SetTitle(" Entries ");
    hadc11fkst->Draw("e1");
    hadc11fkst->Fit("fitlandauexp","R");
    
    TCanvas *c5 = new TCanvas("c5","c5");
    hadc11el->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11el->GetYaxis()->SetTitle(" Entries ");
    hadc11el->Draw("histo");

    TCanvas *c6 = new TCanvas("c6","c6");
    hadc11cap->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11cap->GetYaxis()->SetTitle(" Entries ");
    hadc11cap->Draw("histo");
    */
    TCanvas *c7 = new TCanvas("c7","c7");
    hadc10->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc10->GetYaxis()->SetTitle(" Entries ");
    hadc10->Draw("histo");

    TCanvas *c10 = new TCanvas("c10","c10");
    hadc10_e->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc10_e->GetYaxis()->SetTitle(" Entries ");   
    hadc10_e->Draw("e1");
    hadc10_e->Fit("fitS1","R");


    TCanvas *c11 = new TCanvas("c11","c11");
    hadc10->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc10->GetYaxis()->SetTitle(" Entries ");
    hadc10->Draw("e1");   
    hadc10->Fit("fitS1all","R");
    /*

    TCanvas *c9 = new TCanvas("c9","c9");
    hadc11->SetLineColor(kRed);
    hadc11->Draw("histo");
    hadc11fkst->Draw("histosames");
    c9->BuildLegend();
    
    TCanvas *c8 = new TCanvas("c8","c8");
    hadc11->SetTitle("ADC - SG");
    hadc11->Scale(1/hadc11->GetEntries());
    hadc11PU->Scale(1/hadc11PU->GetEntries());
    hadc11PU->SetLineColor(kRed);
    hadc11->Draw("histo");
    hadc11PU->Draw("histosames");
    c8->BuildLegend();
    
    
    TCanvas *c15 = new TCanvas();
    hclk->Draw("histo");
     c15->SetLogy(1);
    //c4->SaveAs("clkscal.png");
    
    TCanvas *c16 = new TCanvas();
    htdc->Draw("histo");
    c16->SetLogy(1);
    //c5->SaveAs("tdc.png");
    
    TCanvas *c6 = new TCanvas();
    hadc11->Draw("histo");
    c6->BuildLegend();
    //c6->SaveAs("sgboth.png");
    
    wfile->Write();

    */
    TCanvas *c1 = new TCanvas();
      hcorr11->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr11->DrawCopy("COLZ");
    c1->SaveAs("hcorr11.png");

}
