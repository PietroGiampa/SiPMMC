TString GetPDEAsString(double pde)
{
  TString npde = "";
  if (pde==1.0) {npde="1p0";}
  if (pde==0.6) {npde="0p6";}
  if (pde==0.4) {npde="0p4";}
  if (pde==0.1) {npde="0p1";}
  if (pde==0.2) {npde="0p2";}
  return npde;
}
