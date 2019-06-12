#include "MakeList.c"

void TESTING_photo_yield_read(){
  //read in the data file
  TFile *fileIN = TFile::Open("Data/yield_test");
  TTree *test_yield = (TTree*)fileIN->Get("yield_test");

  constant_list constants;

  test_yield->SetBranchAddress("constants", (Long64_t*)(&constants));

  test_yield->GetEntry(1); double new_energy_min = constants.eMin;
  test_yield->GetEntry(5); double new_photo_yield = constants.yield;

  cout << "photo_yield saved in file " << new_photo_yield << endl;
  cout << "energy_min saved in file " << new_energy_min << endl;
}

