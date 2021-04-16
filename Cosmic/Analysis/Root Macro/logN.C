\\ Plot of detector's counts vs HV 
#include "Riostream.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TStyle.h"

void swap(double &a,double&b);
void orderdata (std::vector<double> &xdata, std::vector<double> &ydata); //xdata in ascending order
void printvec (std::vector<double> data);

void logN () {

    std::vector<double> xdata;
    std::vector<double> ydata;
    double x,y;

    ifstream data("s1data.dat"); //data file -> xdata + blank space + ydata

    if (!data) {
        cout<<"Attenzione: il file dati non esiste \n";
        return;
    }
    while(data>>x>>y) {
        xdata.push_back(x);
        ydata.push_back(y);
    }
    if(xdata.size()!=ydata.size()) {
        cout<<" Attenzione: diverse dimensioni fra dati su x e dati su y\n";
        return;
    }

    orderdata(xdata,ydata);
    int size = (int)xdata.size();
    cout<<" Numero di dati: "<<size<<"\n";

    TGraph *g1 = new TGraph(size,&xdata[0],&ydata[0]);
    g1->SetMarkerStyle(21);
    g1->SetMarkerSize(0.8);
    g1->SetMarkerColor(kRed);
    g1->SetTitle(" Caratterizzazione S1 ");
    g1->GetXaxis()->SetTitle(" HV [V] ");
    g1->GetYaxis()->SetTitle(" LogN ");

    TCanvas *c = new TCanvas();
    g1->Draw("ACP");
    c->SetLogy(1);
    c->SaveAs("s1.png");

} 

void swap(double &a,double&b) {
    double t;
    t = b;
    b = a;
    a = t;
}

void orderdata (std::vector<double> &xdata, std::vector<double> &ydata) {
    for(int i=0;i<(int)xdata.size();i++) {
        for(int j=i+1; j<(int)xdata.size(); j++) {
            if (xdata[j]<xdata[i]) {
                swap(xdata[i],xdata[j]);
                swap(ydata[i],ydata[j]);
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

