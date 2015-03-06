// **********************************************************************
// *   Hit clustering functions
// *   Alexis Fagot
// *   2/3/2015
// *********************************************************************

#include "../include/Clusters.h"
#include "../include/MsgSvc.h"

using namespace std;

//*****************************************************************************

void AddCluster2List(vector< pair<int,int> >& Cluster, vector < vector< pair<int,int> > >& ClusterList){
    ClusterList.push_back(Cluster);
    Cluster.clear();
}

//*****************************************************************************

void PrintCluster(int Event, vector< vector< pair<int,int> > >& ClusterList, ostream& output){
    output << Event << " " << ClusterList.size() <<endl;
    for(unsigned int c = 0; c < ClusterList.size(); c++){
        output << c+1 << " " << ClusterList[c].size() << endl;
        for(unsigned int h = 0; h < ClusterList[c].size(); h++){
            output << ClusterList[c][h].first << " " << ClusterList[c][h].second << endl;
        }
    }
}

//*****************************************************************************

void ClustrizeData(string fName){
    ifstream input(fName.c_str(),ios::in);

    if(input){
        fName = "CLUSTERIZED_" + fName;
        ofstream output(fName.c_str(),ios::out);

        while(input.good()){
            int nEvent = 0;
            int nHits = 0;

            input >> nEvent >> nHits;

            vector< vector< pair<int,int> > > ClusterList;
            ClusterList.clear();

            if(nEvent == 0 && nHits == 0){
                MSG_INFO("End of the file.");
                break;
            } else {
                vector< pair<int,int> > Cluster;
                Cluster.clear();

                for(int h = 0; h < nHits; h++){
                    int strip = -1;
                    int time = 0;

                    input >> strip >> time;

                    if(time == 0 && strip == -1){
                        MSG_ERROR("Problem with the event :");
                        cout << nEvent << endl;
                        exit(EXIT_FAILURE);
                    } if(h == 0){
                        Cluster.push_back(make_pair(strip,time));
                        if(h == nHits-1)
                            AddCluster2List(Cluster,ClusterList);
                    } else {
                        if(Cluster.size() > 0){
                            if(abs(strip - Cluster.back().first) == 1 && abs(time - Cluster.back().second) < 100){
                                Cluster.push_back(make_pair(strip,time));
                                if(h == nHits-1)
                                    AddCluster2List(Cluster,ClusterList);
                            } else if(strip == Cluster.back().first && Cluster.size() > 1){
                                vector< pair<int,int> > tempCluster;
                                tempCluster.clear();
                                tempCluster.push_back(make_pair(strip,time));
                                AddCluster2List(tempCluster,ClusterList);
                                if(h == nHits-1)
                                    AddCluster2List(Cluster,ClusterList);
                            } else {
                                AddCluster2List(Cluster,ClusterList);
                                Cluster.push_back(make_pair(strip,time));
                                if(h == nHits-1)
                                    AddCluster2List(Cluster,ClusterList);
                            }
                        } else {
                            Cluster.push_back(make_pair(strip,time));
                            if(h == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        }
                    }
                }
            }
            PrintCluster(nEvent,ClusterList,output);
        }
        output.close();
    } else {
        MSG_ERROR("Couldn't open data file to clusterize.");
        exit(EXIT_FAILURE);
    }
    input.close();
}
