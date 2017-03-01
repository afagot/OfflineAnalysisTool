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

using namespace std;

typedef std::vector< std::pair<int,float> > Cluster;
typedef std::vector< Cluster > ClusterList;

void  AddCluster2List(Cluster& cluster, ClusterList& cList);
void  PrintClusters(int event, ClusterList& cList, ostream& output);
bool  IsInCluster(int hit, Cluster cluster, string option);
void  GroupStrips(Cluster &tCluster, Cluster& sCluster, ClusterList& cList);
float GetClusterStart(Cluster& cluster);
bool  Is2DCluster(Cluster clusterX, Cluster clusterY);
float Get1DClusterCenter(Cluster cluster);
void  Analyse(string fName, float window, float start, float end);

#endif // CLUSTERS_H
