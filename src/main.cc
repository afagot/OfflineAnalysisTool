// **********************************************************************
// *   Offline analysis tool for ASCII files
// *   Alexis Fagot
// *   2/3/2015
// *********************************************************************


#include <sstream>
#include "../include/SortDataFile.h"
#include "../include/Clusters.h"

using namespace std;

int main(int argc, char* argv[]){
    if(argc != 2){
        cout<<"USAGE : "<< argv[0] <<" <filename>\n";
    } else {
        stringstream converter;
        converter << argv[1];
        string fName;
        converter >> fName;

        SortData(fName);

        fName = "SORTED_" + fName;
        Analyse(fName);
        return 0;
    }
}
