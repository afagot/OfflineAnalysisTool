// **********************************************************************
// *   Offline analysis tool for ASCII files
// *   Alexis Fagot
// *   2/3/2015
// *********************************************************************

#include "../include/SortDataFile.h"
#include "../include/Clusters.h"

using namespace std;

int main(){
    //Using a shell script, replace "XXXX" by the name of the file you want to analyse.
    //The advantage is be able to use a "for" loop to perform the analysis on every file
    //of a run in one go.
    //Then the RAW data files are sorted by strip position and neighbor strips are grouped
    //in clusters. The goal is to make a comparison in between the 3 analysis.
    string fName = "HVEffTest_RE2-2_Cosmics_NewPreamp_55mV_9300V_run20150306233745.dat";
    SortData(fName);

    fName = "SORTED_" + fName;
    Analyse(fName);
    return 0;
}
