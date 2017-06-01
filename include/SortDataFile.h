// **********************************************************************
// *   Hit sorting functions
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#ifndef SORTDATAFILE_H
#define SORTDATAFILE_H

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <ctime>
#include <limits>

#include "Clusters.h"

using namespace std;

ifstream& GotoLine(ifstream &file, unsigned int line);
int RandomPivot(int first,int last);
int Partition(Cluster& A, int f, int l, string option);
void SortEvent(Cluster& A, int f, int l, string option);
void SortData(string fName, int nStrips);

#endif // SORTDATAFILE_H
