void ReadOutput(TString filename){

  TFile *fileIN = TFile::Open(filename);
  TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");

  Double_t epsd, rpsd, erecoil;
  SiPMmc->SetBranchAddress("epsd",&epsd);
  SiPMmc->SetBranchAddress("rpsd",&rpsd);

  //making a graph of 100 bins from 0 to 1 on the x axis and 100 bins from 0 to 1 on the y axis
  TH2D *hPSD = new TH2D("hPSD","",100,0,1,100,0,1);

  //loop through all of the events and add them to the graph
  for (int i=0; i<1000; i++){
     SiPMmc->GetEntry(i);
     hPSD->Fill(rpsd,epsd);
  }

  TCanvas *c1 = new TCanvas("c1","c1");
  hPSD->GetXaxis()->SetTitle("Rec PSD");
  hPSD->GetYaxis()->SetTitle("True PSD");
  //graph the data
  hPSD->Draw();
  //at an y=x line
  TF1 *line = new TF1("line","x",0,1);
  line->Draw("same");
  //save the image
  c1->SaveAs("Img/TruePSDvsRecPSD.png");

}
