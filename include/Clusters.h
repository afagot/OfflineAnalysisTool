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
void PrintCluster(int Event, vector< vector< pair<int,int> > >& ClusterList, ostream& output);
void ClustrizeData(string fName);

#endif // CLUSTERS_H
