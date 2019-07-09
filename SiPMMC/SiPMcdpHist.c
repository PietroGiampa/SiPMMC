//Define The Correlated Noise PDF based on Fabrice's paper
//This is extracted for the VUV4
//https://arxiv.org/abs/1703.06204

TH1D* SiPMcdpHist()
{
  TFile *fin = TFile::Open("rate.root");
  TH1D *hCDP = (TH1D*)fin->Get("HRate40");

  return hCDP;
}
