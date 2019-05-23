//Define all the variables
double photo_yield = 40.0; //LAr photo yield [photons/keV]
double coll_eff = 1.0; //detector collection efficiency [%]
double LAr_fast_t = 7.0; //LAr fast scintillation time const [ns]
double LAr_slow_t = 1600.0; //LAr slow scintillation time const [ns]
double TPB_fast_t = 10.0; //TPB Fast time constant [ns]
double TPB_int_t = 49.0; //TPB intermediate time constant [ns]
double TPB_long_t = 3550.0; //TPB long time constant [ns]
double TPB_spu_t = 309.0; //TPB spurious time constant [ns]
double TPB_fast_r = 60.0; //abundance percentage
double TPB_int_r = 30.0; //abundance percentage
double TPB_long_r = 8.0; //abundance percentage
double TPB_spu_r = 2.0; //abundance percentage
TRandom3 rnd;
TRandom3 rndCDP;
double overvoltage = 4.0; //Set the SiPM overvoltage [V]
double surfA = 100.0; //SiPM surface area [mm2]
double time_window = 6000.0; //DAQ recording window [ns]
double time_trigger = 2000.0; //DAQ Trigger time [ns]
double time_full_window = time_window+time_trigger; //DAQ full window [ns]
//double pdePDF = 0.2; //PDE for the SIPM, assumed to be flat between 100-300 [nm]
double DN_rate = 0.08; //Hz/mm2
double n_SiPM = 216; //number of SiPM in the experiment
double SiPM_A = 25; //area in mm2
double energy_min = 4.0 //minimum event energy in keV
double energy_max = 30.0 //maximum event energy in keV
