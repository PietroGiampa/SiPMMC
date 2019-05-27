//Define the time response function for the SIPM
//This gives you the ability of recording a pulse
//Time spread basically.
//NEEDS TO BE FIXED
Double_t SiPMTimeResolution(Double_t *x, Double_t *par)
{
  Double_t func = par[0]*exp(-pow((x[0]-par[1]),2.0)/(2*pow(par[2],2.0)));
  return func;
}
