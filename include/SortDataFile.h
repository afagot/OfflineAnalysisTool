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

bool SortPairs(pair<int,int>,pair<int,int>);
void SortStrips(string);
void SortTime(string);
void SortDataFile(string);

#endif // SORTDATAFILE_H
