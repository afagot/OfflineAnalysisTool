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

void AddCluster2List(vector< pair<int,int> >&, vector < vector< pair<int,int> > >&);
void PrintCluster(int, vector<vector<pair<int, int> > >&, ostream&);
void StripClusters(string);
void TimeClusters(string);
void FinalClusters(string);

#endif // CLUSTERS_H
