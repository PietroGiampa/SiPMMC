//-----------------------------//
//                             //
// Pietro Giampa, TRIUMF, 2018 //
//-----------------------------//

//
// CODE OPERATION
//

1) Run the Simulator with the following line:

   root -l 'RunMC.c(TPN on/off, Random SEED, "NR"/"ER")'

2) Graph the simulation data
   root -l 'ReadOutput.c("Data/<filename>")'

//
// Filename
//
   SiPM_"+number of events+"_"+PDE+"_"+collection efficiency+"_"+Seed+"_"+TPBon/TPBoff+"_"+NR/ER+".root"

//---------------------------//
// AVAILABLE FUNCTIONALITIES //
//---------------------------//

0) constants.c
1) DecayFunction.c
2) LArEmissionSpectrum.c
3) SiPMTimeResolution.c
4) SiPMcdpHist.c
5) SiPMcdpPDF.c
6) SingletToTripletFunction.c
7) SingletToTripletSCENE.c
8) SingletToTripletYALE.c

   
