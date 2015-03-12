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

void AddCluster2List(vector< pair<int,int> >& Cluster, vector < vector< pair<int,int> > >& ClusterList);
void PrintClusters(int Event, vector< vector< pair<int,int> > >& ClusterList, ostream& output);
bool IsInCluster(int hit, vector< pair<int,int> > cluster, string option);
void GroupStrips(vector< pair<int,int> >& tCluster, vector< pair<int,int> >& sCluster, vector< vector< pair<int,int> > >& cList);
void Analyse(string fName);

#endif // CLUSTERS_H
