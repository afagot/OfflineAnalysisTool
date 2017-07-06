// **********************************************************************
// *   Offline analysis tool for ASCII files
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#include <cstdio>
#include <dirent.h>
#include <sstream>
#include <json/json.h>

#include "../include/SortDataFile.h"
#include "../include/Clusters.h"
#include "../include/utils.h"
#include "../include/MsgSvc.h"


using namespace std;
typedef std::map<std::string, Json::Value> Options; // Options read from Json config file 

void loadJsonFile(const std::string& fileName, Options &optionMap){
  
  if (fileName.empty())
  {
    MSG_ERROR("Please provide a config file");
    return;
  }    
  
  Json::Reader reader;
  Json::Value root;   // 'root' will contain the root value after parsing.
  std::ifstream ifs (fileName.c_str(), std::ifstream::in);
  if (!ifs.good()){
    MSG_ERROR("Failed to open file '%s' \n",fileName.c_str());
    return;
  }
  
  bool isParsed = reader.parse(ifs, root, false);
  if(!isParsed)
  {
    // Report failures and their locations in the document.
    MSG_ERROR("Failed to parse JSON\n%s\n",reader.getFormatedErrorMessages().c_str());
    return ;
  }
  const Json::Value &globalValues = root["Global"];
  optionMap["dataPath"] = globalValues.get("DataPath","").asString();
  optionMap["nStrips"] = globalValues.get("NStrips", 16).asInt();

  const Json::Value &sortValues = root["SortData"];
  
  const Json::Value &clusterValues = root["Clusters"];
  optionMap["startTimeCut"] = clusterValues.get("StartTimeCut", 0).asInt();
  optionMap["endTimeCut"] = clusterValues.get("EndTimeCut", 1000).asInt();
  Json::StyledWriter styledWriter;
}

void printOptions(Options &optionMap){
  MSG_INFO("\n\t--- Dumping optionMap ---\n");
  for (auto const& item:optionMap)
    std::cout << item.first << " : " << item.second << std::endl;
  MSG_INFO("\t --- End Dumping optionMap ---\n\n");
}

/** Generate list of files to analyse
    dataPath : folder path to data
    fExt file : extension to look for
    optionMap : Options to populate
  */
void createListDataFiles(std::string dataPath, std::string fExt, Options &optionMap){
  DIR *dir;
  struct dirent *ent;
  Json::Value fileValues;
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
    optionMap["dataFiles"] = fileValues;
    closedir (dir);
  } else {
    /* could not open directory */
    MSG_ERROR ("Could not open directory '%s'",dataPath);
    return;
  }
}

int main(int argc, char* argv[]){
    if(argc != 2 && argc != 4){
        cout<<"USAGE : "<< argv[0] <<" <configFile>\n";
        cout<<"OR :    "<< argv[0] <<" <configFile> <muonpeakstart> <muonpeakend>\n";
        return 1;
    } else {
        Options optionMap;
        loadJsonFile(argv[1], optionMap);
        dumpOptions(optionMap);
        
        createListDataFiles(optionMap["dataPath"].asString(), ".dat", optionMap);
        if (optionMap["dataFiles"].empty())
        {
          MSG_ERROR("Didn't find no files to analyse!!!\n");
          return EXIT_FAILURE;
        } 

        // If param given at commadn line, replace value from Json 
        if(argc == 4)
        {
            float start  = strtof(argv[2],NULL);
            float end    = strtof(argv[3],NULL);
            optionMap["startTimeCut"] = start;
            optionMap["endTimeCut"] = end;
        }
        
        for(const auto &fName: optionMap["dataFiles"])
        {
          //Sort the data by time stamp and
          //divide it into X readout and Y
          //readout data
          SortData(fName.asString(), optionMap);
          //Clusterize data files and make histograms
          Analyse(fName.asString(), optionMap);
        }
        return 0;
    }
}
