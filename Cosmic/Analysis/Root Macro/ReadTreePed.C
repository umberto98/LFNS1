\\Read TTree from .txt pedestal file
#include "TTree.h"
#include "TBranch.h"
#include "Riostream.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TText.h"

//SENSIBILITA
const double adcsens = 0.25; //-0.25 pC per canale -> prendiamo in modulo
const double tdcsens = 2.5; //2.5 ns per canale
const double clksens = 100; //100 ns per canale

//DYNAMIC RANGE
const int adcdyn = 2048; //max num canali adc
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

int adcinv(double charge) {return (int)(charge/adcsens);} //restituisce i canali

void ReadTreePed () {
    
    typedef struct {
        int clk, scal, inhscal, tdc, adc10, adc11, pu; //clk scaler, scaler, scaler inibito, tdc, adc ch10, adc ch11 e pattern unit
    } Ev;

    static Ev event;

    TFile tfile("MyTree.root"); //in lettura
    TTree *tree = (TTree*)tfile.Get("T");

    TFile *wfile = new TFile("hplot.root","recreate");
    TBranch *b1 = tree->GetBranch("Data");
    b1->SetAddress(&event.clk); 

    TH1D *hadc10 = new TH1D("hadc10"," ADC CH10 - S1 ",adcdyn,0.,adcrng);//s1
    TH1D *hadc11 = new TH1D("hadc11"," ADC CH11 - SG ",adcdyn,0.,adcrng);//sg
    TH1D *hadc11flag = new TH1D("hadc11flag"," ADC CH11 - SG - No Electrons ",adcdyn,0.,adcrng);//sg  
    TH1D *hclk = new TH1D("hclk"," CLK SCALER ",clkdyn,0.,clkrng);//clk scaler
    TH1D *htdc = new TH1D("htdc"," TDC ",tdcdyn,0.,tdcrng);//tdc

    TH2D *hcorr11 = new TH2D("hcorr11"," Correlation ADC (SG) - TDC ",adcdyn,0.,adcrng,tdcdyn,0.,tdcrng);
    TH2D *hcorr10 = new TH2D("hcorr10"," Correlation ADC (S1) - TDC ",adcdyn,0.,adcrng,tdcdyn,0.,tdcrng);
    TH2D *hcorr11clk = new TH2D("hcorr11clk"," Correlation ADC (SG) - TDC (CLK) ",adcdyn,0.,adcrng,clkdyn,0.,clkrng);
    TH2D *hcorr10clk = new TH2D("hcorr10clk"," Correlation ADC (S1) - TDC (CLK) ",adcdyn,0.,adcrng,clkdyn,0.,clkrng);

    hadc10->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc10->GetYaxis()->SetTitle(" Entries ");
    hadc11->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11->GetYaxis()->SetTitle(" Entries ");
    hadc11flag->GetXaxis()->SetTitle(" Charge [pC] ");
    hadc11flag->GetYaxis()->SetTitle(" Entries ");
    hclk->GetXaxis()->SetTitle(" Time [ns] ");
    hclk->GetYaxis()->SetTitle(" Entries ");
    htdc->GetXaxis()->SetTitle(" Time [ns] ");
    htdc->GetYaxis()->SetTitle(" Entries ");

    hcorr11->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr10->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr10->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr11clk->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11clk->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr10clk->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr10clk->GetYaxis()->SetTitle(" Time [ns] ");

    for(int ev=0;ev<tree->GetEntriesFast();ev++) {
        tree->GetEvent(ev);
        if (ev%10000==0) cout<<" Leggendo e processando l'evento "<<ev<<endl; 
        if (event.pu==0) {
            hadc11->Fill(adc(event.adc11));
            hadc10->Fill(adc(event.adc10)); 
            hclk->Fill(clkscal(event.clk));
            htdc->Fill(tdcconv(event.tdc));
            hcorr11->Fill(adc(event.adc11),tdcconv(event.tdc));
            hcorr10->Fill(adc(event.adc10),tdcconv(event.tdc));
            hcorr11clk->Fill(adc(event.adc11),clkscal(event.clk));
            hcorr10clk->Fill(adc(event.adc10),clkscal(event.clk));
        }
        else hadc11flag->Fill(adc(event.adc11));
    }

    char strped10[50]; //val medio piedistallo canale 10
    char strped11[50]; //val medio piedistallo canale 11
    char strped10dx[50]; //corno dx canale 10
    char strped11dx[50]; //corno dx canale 11
    char strped10sx[50]; //corno sx canale 10
    char strped11sx[50]; //corno sx canale 11

    double ped10 = hadc10->GetMean();
    double ped11 = hadc11->GetMean();
    int i10dx = 1;
    int i10sx = 1;
    int i11dx = 1;
    int i11sx = 1;
    double max10dx = 0.;
    double max11dx = 0.;
    double max10sx = 0.;
    double max11sx = 0.;

    for (int i=1;i<adcdyn+1;i++) {
        double cont10 = hadc10->GetBinContent(i);
        double cont11 = hadc11->GetBinContent(i);
        double val10 = hadc10->GetBinCenter(i);
        double val11 = hadc11->GetBinCenter(i);
        if (val10>ped10 && cont10>max10dx) {
            max10dx = cont10;
            i10dx = i;
        }
        if (val10<ped10 && cont10>max10sx) {
            max10sx = cont10;
            i10sx = i;
        }
        if (val11>ped11 && cont11>max11dx) {
            max11dx = cont11;
            i11dx = i;
        }
        if (val11<ped11 && cont11>max11sx) {
            max11sx = cont11;
            i11sx = i;
        }
    }

    double ped10dx = hadc10->GetBinCenter(i10dx);
    double ped11dx = hadc11->GetBinCenter(i11dx);
    double ped10sx = hadc10->GetBinCenter(i10sx);
    double ped11sx = hadc11->GetBinCenter(i11sx);

    sprintf(strped10," PEDESTAL CH10 (MEDIO) = %0.1f [pC] / %d [CH] ",ped10,adcinv(ped10));
    sprintf(strped11," PEDESTAL CH11 (MEDIO) = %0.1f [pC] / %d [CH] ",ped11,adcinv(ped11));
    sprintf(strped10dx," PEDESTAL CH10 (DX) = %0.1f [pC] / %d [CH] ",ped10dx,adcinv(ped10dx));
    sprintf(strped11dx," PEDESTAL CH11 (DX) = %0.1f [pC] / %d [CH] ",ped11dx,adcinv(ped11dx));
    sprintf(strped10sx," PEDESTAL CH10 (SX) = %0.1f [pC] / %d [CH] ",ped10sx,adcinv(ped10sx));
    sprintf(strped11sx," PEDESTAL CH11 (SX) = %0.1f [pC] / %d [CH] ",ped11sx,adcinv(ped11sx));

    cout<<strped10<<endl<<strped11<<endl<<strped10dx<<endl<<strped11dx<<endl<<strped10sx<<endl<<strped11sx<<endl;

    TText *t1 = new TText(0.2,0.2,strped10);
    TText *t2 = new TText(0.2,0.2,strped11);
    TText *t3 = new TText(0.2,0.2,strped10dx);
    TText *t4 = new TText(0.2,0.2,strped11dx);
    TText *t5 = new TText(0.2,0.2,strped10sx);
    TText *t6 = new TText(0.2,0.2,strped11sx);

    hadc10->Rebin(8);
    hadc11->Rebin(8);
    
    gStyle->SetOptStat(0);

    TCanvas *c1 = new TCanvas();
    hadc10->Draw("histo");
    t1->Draw("same");
    t3->Draw("same");
    t5->Draw("same");
    //c1->SaveAs("s1adc.png");

    TCanvas *c2 = new TCanvas();
    hadc11->Draw("histo");
    t2->Draw("same");
    t4->Draw("same");
    t6->Draw("same");
    //c2->SaveAs("sgadc.png");

    wfile->Write();



}