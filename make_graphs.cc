#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TStyle.h"
#include "TF1.h"
#include "TH1S.h"
#include "TH1F.h"
#include "TH3S.h"
#include "TH3F.h"
#include "THistPainter.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TGraphPainter.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TLegend.h"

using namespace std;

void make_graphs(string fName){
    if ( fName.substr(fName.find_last_of(".")) == ".csv" )
        fName = fName.erase(fName.find_last_of("."));

    string fNameROOT = fName + ".root";
    TFile TableROOT(fNameROOT.c_str(),"RECREATE");

    //********************************************************************

    string fNameTable = fName + ".csv";
    ifstream TableFile(fNameTable.c_str(),ios::in);

    if(TableFile){
        const int tSize = 20;               //size of the tables/number of points in graphs

        float HVeff[tSize] = {0};           //Can also be the threshold in case of
        float eHVeff[tSize] = {0};          //threshold scan

        float Eff1[tSize] = {0};
        float eEff1[tSize] = {0};
        float HitMult[tSize] = {0};
        float eHitMult[tSize] = {0};

        float Eff2A[tSize] = {0};
        float eEff2A[tSize] = {0};
        float ClustMult2A[tSize] = {0};
        float eClustMult2A[tSize] = {0};
        float ClustSize2A[tSize] = {0};
        float eClustSize2A[tSize] = {0};

        float Eff2B[tSize] = {0};
        float eEff2B[tSize] = {0};
        float ClustMult2B[tSize] = {0};
        float eClustMult2B[tSize] = {0};
        float ClustSize2B[tSize] = {0};
        float eClustSize2B[tSize] = {0};

        float gammaRate[tSize] = {0};
        float egammaRate[tSize] = {0};
        float Eff3[tSize] = {0};
        float eEff3[tSize] = {0};
        float ClustMult3[tSize] = {0};
        float eClustMult3[tSize] = {0};
        float ClustSize3[tSize] = {0};
        float eClustSize3[tSize] = {0};

        for(int c = 0; c<tSize; c++){
            string PartitionLabel;
            string Partition;
            string HVLabel;
            string Eff1Label;
            string HitMultLabel;
            string Eff2ALabel;
            string ClustMult2ALabel;
            string ClustSize2ALabel;
            string Eff2BLabel;
            string ClustMult2BLabel;
            string ClustSize2BLabel;
            string gammaLabel;
            string Eff3Label;
            string ClustMult3Label;
            string ClustSize3Label;

            TableFile >> PartitionLabel >> Partition
                      >> HVLabel >> HVeff[c]
                      >> Eff1Label >> Eff1[c] >> eEff1[c];
            TableFile >> HitMultLabel >> HitMult[c] >> eHitMult[c]
                      >> Eff2ALabel >> Eff2A[c] >> eEff2A[c];
            TableFile >> ClustMult2ALabel >> ClustMult2A[c] >> eClustMult2A[c]
                      >> ClustSize2ALabel >> ClustSize2A[c] >> eClustSize2A[c]
                      >> Eff2BLabel >> Eff2B[c] >> eEff2B[c];
            TableFile >> ClustMult2BLabel >> ClustMult2B[c] >> eClustMult2B[c]
                      >> ClustSize2BLabel >> ClustSize2B[c] >> eClustSize2B[c]
                      >> gammaLabel >> gammaRate[c] >> egammaRate[c]
                      >> Eff3Label >> Eff3[c] >> eEff3[c];
            TableFile >> ClustMult3Label >> ClustMult3[c] >> eClustMult3[c]
                      >> ClustSize3Label >> ClustSize3[c] >> eClustSize3[c];
        }

        gStyle->SetOptFit(1111);

        int lLimit = 8000;
        int uLimit = 9900;
        int HVhalf = (lLimit+uLimit)/2;

        string xLabel;

        if (uLimit == 240) xLabel = "Threshold (mV)";
        else xLabel = "HV_{eff} (V)";

        //****************************************************

        TF1* sigmoid = new TF1("sigmoid","[0]/(1+exp([1]*([2]-x)))",lLimit,uLimit);
        sigmoid->SetParName(0,"#epsilon_{max}");
        sigmoid->SetParName(1,"#lambda");
        sigmoid->SetParName(2,"HV_{50%}");
        sigmoid->SetParameter(0,0.98);
        sigmoid->SetParameter(1,0.01);
        sigmoid->SetParameter(2,HVhalf);

        //****************************************************

        TGraphErrors* Efficiency1 = new TGraphErrors(tSize,HVeff,Eff1,eHVeff,eEff1);
        Efficiency1->GetXaxis()->SetTitle(xLabel.c_str());
        Efficiency1->GetYaxis()->SetTitle("Efficiency");
        Efficiency1->GetXaxis()->SetRangeUser(lLimit,uLimit);
        Efficiency1->SetMarkerColor(4);
        Efficiency1->SetMarkerStyle(34);
        if(uLimit != 240){
            Efficiency1->Fit(sigmoid);
            Efficiency1->GetFunction("sigmoid")->SetLineColor(4);
        }

        TGraphErrors* HitMultiplicity = new TGraphErrors(tSize,HVeff,HitMult,eHVeff,eHitMult);
        HitMultiplicity->GetXaxis()->SetTitle(xLabel.c_str());
        HitMultiplicity->GetYaxis()->SetTitle("Multiplicity");
        HitMultiplicity->GetXaxis()->SetRangeUser(lLimit,uLimit);
        HitMultiplicity->SetMarkerColor(4);
        HitMultiplicity->SetMarkerStyle(20);

        //****************************************************

        sigmoid->SetParameter(0,0.80);
        sigmoid->SetParameter(1,0.01);
        sigmoid->SetParameter(2,HVhalf);

        TGraphErrors* Efficiency2A = new TGraphErrors(tSize,HVeff,Eff2A,eHVeff,eEff2A);
        Efficiency2A->GetXaxis()->SetTitle(xLabel.c_str());
        Efficiency2A->GetYaxis()->SetTitle("Efficiency");
        Efficiency2A->GetXaxis()->SetRangeUser(lLimit,uLimit);
        Efficiency2A->SetMarkerColor(2);
        Efficiency2A->SetMarkerStyle(20);
        if(uLimit != 240){
            Efficiency2A->Fit(sigmoid);
            Efficiency2A->GetFunction("sigmoid")->SetLineColor(2);
        }

        TGraphErrors* ClustMultiplicity2A = new TGraphErrors(tSize,HVeff,ClustMult2A,eHVeff,eClustMult2A);
        ClustMultiplicity2A->GetXaxis()->SetTitle(xLabel.c_str());
        ClustMultiplicity2A->GetYaxis()->SetTitle("Mean cluster multiplicity");
        ClustMultiplicity2A->GetXaxis()->SetRangeUser(lLimit,uLimit);
        ClustMultiplicity2A->SetMarkerColor(2);
        ClustMultiplicity2A->SetMarkerStyle(20);

        TGraphErrors* ClusterSize2A = new TGraphErrors(tSize,HVeff,ClustSize2A,eHVeff,eClustSize2A);
        ClusterSize2A->GetXaxis()->SetTitle(xLabel.c_str());
        ClusterSize2A->GetYaxis()->SetTitle("Mean cluster size");
        ClusterSize2A->GetXaxis()->SetRangeUser(lLimit,uLimit);
        ClusterSize2A->SetMarkerColor(2);
        ClusterSize2A->SetMarkerStyle(20);

        //****************************************************

        sigmoid->SetParameter(0,0.95);
        sigmoid->SetParameter(1,0.01);
        sigmoid->SetParameter(2,HVhalf);

        TGraphErrors* Efficiency2B = new TGraphErrors(tSize,HVeff,Eff2B,eHVeff,eEff2B);
        Efficiency2B->GetXaxis()->SetTitle(xLabel.c_str());
        Efficiency2B->GetYaxis()->SetTitle("Efficiency");
        Efficiency2B->GetXaxis()->SetRangeUser(lLimit,uLimit);
        Efficiency2B->SetMarkerColor(5);
        Efficiency2B->SetMarkerStyle(21);
        if(uLimit != 240){
            Efficiency2B->Fit(sigmoid);
            Efficiency2B->GetFunction("sigmoid")->SetLineColor(5);
        }

        TGraphErrors* ClustMultiplicity2B = new TGraphErrors(tSize,HVeff,ClustMult2B,eHVeff,eClustMult2B);
        ClustMultiplicity2B->GetXaxis()->SetTitle(xLabel.c_str());
        ClustMultiplicity2B->GetYaxis()->SetTitle("Mean cluster multiplicity");
        ClustMultiplicity2B->GetXaxis()->SetRangeUser(lLimit,uLimit);
        ClustMultiplicity2B->SetMarkerColor(5);
        ClustMultiplicity2B->SetMarkerStyle(21);

        TGraphErrors* ClusterSize2B = new TGraphErrors(tSize,HVeff,ClustSize2B,eHVeff,eClustSize2B);
        ClusterSize2B->GetXaxis()->SetTitle(xLabel.c_str());
        ClusterSize2B->GetYaxis()->SetTitle("Mean cluster size");
        ClusterSize2B->GetXaxis()->SetRangeUser(lLimit,uLimit);
        ClusterSize2B->SetMarkerColor(5);
        ClusterSize2B->SetMarkerStyle(21);

        //****************************************************

        sigmoid->SetParameter(0,0.90);
        sigmoid->SetParameter(1,0.01);
        sigmoid->SetParameter(2,HVhalf);

        TGraphErrors* Efficiency3 = new TGraphErrors(tSize,HVeff,Eff3,eHVeff,eEff3);
        Efficiency3->GetXaxis()->SetTitle(xLabel.c_str());
        Efficiency3->GetYaxis()->SetTitle("Efficiency");
        Efficiency3->GetXaxis()->SetRangeUser(lLimit,uLimit);
        Efficiency3->SetMarkerColor(8);
        Efficiency3->SetMarkerStyle(22);
        if(uLimit != 240){
            Efficiency3->Fit(sigmoid);
            Efficiency3->GetFunction("sigmoid")->SetLineColor(8);
        }

        TGraphErrors* GammaRate = new TGraphErrors(tSize,HVeff,gammaRate,eHVeff,egammaRate);
        GammaRate->GetXaxis()->SetTitle(xLabel.c_str());
        GammaRate->GetYaxis()->SetTitle("Rate (KHz/cm^{2})");
        GammaRate->GetXaxis()->SetRangeUser(lLimit,uLimit);
        GammaRate->SetMarkerColor(8);
        GammaRate->SetMarkerStyle(22);

        TGraphErrors* ClustMultiplicity3 = new TGraphErrors(tSize,HVeff,ClustMult3,eHVeff,eClustMult3);
        ClustMultiplicity3->GetXaxis()->SetTitle(xLabel.c_str());
        ClustMultiplicity3->GetYaxis()->SetTitle("Mean cluster multiplicity");
        ClustMultiplicity3->GetXaxis()->SetRangeUser(lLimit,uLimit);
        ClustMultiplicity3->SetMarkerColor(8);
        ClustMultiplicity3->SetMarkerStyle(22);

        TGraphErrors* ClusterSize3 = new TGraphErrors(tSize,HVeff,ClustSize3,eHVeff,eClustSize3);
        ClusterSize3->GetXaxis()->SetTitle(xLabel.c_str());
        ClusterSize3->GetYaxis()->SetTitle("Mean cluster size");
        ClusterSize3->GetXaxis()->SetRangeUser(lLimit,uLimit);
        ClusterSize3->SetMarkerColor(8);
        ClusterSize3->SetMarkerStyle(22);

        //****************************************************

        TMultiGraph* Efficiency = new TMultiGraph();
        Efficiency->Add(Efficiency1);
        Efficiency->Add(Efficiency2A);
        Efficiency->Add(Efficiency2B);
        Efficiency->Add(Efficiency3);
        Efficiency->Draw("ap");
        Efficiency->GetXaxis()->SetTitle(xLabel.c_str());
        Efficiency->GetYaxis()->SetTitle("Efficiency");
        Efficiency->GetXaxis()->SetLimits(lLimit,uLimit);
        Efficiency->SetMinimum(0.);
        Efficiency->SetMaximum(1.);

        TMultiGraph* ClusterSize = new TMultiGraph();
        ClusterSize->Add(ClusterSize2A);
        ClusterSize->Add(ClusterSize2B);
        ClusterSize->Add(ClusterSize3);
        ClusterSize->Draw("ap");
        ClusterSize->GetXaxis()->SetTitle(xLabel.c_str());
        ClusterSize->GetYaxis()->SetTitle("Mean cluster size");
        ClusterSize->GetXaxis()->SetLimits(lLimit,uLimit);
        ClusterSize->SetMinimum(-0.5);
        ClusterSize->SetMaximum(3.5);

        TMultiGraph* ClusterMultiplicity = new TMultiGraph();
        ClusterMultiplicity->Add(ClustMultiplicity2A);
        ClusterMultiplicity->Add(ClustMultiplicity2B);
        ClusterMultiplicity->Add(ClustMultiplicity3);
        ClusterMultiplicity->Draw("ap");
        ClusterMultiplicity->GetXaxis()->SetTitle(xLabel.c_str());
        ClusterMultiplicity->GetYaxis()->SetTitle("Mean cluster multiplicity");
        ClusterMultiplicity->GetXaxis()->SetLimits(lLimit,uLimit);
        ClusterMultiplicity->SetMinimum(-0.5);
        ClusterMultiplicity->SetMaximum(3.5);

        Efficiency1->Write();
        HitMultiplicity->Write();
        Efficiency2A->Write();
        ClustMultiplicity2A->Write();
        ClusterSize2A->Write();
        Efficiency2B->Write();
        ClustMultiplicity2B->Write();
        ClusterSize2B->Write();
        Efficiency3->Write();
        GammaRate->Write();
        ClustMultiplicity3->Write();
        ClusterSize3->Write();

        Efficiency->Write();
        ClusterMultiplicity->Write();
        ClusterSize->Write();
    }
    TableFile.close();

    TableROOT.Close();
}
