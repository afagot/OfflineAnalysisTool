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
    ifstream TableFile(fName.c_str(),ios::in);

    TMultiGraph* Efficiencies = new TMultiGraph();
    TMultiGraph* ClusterSizes = new TMultiGraph();

    int color = 2;
    int marker = 20;

    string xLabel = "HV_{eff} (V)";

    if(TableFile){
        while(TableFile.good()){
            string keyName;
            int nStep = 0;

            vector< float > HV, dHV, Eff, dEff, cSize, dcSize;
            HV.clear();
            dHV.clear();
            Eff.clear();
            dEff.clear();
            cSize.clear();
            dcSize.clear();

            TableFile >> keyName >> nStep;

            if(nStep == 0) break;

            cout << keyName << "\t" << nStep << endl;

            for(int s = 0; s<nStep; s++){
                float hv, dhv, eff, deff, csize, dcsize;
                TableFile >> hv >> dhv >> eff >> deff >> csize >> dcsize;
                cout << hv << "\t" << dhv << "\t" << eff << "\t" << deff << "\t" << csize << "\t" << dcsize << endl;

                HV.push_back(hv);
                dHV.push_back(dhv);
                Eff.push_back(eff);
                dEff.push_back(deff);
                cSize.push_back(csize);
                dcSize.push_back(dcsize);
            }

            string fNameROOTStep = keyName + ".root";
            TFile fROOTStep(fNameROOTStep.c_str(),"RECREATE");

            gStyle->SetOptFit(1111);

            int lLimit = HV.front()-100;
            int uLimit = HV.back()+100;
            int HVhalf = (lLimit+uLimit)/2;

            //****************************************************

            TF1* sigmoid = new TF1("sigmoid","[0]/(1+exp([1]*([2]-x)))",lLimit,uLimit);
            sigmoid->SetParName(0,"#epsilon_{max}");
            sigmoid->SetParName(1,"#lambda");
            sigmoid->SetParName(2,"HV_{50%}");
            sigmoid->SetParameter(0,0.98);
            sigmoid->SetParameter(1,0.01);
            sigmoid->SetParameter(2,HVhalf);

            //****************************************************

            TGraphErrors* Efficiency = new TGraphErrors(nStep,&(HV[0]),&(Eff[0]),&(dHV[0]),&(dEff[0]));
            Efficiency->SetTitle(keyName.c_str());
            Efficiency->GetXaxis()->SetTitle(xLabel.c_str());
            Efficiency->GetYaxis()->SetTitle("Efficiency");
            Efficiency->GetXaxis()->SetRangeUser(lLimit,uLimit);
            Efficiency->SetMarkerColor(color);
            Efficiency->SetMarkerStyle(marker);
            Efficiency->Fit(sigmoid);
            Efficiency->GetFunction("sigmoid")->SetLineColor(color);

            TGraphErrors* ClusterSize = new TGraphErrors(nStep,&(HV[0]),&(cSize[0]),&(dHV[0]),&(dcSize[0]));
            ClusterSize->SetTitle(keyName.c_str());
            ClusterSize->GetXaxis()->SetTitle(xLabel.c_str());
            ClusterSize->GetYaxis()->SetTitle("Mean cluster size");
            ClusterSize->GetXaxis()->SetRangeUser(lLimit,uLimit);
            ClusterSize->SetMarkerColor(color);
            ClusterSize->SetMarkerStyle(marker);

            Efficiencies->Add(Efficiency);
            ClusterSizes->Add(ClusterSize);

            Efficiency->Write();
            ClusterSize->Write();

            fROOTStep.Close();

            color++;
            marker++;
        }

        string fNameROOT = fName.erase(fName.find_last_of(".")) + ".root";
        TFile fROOT(fNameROOT.c_str(),"RECREATE");

        //****************************************************

        gStyle->SetOptFit(1111);

        TCanvas* c1 = new TCanvas();
        c1->cd(0);
        Efficiencies->Draw("ap");
        Efficiencies->GetXaxis()->SetTitle(xLabel.c_str());
        Efficiencies->GetYaxis()->SetTitle("Efficiency");
        Efficiencies->SetMinimum(0.);
        Efficiencies->SetMaximum(100.);

        TCanvas* c2 = new TCanvas();
        c2->cd(0);
        ClusterSizes->Draw("ap");
        ClusterSizes->GetXaxis()->SetTitle(xLabel.c_str());
        ClusterSizes->GetYaxis()->SetTitle("Mean cluster size");
        ClusterSizes->SetMinimum(-0.5);
        ClusterSizes->SetMaximum(3.5);

        Efficiencies->Write();
        ClusterSizes->Write();

        fROOT.Close();
    }
    TableFile.close();
}
