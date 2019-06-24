//TO RUN: define a TString array in the root terminal, then pass that into the function. (I don't know why it doesn't work otherwise)

//Plots multiple simulations, so you can compare the effects of different constants.
//Other than PDE and coll_eff, the files should have the same constants.
//most of this file is copied from ReadOutput.c
//currently have enough colours for 14 files

#include "MakeList.c"
#include <math.h>

void ConstCompare(int len, TString files[len], long total_evts=0){

    //check that they've input total_evts correctly
    for (int i=0;i<len;i++){
	if (files[i].Contains("-") & (total_evts==0)){
	    cout << "If the data file is a combination of multiple simulations, you must set total_evts." << endl;
	    exit(EXIT_FAILURE);
	}
    }

    //Step 1: Plot the first file with title and axis labels
    //A: Read in the data file
    double leak_energy, tot_good_nrg;
    int badPSD;
    constant_list constants;
    long evt_max;
    double energy_min, energy_max, pde, coll_eff, photo_yield;
    int tpb;
    TString evt_type, evt_types[len];

    TFile *fileIN = TFile::Open(files[0]);
    TTree *SiPMmc = (TTree*)fileIN->Get("SiPMmc");
    SiPMmc->SetBranchAddress("leak_energy",&leak_energy);
    SiPMmc->SetBranchAddress("tot_good_nrg",&tot_good_nrg);
    SiPMmc->SetBranchAddress("badPSD",&badPSD);
    SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));

    SiPMmc->GetEntry(0); evt_max = constants.evts;
    if (total_evts==0) total_evts = evt_max;
    SiPMmc->GetEntry(1); energy_min = constants.eMin;
    SiPMmc->GetEntry(2); energy_max = constants.eMax;
    SiPMmc->GetEntry(3); pde = constants.SiPM_pde;
    SiPMmc->GetEntry(4); coll_eff = constants.light_cov;
    SiPMmc->GetEntry(5); photo_yield = constants.yield;
    SiPMmc->GetEntry(6); tpb = constants.tpbOnOff;
    SiPMmc->GetEntry(7); evt_type = constants.recoil;

    evt_types[0] = evt_type;

    //B: format
    //change constants into strings
    TString num = Form("%ld",total_evts);
    TString name_pde = Form("%fd",pde);
    TString name_coll_eff = Form("%fd",coll_eff);
    string OnOff;
    if (tpb==0){OnOff="off";}
    else if (tpb==1){OnOff="on";}

    //get rid of the stats box
    gStyle->SetOptStat(0);
    Color_t colors[14] = {kBlue,kYellow+1,kRed,kGreen,kOrange+10,kBlack,kMagenta,kGray+3,kViolet+3,kOrange+2,kGreen+4,kCyan,kRed+4,kCyan+4};


    //C: graphing
    //Graph1: leakage vs energy
    double numbins = (energy_max-energy_min)*2.0;
    TH1D *hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max);
    TH1D *hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max);
    for (int i=0;i<total_evts;i++){
	SiPMmc->GetEntry(i);
	hLeak->Fill(leak_energy);
	hEnergy->Fill(tot_good_nrg);
    }
    TCanvas *c1 = new TCanvas("c1", "c1");
    hLeak->Sumw2();
    hEnergy->Sumw2();
    TH1D *hLeakEnergy = new TH1D(*hLeak);
    hLeakEnergy->Divide(hEnergy);
    hLeakEnergy->GetYaxis()->SetLimits(0,1);
    hLeakEnergy->SetLineColor(colors[0]);
    hLeakEnergy->SetTitle("Events: "+num+", TPB: "+OnOff+", Type: "+evt_type+"R");
    hLeakEnergy->GetXaxis()->SetTitle("Recoil energy (keV)");
    hLeakEnergy->GetYaxis()->SetTitle("Leakage");
    hLeakEnergy->Draw("E1");
    TLegend *leg1 = new TLegend(.5,.78,.87,.88);
    leg1->AddEntry(hLeakEnergy, "PDE: "+name_pde+" LC: "+name_coll_eff, "L");
    leg1->Draw("same");

    //Graph2: fracBad vs pde
    TCanvas *c2 = new TCanvas("c2", "c2");
    double fracBad = double(badPSD)/double(total_evts);
    double fracErr = fracBad*sqrt(1/badPSD + 1/total_evts);
    TGraphErrors *badPDE = new TGraphErrors(1, &pde, &fracBad, 0, &fracErr);
    if (evt_type=='E') badPDE->SetLineColor(kRed);
    else if (evt_type=='N') badPDE->SetLineColor(kBlack);
    badPDE->SetTitle("Events: "+num+", TPB: "+OnOff);
    badPDE->GetXaxis()->SetLimits(0, 0.6);
//why is this not working?
    badPDE->GetYaxis()->SetLimits(0, 1);
    badPDE->GetXaxis()->SetTitle("PDE");
    badPDE->GetYaxis()->SetTitle("fraction of events with poorly reconstructed PSD");
//why is the point not showing?
    badPDE->Draw("APE");
    TGraphErrors *ER = new TGraphErrors(); ER->SetLineColor(kRed); //stuff of the appropriate colour to add to the legend
    TGraphErrors *NR = new TGraphErrors(); NR->SetLineColor(kBlack);
    TLegend *leg2 = new TLegend(.70,.75,.87,.88);
    leg2->AddEntry(ER, "ER", "L");
    leg2->AddEntry(NR, "NR", "L");
    leg2->Draw("same");

    //Graph3: fracBad vs coll_eff
    TCanvas *c3 = new TCanvas("c3", "c3");
    TGraphErrors *badLC = new TGraphErrors(1, &coll_eff, &fracBad, 0, &fracErr);
    if (evt_type=='E') badLC->SetLineColor(kRed);
    else if (evt_type=='N') badLC->SetLineColor(kBlack);
    badLC->SetTitle("Events: "+num+", TPB: "+OnOff);
    badLC->GetXaxis()->SetLimits(0,1);
//why is this not working?
    badLC->GetYaxis()->SetLimits(0,1);
    badLC->GetXaxis()->SetTitle("Collection Efficiency");
    badLC->GetYaxis()->SetTitle("fraction of events with poorly reconstructed PSD");
//why is the point not showing?
    badLC->Draw("APE");
    leg2->Draw("same");


    //Step 2: read and plot the rest of the files
    for (int i=1;i<len;i++){
	//A: Read in the data file
	fileIN = TFile::Open(files[i]);
	SiPMmc = (TTree*)fileIN->Get("SiPMmc");
	SiPMmc->SetBranchAddress("leak_energy",&leak_energy);
	SiPMmc->SetBranchAddress("tot_good_nrg",&tot_good_nrg);
	SiPMmc->SetBranchAddress("badPSD",&badPSD);
	SiPMmc->SetBranchAddress("constants", (Long64_t*)(&constants));

	SiPMmc->GetEntry(0); evt_max = constants.evts;
	if (total_evts==0) total_evts = evt_max;
	SiPMmc->GetEntry(1); energy_min = constants.eMin;
	SiPMmc->GetEntry(2); energy_max = constants.eMax;
	SiPMmc->GetEntry(3); pde = constants.SiPM_pde;
	SiPMmc->GetEntry(4); coll_eff = constants.light_cov;
	SiPMmc->GetEntry(5); photo_yield = constants.yield;
	SiPMmc->GetEntry(6); tpb = constants.tpbOnOff;
	SiPMmc->GetEntry(7); evt_type = constants.recoil;

	evt_types[i] = evt_type;

	//B: format
	//change constants into strings
	TString num = Form("%ld",total_evts);
	TString name_pde = Form("%fd",pde);
	TString name_coll_eff = Form("%fd",coll_eff);
	string OnOff;
	if (tpb==0){OnOff="off";}
	else if (tpb==1){OnOff="on";}

	//C: graphing
	//Graph1: leakage vs energy
	hLeak = new TH1D("hLeak","",numbins,energy_min,energy_max); //get rid of previous graphs
	hEnergy = new TH1D("hEnergy","",numbins,energy_min,energy_max);
	for (int i=0;i<total_evts;i++){
	    SiPMmc->GetEntry(i);
	    hLeak->Fill(leak_energy);
	    hEnergy->Fill(tot_good_nrg);
	}
	c1->cd();
	hLeak->Sumw2();
	hEnergy->Sumw2();
	hLeakEnergy = new TH1D(*hLeak);
	hLeakEnergy->Divide(hEnergy);
	hLeakEnergy->SetLineColor(colors[i]);
	hLeakEnergy->Draw("E1same");
	leg1->AddEntry(hLeakEnergy, "PDE: "+name_pde+" LC: "+name_coll_eff, "L");
	leg1->Draw("same");

	//Graph2: fracBad vs PDE
	c2->cd();
	fracBad = double(badPSD)/double(total_evts);
	fracErr = fracBad*sqrt(1/badPSD + 1/total_evts);
	badPDE = new TGraphErrors(1, &pde, &fracBad, 0, &fracErr);
	if (evt_type=='E') badPDE->SetLineColor(kRed);
	else if (evt_type=='N') badPDE->SetLineColor(kBlack);
	badPDE->Draw("APE");

	//Graph3: fracBad vs coll_eff
	c3->cd();
	badLC = new TGraphErrors(1, &coll_eff, &fracBad, 0, &fracErr);
	if (evt_type=='E') badLC->SetLineColor(kRed);
	else if (evt_type=='N') badLC->SetLineColor(kBlack);
	badLC->Draw("APE");
    }


    //Step 3: save stuff
    gSystem->Exec("mkdir Img/ConstCompare");
    if (find(evt_types, evt_types+len, 'E')<evt_types+len) c1->SaveAs("Img/ConstCompare/Evts:"+num+"TPB:"+OnOff+"__LeakageVsEnergy.png");
    c2->SaveAs("Img/ConstCompare/Evts:"+num+"TPB:"+OnOff+"__badPSDvsPDE.png");
    c3->SaveAs("Img/ConstCompare/Evts:"+num+"TPB:"+OnOff+"__badPSDvsLC.png");
}
