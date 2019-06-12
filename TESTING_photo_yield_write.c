#include "MakeList.c"
#include "constants.c"

void TESTING_photo_yield_write(){

  //save photo_yield in a data file
  cout << "photo_yield from constants.c " << photo_yield << endl;
  cout << "energy_min from constants.c " << energy_min << endl;

  TFile *fout = TFile::Open("Data/yield_test","recreate");

  TTree *yield_test = new TTree("yield_test", "");

//  yield_test->Branch("photo_yield",&photo_yield);
  double evt_max = 2;
  char recoil_type = 'N';
  int IsTPBon = 0;
  constant_list this_run = MakeList(evt_max, energy_min, energy_max, pde, coll_eff, photo_yield, IsTPBon, recoil_type);
  yield_test->Branch("constants", &this_run, "evt_max/L:energy_min/D:energy_max/D:pde/D:coll_eff/D:photo_yield/D:tpb/I:evt_type/C");

//  cout << "photo_yield Branch"; yield_test->Scan("*");

  yield_test->Write();
  fout->Close();
}
