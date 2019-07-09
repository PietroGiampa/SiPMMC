//This will be an attempt to implement ReadOutput.c using the same strategy as ConstCompare.c. If successful, this file can be merged with ConstCompare.c.
//TO RUN: define a TString array in the root terminal, then pass that into the function. (I don't know why it doesn't work otherwise)

#include "MakeList.c"
#include "SingletToTripletYALE.c"
#include "GetFloatAsString.c"

void ReadOutputNew(int len, TString files[len], int heat_map=0, long total_evts=0){

    //check that they've input total_evts correctly
    for (int i=0;i<len;i++){
        if (files[i].Contains("-") & (total_evts==0)){
            cout << "If the data file is a combination of multiple simulations, you must set total_evts." << endl;
            exit(EXIT_FAILURE);
        }
    }

    //Step 1: Plot the first file with title and axis labels
    //A: Read in the data file
    double tru_psd, rec_psd, residual, erecoil, leak_energy, tot_good_nrg;
    int n_coll_p, badPSD;
    constant_list constants;
    long evt_max;
    double energy_min, energy_max, pde, coll_eff, photo_yield;
    int tpb;
    TString evt_type;
    TFile *fileIN;
    TTree *SiPMmc;

    fileIN = TFile::Open(files[0]);
    SiPMmc = (TTree*)fileIN->Get("SiPMmc");
    SiPMmc->SetBranchAddress("tru_psd",&tru_psd);
    SiPMmc->SetBranchAddress("rec_psd",&rec_psd);
    SiPMmc->SetBranchAddress("residual",&residual);
    SiPMmc->SetBranchAddress("n_coll_p",&n_coll_p);
    SiPMmc->SetBranchAddress("erecoil",&erecoil);
    SiPMmc->SetBranchAddress("leak_energy",&leak_energy);
    SiPMmc->SetBranchAddress("tot_good_nrg",&tot_good_nrg);
    SiPMmc->SetBranchAddress("badPSD",&badPSD);
    SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));
    //load the constants
    //I'm not sure if the SiPMmc->GetEntry(n) is necessary, but as long as it works I won't change it.
    //if using two files or only using ER, use the constants from ER
    SiPMmc->GetEntry(0); evt_max = constants.evts;
    if (total_evts==0) total_evts = evt_max;
    SiPMmc->GetEntry(1); energy_min = constants.eMin;
    SiPMmc->GetEntry(2); energy_max = constants.eMax;
    SiPMmc->GetEntry(3); pde = constants.SiPM_pde;
    SiPMmc->GetEntry(4); coll_eff = constants.light_cov;
    SiPMmc->GetEntry(5); photo_yield = constants.yield;
    SiPMmc->GetEntry(6); tpb = constants.tpbOnOff;
    SiPMmc->GetEntry(7); evt_type = constants.recoil;

//if there are 2 files, add a mechanism to check if the constants are the same. If so, we compare ER and NR. If not, we compare constants.


    //B: format
    //change constants into strings
    TString num = Form("%ld",total_evts);
    TString name_pde = Form("%fd",pde);
    TString name_coll_eff = Form("%fd",coll_eff);
    string OnOff;
    if (tpb==0) OnOff="off";
    else if (tpb==1) OnOff="on";

    TString directory;
    //if ER/NR comparison
	directory = files[0];
   	directory.ReplaceAll("_ER","");
	directory.ReplaceAll("_NR","");
	directory.ReplaceAll("Data/","");
	directory.ReplaceAll(".root","");
    //if constants comparison
//	directory = "ConstCompare";

    //get rid of the stats box
    gStyle->SetOptStat(0);


    //C: graphing
    TH2D *hPSD = new TH2D("hPSD","",100,-0.02,1.02,100,0,1); //Graph1
    double max_ph = energy_max*photo_yield*coll_eff;
    TH2D *hPhotons = new TH2D("hphotons", "", 100, energy_min, energy_max, 100, 0, max_ph+50); //Graph2
    TH2D *hPSDenergy = new TH2D("hPSDenergy", "", 100, energy_min, energy_max, 100, -0.02, 1.02); //Graph3
    TH1D *hRes = new TH1D("hRes","",100, 0, 1); //Graph4
    double numbins = (energy_max-energy_min)*2.0;
    TH1D *hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max); //Graph5
    TH1D *hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max); //Graph5

    //loop through all of the events and add them to the graphs
    for (int i=0; i<total_evts; i++){
	SiPMmc->GetEntry(i);
	hPSD->Fill(rec_psd,tru_psd);
	hPhotons->Fill(erecoil,n_coll_p);
	hPSDenergy->Fill(erecoil,rec_psd);
	hRes->Fill(residual);
	hLeak->Fill(leak_energy);
	hEnergy->Fill(tot_good_nrg);
    }

    //declaring stuff
    TGraphErrors *ER = new TGraphErrors(); ER->SetFillColor(kRed); //stuff of the appropriate colour to add to the legend
    TGraphErrors *NR = new TGraphErrors(); NR->SetFillColor(kBlack);
    TCanvas *c1, *c2, *c3, *c4, *c5, *c6;
    TLegend *leg1, *leg3, *leg4;
    TF1 *line1;
    TGraph *line3;
    TH1D *hLeakEnergy;
    TString name_fracBad;
    double fracBad;

    //Graph1: true PSD vs recorded PSD
    //if ER/NR comparison
    c1 = new TCanvas("c1","c1");
    hPSD->GetXaxis()->SetTitle("Recorded PSD");
    hPSD->GetYaxis()->SetTitle("True PSD");
    if (evt_type=='E') hPSD->SetMarkerColor(kRed);
    if (len==1){
	hPSD->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
        if (evt_type=='N') hPSDenergy->SetMarkerColor(kBlack);
	if ((heat_map==1) & (evt_type=='N')) gStyle->SetPalette(kPigeon);
    }
    else { //len==2
	hPSD->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
        if (evt_type=='N') hPSDenergy->SetMarkerColorAlpha(kBlack, 0.5); //not working
	//legend
	leg1 = new TLegend(.13,.68,.30,.88);
	leg1->AddEntry(ER, "ER", "F");
	leg1->AddEntry(NR, "NR", "F");
	leg1->Draw();
    }
    if (heat_map==1){
	if (evt_type=='E') gStyle->SetPalette(kCherry);
	TColor::InvertPalette();
	hPSD->Draw("COLZ same");
    }
    else hPSD->Draw("same"); //heat_map==0
    //add a y=x line
    line1 = new TF1("line","x",-0.02,1.02);
    line1->SetLineColor(kBlue);
    line1->Draw("same");

    //Graph2: detected photons vs energy
    //if ER/NR comparison
    c2 = new TCanvas("c2","c2");
    hPhotons->GetXaxis()->SetTitle("Recoil energy (keV)");
    hPhotons->GetYaxis()->SetTitle("Number of collected photons");
    if (evt_type=='E') hPhotons->SetMarkerColor(kRed);
    else hPSD->SetMarkerColor(kBlack); //evt_type=='N'
    if (len==1)	hPhotons->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    else {
	hPhotons->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff); //len==2
	leg1->Draw();
    }
    hPhotons->Draw("same");

    //Graph3: recorded PSD vs energy
    //if ER/NR comparison
    c3 = new TCanvas("c3","c3");
    //add a SingletToTripletYale line; it's stepped because that's how I implemented the leakage cut.
    double energyLine[2*size];
    double nuclearLine[2*size];
    for (int i=0;i<size;i++){
        energyLine[2*i] = energy[i]-0.5;
        energyLine[2*i + 1] = energy[i]+0.5;
        nuclearLine[2*i] = nuclear[i];
        nuclearLine[2*i + 1] = nuclear[i];
    }
    line3 = new TGraph(2*size, energyLine, nuclearLine);
    line3->SetLineColor(kBlue);
    line3->SetLineWidth(3);
    hPSDenergy->GetXaxis()->SetTitle("Recoil energy (keV)");
    hPSDenergy->GetYaxis()->SetTitle("Recorded PSD");
    if (evt_type=='E') hPSDenergy->SetMarkerColor(kRed);
    if (len==1){
        hPSDenergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
	if (evt_type=='N') hPSDenergy->SetMarkerColor(kBlack);
        if ((heat_map==1) & (evt_type=='N')) gStyle->SetPalette(kPigeon);
    }
    else { //len==2
        hPSDenergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff);
	if (evt_type=='N') hPSDenergy->SetMarkerColorAlpha(kBlack, 0.5); //not working
	//legend
        leg3 = new TLegend(.88,.35,.70,.11);
        leg3->AddEntry(ER, "ER", "F");
        leg3->AddEntry(NR, "NR", "F");
	leg3->AddEntry(line3, "50% NR", "L");
        leg3->Draw();
    }
    if (heat_map==1){
        if (evt_type=='E') gStyle->SetPalette(kCherry);
        hPSDenergy->Draw("COLZ same");
    }
    else hPSDenergy->Draw("same"); //heat_map==0

    line3->Draw("same");

    //Graph4: histogram of residual
    c4 = new TCanvas("c4", "c4");
    hRes->GetXaxis()->SetTitle("PSD Residual");
    hRes->GetYaxis()->SetTitle("number of events");
    if (evt_type=='E') hRes->SetLineColor(kRed);
    else hRes->SetLineColor(kBlack); //evt_type=='N'
    if (len==1) hRes->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
    else {
        hRes->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff); //len==2
	leg4 = new TLegend(.88,.72,.74,.89);
	leg4->AddEntry(ER, "ER", "F");
	leg4->AddEntry(NR, "NR", "F");
	leg4->Draw("same");
    }
    hRes->Draw("same");

    //Graph5: leakage vs energy
    c5 = new TCanvas("c5", "c5");
    if (evt_type=='E'){
	//calculate error bars
	hLeak->Sumw2();
	hEnergy->Sumw2();
	hLeakEnergy = new TH1D(*hLeak);
	hLeakEnergy->Divide(hEnergy);
	hLeakEnergy->SetTitle("Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R");
	hLeakEnergy->GetXaxis()->SetTitle("Recoil energy (keV)");
	hLeakEnergy->GetYaxis()->SetTitle("Leakage");
	hLeakEnergy->Draw("E1");
    }

    //Canvas6: Fraction of events with PSD zero or 1
    c6 = new TCanvas("c6", "c6");
    fracBad = double(badPSD)/double(total_evts);
    name_fracBad = GetFloatAsString(fracBad);
    cout << "The fraction of events with PSD zero or one" << endl;
    cout<<"Events: "<<total_evts<<", PDE: "<<pde<<", Collection Efficiency: "<<coll_eff<<", TPB: "<<OnOff<<", Type: "<<evt_type<<"R, fracBad: "<<fracBad<<endl;
//why isn't this showing up?
    TLatex text1(0.25, 0.9, "The fraction of ER events with PSD zero or one");
    text1.Draw();
    TLatex text2(0.25, 0.8, "Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R, fracBad: "+name_fracBad);
    text2.Draw("same");

    //graphs from ConstCompare.c

    //Step 2: read and plot the rest of the files
    for (int i=1;i<len;i++){
	//A: Read in the data file
	fileIN = TFile::Open(files[i]);
	SiPMmc = (TTree*)fileIN->Get("SiPMmc");
	SiPMmc->SetBranchAddress("tru_psd",&tru_psd);
	SiPMmc->SetBranchAddress("rec_psd",&rec_psd);
	SiPMmc->SetBranchAddress("residual",&residual);
	SiPMmc->SetBranchAddress("n_coll_p",&n_coll_p);
	SiPMmc->SetBranchAddress("erecoil",&erecoil);
	SiPMmc->SetBranchAddress("leak_energy",&leak_energy);
	SiPMmc->SetBranchAddress("tot_good_nrg",&tot_good_nrg);
	SiPMmc->SetBranchAddress("badPSD",&badPSD);
	SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));
	//load the constants
	SiPMmc->GetEntry(0); evt_max = constants.evts;
	if (total_evts==0) total_evts = evt_max;
	SiPMmc->GetEntry(1); energy_min = constants.eMin;
	SiPMmc->GetEntry(2); energy_max = constants.eMax;
	SiPMmc->GetEntry(3); pde = constants.SiPM_pde;
	SiPMmc->GetEntry(4); coll_eff = constants.light_cov;
	SiPMmc->GetEntry(5); photo_yield = constants.yield;
	SiPMmc->GetEntry(6); tpb = constants.tpbOnOff;
	SiPMmc->GetEntry(7); evt_type = constants.recoil;

	//B: format
	//change constants into strings
	TString num = Form("%ld",total_evts);
	TString name_pde = Form("%fd",pde);
	TString name_coll_eff = Form("%fd",coll_eff);
	string OnOff;
	if (tpb==0) OnOff="off";
	else if (tpb==1) OnOff="on";

	//C: graphing
	hPSD = new TH2D("hPSD","",100,-0.02,1.02,100,0,1); //Graph1
	max_ph = energy_max*photo_yield*coll_eff;
	hPhotons = new TH2D("hphotons", "", 100, energy_min, energy_max, 100, 0, max_ph+50); //Graph2
	hPSDenergy = new TH2D("hPSDenergy", "", 100, energy_min, energy_max, 100, -0.02, 1.02); //Graph3
	hRes = new TH1D("hRes","",100, 0, 1); //Graph4
	numbins = (energy_max-energy_min)*2.0;
	hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max); //Graph5
	hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max); //Graph5

	//loop through all of the events and add them to the graphs
	for (int i=0; i<total_evts; i++){
	    SiPMmc->GetEntry(i);
	    hPSD->Fill(rec_psd,tru_psd);
	    hPhotons->Fill(erecoil,n_coll_p);
	    hPSDenergy->Fill(erecoil,rec_psd);
	    hRes->Fill(residual);
	    hLeak->Fill(leak_energy);
	    hEnergy->Fill(tot_good_nrg);
	}

	//Graph1: true PSD vs recorded PSD
	//if ER/NR comparison
	c1->cd();
	if (evt_type=='E') hPSD->SetMarkerColor(kRed);
	else if (evt_type=='N') hPSDenergy->SetMarkerColorAlpha(kBlack, 0.5); //not working
	if (heat_map==1){
	    if (evt_type=='E') gStyle->SetPalette(kCherry);
	    hPSD->Draw("COLZ same");
	}
	else hPSD->Draw("same"); //heat_map==0

	//Graph2: detected photons vs energy
	//if ER/NR comparison
	c2->cd();
	if (evt_type=='E') hPSDenergy->SetMarkerColor(kRed);
	else hPSD->SetMarkerColor(kBlack); //evt_type=='N'
	hPhotons->Draw("same");

	//Graph3
	c3->cd();
	if (evt_type=='E') hPSDenergy->SetMarkerColor(kRed);
	else if (evt_type=='N') hPSDenergy->SetMarkerColorAlpha(kBlack, 0.5); //not working
	if (heat_map==1){
	    if (evt_type=='E') gStyle->SetPalette(kCherry);
	    hPSDenergy->Draw("COLZ same");
	}
	else hPSDenergy->Draw("same"); //heat_map==0

	//Graph4: histogram of residual
	c4->cd();
	if (evt_type=='E') hRes->SetLineColor(kRed);
	else hRes->SetLineColor(kBlack); //evt_type=='N'
	hRes->Draw("same");

	//Graph5: leakage vs energy
	c5->cd();
	if (evt_type=='E'){
	    hLeak->Sumw2();
	    hEnergy->Sumw2();
	    hLeakEnergy = new TH1D(*hLeak);
	    hLeakEnergy->Divide(hEnergy);
	    hLeakEnergy->Draw("E1 same");
	}

	//Canvas6: Fraction of events with PSD zero or 1
	c6->cd();
	fracBad = double(badPSD)/double(total_evts);
	name_fracBad = GetFloatAsString(fracBad);
	cout<<"Events: "<<total_evts<<", PDE: "<<pde<<", Collection Efficiency: "<<coll_eff<<", TPB: "<<OnOff<<", Type: "<<evt_type<<"R, fracBad: "<<fracBad<<endl;
//why isn't this showing up?
	TLatex text2(0.25, 0.8-(0.1*i), "Events: "+num+", PDE: "+name_pde+", Collection Efficiency: "+name_coll_eff+", TPB: "+OnOff+", Type: "+evt_type+"R, fracBad: "+name_fracBad);
	text2.Draw("same");

	//graphs from ConstCompare

    }

    //Step 3: save stuff
    gSystem->Exec("mkdir Img/"+directory);
    TString evts;
    if (len==1) evts = evt_type+"R";
    else if (len==2) evts = "ER&NR";
    else evts = "";
    TString heatMap;
    if (heat_map==0) heatMap="";
    else if (heat_map==1) heatMap="_heatMap";

    c1->SaveAs("Img/"+directory+"/"+evts+"__TruePSDvsRecPSD"+heatMap+".png");
    c2->SaveAs("Img/"+directory+"/"+evts+"__PhotonsVsEnergy.png");
    c3->SaveAs("Img/"+directory+"/"+evts+"__RecPSDvsEnergy"+heatMap+".png");
    c4->SaveAs("Img/"+directory+"/"+evts+"__residual.png");
    if (evt_type=='E') c5->SaveAs("Img/"+directory+"/"+evts+"__LeakageVsEnergy.png");
    c6->SaveAs("Img/"+directory+"/"+evts+"__fracBad.png");

}
