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
    if(argc != 2 && argc != 4){
        cout<<"USAGE : "<< argv[0] <<" <filename>\n";
        cout<<"OR :    "<< argv[0] <<" <filename> <muonpeakstart> <muonpeakend>\n";
        return 1;
    } else {
        string fName;
        stringstream ss;
        ss << argv[1];
        ss>> fName;

        //Sort the data by time stamp and
        //divide it into X readout and Y
        //readout data
        SortData(fName);

        //Clusterize data files and make histograms
        if(argc == 2) Analyse(fName, 0, 1000);
        else{
            float start  = strtof(argv[2],NULL);
            float end    = strtof(argv[3],NULL);

            Analyse(fName, start, end);
        }
        return 0;
    }
}
