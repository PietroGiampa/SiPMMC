TGraphErrors* SingletToTripletSCENE(TString interaction)
{

  const int max = 9;
  double energy[max] = {10.3, 14.8, 16.9, 20.5, 25.4, 28.7, 36.1, 49.7, 57.3};
  double energyR[max] = {.5,.5,.5,.5,.5,.5,.5,.5,.5};

  double nuclear[max] = {0.536, 0.576, 0.591, 0.620, 0.642, 0.657, 0.693, 0.712, 0.720};

  double nuclearR[max] ={.03,0.05,0.05,0.01,0.03,0.06,0.03,0.04,0.02};


  TGraphErrors *gr;
  if (interaction=="NR"){gr = new TGraphErrors(max,energy,nuclear,energyR,nuclearR);}
  return gr;
}
