//Standard function for the scintillation emission time of individual photons
//assume standard exponential decay function
//Standard function for the TPB re-emission time constant.
//This is model for 4 different time constans as model by the
//Segreto paper:https://arxiv.org/abs/1411.4524 
Double_t DecayFunction(Double_t *x, Double_t *par)
{
  Double_t func = par[0]*exp(-0.693*x[0]/par[1]);
  return func;
}
