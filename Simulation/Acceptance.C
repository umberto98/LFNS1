#include "Riostream.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TH1D.h"
#include "TLatex.h"
#include <math.h> //contiene cbrt

//SCINTILLATORE 1 -> S1
const double lungh1 = 403; //mm
const double largh1 = 111;
const double spess1 = 18;

//VETRO SCINTILLANTE -> SG
const double lunghg = 600;
const double larghg = 170;
const double spessg = 170;

//SCINTILLATORE 2 -> S2
const double lungh2 = 735;
const double largh2 = 161;
const double spess2 = 10;

const double z1g = 181; //distanza s1 e sg
const double z2g = 59; //distanza sg e s2

const double inters1 = spess1/2.+z1g+spessg/2.; //arriva da metà di S1 a metà di SG
const double inters2 = inters1+spessg/2.+z2g+spess2; //arriva fino alla fine di S2

const int kNorm = 1e7; //numero di simulazioni
const double block = 0.10; //10% di muoni vengono bloccati in SG
const bool blkbool = false; //false se non si vuole la correzione su SG

double cosinv (); //estrae il coseno
void transp (double &x0,double &y0, double &z0, double costh, double phi,double dz); //trasporto muone

void Acceptance () {

  cout<<"\n//-------------------------------------------------//\n";
  cout<<" CALCOLO ACCETTANZA S1 - SG - S2 \n";
  cout<<"//-------------------------------------------------//\n";
  
  cout<<"\n//-------------------------------------------------//\n";
  cout<<" DATI S1 - SG - S2 \n";
  cout<<"//-------------------------------------------------//\n";
  cout<<"- Lunghezza S1 [mm] - "<<lungh1<<endl;
  cout<<"- Larghezza S1 [mm] - "<<largh1<<endl;
  cout<<"- Spessore S1 [mm] - "<<spess1<<endl;
  cout<<"- Lunghezza SG [mm] - "<<lunghg<<endl;
  cout<<"- Larghezza SG [mm] - "<<larghg<<endl;
  cout<<"- Spessore SG [mm] - "<<spessg<<endl;
  cout<<"- Lunghezza S2 [mm] - "<<lungh2<<endl;
  cout<<"- Larghezza S2 [mm] - "<<largh2<<endl;
  cout<<"- Spessore S2 [mm] - "<<spess2<<endl;
  cout<<"- Distanza S1 (bottom) - SG (Up) [mm] - "<<z1g<<endl;
  cout<<"- Distanza S2 (up) - SG (Bottom) [mm] - "<<z2g<<endl;
  cout<<"- Intersezione in SG [mm] "<<inters1<<endl;
  cout<<"- Intersezione in S2 [mm] "<<inters2<<endl;

  int cnt1 = 0;
  int cntg = 0;
  int cnt2 = 0;

  double rng = TMath::Pi();
  int bins = 100;
  double step = rng/bins;

  TH1D *h1 = new TH1D("h1"," Distribuzione Teorica Muoni ",bins,-step/2.-rng/2.,rng/2.+step/2.);
  h1->GetXaxis()->SetTitle(" #theta [RAD] ");
  h1->GetYaxis()->SetTitle(" #frac{dN}{d#theta} ");
  double integ = (4*TMath::Pi())/3.;

  //double integ = 0.;
  for (double x=-rng/2;x<=rng;x=x+step) {
    if (!(x>0 && x<step)) h1->Fill(x,TMath::Cos(x)*TMath::Cos(x));
    //integ = integ + TMath::Cos(x)*TMath::Cos(x)*step;
  }
  h1->Scale(1./integ);
  h1->Draw("hist");

  cout<<"\n//-------------------------------------------------//\n";
  cout<<" INIZIO SIMULAZIONE \n";
  cout<<"//-------------------------------------------------//\n";
  cout<<"- Correzione su eventi fermati in SG? ";
  if (blkbool) cout<<" Sì - "<<(double)block*100<<" % \n";
  else cout<<" No \n";
  cout<<"- Numero Eventi "<<kNorm<<endl<<endl;

  for (int i=0;i<kNorm;i++) {

    if (i%1000000==0) cout<<" Evento "<<i<<endl;

    cnt1++; //punto su S1

    //PUNTO INIZIALE (ORIGINE NEL BARICENTRO DI S1)
    double x = -lungh1/2. + lungh1*gRandom->Rndm(); //fra -l1/2 e +l1/2
    double y = -largh1/2. + largh1*gRandom->Rndm();
    double z = -spess1/2. + spess1*gRandom->Rndm(); 

    //double phi = -TMath::Pi() + 2.*TMath::Pi()*gRandom->Rndm(); //phi
    //double costh = TMath::Cos(h1->GetRandom()); //theta

    //double phi = -TMath::Pi() + 2.*TMath::Pi()*gRandom->Rndm(); //phi
    //double costh = cbrt(gRandom->Rndm());

    double phi = -TMath::Pi() + 2.*TMath::Pi()*gRandom->Rndm(); //phi
    double costh = cbrt(-1.+2*gRandom->Rndm());

    //TRASPORTO S1->SG
    transp(x,y,z,costh,phi,inters1); //deve arrivare all'inizio di SG
    if (TMath::Abs(x)<lunghg/2. && TMath::Abs(y)<larghg/2.) { //c'è hit sia su S1 che SG
      cntg++; //punto su SG
      transp(x,y,z,costh,phi,inters2); //deve percorrere tutto SG e la distanza SG-S2
      if (TMath::Abs(x)<lungh2/2. && TMath::Abs(y)<largh2/2.) cnt2++; //c'è hit su S1, SG e S2
    }
  }

  cout<<"\n//-------------------------------------------------//\n";
  cout<<" RISULTATI \n";
  cout<<"//-------------------------------------------------//\n";
  cout<<" ACCETTANZA \n - Hit Tripla: "<<100*(double)cnt2/cnt1<<" % \n - Hit Doppia (S1-SG): "<<100*(double)cntg/cnt1;
  if (blkbool) cntg = cntg - block*cntg; //in percentuale block*cntg vengono frenati da SG
  cout<<" % \n - Hit su S2 con Hit S1-SG : "<<100*(double)cnt2/cntg<<" % \n ";

}

void transp (double &x0,double &y0, double &z0, double costh, double phi,double dz) {
  
  double c1 = TMath::Sqrt(1-costh*costh)*TMath::Cos(phi);
  double c2 = TMath::Sqrt(1-costh*costh)*TMath::Sin(phi);
  double c3 = costh;

  //zfin = zin+c3*t -> t = [...]
  double t = (dz-z0)/c3;

  //aggiorno le coordinate
  x0 = x0 + c1*t;
  y0 = y0 + c2*t;
  z0 = z0 + c3*t;

}
