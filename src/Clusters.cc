// **********************************************************************
// *   Hit clustering functions
// *   Alexis Fagot
// *   2/3/2015
// *********************************************************************

#include "../include/Clusters.h"
#include "../include/SortDataFile.h"
#include "../include/MsgSvc.h"

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
    output << Event << " " << ClusterList.size() <<endl;
    for(unsigned int c = 0; c < ClusterList.size(); c++){
        output << c+1 << " " << ClusterList[c].size() << endl;
        for(unsigned int h = 0; h < ClusterList[c].size(); h++){
            output << ClusterList[c][h].first << " " << ClusterList[c][h].second << endl;
        }
    }
}

//*****************************************************************************
//Function that returns if the hit is part of the cluster.

bool IsInCluster(int hit, vector< pair<int,float> > cluster, string option){
    if(option == "TIME"){                                                           //Time condition for cluters :
        return hit-cluster.front().second < 10.0;                                   //10 ns wide (unit = 100 ps).
    } else if(option == "STRIP"){                                                   //Strip condition for clusters :
        return hit-cluster.back().first == 1;                                       //neighbor strips.
    }
}

//*****************************************************************************
//Function that groups neighbor hits, among an an array of "on time" hits, in
//order to make the clusters.

void GroupStrips(vector< pair<int,float> >& tCluster, vector< pair<int,float> >& sCluster, vector< vector< pair<int,float> > >& cList){
    SortEvent(tCluster,0,tCluster.size()-1,"STRIP");                                //Sort your time array by
                                                                                    //increasing strip number.

    for(int h = 0; h < tCluster.size(); h++){                                       //Loop on the time hits.
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
                                                                                   //both time and strip arrays.
}

//*****************************************************************************
//Function that analyses a file containing data with hits sorted by time stamp
//in each event to make a new outputfile the the hits grouped as clusters in
//each event.

void Analyse(string fName){
    ifstream input(fName.c_str(),ios::in);                                          //Open inputfile (data)

        if(input){
            MSG_INFO("Open the file and start clusterization.\n");                  //If well open,
            unsigned NameInPath = fName.find_last_of("/")+1;                        //open outputfile
            fName.insert(NameInPath,"CLUSTERIZED_");
            ofstream output(fName.c_str(),ios::out);

            while(input.good()){                                                    //If well open
                int nEvent = -1;
                int nHits = -1;

                input >> nEvent >> nHits;                                           //Read event label and
                                                                                    //number of hits in event
                vector< vector< pair<int,float> > > ClusterList;
                ClusterList.clear();

                if(nEvent == -1 && nHits == -1){                                    //If they are still at their
                    MSG_INFO("End of clusterization.\n");                           //initial values, this is the
                    break;                                                          //end of the data file.
                } else if(nHits > 0){                                               //Else,
                    vector< pair<int,float> > TimeCluster, StripCluster;
                    TimeCluster.clear();
                    StripCluster.clear();

                    for(int h = 0; h < nHits; h++){                                 //loop on the hits
                        int strip = -1;                                             //to make clusters.
                        float time = -1;

                        input >> strip >> time;                                     //Read each channel/time pairs

                        if(time == -1 && strip == -1){                              //If there is nothing in the file
                            MSG_ERROR("Problem with the event : ");                 //This is a problem !
                            cout << nEvent << endl;
                            exit(EXIT_FAILURE);
                        } else {                                                    //Else, in the case...
                            if(TimeCluster.size() > 0){                             //... the array isn't empty,
                                if(IsInCluster(time,TimeCluster,"TIME")){           //check if the hit is on time
                                    TimeCluster.push_back(make_pair(strip,time));   //and add it to the array.
                                } else {                                            //If not, sort the array and
                                    GroupStrips(TimeCluster,StripCluster,ClusterList);//make strip clusters from it.
                                    TimeCluster.clear();                            //Clear the array and fill
                                    TimeCluster.push_back(make_pair(strip,time));   //it with this hit.
                                }
                            } else {                                                //... the array is empty,
                                TimeCluster.push_back(make_pair(strip,time));       //initialise it with this
                            }                                                       //hit.
                        }
                    }
                    GroupStrips(TimeCluster,StripCluster,ClusterList);              //Sort the very last time array
                    TimeCluster.clear();                                            //make clusters and clear.
                }
                PrintClusters(nEvent,ClusterList,output);                           //Finally print the list of
            }                                                                       //cluster into the output.
            output.close();
        } else {
            MSG_ERROR("Couldn't open data file to clusterize.\n");
            exit(EXIT_FAILURE);
        }
        input.close();
}
