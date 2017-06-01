//***************************************************************
// *    utils.cc
// *    Developped by : Alexis Fagot
// *    07/03/2017
//***************************************************************

#include <cstdlib>
#include <sstream>
#include <ctime>
#include <fstream>
#include <cstdio>
#include <map>

#include "../include/utils.h"

#include "TStyle.h"

using namespace std;

// ****************************************************************************************************
// *    string intTostring(int value)
//
//  Function that casts an int into a string
// ****************************************************************************************************

string intToString(int value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************
// *    string longTostring(long value)
//
//  Function that casts a long into a string
// ****************************************************************************************************

string longTostring(long value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************
// *    string floatTostring(float value)
//
//  Function that casts a float into a string
// ****************************************************************************************************

string floatTostring(float value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************
// *    string GetVoltage(string fName)
//
//  Function that returns the voltage of the current HV step
// ****************************************************************************************************

string GetVoltage(string fName){
    ifstream file(fName.c_str(), ios::in);

    string HV;
    string word;

    //There are 5 words per line in the data file header:
    // # Step X of N
    // # Voltage = YYYY V
    // We thus want to extract the word YYYY which is the 9th one

    for(unsigned int w = 0; w < 8; w++) file >> word;
    file >> HV;

    return HV;
}

// ****************************************************************************************************
// *    void SetTitleName(string rpcID, unsigned int partition, char* Name, char* Title,
// *                      string Namebase, string Titlebase)
//
// Function that sets title and name formats of ROOT histograms
// ****************************************************************************************************

void SetTitleName(string rpcID, unsigned int partition, char* Name, char* Title, string Namebase, string Titlebase){
    string P[4] = {"A","B","C","D"};
    sprintf(Name,"%s_%s_%s",Namebase.c_str(),rpcID.c_str(),P[partition].c_str());
    sprintf(Title,"%s %s_%s",Titlebase.c_str(),rpcID.c_str(),P[partition].c_str());
}

// ****************************************************************************************************
// *    float GetTH1Mean(TH1* H)
//
// Function that returns the mean Y value of a 1D ROOT histogram
// ****************************************************************************************************

float GetTH1Mean(TH1* H){
    int nBins = H->GetNbinsX();
    float mean = 0.;

    for(int b = 1; b <= nBins; b++) mean += H->GetBinContent(b);

    mean /= (float)nBins;

    return mean;
}

// ****************************************************************************************************
// *    float GetTH1StdDev(TH1* H)
//
// Function tha treturns the standard deviation along the Y axis of a 1D ROOT histogram
// ****************************************************************************************************

//Get standard deviation of 1D histograms
float GetTH1StdDev(TH1* H){
    int nBins = H->GetNbinsX();
    float mean = GetTH1Mean(H);
    float stddev = 0.;
    float variance = 0.;

    for(int b = 1; b <= nBins; b++)
        variance += (H->GetBinContent(b)-mean)*(H->GetBinContent(b)-mean);

    stddev = sqrt(variance/nBins);

    return stddev;
}

// ****************************************************************************************************
// *    void DrawTH1(TCanvas* C, TH1* H, string xtitle, string ytitle, string option)
//
// Function that draws 1D ROOT histograms
// ****************************************************************************************************

void DrawTH1(TCanvas* C, TH1* H, string xtitle, string ytitle, string option){
    C->cd(0);
    H->SetXTitle(xtitle.c_str());
    H->SetYTitle(ytitle.c_str());
    H->SetFillColor(kBlue);
    H->Draw(option.c_str());
    C->Update();
}

// ****************************************************************************************************
// *    void DrawTH2(TCanvas* C, TH2* H, string xtitle, string ytitle, string ztitle, string option)
//
// Function that draws 2D ROOT histograms
// ****************************************************************************************************

void DrawTH2(TCanvas* C, TH2* H, string xtitle, string ytitle, string ztitle, string option){
    C->cd(0);
    H->SetXTitle(xtitle.c_str());
    H->SetYTitle(ytitle.c_str());
    H->SetXTitle(ztitle.c_str());
    gStyle->SetPalette(55);
    H->Draw(option.c_str());
    C->SetLogz(1);
    C->Update();
}
