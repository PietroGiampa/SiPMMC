void ReadOutput(filename){
  //PSD vs energy
  varexp1 = epsd:erecoil
  selection1 = ?
  option1 = "AP"

  TTree::Draw(*varexp1, *selection1, *option1, nevents1, firstevent1);

  //# of collected photons vs total number of events
  varexp2 = n_coll_p:ievt
  selection2 = ?
  option2 = "AP"
  TTree::Draw(*varexp2, *selection2, *option2, nevents2, firstevent2);
}
