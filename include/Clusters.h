// **********************************************************************
// *   Hit clustering functions
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#ifndef CLUSTERS_H
#define CLUSTERS_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <cmath>
#include <json/json.h>

using namespace std;

const float ClustDev = 20.;

typedef vector<int> Events;                 //List of event tags
typedef vector<int> hMult;                  //List of TDC hit multiplicity
typedef vector<int> eStrips;                //List of fired strips per event
typedef vector<float> eTimes;               //List of hit time stamps per event
typedef vector<eStrips> esList;             //List of eStrip
typedef vector<eTimes> etList;              //List of eTime
typedef vector< pair<int,float> > Cluster;  //List of strip + time stamps grouped as cluster
typedef vector<Cluster> ClusterList;        //List of clusters
typedef std::map<std::string, Json::Value> Options; // Options read from Json config file 

struct RAWData{
    Events* EventList;
    hMult*  NHitsList;
    esList* ChannelList;
    etList* TimeStampList;
};

void  AddCluster2List(Cluster& cluster, ClusterList& cList);
void  PrintClusters(int event, ClusterList& cList, ostream& output);
bool  IsInCluster(int hit, Cluster cluster, string option);
void  GroupStrips(Cluster &tCluster, Cluster& sCluster, ClusterList& cList);
float GetClusterStart(Cluster& cluster);
bool  Is2DCluster(Cluster clusterX, Cluster clusterY);
float Get1DClusterCenter(Cluster cluster);
// void  Analyse(string fName, int nStrips, float start, float end); // Old
int  Analyse(const string fName, Json::Value &options);

#endif // CLUSTERS_H
