//---------------------------------------//
// Pietro Giampa, TRIUMF, 2018           //
// Edited by Frankie Polak, TRIUMF, 2019 //
//---------------------------------------//
//---------------------------------------//
// How to run the MC:                    //
//   1) int evt_max, set n of sim event  //
//   2) int IsTPBon, set to 1 if there   //
//      is TPB, 0 if there is not        //
//   3) TString evt_type: NR or ER       //
//                                       //
// root -l 'RunMC(0)'                    //
//---------------------------------------//

//Include Function
#include "constants.c"
#include "DecayFunction.c"
#include "LArEmissionSpectrum.c"
#include "SiPMcdpPDF.c"
#include "SiPMcdpHist.c"
#include "GetPDEAsString.c"
#include "SiPMTimeResolution.c"
#include "SingletToTripletFunction.c"
#include "SingletToTripletYALE.c"
#include "SingletToTripletSCENE.c"

void RunMC(int evt_max, int IsTPBon, int seed, TString evt_type)
{

  //--------------------------------------------------------------------//
  // Stage 1, Set Up                                                    //
  //--------------------------------------------------------------------//
  //Get the LAr emission Spectrum
  TF1 *LArEmission = new TF1("LArEmission",LArEmissionSpectrum,100,300,12);
  LArEmission->SetParameter(1,128);
  LArEmission->SetParameter(2,5);
  LArEmission->SetParameter(0,0.999);
  LArEmission->SetParameter(4,158);
  LArEmission->SetParameter(5,4);
  LArEmission->SetParameter(3,0.0002);
  LArEmission->SetParameter(7,195);
  LArEmission->SetParameter(8,30);
  LArEmission->SetParameter(6,0.0002);
  LArEmission->SetParameter(10,272);
  LArEmission->SetParameter(11,20);
  LArEmission->SetParameter(9,0.0002);

  //Set UP the true PSD response function for NR and ER
  TF1 *NRfunc = new TF1("NRfunc",SingletToTripletFunction,0.0,100.0,3);
  NRfunc->SetParameter(1,100);
  NRfunc->SetParameter(2,0.7);
  NRfunc->SetParameter(0,10.0);
  TF1 *ERfunc =new TF1("ERfunc",SingletToTripletFunction,0.0,100.0,3);
  ERfunc->SetParameter(1,100);
  ERfunc->SetParameter(2,0.3);
  ERfunc->SetParameter(0,10.0);

  //Set up the two scintillation time PDFs
  TF1 *fastPDF = new TF1("fastPDF",DecayFunction,0,400,2);
  fastPDF->SetParameter(0,1.0);
  fastPDF->SetParameter(1,LAr_fast_t);
  TF1 *slowPDF = new TF1("slowPDF",DecayFunction,0,time_window,2);
  slowPDF->SetParameter(0,1.0);
  slowPDF->SetParameter(1,LAr_slow_t);

  //Standard function for the TPB re-emission time constant.
  //This is model for 4 different time constants as modelled by the
  //Segreto paper:https://arxiv.org/abs/1411.4524
  TF1 *fastTPB = new TF1("fastTPB",DecayFunction,0,time_window,2);
  fastTPB->SetParameter(0,1.0);
  fastTPB->SetParameter(1,TPB_fast_t);
  TF1 *intTPB = new TF1("intTPB",DecayFunction,0,time_window,2);
  intTPB->SetParameter(0,1.0);
  intTPB->SetParameter(1,TPB_int_t);
  TF1 *longTPB = new TF1("longTPB",DecayFunction,0,time_window,2);
  longTPB->SetParameter(0,1.0);
  longTPB->SetParameter(1,TPB_long_t);
  TF1 *spuTPB = new TF1("spuTPB",DecayFunction,0,time_window,2);
  spuTPB->SetParameter(0,1.0);
  spuTPB->SetParameter(1,TPB_spu_t);

  //Set the SiPM correlated delayed pulse PDF
  //This is based on the following paper:
  //https://arxiv.org/abs/1703.06204
  TF1 *SiPMCDP = new TF1("SiPMCDP",SiPMcdpPDF,0,6000,6);
  SiPMCDP->SetParameter(5,0.005);
  SiPMCDP->SetParameter(0,1.0);
  SiPMCDP->SetParameter(1,22);
  SiPMCDP->SetParameter(2,10);
  SiPMCDP->SetParameter(3,0.05);
  SiPMCDP->SetParameter(4,100);
  TH1D *hCDP = SiPMcdpHist();
  int tw_bin = hCDP->GetXaxis()->FindBin(time_window-time_trigger); //tw: time window; h, hist: histogram
  double cdp_rate = hCDP->Integral(0,tw_bin)/1E9;

  //Set Up DarkNoise ranges
  double final_DN_rate = (DN_rate*n_SiPM*SiPM_A/1E9)*time_window;

  //Set up time response function for the SiPM
  //This is a bit of a SiPM/DAQ related function
  //Smearing to the original "arrival time"
  TF1 *SiPMTimeResPDF = new TF1("SiPMTimeResPDF",SiPMTimeResolution,-3,3,3);
  SiPMTimeResPDF->SetParameter(0,1.0);
  SiPMTimeResPDF->SetParameter(1,0.0);
  SiPMTimeResPDF->SetParameter(2,SiPMTimeResWidth);

  //Define TGraphErrorrs for mean Prompt Ratio
  //of singlet and triplet based on YALE paper
  TGraphErrors *gNR = SingletToTripletYALE("NR");
  TGraphErrors *gNRs = SingletToTripletSCENE("NR");
  TGraphErrors *gER = SingletToTripletYALE("ER");

  //Set Simulation SEED
  rnd.SetSeed(seed);
  rndCDP.SetSeed(seed);

  //Define the Boundaries for reconstructed PSD
  double low_int_bound = time_trigger-4.0;
  double high_int_bound = time_trigger+86.0;

  //Define Ntuple TTree
  //Write output
  TString num = Form("%d",evt_max); //TString::Form formats a string
  TString nSeed = Form("S%d",seed);
  TString name_pde = GetPDEAsString(pde);
  TString tpb = "";
  if (IsTPBon==1){tpb="TPBon";}
  else if (IsTPBon==0){tpb="TPBoff";}
  TString filename = "Data/SiPM_"+num+"_"+name_pde+"_"+nSeed+"_"+tpb+"_"+evt_type+".root";
  TFile *fout = TFile::Open(filename,"recreate");

  //defining variables for the next section
  int ievt, n_scint_p, n_coll_p; //ievt: event number; n_scint_p: number of scintillation photons; n_coll_p: number of collected photons
  double epsd, erecoil, u_pr, rpsd; // epsd: expected (true) PSD, erecoil: recoil energy, u_pr: ?, rpsd: recorded PSD (with noise & stuff added)
  vector<double> pht_wl; //pulse vector? photon wavelength
  vector<double> pht_st; //pulse vector? phton emission time
  //defining a TTree
  TTree *SiPMmc = new TTree("SiPMmc","SiPM LAr Simulation");
  SiPMmc->Branch("ievt",&ievt);
  SiPMmc->Branch("epsd",&epsd);
  SiPMmc->Branch("rpsd",&rpsd);
  SiPMmc->Branch("u_pr",&u_pr);
  SiPMmc->Branch("erecoil",&erecoil);
  SiPMmc->Branch("n_scint_p",&n_scint_p);
  SiPMmc->Branch("n_coll_p",&n_coll_p);
  //SiPMmc->Branch("pht_st",&pht_st);
  //SiPMmc->Branch("pht_wl",&pht_wl);
  //--------------------------------------------------------------------//
  // Stage 2, Event by Event simulation                                //
  //--------------------------------------------------------------------//
  //Loop through all events
  for (int ievt=0; ievt<evt_max; ievt++)
    {

      //Clear Pulse Vector
      pht_wl.clear();
      pht_st.clear();

      //Printout progress in simulation
      if ((ievt%500)==0) {cout << "Event: " << ievt << " out of " << evt_max << endl;}

      //Generate a recoil energy for the event
      //Randomly distributed between energy_min and energy_max
      erecoil = rnd.Uniform(energy_min,energy_max);

      //Generate the true PSD for this event
      if (evt_type=="NR")
	{
	  u_pr = gNR->Eval(erecoil);
	  NRfunc->SetParameter(2,u_pr);
	  epsd = NRfunc->GetRandom()*0.01;
	}
      if (evt_type=="ER")
	{
	  u_pr = gER->Eval(erecoil);
	  ERfunc->SetParameter(2,u_pr);
	  epsd = ERfunc->GetRandom()*0.01;
	}


      //Get the number of scintillation photons produced for the
      //generated recoil energy
      n_scint_p = erecoil*photo_yield;

      //Account for detector collection efficiency
      //This can be changed for different experiments
      n_coll_p = n_scint_p*coll_eff;

      //Define Photons vectors
      //Plus define other variables
      int n_fast_p = n_coll_p*epsd; // number of prompt photon, based on gen PSD

      //Loop through all the generated photons
      //to assign a given wavelength and an emission time
      for (int ipht=0; ipht<n_coll_p; ipht++)
	{
	  //Define some basic variables for definition loop
	  double gp_wl=1.0;
	  double ep_wl=0.0;
	  double gp_st=1.0;
	  double ep_st=0.0;
	  double wl=0.0; //wavelength
	  double st=0.0; //emission time

	  //First off, assign a scintillation wavelength to the photon
	  //Based on the constructed emission spectrum
	  wl = LArEmission->GetRandom();

	  //Assign the emission time to each photon
	  //ratio of single to triplet is selected based
	  //on the simulated true PSD
	  double st_ratio = rnd.Uniform(0.0,1.0); //singlet triplet ratio?
	  if (st_ratio<=epsd){st = fastPDF->GetRandom();}
	  else if (st_ratio>epsd){st = slowPDF->GetRandom();}

	  //If TPB is applied to the detector
	  //Absorb and re-emit the photons based on
	  //Segreto's measurement
	  if (IsTPBon==1){
	    double tpb_emi_prob = rnd.Uniform(0.0,1.0);
	    if (tpb_emi_prob <= TPB_fast_r){st += fastTPB->GetRandom();}
	    if (tpb_emi_prob > TPB_fast_r && tpb_emi_prob <= (TPB_int_r+TPB_fast_r)){st += intTPB->GetRandom();}
	    if (tpb_emi_prob > (TPB_int_r+TPB_fast_r) && tpb_emi_prob <= (TPB_long_r+TPB_int_r+TPB_fast_r)){st += longTPB->GetRandom();}
	    if (tpb_emi_prob > (TPB_long_r+TPB_int_r+TPB_fast_r) && tpb_emi_prob <= 1.0){st += spuTPB->GetRandom();}
	  }

	  //if(ipht<=n_fast_p){st = fastPDF->GetRandom();}
	  //else{st = slowPDF->GetRandom();}

	  //Add the intrinsic resolution time for the SiPM devices
	  double st_offset = SiPMTimeResPDF->GetRandom(-3.0,3.0);
	  st += st_offset;

	  //Add DAQ Trigger time offset
	  st += time_trigger;

	  //Establish the survival probability for the generated photon
	  //based on the simulated emission wavelength and the selected
	  //SiPM PDE PDFs
	  double see_photon = rnd.Uniform(0.0,1.0);
	  if (see_photon<=pde)
	    {
	      pht_wl.push_back(wl);
	      pht_st.push_back(st);
	    }//see-loop
	}//end of ipht-loop

      //--------------------------------------------------------------------//
      // Stage 3, Add Correlated Noise and DN                               //
      //--------------------------------------------------------------------//
      //--------------------------------------------------------------------//
      // Add CDP pulses                                                     //
      //--------------------------------------------------------------------//
      for (int icdp=0; icdp<pht_st.size(); icdp++)
	{
	  double pp_time = pht_st.at(icdp);
	  double gen_p = rndCDP.Uniform(0.0,1.0);
	  double p0 = TMath::Poisson(0,cdp_rate);
	  double p1 = p0 + TMath::Poisson(1,cdp_rate);
	  double p2 = p1 + TMath::Poisson(2,cdp_rate);

	  int n=5;
	  double poisson=1.0;
	  while(1)
	    {
	      poisson = TMath::Poisson(n,cdp_rate);
	      if (gen_p<poisson || n==0){break;}
	      n--;
	    }//end while-loop

	  if (n==0){continue;}
	  else if (n>0)
	    {
	      for (int ap=0; ap<n; ap++)
		{

		  double ap_time = pp_time + hCDP->GetRandom();
		  pht_st.push_back(ap_time);
		}//end for loop
	    }//end if-else loop
	}//end of icdp-loop
      //--------------------------------------------------------------------//
      // Add Dark Noise                                                     //
      //--------------------------------------------------------------------//
      int n_dn = 5;
      double p_dn = rndCDP.Uniform(0.0000,1.0000);
      double poisson_dn = 1.0;

      while(1)
	{
	  poisson_dn = TMath::Poisson(n_dn,final_DN_rate);
	  if (p_dn<poisson_dn || n_dn==0){break;}
	  n_dn--;
	}//end while-loop

      if (n_dn>0)
	{
	  for (int dn=0; dn<n_dn; dn++)
	    {
	      double dn_time = rnd.Uniform(0,time_window);
	      pht_st.push_back(dn_time);
	    }//end for-loop
	}//end if-else

      //Estimate the Reconstructed PSD after the SiPM WF Simulation
      double rec_prompt_count = 0.0;
      double rec_full_count = 0.0;
      for (int kk=0; kk<pht_st.size(); kk++)
	{
	  double single_pulse_time = pht_st.at(kk);
	  if (single_pulse_time>=low_int_bound && single_pulse_time<=high_int_bound){rec_prompt_count++;}
	  if (single_pulse_time>=low_int_bound){rec_full_count++;}
	}//end of kk-loop
      rpsd = rec_prompt_count/rec_full_count;//estimate reconstructed PSD

      //Fill TTree
      SiPMmc->Fill();

    }//end of ievt-loop

  //Write output
  SiPMmc->Write();
  fout->Close();

}//end void
