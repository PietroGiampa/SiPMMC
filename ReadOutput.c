void ReadOutput(const char * filename){
  //copied from https://root.cern.ch/doc/v612/ntuple1_8C.html-----------//
  gInterpreter->LoadMacro(filename);
  //--------------------------------------------------------------------//

//  TCanvas *c1 = new TCanvas("c1","The Ntuple canvas",200,10,700,780);
//
//  //PSD vs energy
//  const char varexp1 = epsd:erecoil
//  const char selection1 = ?
//  Option_t option1 = "AP"
//  TTree::Draw(*varexp1, *selection1, *option1);
}
