//the items in constant_list need to be in order of descending size, or it won't read properly (long, double, int, char)

struct constant_list {
  long evts;
  double eMin;
  double eMax;
  double SiPM_pde;
  double light_cov;
  double yield;
  int tpbOnOff;
  char recoil;
};

constant_list MakeList(long evt_max, double energy_min, double energy_max, double pde, double coll_eff, double photo_yield, int IsTPBon, char recoil_type){
  constant_list output;

  output.evts = evt_max;
  output.eMin = energy_min;
  output.eMax = energy_max;
  output.SiPM_pde = pde;
  output.light_cov = coll_eff;
  output.yield = photo_yield;
  output.tpbOnOff = IsTPBon;
  output.recoil = recoil_type;

  return output;

}
