#include "Riostream.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"

void swap(double &a,double&b);
void orderdata (std::vector<double> &xdata, std::vector<double> &ydata, std::vector<double> &exdata, std::vector<double> &eydata); //xdata in ascending order
void printvec (std::vector<double> data);

void effsG () {

    std::vector<double> xdata;
    std::vector<double> ydata;
    std::vector<double> exdata;
    std::vector<double> eydata;
    double x,y,ex,ey;

    ifstream data("effdata.dat"); //data file -> xdata + blank space + ydata + blank space + exdata + blank space + eydata

    if (!data) {
        cout<<"Attenzione: il file dati non esiste \n";
        return;
    }
    while(data>>x>>y>>ex>>ey) {
        xdata.push_back(x);
        ydata.push_back(y);
        exdata.push_back(ex);
        eydata.push_back(ey);
    }
    if(xdata.size()!=ydata.size()) {
        cout<<" Attenzione: diverse dimensioni fra dati su x e dati su y\n";
        return;
    }

    orderdata(xdata,ydata,exdata,eydata);
    int size = (int)xdata.size();
    cout<<" Numero di dati: "<<size<<"\n";

    TGraphErrors *g1 = new TGraphErrors(size,&xdata[0],&ydata[0],&exdata[0],&eydata[0]);
    g1->SetMarkerStyle(21);
    g1->SetMarkerSize(0.8);
    g1->SetMarkerColor(kRed);
    g1->SetTitle(" Efficienza SG ");
    g1->GetXaxis()->SetTitle(" HV [V] ");
    g1->GetYaxis()->SetTitle(" #varepsilon ");

    TF1 *f1 = new TF1("f1","[0]/(1+exp([2]-[1]*x))",1000,2500);
    f1->SetParameter(0,0.7);
    f1->SetParameter(1,1);
    f1->SetParameter(2,1400);
    f1->SetLineColor(kBlue);

    gStyle->SetOptFit(1);

    TCanvas *c = new TCanvas();
    g1->Draw("AP");
    //g1->Fit("f1","R");
    c->SaveAs("effsG.png");

} 

void swap(double &a,double&b) {
    double t;
    t = b;
    b = a;
    a = t;
}

void orderdata (std::vector<double> &xdata, std::vector<double> &ydata, std::vector<double> &exdata, std::vector<double> &eydata) {
    for(int i=0;i<(int)xdata.size();i++) {
        for(int j=i+1; j<(int)xdata.size(); j++) {
            if (xdata[j]<xdata[i]) {
                swap(xdata[i],xdata[j]);
                swap(ydata[i],ydata[j]);
                swap(exdata[i],exdata[j]);
                swap(eydata[i],eydata[j]);
            }
        }
    }
}

void printvec (std::vector<double> data) {
    cout<<" v = {";
    for(int i=0;i<(int)data.size();i++) {
        cout<<data[i]<<" ";
    }
    cout<<"}\n";
}
