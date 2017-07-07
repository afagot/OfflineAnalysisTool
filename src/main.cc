// **********************************************************************
// *   Offline analysis tool for ASCII files
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#include <cstdio>
#include <dirent.h>
#include <sstream>
#include <json/json.h>

#include "SortDataFile.h"
#include "Clusters.h"
#include "utils.h"
#include "MsgSvc.h"


using namespace std;

int main(int argc, char* argv[]){
    if(argc != 2 && argc != 4){
        cout<<"USAGE : "<< argv[0] <<" <configFile>\n";
        cout<<"OR :    "<< argv[0] <<" <configFile> <muonpeakstart> <muonpeakend>\n";
        return 1;
    } else {
        Json::Value root; // Will contain the root of Json file
        if( -1 != loadJsonFile(argv[1], root)){
          return EXIT_FAILURE;
        }
        // Print  content of the option file
        std::cout << root << std::endl;
        
        Json::Value fileValues = root["Global"]["DataFiles"];
        if( -1 != createListDataFiles(root["Global"].get("DataPath","nowhere").asString(), ".dat", fileValues)){        
          return EXIT_FAILURE;
        }
        
        if (fileValues.empty())
        {
          MSG_ERROR("Didn't find no files to analyse!!!\n");
          return EXIT_FAILURE;
        } 

        // If param given at commadn line, replace value from Json 
        if(argc == 4)
        {
            float start  = strtof(argv[2],NULL);
            float end    = strtof(argv[3],NULL);
            root["Clusters"]["StartTimeCut"] = start;
            root["Clusters"]["EndTimeCut"] = end;
        }
        
        Options options(root);
        for(auto &fName: fileValues)
        {
          //Sort the data by time stamp and
          //divide it into X readout and Y
          //readout data
          if (-1 != SortData(fName.asString(), options))
          {
            MSG_ERROR("Failed to sort the data, exiting...\n");
            return EXIT_FAILURE;
          }
          //Clusterize data files and make histograms
          if (-1 != Analyse(fName.asString(), options))
          {
            MSG_ERROR("Failed to sort the data, exiting...\n");
            return EXIT_FAILURE;
          }
        }
        return 0;
    }
}
