// **********************************************************************
// *   Hit sorting functions
// *   Alexis Fagot
// *   2/3/2015
// *********************************************************************

#include "../include/SortDataFile.h"
#include "../include/MsgSvc.h"

using namespace std;

//*************************************************************************************
//TDCs give 2 informations :
//  - Channel (that leads to the strip)
//  - Time stamp
//This function allows to sort data pairs according to the option specified
//STRIP (to sort the strips by increasing channel)
//TIME  (to sort the time stamps by increasing time)

/*bool SortPairs(pair<int,int> A, pair<int,int> B, string option){
    if(option == "STRIP")
        return (A.first < B.first);
    else if(option == "TIME")
        return (A.second < B.second);
    else{
        MSG_ERROR("The option specified for sorting is wrong (it should be STRIP or TIME).");
        exit(EXIT_FAILURE);
    }
}*/

int Partitionning(int first, int last, int pivot){

}

void SortHits(vector< pair<int,int> >, string option){

}

//*************************************************************************************
//This function sorts data by increasing strip number

void SortData(string fName, string option){
    ifstream rawFile(fName.c_str(),ios::in);

    if(rawFile){
        vector < pair<int,int> > Data;
        Data.clear();

        int nEvent = 0;

        fName = option + "SORTED_" + fName;

        ofstream sortedFile(fName.c_str(), ios::out);

        while(rawFile.good()){
            int strip = -1;
            int time = -1;

            rawFile >> strip >> time;
            if(strip == 0 && time == 0){
                nEvent++;
                SortHits(Data,option);

                sortedFile << nEvent << " " << Data.size() << endl;
                for(unsigned int hit = 0; hit < Data.size(); hit++){
                    sortedFile << Data[hit].first << " " << Data[hit].second << endl;
                }
                Data.clear();
            } else if (strip == -1 && time == -1){
                cout << "End of file " << fName << endl;
                break;
            } else if(strip != 16){
                Data.push_back(make_pair(time,strip));
            }
        }
        sortedFile.close();
    } else cout << "Error : file " << rawFile << " was't opened." << endl;
    rawFile.close();
}
