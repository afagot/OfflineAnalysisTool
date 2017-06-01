// **********************************************************************
// *   Offline analysis tool for ASCII files
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#include <cstdio>
#include <sstream>
#include "../include/SortDataFile.h"
#include "../include/Clusters.h"
#include "../include/utils.h"

using namespace std;

int main(int argc, char* argv[]){
    if(argc != 3 && argc != 5){
        cout<<"USAGE : "<< argv[0] <<" <filename> <nStrips>\n";
        cout<<"OR :    "<< argv[0] <<" <filename> <nStrips> <muonpeakstart> <muonpeakend>\n";
        return 1;
    } else {
        string fName;
        stringstream ss;
        ss << argv[1];
        ss>> fName;
        ss.clear();

        int nStrips = 0;
        ss << argv[2];
        ss>> nStrips;
        ss.clear();

        //Sort the data by time stamp and
        //divide it into X readout and Y
        //readout data
        SortData(fName,nStrips);

        //Clusterize data files and make histograms
        if(argc == 3) Analyse(fName, nStrips, 0, 1000);
        else {
            float start  = strtof(argv[3],NULL);
            float end    = strtof(argv[4],NULL);

            Analyse(fName, nStrips, start, end);
        }
        return 0;
    }
}
