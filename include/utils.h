#ifndef UTILS_H
#define UTILS_H

//***************************************************************
// *    utils.h
// *    Developped by : Alexis Fagot
// *    07/03/2017
//***************************************************************

#include <iostream>
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

int RandomPivot(int first,int last);
int Partition(Cluster& A, int f, int l, string option);
void SortEvent(Cluster& A, int f, int l, string option);

/** if dirPath already exist do nothing
return status code
*/
int createDir(std::string dirPath);

/** Generate list of files to analyse
    dataPath : folder path to data
    fExt file : extension to look for
    fileValues : jsonValue to populate
  */
int createListDataFiles(const std::string &dataPath, const std::string &fExt, Json::Value &fileValues);


class Options{
public:
  Options(const std::string &fileName);
  ~Options(){;}
  void dump();
  
private:
  /** Load fileName into Json buffer
    */
    Json::Value m_root;
  int loadJsonFile(const std::string &fileName);

Json::Value getValue(const Json::Value &root, const std::string &key)
{
    return root[key];
}


template <class T>
Json::Value getOptionValue(std::string optionStr, T defaultValue)
{
  size_t pos = 0;
  std::string token;
  std::string delimiter = ".";
  std::vector<std::string> keyList;
  
  while ((pos = optionStr.find(delimiter)) != std::string::npos) {
    keyList.push_back(optionStr.substr(0, pos));
    optionStr.erase(0, pos + delimiter.length());
  }
  
  keyList.push_back(optionStr);
  Json::Value root = m_root;
  Json::Value value;
  for (auto key = keyList.begin(); key != --keyList.end(); ++key)
  {
    value = getValue(root, *key);
    root = value;
  }
  
  std::string key = *(--keyList.end());
  return value.get(key,defaultValue);
}

public:
  // Global
  std::string m_dataPath;
  Json::Value m_dataFiles;
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
  int m_nSkipEvents;
  int m_maxEvent;
  int m_xClusterSizeMax;
  int m_xClusterSizeMin;
  int m_yClusterSizeMax;
  int m_yClusterSizeMin;

};

#endif // UTILS_H
