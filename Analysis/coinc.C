#include "Riostream.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TF1.h"

void swap(double &a,double&b);
void orderdata (std::vector<double> &xdata, std::vector<double> &ydata, std::vector<double> &exdata, std::vector<double> &eydata); //xdata in ascending order
void printvec (std::vector<double> data);

const double dx = 0.; //limite DESTRO per la curva SINISTRA
const double sx = 0.; //limite SINISTRO per la curva DESTRA


void coinc () {

    std::vector<double> xdata;
    std::vector<double> ydata;
    std::vector<double> exdata;
    std::vector<double> eydata;

    std::vector<double> rightxdata;
    std::vector<double> rightydata;
    std::vector<double> rightexdata;
    std::vector<double> righteydata;

    std::vector<double> leftxdata;
    std::vector<double> leftydata;
    std::vector<double> leftexdata;
    std::vector<double> lefteydata;

    double x,y,ex,ey;

    ifstream data("coincdata.dat"); //data file -> xdata + blank space + ydata + blank space + exdata + blank space + eydata

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

    for(int i=0;i<size;i++) {
        if (xdata[i]<dx) {
            leftxdata.push_back(xdata[i]);
            leftydata.push_back(ydata[i]);
            leftexdata.push_back(exdata[i]);
            lefteydata.push_back(eydata[i]);
        }
        if (xdata[i]>sx) {
            rightxdata.push_back(xdata[i]);
            rightydata.push_back(ydata[i]);
            rightexdata.push_back(exdata[i]);
            righteydata.push_back(eydata[i]);
        }
    }

    int size2 = (int)rightxdata.size();
    int size3 = (int)leftxdata.size();

    TGraphErrors *g1 = new TGraphErrors(size,&xdata[0],&ydata[0],&exdata[0],&eydata[0]);
    g1->SetTitle(" Coincidence Curve ");
    g1->GetXaxis()->SetTitle(" Delay [ns] ");
    g1->GetYaxis()->SetTitle(" Rate [Hz] ");
    g1->SetMarkerColor(kViolet);
    g1->SetMarkerStyle(20);
    g1->SetMarkerSize(0.7);

    TGraphErrors *g2 = new TGraphErrors(size2,&rightxdata[0],&rightydata[0],&rightexdata[0],&righteydata[0]);
    g2->SetTitle(" Coincidence Curve - Parte Destra ");
    g2->GetXaxis()->SetTitle(" Delay [ns] ");
    g2->GetYaxis()->SetTitle(" Rate [Hz] ");
    g2->SetMarkerColor(kBlack);
    g2->SetMarkerStyle(20);
    g2->SetMarkerSize(0.7);

    TGraphErrors *g3 = new TGraphErrors(size3,&leftxdata[0],&leftydata[0],&leftexdata[0],&lefteydata[0]);
    g3->SetTitle(" Coincidence Curve - Parte Sinistra ");
    g3->GetXaxis()->SetTitle(" Delay [ns] ");
    g3->GetYaxis()->SetTitle(" Rate [Hz] ");
    g3->SetMarkerColor(kBlue);
    g3->SetMarkerStyle(20);
    g3->SetMarkerSize(0.7);

    TF1 *f1 = new TF1("f1","[5]+[0]/((1+exp((x-[1])/[2]))*(1+exp((-x-[3])/[4])))",-90,90);
    f1->SetParameter(0,2.5);
    f1->SetParameter(1,40);
    f1->SetParameter(3,50);
    f1->SetParameter(2,4);
    f1->SetParameter(4,4); 
    f1->SetParameter(5,0);
    f1->SetParLimits(5,0,1);

    TF1 *f2 = new TF1("f2","[3]+[0]/(1+exp((x-[1])/[2]))",sx-5,90);
    f2->SetParameter(0,2.5);
    f2->SetParameter(1,35);
    f2->SetParameter(2,4); 
    f2->SetParameter(3,0);
    f2->SetParLimits(3,0,1);

    TF1 *f3 = new TF1("f3","[3]+[0]/(1+exp((-x-[1])/[2]))",-90,dx+5);
    f3->SetParameter(0,2.5);
    f3->SetParameter(1,50);
    f3->SetParameter(2,4); 
    f3->SetParameter(3,0);
    f3->SetParLimits(3,0,1);

    gStyle->SetOptFit(1);

    TCanvas *c1 = new TCanvas();
    g1->Draw("AP");
    g1->Fit("f1","R");
    c1->SaveAs("coinctot.png");

    cout<<" Larghezza Curva = ("<<f1->GetParameter(1)+f1->GetParameter(3)<<" +- "<<f1->GetParError(1)+f1->GetParError(3)<<") ns\n";

    TCanvas *c2 = new TCanvas();
    g2->Draw("AP");
    g2->Fit("f2","R");
    c2->SaveAs("coincdx.png");

    TCanvas *c3 = new TCanvas();
    g3->Draw("AP");
    g3->Fit("f3","R");
    c3->SaveAs("coincsx.png");

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

