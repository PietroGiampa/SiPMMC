#include "SiPMcdpHist.c"
#include "constants.c"

void TESTING_SiPMcdpHist(){
  TH1D *hCDP = SiPMcdpHist();
  TCanvas *c1 = new TCanvas("c1","c1");
  c1->SetLogy();
  c1->SetLogx();
  hCDP->Draw();
  c1->SaveAs("Img/SiPMcdpHist.png");
}
