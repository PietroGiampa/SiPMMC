//-----------------------------//
// Pietro Giampa, TRIUMF, 2018 //
//-----------------------------//

//Based on modeling from the ArXiv paper: https://arxiv.org/abs/1511.07718
Double_t LArEmissionSpectrum(Double_t *x, Double_t *par)
{
  Double_t func1 = par[0]*exp(-pow((x[0]-par[1]),2.0)/(2*pow(par[2],2.0)));
  Double_t func2 = par[3]*exp(-pow((x[0]-par[4]),2.0)/(2*pow(par[5],2.0)));
  Double_t func3 = par[6]*exp(-pow((x[0]-par[7]),2.0)/(2*pow(par[8],2.0)));
  Double_t func4 = par[9]*exp(-pow((x[0]-par[10]),2.0)/(2*pow(par[11],2.0)));
  Double_t func = func1+func2+func3+func4;
  return func;
}
