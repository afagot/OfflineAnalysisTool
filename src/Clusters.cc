// **********************************************************************
// *   Hit clustering functions
// *   Alexis Fagot
// *   27/02/2017
// *********************************************************************

#include "../include/Clusters.h"
#include "../include/SortDataFile.h"
#include "../include/MsgSvc.h"
#include "../include/utils.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1S.h"
#include "TH2S.h"
#include "TH1F.h"

using namespace std;

//*****************************************************************************
//This function adds the hit array into the cluster list as new cluster and
//clears the hit array.

void AddCluster2List(Cluster& cluster, ClusterList& cList){
    cList.push_back(cluster);
    cluster.clear();
}

//*****************************************************************************
//This function prints the cluster list into the output file. The format is :

//EventLabel    NumberOfClusters
//Cluster1      NumberOfHits1
//Hit1          Time1
//Hit2          Time2
//...
//HitN          TimeN
//Cluster2      NumberOfHits2
//Hit1          Time1
//Hit2          Time2
//...
//HitN          TimeN
//...
//...
//ClusterN      NumberOfHitsN
//Hit1          Time1
//Hit2          Time2
//...
//HitN          TimeN


void PrintClusters(int event, ClusterList& cList, ostream& output){
    if(cList.size() == 1 && cList[0].size() == 0)
        return;
    else{
        output << event << " " << cList.size() <<endl;
        for(unsigned int c = 0; c < cList.size(); c++){
            output << '\t' << c+1 << " " << cList[c].size() << endl;
            for(unsigned int h = 0; h < cList[c].size(); h++){
                output << '\t' << '\t' << cList[c][h].first << " " << cList[c][h].second << endl;
            }
        }
    }
}

//*****************************************************************************
//Function that returns if the hit is part of the cluster.

bool IsInCluster(int hit, Cluster cluster, string option){
    //Time clustering consition : 10ns
    if(option == "TIME")
        return hit-cluster.front().second < 10.0;

    //Strip clustering condition : adjacent strips
    else if(option == "STRIP")
        return hit-cluster.back().first == 1;

    return false;
}

//*****************************************************************************
//Function that groups neighbor hits, among an an array of "on time" hits, in
//order to make the clusters.

void GroupStrips(Cluster& tCluster, Cluster& sCluster, ClusterList& cList){
    if(tCluster.size() > 1) SortEvent(tCluster,0,tCluster.size()-1,"STRIP");        //Sort your time array by
                                                                                    //increasing strip number.

    for(unsigned int h = 0; h < tCluster.size(); h++){                              //Loop on the time hits.
        int tStrip = tCluster[h].first;
        float tTime = tCluster[h].second;
                                                                                    //If the strip array isn't
        if(sCluster.size() > 0){                                                    //empty, check if the hit
            if(IsInCluster(tStrip,sCluster,"STRIP")){                               //is adjacent to the last
                sCluster.push_back(make_pair(tStrip,tTime));                        //and add it to the array
            } else {                                                                //or if not, add the array
                AddCluster2List(sCluster,cList);                                    //to the cluster list,
                sCluster.clear();                                                   //clear it and reinitialize
                sCluster.push_back(make_pair(tStrip,tTime));                        //it with this hit.
            }
        } else {                                                                    //If the strip array is empty,
            sCluster.push_back(make_pair(tStrip,tTime));                            //initialise it with this hit.
        }
    }                                                                               //In the end, add the last
    AddCluster2List(sCluster,cList);                                                //content of the array into
    sCluster.clear();                                                               //the cluster list and clear
}                                                                                   //both time and strip arrays.

//*****************************************************************************
//Function that returns the time a cluster started. This will be used to group
//X and Y clusters into 1.

float GetClusterStart(Cluster& cluster){
    if(cluster.size() > 0){
        //First sort back the cluster by time (indeed the cluster was
        //formed by sorting by strips)
        if(cluster.size() > 1) SortEvent(cluster,0,cluster.size()-1,"TIME");

        //return the time of the first hit in the cluster
        return cluster.front().second;
    } else
        return 0.;
}

//*****************************************************************************
//Function that returns a bool to tell us if the time arrival of X and Y
//clusters coincides. This will be used to groupe X and Y clusters into 1.

bool Is2DCluster(Cluster clusterX, Cluster clusterY){
    //Get both cluster starting time stamps
    float xTime = GetClusterStart(clusterX);
    float yTime = GetClusterStart(clusterY);

    //Use the 10ns condition to test if they are part of the same cluster
    return abs(xTime-yTime) <= 10.;
}

//*****************************************************************************
//Function that returns the center a cluster. This will then be used to get the
//position of the clusters to fill in histograms.

float Get1DClusterCenter(Cluster cluster){
    float center = 0.;

    if(cluster.size() > 0){
        //Sum all the strip numbers in the cluster
        for(unsigned int i; i<cluster.size(); i++)
            center += cluster[i].first;

        //divide by the cluster size
        center = center/cluster.size();
    }

    return center;
}

//*****************************************************************************
//Function that analyses a file containing data with hits sorted by time stamp
//in each event to make a new outputfile the the hits grouped as clusters in
//each event.

void Analyse(string fName, int nStrips, float start, float end){
    float window = end - start;

    string HVstep = GetVoltage(fName);
    string prefix = "SORTED_" + HVstep + "V_";

    //Open inputfile
    unsigned NameInPath = fName.find_last_of("/")+1;
    string dName = fName.insert(NameInPath,prefix);
    ifstream input(dName.c_str(),ios::in);

    if(input.is_open()){
        MSG_INFO("Open the file and start clusterization.\n");

        //******************** CLUSTERIZED FILE **********************

        dName.erase(NameInPath,7);
        dName.insert(NameInPath,"CLUSTERIZED_");
        ofstream output(dName.c_str(),ios::out);

        //******************** CSV OUTPUT FILE ***********************

        if ( dName.substr(dName.find_last_of(".")) == ".dat" )
            dName.erase(dName.find_last_of("."));

        string pathName = dName.substr(0,dName.find_last_of("/")+1);
        string outputName = dName.substr(dName.find_first_of("_")+1);

        string ResultName = pathName + "RESULT_" + outputName + ".csv";
        ofstream ResultFile(ResultName.c_str(),ios::out);

        //******************** ROOT OUTPUT FILE **********************

        string fNameROOT = pathName + "ROOT_" + outputName + ".root";
        TFile ResultROOT(fNameROOT.c_str(),"RECREATE");

        //************************ DATA TREE *************************

        //Create the data tree where the data will be saved
        //For each entry will be saved the event tag, the number of hits recorded
        //in the TDCs, the list of fired TDC channels and the time stamps of the
        //hits.
        TTree* RAWDataTree = new TTree("RAWData","RAWData");

        int    EventCount = -9;  //Event tag
        int    nHits = -8;       //Number of fired X TDC channels in event
        eStrips TDCCh;            //List of fired X TDC channels in event
        eTimes  TDCTS;            //list of fired X TDC channels time stamps

        TDCCh.clear();
        TDCTS.clear();

        //Set the branches that will contain the previously defined variables
        RAWDataTree->Branch("EventNumber",    &EventCount,  "EventNumber/I");
        RAWDataTree->Branch("number_of_hits", &nHits,       "number_of_hits/I");
        RAWDataTree->Branch("TDC_channel",    &TDCCh);
        RAWDataTree->Branch("TDC_TimeStamp",  &TDCTS);

        //Cleaning all the vectors that will contain the data
        RAWData TDCData;

        //Initialisation of the RAWData vectors
        TDCData.EventList = new Events;
        TDCData.NHitsList = new hMult;
        TDCData.ChannelList = new esList;
        TDCData.TimeStampList = new etList;

        //Cleaning all the vectors
        TDCData.EventList->clear();
        TDCData.NHitsList->clear();
        TDCData.ChannelList->clear();
        TDCData.TimeStampList->clear();

        //*********************** HISTOGRAMS *************************

        //************ STRIPS

        //Hit Profiles
        TH1S* StripProfileX = new TH1S("StripProfileX","X Strip profile",nStrips,-0.5,nStrips-0.5);
        StripProfileX->SetXTitle("Strip");
        StripProfileX->SetYTitle("# of events");

        TH1S* StripProfileY = new TH1S("StripProfileY","Y Strip profile",nStrips,nStrips-0.5,2*nStrips-0.5);
        StripProfileY->SetXTitle("Strip");
        StripProfileY->SetYTitle("# of events");

        //Time Profiles
        TH1F* TimeProfileX = new TH1F("TimeProfileX","Arrival time profile of X hits",window/10,start,end);
        TimeProfileX->SetXTitle("Time [ns]");
        TimeProfileX->SetYTitle("# of events");

        TH1F* TimeProfileY = new TH1F("TimeProfileY","Arrival time profile of Y hits",window/10,start,end);
        TimeProfileY->SetXTitle("Time [ns]");
        TimeProfileY->SetYTitle("# of events");

        //Multiplicities
        TH1S* HitMultiplicityX = new TH1S("HitMultiplicityX","Hit multiplicity of X hits",2*nStrips+1,-0.5,2*nStrips+0.5);
        HitMultiplicityX->SetXTitle("Multiplicity");
        HitMultiplicityX->SetYTitle("# of events");

        TH1S* HitMultiplicityY = new TH1S("HitMultiplicityY","Hit multiplicity of Y hits",2*nStrips+1,-0.5,2*nStrips+0.5);
        HitMultiplicityY->SetXTitle("Multiplicity");
        HitMultiplicityY->SetYTitle("# of events");

        //************ CLUSTERS

        //Cluster Profiles
        TH1S* ClusterProfileX = new TH1S("ClusterProfileX","X Cluster profile",2*nStrips,-0.25,nStrips-0.75);
        ClusterProfileX->SetXTitle("Strip");
        ClusterProfileX->SetYTitle("# of events");

        TH1S* ClusterProfileY = new TH1S("ClusterProfileY","Y Cluster profile",2*nStrips,nStrips-0.25,2*nStrips-0.75);
        ClusterProfileY->SetXTitle("Strip");
        ClusterProfileY->SetYTitle("# of events");

        TH2S* ClusterProfileXY = new TH2S("ClusterProfileXY","XY Cluster profile",2*nStrips,-0.25,nStrips-0.75,2*nStrips,nStrips-0.25,2*nStrips-0.75);
        ClusterProfileXY->SetXTitle("Strip X");
        ClusterProfileXY->SetYTitle("Strip Y");
        ClusterProfileXY->SetZTitle("# of events");

        //Time Profile
        TH1F* ClusterTimeX = new TH1F("ClusterTimeX","Arrival time profile of X clusters",window/10,start,end);
        ClusterTimeX->SetXTitle("Time [ns]");
        ClusterTimeX->SetYTitle("# of events");

        TH1F* ClusterTimeY = new TH1F("ClusterTimeY","Arrival time profile of Y clusters",window/10,start,end);
        ClusterTimeY->SetXTitle("Time [ns]");
        ClusterTimeY->SetYTitle("# of events");

        TH2F* ClusterTimeXY = new TH2F("ClusterTimeXY","Arrival time profile of XY clusters",window/10,start,end,window/10,start,end);
        ClusterTimeXY->SetXTitle("Time X [ns]");
        ClusterTimeXY->SetYTitle("Time Y [ns]");
        ClusterTimeXY->SetZTitle("# of events");

        //Time difference between first and last hit in the cluster
        TH1F* ClusterDiffX = new TH1F("ClusterDiffX","Time spread of X clusters",50,0,50);
        ClusterDiffX->SetXTitle("Time [ns]");
        ClusterDiffX->SetYTitle("# of events");

        TH1F* ClusterDiffY = new TH1F("ClusterDiffY","Time spread of Y clusters",50,0,50);
        ClusterDiffY->SetXTitle("Time [ns]");
        ClusterDiffY->SetYTitle("# of events");

        TH1F* ClusterDiffXY = new TH1F("ClusterDiffXY","Time spread of 2D clusters",50,0,50);
        ClusterDiffXY->SetXTitle("Time [ns]");
        ClusterDiffXY->SetYTitle("# of events");

        //Multiplicities
        TH1S* ClusterMultiplicityX = new TH1S("ClusterMultiplicityX","Cluster multiplicity of X clusters",2*nStrips+1,-0.5,2*nStrips+0.5);
        ClusterMultiplicityX->SetXTitle("Multiplicity");
        ClusterMultiplicityX->SetYTitle("# of events");

        TH1S* ClusterMultiplicityY = new TH1S("ClusterMultiplicityY","Cluster multiplicity of Y clusters",2*nStrips+1,-0.5,2*nStrips+0.5);
        ClusterMultiplicityY->SetXTitle("Multiplicity");
        ClusterMultiplicityY->SetYTitle("# of events");

        TH1S* ClusterMultiplicityXY = new TH1S("ClusterMultiplicityXY","Cluster multiplicity of XY clusters",2*nStrips+1,-0.5,2*nStrips+0.5);
        ClusterMultiplicityXY->SetXTitle("Multiplicity");
        ClusterMultiplicityXY->SetYTitle("# of events");

        //Multiplicity vs 1D cluster position
        TH2S* ClusterMultPosX = new TH2S("ClusterMultPosX","Cluster multiplicity vs position of X clusters",2*nStrips+1,-0.5,2*nStrips+0.5,nStrips,-0.5,nStrips-0.5);
        ClusterMultPosX->SetXTitle("Multiplicity");
        ClusterMultPosX->SetYTitle("Strip");
        ClusterMultPosX->SetZTitle("# of events");

        TH2S* ClusterMultPosY = new TH2S("ClusterMultPosY","Cluster multiplicity vs position of Y clusters",2*nStrips+1,-0.5,2*nStrips+0.5,nStrips,nStrips-0.5,2*nStrips-0.5);
        ClusterMultPosY->SetXTitle("Multiplicity");
        ClusterMultPosY->SetYTitle("Strip");
        ClusterMultPosY->SetZTitle("# of events");

        //Cluster Sizes
        TH1S* ClusterSizeX = new TH1S("ClusterSizeX","Size of X cluster",nStrips,0.5,nStrips+0.5);
        ClusterSizeX->SetXTitle("Multiplicity");
        ClusterSizeX->SetYTitle("# of events");

        TH1S* ClusterSizeY = new TH1S("ClusterSizeY","Size of Y cluster",nStrips,0.5,nStrips+0.5);
        ClusterSizeY->SetXTitle("Multiplicity");
        ClusterSizeY->SetYTitle("# of events");

        TH2S* ClusterSizeXY = new TH2S("ClusterSizeXY","Size of XY cluster",nStrips,0.5,nStrips+0.5,nStrips,0.5,nStrips+0.5);
        ClusterSizeXY->SetXTitle("Multiplicity X");
        ClusterSizeXY->SetYTitle("Multiplicity Y");
        ClusterSizeXY->SetZTitle("# of events");

        //************ EFFICIENCY

        TH1S* Efficiency = new TH1S("Efficiency","Detection efficiency",2,-0.5,1.5);
        Efficiency->SetXTitle("(Not detected/Detected)");
        Efficiency->SetYTitle("# of events");

        //********************* READ DATA ****************************

        while(input.good()){
            int nEvent = -1;
            int nHitsX = -1;
            int nHitsY = -1;

            //Read the event number and number of hits on the X and Y
            //readouts
            input >> nEvent >> nHitsX >> nHitsY;

            //The clusters will be vectors of pairs.
            //They are stored in a vector to get a list of
            //clusters.
            ClusterList ClusterListX, ClusterListY, ClusterListXY;
            ClusterListX.clear();
            ClusterListY.clear();
            ClusterListXY.clear();

            //Check if end of file
            if(nEvent == -1 && nHitsX == -1 && nHitsY == -1){
                MSG_INFO("End of clusterization.\n");
            } else {
                //Fill the RAWData vectors:
                TDCData.EventList->push_back(nEvent);
                TDCData.NHitsList->push_back(nHitsX+nHitsY);

                //Create objects to contain the full list of hits and
                //time stamps to push into the TDCData
                eStrips tmpStrips;
                eTimes tmpTimes;
                tmpStrips.clear();
                tmpTimes.clear();

                //Fill hit multiplicity histograms
                HitMultiplicityX->Fill(nHitsX);
                HitMultiplicityY->Fill(nHitsY);

                //Start looping over the hits and reconstruct 1D clusters
                Cluster TimeCluster, StripCluster;
                TimeCluster.clear();
                StripCluster.clear();

                //First, loop over X readout hits
                if(nHitsX > 0){
                    for(int h = 0; h < nHitsX; h++){
                        int strip = -1;
                        float time = -1;

                        //Read the strip number and time stamp of each
                        //hit in the X readout
                        input >> strip >> time;

                        if(time == -1 && strip == -1){
                            MSG_ERROR("Problem with the X event : %d\n",nEvent);
                            return;
                        } else {
                            //Fill the strips and time stamps for TDCData
                            tmpStrips.push_back(strip);
                            tmpTimes.push_back(time);

                            if(time >= start && time <= end){
                                //Fill the hit and time profile histograms
                                StripProfileX->Fill(strip);
                                TimeProfileX->Fill(time);

                                //build clusters

                                //If the size if bigger than 0, the time
                                //cluster has already started to be filled
                                //Else that means that the current hit is
                                //the first one in the time cluster and is
                                //added into the cluster
                                if(TimeCluster.size() > 0){
                                    //Resort the cluster
                                    if(TimeCluster.size() > 1) SortEvent(TimeCluster,0,TimeCluster.size()-1,"TIME");
                                    //Check if next hit is in time cluster
                                    //else sort hits by strip number and
                                    //group hits together into 1D clusters
                                    if(IsInCluster(time,TimeCluster,"TIME")){
                                        TimeCluster.push_back(make_pair(strip,time));
                                    } else {
                                        GroupStrips(TimeCluster,StripCluster,ClusterListX);
                                        TimeCluster.clear();
                                        TimeCluster.push_back(make_pair(strip,time));
                                    }
                                } else {
                                    TimeCluster.push_back(make_pair(strip,time));
                                }
                            }
                        }
                    }
                    //When the code exits the loop, sort the hits of the
                    //very last time cluster by strip number and group
                    //hits into 1D clusters
                    GroupStrips(TimeCluster,StripCluster,ClusterListX);
                    TimeCluster.clear();
                }
                //Finally, print the clusters into a dat file to keep
                //track of the data
                PrintClusters(nEvent,ClusterListX,output);
                TimeCluster.clear();
                StripCluster.clear();

                //Repeat the same for the Y array data
                if(nHitsY > 0){
                    for(int h = 0; h < nHitsY; h++){
                        int strip = -1;
                        float time = -1;

                        //Read the strip number and time stamp of each
                        //hit in the Y readout
                        input >> strip >> time;

                        if(time == -1 && strip == -1){
                            MSG_ERROR("Problem with the Y event : ");
                            cout << nEvent << endl;
                            return;
                        } else {
                            //Fill the strips and time stamps for TDCData
                            tmpStrips.push_back(strip);
                            tmpTimes.push_back(time);

                            if(time >= start && time <= end){
                                //Fill the hit and time profile histograms
                                StripProfileY->Fill(strip);
                                TimeProfileY->Fill(time);

                                //build clusters
                                if(TimeCluster.size() > 0){
                                    if(TimeCluster.size() > 1) SortEvent(TimeCluster,0,TimeCluster.size()-1,"TIME");
                                    if(IsInCluster(time,TimeCluster,"TIME")){
                                        TimeCluster.push_back(make_pair(strip,time));
                                    } else {
                                        GroupStrips(TimeCluster,StripCluster,ClusterListY);
                                        TimeCluster.clear();
                                        TimeCluster.push_back(make_pair(strip,time));
                                    }
                                } else {
                                    TimeCluster.push_back(make_pair(strip,time));
                                }
                            }
                        }
                    }
                    GroupStrips(TimeCluster,StripCluster,ClusterListY);
                    TimeCluster.clear();
                }
                PrintClusters(nEvent,ClusterListY,output);
                TimeCluster.clear();
                StripCluster.clear();

                //Fill the 1D cluster multiplicity histograms
                ClusterMultiplicityX->Fill(ClusterListX.size());
                ClusterMultiplicityY->Fill(ClusterListY.size());

                //For each 1D cluster on X readout
                for(unsigned int x = 0; x<ClusterListX.size(); x++){
                    //Fill the 1D cluster histograms
                    //of X readout
                    float xCenter = Get1DClusterCenter(ClusterListX[x]);
                    ClusterProfileX->Fill(xCenter);
                    ClusterMultPosX->Fill(ClusterListX.size(),xCenter);

                    float xTime = GetClusterStart(ClusterListX[x]);
                    ClusterTimeX->Fill(xTime);

                    ClusterSizeX->Fill(ClusterListX[x].size());

                    //Get the time spread of the cluster
                    if(ClusterListX[x].size() > 1 && ClusterListX[x].size() < 5){
                        float lastXstamp = ClusterListX[x].back().second;
                        ClusterDiffX->Fill(lastXstamp-xTime);
                    }
                }

                //For each 1D cluster on Y readout
                for(unsigned int y = 0; y<ClusterListY.size(); y++){
                    //Fill the 1D cluster histograms
                    //of Y readout
                    float yCenter = Get1DClusterCenter(ClusterListY[y]);
                    ClusterProfileY->Fill(yCenter);
                    ClusterMultPosY->Fill(ClusterListY.size(),yCenter);

                    float yTime = GetClusterStart(ClusterListY[y]);
                    ClusterTimeY->Fill(yTime);

                    ClusterSizeY->Fill(ClusterListY[y].size());

                    //Get the time spread of the cluster
                    if(ClusterListY[y].size() > 1 && ClusterListY[y].size() < 5){
                        float lastYstamp = ClusterListY[y].back().second;
                        ClusterDiffY->Fill(lastYstamp-yTime);
                    }
                }

                //Loop over clusters and build 2D clusters
                if(ClusterListX.size() > 0 && ClusterListY.size() > 0){
                    //For each 1D cluster on X readout
                    for(unsigned int x = 0; x<ClusterListX.size(); x++){
                        //Check if the clusters on Y readout have a
                        //comparable starting time stamp
                        for(unsigned int y = 0; y<ClusterListY.size(); y++){
                            if(Is2DCluster(ClusterListX[x],ClusterListY[y])){
                                //In this case, fill the 2D profile with
                                //both X and Y cluster positions
                                float xCenter = Get1DClusterCenter(ClusterListX[x]);
                                float yCenter = Get1DClusterCenter(ClusterListY[y]);
                                ClusterProfileXY->Fill(xCenter,yCenter);

                                //Get the earliest starting time stamp
                                //as time stamp of the 2D cluster
                                float xTime = GetClusterStart(ClusterListX[x]);
                                float yTime = GetClusterStart(ClusterListY[y]);
                                ClusterTimeXY->Fill(xTime,yTime);

                                //Use a temporary cluster to fill the 2D
                                //cluster list
                                Cluster tmpCluster;
                                tmpCluster.insert(tmpCluster.begin(),ClusterListX[x].begin(),ClusterListX[x].end());
                                tmpCluster.insert(tmpCluster.begin(),ClusterListY[y].begin(),ClusterListY[y].end());
                                ClusterListXY.push_back(tmpCluster);

                                //Fill the 2D cluster size histo with the
                                //size of both X and Y clusters
                                ClusterSizeXY->Fill(ClusterListX[x].size(),ClusterListY[y].size());
                            }
                        }
                    }
                    //Fill the multiplicity of 2D clusters defined as the
                    //number of 2D clusters
                    ClusterMultiplicityXY->Fill(ClusterListXY.size());

                    for(unsigned int xy = 0; xy < ClusterListXY.size();xy++){
                        if(ClusterListXY[xy].size()>1 && ClusterListXY[xy].size()<5){
                            float xyTime = GetClusterStart(ClusterListXY[xy]);
                            float lastXYstamp = ClusterListXY[xy].back().second;
                            ClusterDiffXY->Fill(lastXYstamp-xyTime);
                        }
                    }
                } else {
                    ClusterMultiplicityXY->Fill(0.);
                }

                //The detector is defined as efficient is there was at least
                //1 2D cluster
                if((ClusterListX.size() > 0 && ClusterListX.size() < 3) || (ClusterListY.size() > 0 && ClusterListY.size() < 3)) Efficiency->Fill(1);
                else Efficiency->Fill(0);

                //Push the list of strips and time stamps into TDCData
                TDCData.ChannelList->push_back(tmpStrips);
                TDCData.TimeStampList->push_back(tmpTimes);
            }
        }

        //Write efficiency results in CSV file
        ResultFile << HVstep << '\t' << Efficiency->GetMean() << '\t' << Efficiency->GetStdDev() << endl;

        //Write all histograms into the ROOT file
        StripProfileX->Write();
        StripProfileY->Write();
        TimeProfileX->Write();
        TimeProfileY->Write();
        HitMultiplicityX->Write();
        HitMultiplicityY->Write();

        ClusterProfileX->Write();
        ClusterProfileY->Write();
        ClusterProfileXY->Write();
        ClusterTimeX->Write();
        ClusterTimeY->Write();
        ClusterTimeXY->Write();
        ClusterDiffX->Write();
        ClusterDiffY->Write();
        ClusterDiffXY->Write();
        ClusterMultiplicityX->Write();
        ClusterMultiplicityY->Write();
        ClusterMultiplicityXY->Write();
        ClusterMultPosX->Write();
        ClusterMultPosY->Write();
        ClusterSizeX->Write();
        ClusterSizeY->Write();
        ClusterSizeXY->Write();

        Efficiency->Write();

        //Write the data from the RAWData structure to the TTree
        for(unsigned int i=0; i<TDCData.EventList->size(); i++){
            EventCount  = TDCData.EventList->at(i);
            nHits       = TDCData.NHitsList->at(i);
            TDCCh       = TDCData.ChannelList->at(i);
            TDCTS       = TDCData.TimeStampList->at(i);

            RAWDataTree->Fill();
        }
        RAWDataTree->Write();

        //Close the files
        ResultROOT.Close();
        ResultFile.close();
        output.close();
        MSG_INFO("Data analysis done.\n");
    } else {
        MSG_ERROR("Couldn't open data file to clusterize.\n");
        return;
    }
    input.close();
}
