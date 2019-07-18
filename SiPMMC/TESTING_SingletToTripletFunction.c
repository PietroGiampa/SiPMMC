#include "SingletToTripletFunction.c"
#include "constants.c"
#include "SingletToTripletYALE.c"

void TESTING_SingletToTripletFunction(){
	TCanvas *c1 = new TCanvas("c1","c1");
	TCanvas *c2 = new TCanvas("c2","c2");

	TGraphErrors *gNR = SingletToTripletYALE("NR");
	TGraphErrors *gER = SingletToTripletYALE("ER");

	TF1 *NRfunc = new TF1("NRfunc",SingletToTripletFunction,0.0,100.0,3);
	NRfunc->SetParameter(1,100);
	NRfunc->SetParameter(2,0.7);
	NRfunc->SetParameter(0,10.0);
	TF1 *ERfunc =new TF1("ERfunc",SingletToTripletFunction,0.0,100.0,3);
	ERfunc->SetParameter(1,100);
	ERfunc->SetParameter(2,0.3);
	ERfunc->SetParameter(0,10.0);

	double N,E;

	for (double erecoil=energy_min;erecoil<=energy_max;erecoil+=5){
		N = gNR->Eval(erecoil);
		NRfunc->SetParameter(2,N);

		E = gER->Eval(erecoil);
		ERfunc->SetParameter(2,E);

		if (erecoil==5){
			c1->cd();
			NRfunc->Draw();
			c2->cd();
			ERfunc->Draw();
		}
		else {
			c1->cd();
			NRfunc->Draw("same");
			c2->cd();
			ERfunc->Draw("same");
		}
	}
}
