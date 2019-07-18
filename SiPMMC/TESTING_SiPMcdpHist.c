#include "SiPMcdpHist.c"
#include "constants.c"

void TESTING_SiPMcdpHist(){
  TH1D *hCDP = SiPMcdpHist();
  TCanvas *c1 = new TCanvas("c1","c1");
  c1->SetLogy();
  c1->SetLogx();
  hCDP->Draw();
  c1->SaveAs("Img/SiPMcdpHist.png");

  int tw_bin = hCDP->GetXaxis()->FindBin(time_window-time_trigger); //tw: time window; h, hist: histogram
  double cdp_rate = hCDP->Integral(0,tw_bin)/1E9;
  cout << "cdp_rate: " << cdp_rate << endl;
}
