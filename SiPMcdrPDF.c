//Define The Noise Cooreated Pdf based on Fabrices paper
//This is extracted for the VUV4
//https://arxiv.org/abs/1703.06204

Double_t SiPMcdpPDF(Double_t *x, Double_t *par)
{

  //Double_t CDP = par[0] + par[1]*par[2]*exp(-x[0]/par[3]);
  Double_t CDP_gaus = par[0]*exp(-pow((x[0]-par[1]),2.0)/(2*pow(par[2],2.0)));
  Double_t CDP_exp = par[3]*exp(-x[0]/par[4]);
  Double_t CDP_DN = par[5];
  Double_t CDP = CDP_gaus + CDP_exp + CDP_DN;
  return CDP;
  
}
