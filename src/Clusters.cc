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
#include "TH1S.h"
#include "TH2S.h"
#include "TH1F.h"

using namespace std;

//*****************************************************************************
//This function adds the hit array into the cluster list as new cluster and
//clears the hit array.

void AddCluster2List(vector< pair<int,float> >& Cluster, vector < vector< pair<int,float> > >& ClusterList){
    ClusterList.push_back(Cluster);
    Cluster.clear();
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


void PrintClusters(int Event, vector< vector< pair<int,float> > >& ClusterList, ostream& output){
    if(ClusterList.size() == 1 && ClusterList[0].size() == 0)
        MSG_WARNING("Skipped empty cluster\n");
    else{
        output << Event << " " << ClusterList.size() <<endl;
        for(unsigned int c = 0; c < ClusterList.size(); c++){
            output << '\t' << c+1 << " " << ClusterList[c].size() << endl;
            for(unsigned int h = 0; h < ClusterList[c].size(); h++){
                output << '\t' << '\t' << ClusterList[c][h].first << " " << ClusterList[c][h].second << endl;
            }
        }
    }
}

//*****************************************************************************
//Function that returns if the hit is part of the cluster.

bool IsInCluster(int hit, vector< pair<int,float> > cluster, string option){
    if(option == "TIME")                                                            //Time condition for cluters :
        return hit-cluster.front().second < 10.0;                                   //10 ns wide (unit = 100 ps).
    else if(option == "STRIP")                                                      //Strip condition for clusters :
        return hit-cluster.back().first == 1;                                       //neighbor strips.

    return false;
}

//*****************************************************************************
//Function that groups neighbor hits, among an an array of "on time" hits, in
//order to make the clusters.

void GroupStrips(vector< pair<int,float> >& tCluster, vector< pair<int,float> >& sCluster, vector< vector< pair<int,float> > >& cList){
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

float GetClusterStart(vector< pair<int,float> >& Cluster) {
    //First sort back the cluster by time (indeed the cluster was
    //formed by sorting by strips)
    if(Cluster.size() > 1) SortEvent(Cluster,0,Cluster.size()-1,"TIME");

    //return the time of the first hit in the cluster
    return Cluster[0].second;
}

//*****************************************************************************
//Function that returns a bool to tell us if the time arrival of X and Y
//clusters coincides. This will be used to groupe X and Y clusters into 1.

bool Is2DCluster(vector< pair<int,float> >& ClusterX, vector< pair<int,float> >& ClusterY) {
    //Get both cluster starting time stamps
    float xTime = GetClusterStart(ClusterX);
    float yTime = GetClusterStart(ClusterY);

    //Use the 10ns condition to test if they are part of the same cluster
    return abs(xTime-yTime) <= 10.;
}

//*****************************************************************************
//Function that returns the center a cluster. This will then be used to get the
//position of the clusters to fill in histograms.

float Get1DClusterCenter(vector< pair<int,float> >& Cluster) {
    float center = 0.;

    //Sum all the strip numbers in the cluster
    for(unsigned int i; i<Cluster.size(); i++)
        center += Cluster[i].first;

    //divide by the cluster size
    center /= Cluster.size();

    return center;
}

//*****************************************************************************
//Function that analyses a file containing data with hits sorted by time stamp
//in each event to make a new outputfile the the hits grouped as clusters in
//each event.

void Analyse(string fName, float window, float start, float end){
    //Open inputfile
    string dName = "SORTED_" + fName;
    ifstream input(dName.c_str(),ios::in);

        if(input){
            MSG_INFO("Open the file and start clusterization.\n");

            //******************** CLUSTERIZED FILE **********************

            unsigned NameInPath = fName.find_last_of("/")+1;
            fName.insert(NameInPath,"CLUSTERIZED_");
            ofstream output(fName.c_str(),ios::out);

            //******************** CSV OUTPUT FILE ***********************

            if ( fName.substr(fName.find_last_of(".")) == ".dat" )
                fName = fName.erase(fName.find_last_of("."));

            string pathName = fName.substr(0,fName.find_last_of("/")+1);
            string outputName = fName.substr(fName.find_first_of("_")+1);

            string ResultName = pathName + "RESULT_" + outputName + ".csv";
            ofstream ResultFile(ResultName.c_str(),ios::out);

            //******************** ROOT OUTPUT FILE **********************

            string fNameROOT = pathName + "ROOT_" + outputName + ".root";
            TFile ResultROOT(fNameROOT.c_str(),"RECREATE");

            //*********************** HISTOGRAMS *************************

            //************ STRIPS

            //Hit Profiles
            TH1S* StripProfileX = new TH1S("StripProfileX","X Strip profile",8,-0.5,7.5);
            StripProfileX->SetXTitle("Strip");
            StripProfileX->SetYTitle("# of events");

            TH1S* StripProfileY = new TH1S("StripProfileY","Y Strip profile",8,7.5,15.5);
            StripProfileY->SetXTitle("Strip");
            StripProfileY->SetYTitle("# of events");

            //Time Profiles
            TH1F* TimeProfileX = new TH1F("TimeProfileX","Arrival time profile of X hits",window/10,0,window);
            TimeProfileX->SetXTitle("Time [ns]");
            TimeProfileX->SetYTitle("# of events");

            TH1F* TimeProfileY = new TH1F("TimeProfileY","Arrival time profile of Y hits",window/10,0,window);
            TimeProfileY->SetXTitle("Time [ns]");
            TimeProfileY->SetYTitle("# of events");

            //Multiplicities
            TH1S* HitMultiplicityX = new TH1S("HitMultiplicityX","Hit multiplicity of X hits",11,-0.5,10.5);
            HitMultiplicityX->SetXTitle("Multiplicity");
            HitMultiplicityX->SetYTitle("# of events");

            TH1S* HitMultiplicityY = new TH1S("HitMultiplicityY","Hit multiplicity of Y hits",11,-0.5,10.5);
            HitMultiplicityY->SetXTitle("Multiplicity");
            HitMultiplicityY->SetYTitle("# of events");

            //************ CLUSTERS

            //Cluster Profiles
            TH1S* ClusterProfileX = new TH1S("ClusterProfileX","X Cluster profile",15,-0.25,7.25);
            ClusterProfileX->SetXTitle("Strip");
            ClusterProfileX->SetYTitle("# of events");

            TH1S* ClusterProfileY = new TH1S("ClusterProfileY","Y Cluster profile",15,7.75,15.25);
            ClusterProfileY->SetXTitle("Strip");
            ClusterProfileY->SetYTitle("# of events");

            TH2S* ClusterProfileXY = new TH2S("ClusterProfileXY","XY Cluster profile",15,-0.25,7.25,15,7.75,15.25);
            ClusterProfileXY->SetXTitle("Strip X");
            ClusterProfileXY->SetYTitle("Strip Y");
            ClusterProfileXY->SetZTitle("# of events");

            //Time Profile
            TH1F* ClusterTimeX = new TH1F("ClusterTimeX","Arrival time profile of X clusters",window/10,0,window);
            ClusterTimeX->SetXTitle("Time [ns]");
            ClusterTimeX->SetYTitle("# of events");

            TH1F* ClusterTimeY = new TH1F("ClusterTimeY","Arrival time profile of Y clusters",window/10,0,window);
            ClusterTimeY->SetXTitle("Time [ns]");
            ClusterTimeY->SetYTitle("# of events");

            TH1F* ClusterTimeXY = new TH1F("ClusterTimeY","Arrival time profile of XY clusters",window/10,0,window);
            ClusterTimeXY->SetXTitle("Time [ns]");
            ClusterTimeXY->SetYTitle("# of events");

            //Multiplicities
            TH1S* ClusterMultiplicityX = new TH1S("ClusterMultiplicityX","Cluster multiplicity of X clusters",11,-0.5,10.5);
            ClusterMultiplicityX->SetXTitle("Multiplicity");
            ClusterMultiplicityX->SetYTitle("# of events");

            TH1S* ClusterMultiplicityY = new TH1S("ClusterMultiplicityY","Cluster multiplicity of Y clusters",11,-0.5,10.5);
            ClusterMultiplicityY->SetXTitle("Multiplicity");
            ClusterMultiplicityY->SetYTitle("# of events");

            TH1S* ClusterMultiplicityXY = new TH1S("ClusterMultiplicityXY","Cluster multiplicity of XY clusters",11,-0.5,10.5);
            ClusterMultiplicityXY->SetXTitle("Multiplicity");
            ClusterMultiplicityXY->SetYTitle("# of events");

            //Cluster Sizes
            TH1S* ClusterSizeX = new TH1S("ClusterSizeX","Size of X cluster",8,0.5,8.5);
            ClusterSizeX->SetXTitle("Multiplicity");
            ClusterSizeX->SetYTitle("# of events");

            TH1S* ClusterSizeY = new TH1S("ClusterSizeY","Size of Y cluster",8,0.5,8.5);
            ClusterSizeY->SetXTitle("Multiplicity");
            ClusterSizeY->SetYTitle("# of events");

            TH2S* ClusterSizeXY = new TH2S("ClusterSizeXY","Size of XY cluster",8,0.5,8.5,8,0.5,8.5);
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
                vector< vector< pair<int,float> > > ClusterListX;
                vector< vector< pair<int,float> > > ClusterListY;
                vector< vector< pair<int,float> > > ClusterListXY;
                ClusterListX.clear();
                ClusterListY.clear();
                ClusterListXY.clear();

                //Check if end of file
                if(nEvent == -1 && nHitsX == -1 && nHitsY == -1){
                    MSG_INFO("End of clusterization.\n");
                    input.close();
                    return;
                } else {
                    //Fill hit multiplicity histograms
                    HitMultiplicityX->Fill(nHitsX);
                    HitMultiplicityY->Fill(nHitsY);

                    //Start looping over the hits and reconstruct 1D clusters
                    vector< pair<int,float> > TimeCluster, StripCluster;
                    TimeCluster.clear();
                    StripCluster.clear();

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
                                //Fill the hit and time profile histograms
                                StripProfileX->Fill(strip);
                                TimeProfileX->Fill(time);

                                //build clusters
                                if(TimeCluster.size() > 0){
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
                        GroupStrips(TimeCluster,StripCluster,ClusterListX);
                        TimeCluster.clear();
                    }
                    PrintClusters(nEvent,ClusterListX,output);
                    TimeCluster.clear();
                    StripCluster.clear();

                    //Fill the 1D cluster histograms
                    ClusterMultiplicityX->Fill(ClusterListX.size());
                    for(unsigned int c = 0; c < ClusterListX.size(); c++){
                        ClusterProfileX->Fill(Get1DClusterCenter(ClusterListX[c]));
                        ClusterTimeX->Fill(GetClusterStart(ClusterListX[c]));
                        ClusterSizeX->Fill(ClusterListX[c].size());
                    }

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
                                //Fill the hit and time profile histograms
                                StripProfileY->Fill(strip);
                                TimeProfileY->Fill(time);

                                //build clusters
                                if(TimeCluster.size() > 0){
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
                        GroupStrips(TimeCluster,StripCluster,ClusterListY);
                        TimeCluster.clear();
                    }
                    PrintClusters(nEvent,ClusterListY,output);
                    TimeCluster.clear();
                    StripCluster.clear();

                    //Fill the 1D cluster histograms
                    ClusterMultiplicityY->Fill(ClusterListY.size());
                    for(unsigned int c = 0; c < ClusterListY.size(); c++){
                        ClusterProfileY->Fill(Get1DClusterCenter(ClusterListY[c]));
                        ClusterTimeY->Fill(GetClusterStart(ClusterListY[c]));
                        ClusterSizeY->Fill(ClusterListY[c].size());
                    }

                    //Loop over clusters and build 2D clusters
                    if(ClusterListX.size() > 0 && ClusterListY.size() > 0){
                        for(unsigned int x = 0; x<ClusterListX.size(); x++){
                            for(unsigned int y = 0; y<ClusterListY.size(); y++){

                            }
                        }
                    }
                }
            }

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
            ClusterMultiplicityX->Write();
            ClusterMultiplicityY->Write();
            ClusterMultiplicityXY->Write();
            ClusterSizeX->Write();
            ClusterSizeY->Write();
            ClusterSizeXY->Write();

            Efficiency->Write();

            ResultROOT.Close();
            ResultFile.close();
            output.close();
        } else {
            MSG_ERROR("Couldn't open data file to clusterize.\n");
            return;
        }
}
