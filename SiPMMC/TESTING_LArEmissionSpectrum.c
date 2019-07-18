#include "LArEmissionSpectrum.c"

void TESTING_LArEmissionSpectrum(){
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

	TCanvas *c1 = new TCanvas("c1","c1");
	LArEmission->Draw();

	TCanvas *c2 = new TCanvas("c2","c2");
	c2->SetLogy();
	LArEmission->Draw();

	cout << "Proportion of area between 110 and 146 nm: " << LArEmission->Integral(110,146)/LArEmission->Integral(100,300) << endl;
}
