#include "../include/SortDataFile.h"
#include "../include/Clusters.h"

using namespace std;

int main(){
    string fName = "XXXX";
    SortDataFile(fName);

    string fNameTime = "TIMESORTED_" + fName;
    TimeClusters(fNameTime);

    string fNameStrip = "STRIPSORTED_" + fName;
    StripClusters(fNameStrip);

    string fNameFinal = "STRIPSORTED_" + fName;
    FinalClusters(fNameFinal);

    return 0;
}
