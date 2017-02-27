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

        unsigned NameInPath = fName.find_last_of("/")+1;
        fName.insert(NameInPath,"SORTED_");
        Analyse(fName);
        return 0;
    }
}
