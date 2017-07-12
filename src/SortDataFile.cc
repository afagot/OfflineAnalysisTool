// **********************************************************************
// *   Hit sorting functions
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#include <sstream>

#include "SortDataFile.h"
#include "MsgSvc.h"
#include "utils.h"


using namespace std;

ifstream &GotoLine(ifstream& file, unsigned int line){
    file.seekg(ios::beg);
    for(unsigned int i=0; i < (line-1); ++i){
        file.ignore(numeric_limits<streamsize>::max(),'\n');
    }
    return file;
}

//*************************************************************************************
//This function sorts data by increasing strip number or increasing time stamp.
//Data format is :

//EventCount \t NumOfHitsX \t NumberOfHitsY
//ChannelX \t TimeStampX
//ChannelX \t TimeStampX
//ChannelX \t TimeStampX
//ChannelY \t TimeStampY
//ChannelY \t TimeStampY
//EventCount \t NumOfHitsX \t NumberOfHitsY
//ChannelX \t TimeStampX
//ChannelX \t TimeStampX
//ChannelY \t TimeStampY
//ChannelY \t TimeStampY
//ChannelY \t TimeStampY
//EventCount \t NumOfHitsX \t NumberOfHitsY
//ChannelX \t TimeStampX
//ChannelY \t TimeStampY
//ChannelY \t TimeStampY
//ChannelY \t TimeStampY
//...

//with a line of 0s in between each event (trigger).

//-------------------------------------------------------------------------------------------------
int SortData(string fName, const Options &options){
    string HVstep = GetVoltage(fName)+"V_";              //Extract voltage step from file header.
    ifstream rawFile(fName.c_str(),ios::in);            //Open data file in read mode.

    if(rawFile.is_open()){
        MSG_INFO("Open the file and start sorting.\n");

        if(rawFile.get() == '#') GotoLine(rawFile,3);
        else rawFile.seekg(ios::beg);

        vector < pair<int,float> > XData;               //Array to contain hit list for
        XData.clear();                                  //each event in X readout.

        vector < pair<int,float> > YData;               //Array to contain hit list for
        YData.clear();                                  //each event in Y readout.
        

        if (-1 != createDir( options.m_dataPath + "DAT/"))
          return(EXIT_FAILURE);
        
        unsigned NameInPath = fName.find_last_of("/")+1;
        string oName = fName.insert(NameInPath,HVstep);
        oName = oName.insert(NameInPath,"DAT/SORTED_");
        ofstream sortedFile(oName.c_str(), ios::out);   //Open output file in write mode.

        while(rawFile.good()){
            int nEvent = -1;
            int nHits = -1;

            rawFile >> nEvent >> nHits;                 //From data file, read headerline

            if(nEvent == -1 && nHits == -1){            //If nothing is read, this is the
                MSG_INFO("End of sorting.\n");          //end.
                rawFile.close();
                return(-1);
            } else {
                for(int h=0; h<nHits; h++){             //else loop on every hit.
                    int strip = -1;
                    float time = -1;

                    rawFile >> strip >> time;       //Save data pairs into the array.
                    if(strip < options.m_nStrips && (strip >= options.m_xStripMin && strip <= options.m_xStripMax)){
                          XData.push_back(make_pair(strip,time));
                    } else if(strip < 2*options.m_nStrips && (strip >= options.m_yStripMin && strip <= options.m_yStripMax)){
                          YData.push_back(make_pair(strip,time));
                    } else{
                        std::ostringstream oss;
                        oss << "Found hit in strip '" << strip << "' But only '" << 2*options.m_nStrips <<"' were defined.\n";
                        MSG_WARNING("%s",oss.str().c_str());
                        return EXIT_FAILURE;
                    }
                }

                //Sort the arrays per time stamp and print the sorted data
                //add a cut at nHits == 5
              //  if(YData.size() <= 5){
                    bool cutIsPassed = false;
                    if(XData.size() > 1 && XData.size() <= options.m_xNHitMax)
                    {
                      SortEvent(XData,0,XData.size()-1,"TIME");
                      cutIsPassed = true;
                    }
                    if(YData.size() > 1 && YData.size() <= options.m_yNHitMax)
                    {
                      SortEvent(YData,0,YData.size()-1,"TIME");
                      cutIsPassed = true;
                    }
                    if (cutIsPassed){
                      sortedFile << nEvent << '\t' << XData.size() << '\t' << YData.size() << endl;

                      for(unsigned int h = 0; h < XData.size(); h++){
                        sortedFile << '\t' << XData[h].first << '\t' << XData[h].second << endl;
                      }

                      for(unsigned int h = 0; h < YData.size(); h++){
                        sortedFile << '\t' << YData[h].first << '\t' << YData[h].second << endl;
                      }
                    }
              //  }
                XData.clear();
                YData.clear();
            }
        }
        sortedFile.close();
        MSG_INFO("Data sorting finished.\n");
        return(-1);
    } else {
        MSG_ERROR("Couldn't open data file to sort.\n");
        return(1);
    }
}
