#ifndef UTILS_H
#define UTILS_H

//***************************************************************
// *    utils.h
// *    Developped by : Alexis Fagot
// *    07/03/2017
//***************************************************************

#include <string>
#include <vector>
#include <json/json.h>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

using namespace std;
typedef vector< pair<int,float> > Cluster;  //List of strip + time stamps grouped as cluster

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
int RandomPivot(int first,int last);
int Partition(Cluster& A, int f, int l, string option);
void SortEvent(Cluster& A, int f, int l, string option);

class Options{
public:
  Options(Json::Value &options);
  ~Options(){;}
  
  // Global
  std::string m_dataPath;
  int m_nStrips;
  
  //SortData
  int m_xStripMin;
  int m_xStripMax;
  int m_yStripMin;
  int m_yStripMax;
  int m_xNHitMax;
  int m_yNHitMax;
  
  //Cluster
  float m_startTimeCut;
  float m_endTimeCut;
};

#endif // UTILS_H
