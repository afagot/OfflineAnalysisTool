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
#include <ctime>

using namespace std;

int RandomPivot(int first,int last);
int Partition(vector< pair<int,float> >& A, int f, int l, string option);
void SortEvent(vector< pair<int,float> >& A, int f, int l, string option);
void SortData(string fName);

#endif // SORTDATAFILE_H