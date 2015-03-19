// **********************************************************************
// *   Hit clustering functions
// *   Alexis Fagot
// *   2/3/2015
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

void AddCluster2List(vector< pair<int,float> >& Cluster, vector < vector< pair<int,float> > >& ClusterList);
void PrintClusters(int Event, vector< vector< pair<int,float> > >& ClusterList, ostream& output);
bool IsInCluster(int hit, vector< pair<int,float> > cluster, string option);
void GroupStrips(vector< pair<int,float> >& tCluster, vector< pair<int,float> >& sCluster, vector< vector< pair<int,float> > >& cList);
void Analyse(string fName);

#endif // CLUSTERS_H
