\\Study on detector efficiency
#include "Riostream.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TF1.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"

void swap(double &a,double&b);
void orderdata (std::vector<double> &xdata, std::vector<double> &ydata, std::vector<double> &exdata, std::vector<double> &eydata); //xdata in ascending order
void printvec (std::vector<double> data);

const int fitdata = 5; //escludo gli ultimi 5 dati

void Efficiency () {

    std::vector<double> xdata;
    std::vector<double> ydata;
    std::vector<double> exdata;
    std::vector<double> eydata;

    std::vector<double> xdataex;
    std::vector<double> ydataex;
    std::vector<double> exdataex;
    std::vector<double> eydataex;

    std::vector<double> newxdata;
    std::vector<double> newydata;
    std::vector<double> newexdata;
    std::vector<double> neweydata;

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

    cout<<" Dati X:";
    printvec(xdata);
    cout<<" Dati Y:";
    printvec(ydata);

    for(int i=0;i<size;i++) {

        if(i<(size-fitdata)) {
            newxdata.push_back(xdata[i]);
            newydata.push_back(ydata[i]);
            newexdata.push_back(exdata[i]);
            neweydata.push_back(eydata[i]);
        }
        else {
            xdataex.push_back(xdata[i]);
            ydataex.push_back(ydata[i]);
            exdataex.push_back(exdata[i]);
            eydataex.push_back(eydata[i]);
        }

    }

    cout<<" Nuovi Dati X:";
    printvec(newxdata);
    cout<<" Nuovi Dati Y:";
    printvec(newydata);

    cout<<" Dati Rimossi X:";
    printvec(xdataex);
    cout<<" Dati Rimossi Y:";
    printvec(ydataex);

    TGraphErrors *g1 = new TGraphErrors(size-fitdata,&newxdata[0],&newydata[0],&newexdata[0],&neweydata[0]);
    g1->SetMarkerStyle(21);
    g1->SetMarkerSize(0.8);
    g1->SetMarkerColor(kGreen);

    TGraphErrors *g2 = new TGraphErrors(fitdata,&xdataex[0],&ydataex[0],&exdataex[0],&eydataex[0]);
    g2->SetMarkerStyle(5);
    g2->SetMarkerSize(0.8);
    g2->SetMarkerColor(kRed);

    TMultiGraph *gtot = new TMultiGraph();
    gtot->Add(g1);
    gtot->Add(g2);
    gtot->SetTitle(" Efficienza SG ");
    gtot->GetXaxis()->SetTitle(" HV [V] ");
    gtot->GetYaxis()->SetTitle(" #varepsilon ");

    TF1 *f1 = new TF1("f1","[0]/(1+exp((x-[1])/[2]))",newxdata[0]-10,xdataex[0]-10);
    f1->SetParameter(0,0.7);
    f1->SetParameter(1,1300);
    f1->SetParameter(2,-50);
    f1->SetLineColor(kBlue);

    gStyle->SetOptFit(1);

    TCanvas *c = new TCanvas();
    gtot->Draw("AP");
    g1->Fit("f1","R");
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
    cout<<" v = { ";
    for(int i=0;i<(int)data.size();i++) {
        cout<<data[i]<<" ";
    }
    cout<<"}\n";
}
