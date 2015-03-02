#include "../include/SortDataFile.h"

using namespace std;

bool SortPairs(pair<int,int> A, pair<int,int> B){
    return (A.first < B.first);
}

//*************************************************************************************

void SortStrips(string fName){
    ifstream rawFile(fName.c_str(),ios::in);

    if(rawFile){
        vector < pair<int,int> > Data;
        Data.clear();

        int nEvent = 0;

        fName = "STRIPSORTED_" + fName;

        ofstream sortedFile(fName.c_str(), ios::out);

        while(rawFile.good()){
            int strip = -1;
            int time = -1;

            rawFile >> strip >> time;

            if(strip == 0 && time == 0){
                nEvent++;
                sort(Data.begin(),Data.end(),SortPairs);

                if(Data.size() > 1 && Data.at(Data.size()-2).first == 100){
                    if(Data.back().second > 20000 && Data.back().second < 21000)
                        Data.erase(Data.end()-1);
                    else
                        Data.pop_back();
                }
                sortedFile << nEvent << " " << Data.size() << endl;

                for(unsigned int hit = 0; hit < Data.size(); hit++){
                    sortedFile << Data[hit].first << " " << Data[hit].second << endl;
                }
                Data.clear();
            } else if (strip == -1 && time == -1){
                cout << "End of file " << fName << endl;
                break;
            } else if(strip != 16){
                Data.push_back(make_pair(strip,time));
            }
        }
        sortedFile.close();
    } else cout << "Error : file " << rawFile << " was't opened." << endl;
    rawFile.close();
}

//*************************************************************************************

void SortTime(string fName){
    ifstream rawFile(fName.c_str(),ios::in);

    if(rawFile){
        vector < pair<int,int> > Data;
        Data.clear();

        int nEvent = 0;

        fName = "TIMESORTED_" + fName;

        ofstream sortedFile(fName.c_str(), ios::out);

        while(rawFile.good()){
            int strip = -1;
            int time = -1;

            rawFile >> strip >> time;
            if(strip == 0 && time == 0){
                nEvent++;
                sort(Data.begin(),Data.end(),SortPairs);

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
    }
    rawFile.close();
}

//*************************************************************************************

void SortDataFile(string fName){
    SortStrips(fName);
    SortTime(fName);
}
