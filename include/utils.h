#ifndef UTILS_H
#define UTILS_H

//***************************************************************
// *    utils.h
// *    Developped by : Alexis Fagot
// *    07/03/2017
//***************************************************************

#include <string>
#include <vector>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

using namespace std;

string          intToString(int value);
string          longTostring(long value);
string          floatTostring(float value);
string          GetVoltage(string fName);
void            SetTitleName(string rpcID, unsigned int partition, char* Name, char* Title, string Namebase, string Titlebase);
float           GetTH1Mean(TH1* H);
float           GetTH1StdDev(TH1* H);
void            DrawTH1(TCanvas* C, TH1* H, string xtitle, string ytitle, string option);
void            DrawTH2(TCanvas* C, TH2* H, string xtitle, string ytitle, string ztitle, string option);
int createDir(std::string dirPath);


#endif // UTILS_H
