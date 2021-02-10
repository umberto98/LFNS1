#include "Riostream.h"
#include "TMath.h"
#include "TRandom3.h"
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

const int kNorm = 1e7; //numero di simulazioni

double cosinv (); //estrae il coseno
void transp (double &x0,double &y0, double &z0, double costh, double phi,double dz); //trasporto muone

const double pi = TMath::Pi();

void Acceptance () {

  cout<<"//-------------------------------------------------//\n";
  cout<<" CALCOLO ACCETTANZA S1 - SG - S2 \n";
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

  int cnt1 = 0;
  int cntg = 0;
  int cnt2 = 0;

  for (int i=0;i<kNorm;i++) {

    if (i%1000000==0) cout<<" Evento "<<i<<endl;

    cnt1++; //punto su S1

    //PUNTO INIZIALE (ORIGINE NEL BARICENTRO DI S1)
    double x = -lungh1/2. + lungh1*gRandom->Rndm(); //fra -l1/2 e +l1/2
    double y = -largh1/2. + largh1*gRandom->Rndm();
    double z = -spess1/2. + spess1*gRandom->Rndm(); 

    double phi = 2.*TMath::Pi()*gRandom->Rndm(); //phi
    double costh = cosinv(); //costheta

    //TRASPORTO S1->SG
    transp(x,y,x,costh,phi,z1g+spess1/2.); //deve arrivare all'inizio di SG
    if (TMath::Abs(x)<lunghg/2. && TMath::Abs(y)<larghg/2.) { //c'è hit sia su S1 che SG
      cntg++; //punto su SG
      transp(x,y,x,costh,phi,z1g+spess1/2.+spessg+z2g); //deve percorrere tutto SG e la distanza SG-S2
      if (TMath::Abs(x)<lungh2/2. && TMath::Abs(y)<largh2/2.) cnt2++; //c'è hit su S1, SG e S2
    }
  }

  cout<<" ACCETTANZA \n - Hit Tripla: "<<100*(double)cnt2/cnt1<<" % \n - Hit Doppia: "<<100*(double)cntg/cnt1<<" % \n - Hit su S2 con Hit S1-SG : "<<100*(double)cnt2/cntg<<" % \n ";


}

double cosinv () { //estrae costheta
  double costh;
  do {
    double u = gRandom->Rndm(); //cumulativa
    costh = cbrt(u); //la distr è cos quadro theta -> per estrarre costh serve la radice cubica
  } while (costh<0); //da 90 a 180 tornerebbero indietro
  return costh;
}

void transp (double &x0,double &y0, double &z0, double costh, double phi,double dz) {
  
  double c1 = TMath::Sqrt(1-costh*costh)*TMath::Cos(phi);
  double c2 = TMath::Sqrt(1-costh*costh)*TMath::Sin(phi);
  double c3 = costh;

  //zfin = zin+c3*t -> t = [...]
  double t = (dz-z0)/c3;

  /*double delta = (x0*c1 + y0*c2)*(x0*c1 + y0*c2) - (c1*c1 + c2*c2)*(x0*x0 + y0*y0 - dz*dz);

  //check
  if(delta<0) cout<<"Attenzione! Discriminante negativo!"<<endl;

  //prendiamo la sol positiva
  double t = (-(x0*c1 + y0*c2) + TMath::Sqrt(delta) )/(c1*c1 + c2*c2);
  if(t<0) t = (-(x0*c1 + y0*c2)-TMath::Sqrt(delta))/(c1*c1 + c2*c2);
  if(t<0) cout<<"Attenzione! 2 soluzioni negative!"<<endl;
  */

  //aggiorno le coordinate
  x0 = x0 + c1*t;
  y0 = y0 + c2*t;
  z0 = z0 + c3*t;

}
