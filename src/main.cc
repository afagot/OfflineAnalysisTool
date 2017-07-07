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
int loadJsonFile(const std::string& fileName, Json::Value &root){
  if (fileName.empty())
  {
    MSG_ERROR("Please provide a config file");
    return EXIT_FAILURE;
  }    
  
  Json::Reader reader;
  // Json::Value root;   // 'root' will contain the root value after parsing.
  std::ifstream ifs (fileName.c_str(), std::ifstream::in);
  if (!ifs.good()){
    MSG_ERROR("Failed to open file '%s' \n",fileName.c_str());
    return EXIT_FAILURE;
  }
  
  bool isParsed = reader.parse(ifs, root, false);
  if(!isParsed)
  {
    // Report failures and their locations in the document.
    MSG_ERROR("Failed to parse JSON\n%s\n",reader.getFormatedErrorMessages().c_str());
    return EXIT_FAILURE;
  }
  
  Json::StyledWriter styledWriter;
  return -1;
}
/** Generate list of files to analyse
    dataPath : folder path to data
    fExt file : extension to look for
    optionMap : Options to populate
  */
int createListDataFiles(const std::string &dataPath, const std::string fExt, Json::Value &fileValues){
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (dataPath.c_str())) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      std::string fName = ent->d_name;
      if (fName.length() >= fExt.length()) {
        // grep files with extension fExt
        if (0 == fName.compare(fName.length() - fExt.length(), fExt.length(), fExt)){ 
          MSG_DEBUG("%s\n",fName.c_str());
          fileValues.append(Json::Value(dataPath + fName));
        }
      }
    }
    closedir (dir);
    return -1;
  } else {
    /* could not open directory */
    MSG_ERROR ("Could not open directory '%s'",dataPath.c_str());
    return EXIT_FAILURE;
  }
}

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
        
        Json::Value fileValues = root["Global"].get("dataFiles","");
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
