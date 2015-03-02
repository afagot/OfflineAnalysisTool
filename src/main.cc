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
    //Then the RAW data files are sorted. Hits are sorted in 3 ways :
    //  - By strip position and neighbor strips are grouped in clusters
    //  - By time stamp and strips are grouped in cluster by time windows of 10ns
    //  - By using both criteria cited above
    //The goal is to make a comparison in between the 3 analysis.
    string fName = "XXXX";
    SortData(fName);

    //Using the time clusters only, get the efficiency, mean cluster size, mean number of
    //clusters per event, etc...
    string fNameTime = "TIMESORTED_" + fName;
    TimeClusters(fNameTime);

    //Using the strip clusters only, get the efficiency, mean cluster size, mean number of
    //clusters per event, etc...
    string fNameStrip = "STRIPSORTED_" + fName;
    StripClusters(fNameStrip);

    //Using the time & strip clusters, get the efficiency, mean cluster size, mean number of
    //clusters per event, etc...
    string fNameFinal = "STRIPSORTED_" + fName;
    FinalClusters(fNameFinal);

    return 0;
}
