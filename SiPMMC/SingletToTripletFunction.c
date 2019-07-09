//Assume the true emission PSD function
//are true gaussians, estimate the mean with simple hinkley function
Double_t SingletToTripletFunction(Double_t *x, Double_t *par)
{
  //Double_t func = par[0]*exp(-pow((x[0]-par[1]),2.0)/(2*pow(par[2],2.0)));
  Double_t f1 = par[0]*TMath::Binomial(par[1],x[0]);
  Double_t f2 = pow(par[2],(int(x[0])));
  Double_t f3 = pow((1.0-par[2]),(par[1]-int(x[0])));
  Double_t func = f1*f2*f3;

  return func;
}
