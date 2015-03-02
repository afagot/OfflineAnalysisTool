// **********************************************************************
// *   Hit sorting functions
// *   Alexis Fagot
// *   2/3/2015
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

using namespace std;

int Partitionning(int,int,int);
bool SortHits(pair<int,int>,pair<int,int>,string);
void SortData(string,string);

#endif // SORTDATAFILE_H
