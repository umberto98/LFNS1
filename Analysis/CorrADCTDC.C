#include "Riostream.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TCanvas.h"

void CorrADCTDC () {

    TFile *cfile= new TFile("hanal.root"); //lettura file analisi

    TH1D *hdat10 = (TH1D*)cfile->Get("hadc10");
    TH1D *hdat11 = (TH1D*)cfile->Get("hadc11");
    TH1D *hadc11flag = (TH1D*)cfile->Get("hadc11flag");
    TH1D *htdc = (TH1D*)cfile->Get("htdc");
    TH1D *hclk = (TH1D*)cfile->Get("hclk");

    int bins10 = hdat10->GetNbinsX();
    int bins11 = hdat11->GetNbinsX();
    int tdcbins = htdc->GetNbinsX();
    int clkbins = hclk->GetNbinsX();

    double min10 = hdat10->GetXaxis()->GetXmin();
    double min11 = hdat11->GetXaxis()->GetXmin();
    double mintdc = hclk->GetXaxis()->GetXmin();
    double minclk = htdc->GetXaxis()->GetXmin();

    //vanno tolti overflow
    bins10=bins10-1;
    bins11=bins11-1;
    clkbins=clkbins-1;

    double max10 = hdat10->GetXaxis()->GetBinUpEdge(bins10); //cambia il range -> tolgo un bin
    double max11 = hdat11->GetXaxis()->GetBinUpEdge(bins11);
    double maxtdc = hclk->GetXaxis()->GetXmax();
    double maxclk = htdc->GetXaxis()->GetBinUpEdge(clkbins);

    //dichiarazione 2D
    TH2D *hcorr11 = new TH2D("hcorr11"," Correlation ADC (SG) - TDC ",bins11,min11,max11,tdcbins,mintdc,maxtdc);
    TH2D *hcorr10 = new TH2D("hcorr10"," Correlation ADC (S1) - TDC ",bins10,min10,max10,tdcbins,mintdc,maxtdc);
    TH2D *hcorr11clk = new TH2D("hcorr11clk"," Correlation ADC (SG) - TDC (CLK) ",bins11,min11,max11,clkbins,minclk,maxclk);
    TH2D *hcorr10clk = new TH2D("hcorr10clk"," Correlation ADC (S1) - TDC (CLK) ",bins10,min10,max10,clkbins,minclk,maxclk);

    hcorr11->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr10->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr10->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr11clk->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr11clk->GetYaxis()->SetTitle(" Time [ns] ");
    hcorr10clk->GetXaxis()->SetTitle(" Charge [pC] ");
    hcorr10clk->GetYaxis()->SetTitle(" Time [ns] ");

    //riempimento
    for(int i=1;i<bins10+1;i++) {
        for (int j=1;j<tdcbins+1;j++) hcorr10->SetBinContent(i,j,hdat10->GetBinContent(i)+htdc->GetBinContent(j));
    }

    for(int i=1;i<bins11+1;i++) {
        for (int j=1;j<tdcbins+1;j++) hcorr11->SetBinContent(i,j,hdat11->GetBinContent(i)+htdc->GetBinContent(j));
    }

    for(int i=1;i<bins10+1;i++) {
        for (int j=1;j<clkbins+1;j++) hcorr10clk->SetBinContent(i,j,hdat10->GetBinContent(i)+hclk->GetBinContent(j));
    }

    for(int i=1;i<bins11+1;i++) {
        for (int j=1;j<clkbins+1;j++) hcorr11clk->SetBinContent(i,j,hdat11->GetBinContent(i)+hclk->GetBinContent(j));
    }

    TCanvas *c1 = new TCanvas();
    hcorr11->DrawCopy("COLZ");
    c1->SaveAs("hcorr11.png");

    TCanvas *c2 = new TCanvas();
    hcorr10->DrawCopy("COLZ");
    c2->SaveAs("hcorr10.png");

    TCanvas *c3 = new TCanvas();
    hcorr11clk->DrawCopy("COLZ");
    c3->SaveAs("hcorr11clk.png");

    TCanvas *c4 = new TCanvas();
    hcorr10clk->DrawCopy("COLZ");
    c4->SaveAs("hcorr10clk.png");

}