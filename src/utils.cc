//***************************************************************
// *    utils.cc
// *    Developped by : Alexis Fagot
// *    07/03/2017
//***************************************************************

#include <cstdlib>
#include <sstream>
#include <ctime>
#include <fstream>
#include <cstdio>
#include <map>
#include <sys/stat.h>
#include <dirent.h>


#include "utils.h"
#include "MsgSvc.h"

#include "TStyle.h"

using namespace std;

//*************************************************************************************

//TDCs give 2 informations :
//  - Channel (that leads to the strip)
//  - Time stamp
//This function allows to sort data pairs by increasing strip number ortime stamp
//depending on an option.

//To sort the data, a quicksort algorithm is used. It consists in 3 steps :

//  1- Pick an element from the array A called Pivot (p) between its first (f) and
//last (l) element.
//  2- Reorder A so that all elements that all elements with values less than p come
//before p, while all elements with values greater than p come after it (equal values
//can go either way). After this partitionning, p is in its final position. This is
//called the Partition operation. In our case, this operation is released on the
//  3- Recursively apply the above steps to the sub-array of elements with smaller
//values and separately to the sub-array of elements with greater values.

//*************************************************************************************

int RandomPivot(int first,int last){        //Return a random element index in range
    return rand()%(last-first)+first;       //[first;last]
}

//*************************************************************************************

int Partition(Cluster& A, int f, int l, string option){
    pair<int,float> tPair;                  //Temporary pair to help swaping elements.

    int p = RandomPivot(f,l);               //Pick a random element as pivot and then
    tPair = A[p];                           //swap the pivot element with the last one.
    A[p] = A[l];
    A[l] = tPair;

    int j = f;                              //j starts at the first element of the array
                                            //or sub-array you want to sort. Every time
                                            //you find an element lower than the pivot,
                                            //you increment j.
                                            //At the end, j corresponds to the final of
                                            //the pivot.

    for(int i=f; i<l; i++){                 //If element i is smaller than the pivot :
        if((option == "STRIP" && A[i].first < A[l].first) || (option == "TIME" && A[i].second < A[l].second)){
            tPair = A[i];                   //swap the element i with the element j
            A[i] = A[j];                    //which is the last element that was greater
            A[j] = tPair;                   //than the pivot.
            j++;                            //Increment j.
        }
    }

    tPair = A[j];                           //Finally swap element j which is greater
    A[j] = A[l];                            //than the pivot and the pivot. p is now
    A[l] = tPair;                           //at its final position j.
    return j;                               //Return the final position of the pivot.
}

//*************************************************************************************

void SortEvent(Cluster& A, int f, int l, string option){
    if(f < l){
        int pivot = Partition(A,f,l,option);//Partition your array and get the pivot.
        if(A.size() > 2){
            if(pivot != f)
                SortEvent(A,f,pivot-1,option);//Partition the lower sub-array.
            if(pivot != l)
                SortEvent(A,pivot+1,l,option);//Partition the greater sub-array.
        }
    } else if (f > l){
        MSG_ERROR("Problem with the indexes : first index f > last index l\n");
        exit(EXIT_FAILURE);
    }
}
// ****************************************************************************************************
// *    string intTostring(int value)
//
//  Function that casts an int into a string
// ****************************************************************************************************

string intToString(int value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************
// *    string longTostring(long value)
//
//  Function that casts a long into a string
// ****************************************************************************************************

string longTostring(long value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************
// *    string floatTostring(float value)
//
//  Function that casts a float into a string
// ****************************************************************************************************

string floatTostring(float value){
    string word;
    stringstream ss;
    ss << value;
    ss>> word;

    return word;
}

// ****************************************************************************************************
// *    string GetVoltage(string fName)
//
//  Function that returns the voltage of the current HV step
// ****************************************************************************************************

string GetVoltage(string fName){
    ifstream file(fName.c_str(), ios::in);

    string HV;
    string word;

    //There are 5 words per line in the data file header:
    // # Step X of N
    // # Voltage = YYYY V
    // We thus want to extract the word YYYY which is the 9th one

    for(unsigned int w = 0; w < 8; w++) file >> word;
    file >> HV;

    return HV;
}

// ****************************************************************************************************
// *    void SetTitleName(string rpcID, unsigned int partition, char* Name, char* Title,
// *                      string Namebase, string Titlebase)
//
// Function that sets title and name formats of ROOT histograms
// ****************************************************************************************************

void SetTitleName(string rpcID, unsigned int partition, char* Name, char* Title, string Namebase, string Titlebase){
    string P[4] = {"A","B","C","D"};
    sprintf(Name,"%s_%s_%s",Namebase.c_str(),rpcID.c_str(),P[partition].c_str());
    sprintf(Title,"%s %s_%s",Titlebase.c_str(),rpcID.c_str(),P[partition].c_str());
}

// ****************************************************************************************************
// *    float GetTH1Mean(TH1* H)
//
// Function that returns the mean Y value of a 1D ROOT histogram
// ****************************************************************************************************

float GetTH1Mean(TH1* H){
    int nBins = H->GetNbinsX();
    float mean = 0.;

    for(int b = 1; b <= nBins; b++) mean += H->GetBinContent(b);

    mean /= (float)nBins;

    return mean;
}

// ****************************************************************************************************
// *    float GetTH1StdDev(TH1* H)
//
// Function tha treturns the standard deviation along the Y axis of a 1D ROOT histogram
// ****************************************************************************************************

//Get standard deviation of 1D histograms
float GetTH1StdDev(TH1* H){
    int nBins = H->GetNbinsX();
    float mean = GetTH1Mean(H);
    float stddev = 0.;
    float variance = 0.;

    for(int b = 1; b <= nBins; b++)
        variance += (H->GetBinContent(b)-mean)*(H->GetBinContent(b)-mean);

    stddev = sqrt(variance/nBins);

    return stddev;
}

// ****************************************************************************************************
// *    void DrawTH1(TCanvas* C, TH1* H, string xtitle, string ytitle, string option)
//
// Function that draws 1D ROOT histograms
// ****************************************************************************************************

void DrawTH1(TCanvas* C, TH1* H, string xtitle, string ytitle, string option){
    C->cd(0);
    H->SetXTitle(xtitle.c_str());
    H->SetYTitle(ytitle.c_str());
    H->SetFillColor(kBlue);
    H->Draw(option.c_str());
    C->Update();
}

// ****************************************************************************************************
// *    void DrawTH2(TCanvas* C, TH2* H, string xtitle, string ytitle, string ztitle, string option)
//
// Function that draws 2D ROOT histograms
// ****************************************************************************************************

void DrawTH2(TCanvas* C, TH2* H, string xtitle, string ytitle, string ztitle, string option){
    C->cd(0);
    H->SetXTitle(xtitle.c_str());
    H->SetYTitle(ytitle.c_str());
    H->SetXTitle(ztitle.c_str());
    gStyle->SetPalette(55);
    H->Draw(option.c_str());
    C->SetLogz(1);
    C->Update();
}

  /** 
      if dirPath already exist do nothing
      return status code
    */
int createDir(std::string dirPath){
  DIR* dir = opendir(dirPath.c_str());
  if (dir)
  {
    closedir(dir);
    return -1;
  }
  else if (ENOENT == errno)
  {
    /* Directory does not exist. */
    MSG_INFO("Creating directory '%s'\n", dirPath.c_str());
    const int dirCode = mkdir(dirPath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); // 755 rights
    if (-1 == dirCode)
    {
      MSG_ERROR("Error creating directory!\n");
      return(EXIT_FAILURE);
    }
    return -1;
  }
  else
  {
      /* opendir() failed for some other reason. */
      MSG_ERROR("Failed to open directory '%s'\n", dirPath.c_str());
      return(EXIT_FAILURE);
  }
}

Options::Options(Json::Value &options){
  m_dataPath  = options["Global"].get("DataPath","nowhere").asString();
  m_nStrips   = options["Global"].get("NStrips","100").asInt();

  // Cut on Strip
  m_xStripMin = options["SortData"].get("XStripMin",-1).asInt();
  m_xStripMax = options["SortData"].get("XStripMax",100).asInt();
  m_yStripMin = options["SortData"].get("YStripMin",-1).asInt();
  m_yStripMax = options["SortData"].get("YStripMax",100).asInt();
  
  // Cut on max hit
  m_xNHitMax = options["SortData"].get("XNHitMax",500).asInt();
  m_yNHitMax = options["SortData"].get("YNHitMax",500).asInt(); 
  
  m_startTimeCut = options["CLusters"].get("StartTimeCut",-1).asFloat();
  m_endTimeCut   = options["Clusters"].get("EndTimeCut",-1).asFloat(); 
}
