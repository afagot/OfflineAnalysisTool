// **********************************************************************
// *   Hit clustering functions
// *   Alexis Fagot
// *   2/3/2015
// *********************************************************************

#include "../include/Clusters.h"

using namespace std;

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

void StripClusters(string fName){
    ifstream input(fName.c_str(),ios::in);

    if(input){
        fName = "CLUSTERIZED_" + fName;
        ofstream output(fName.c_str(),ios::out);

        while(input.good()){
            int Event = 0;
            int nHits = 0;

            input >> Event >> nHits;

            vector< vector< pair<int,int> > > ClusterList;
            ClusterList.clear();

            if(Event == 0 && nHits == 0){
                cout << "End of file " << fName << endl;
                break;
            } else {
                vector< pair<int,int> > Cluster;
                Cluster.clear();

                for(int n = 0; n < nHits; n++){
                    int strip = -1;
                    int time = 0;

                    input >> strip >> time;

                    if(time == 0 && strip == -1){
                        cout << "Problem with event #" << Event << " hit #" << n+1 << endl;
                        break;
                    } else if(n == 0){
                        Cluster.push_back(make_pair(strip,time));
                        if(n == nHits-1)
                            AddCluster2List(Cluster,ClusterList);
                    } else {
                        if(abs(strip - Cluster.back().first) == 1){
                            Cluster.push_back(make_pair(strip,time));
                            if(n == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        } else if(strip == Cluster.back().first && Cluster.size() > 1){
                            vector< pair<int,int> > tempCluster;
                            tempCluster.clear();
                            tempCluster.push_back(make_pair(strip,time));
                            AddCluster2List(tempCluster,ClusterList);
                            if(n == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        } else {
                            AddCluster2List(Cluster,ClusterList);
                            Cluster.push_back(make_pair(strip,time));
                            if(n == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        }
                    }
                }
            }
            PrintCluster(Event,ClusterList,output);
        }
        output.close();
    }
    input.close();
}

//*****************************************************************************

void TimeClusters(string fName){
    ifstream input(fName.c_str(),ios::in);

    if(input){
        fName = "CLUSTERIZED_" + fName;
        ofstream output(fName.c_str(),ios::out);

        while(input.good()){
            int Event = 0;
            int nHits = 0;

            input >> Event >> nHits;

            vector< vector< pair<int,int> > > ClusterList;
            ClusterList.clear();

            if(Event == 0 && nHits == 0){
                cout << "End of file " << fName << endl;
                break;
            } else {
                vector< pair<int,int> > Cluster;
                Cluster.clear();

                for(int n = 0; n < nHits; n++){
                    int time = 0;
                    int strip = -1;

                    input >> time >> strip;

                    if(time == 0 && strip == -1){
                        cout << "Problem with event #" << Event << " hit #" << n+1 << endl;
                        break;
                    } else if(n == 0){
                        Cluster.push_back(make_pair(time,strip));
                        if(n == nHits-1)
                            AddCluster2List(Cluster,ClusterList);
                    } else {
                        if(abs(time - Cluster[0].first) < 100){
                            Cluster.push_back(make_pair(time,strip));
                            if(n == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        } else {
                            AddCluster2List(Cluster,ClusterList);
                            Cluster.push_back(make_pair(time,strip));
                            if(n == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        }
                    }
                }
            }
            PrintCluster(Event,ClusterList,output);
        }
        output.close();
    }
    input.close();
}

//*****************************************************************************

void FinalClusters(string fName){
    ifstream input(fName.c_str(),ios::in);

    if(input){
        fName = "TIMECLUSTERIZED_" + fName;
        ofstream output(fName.c_str(),ios::out);

        while(input.good()){
            int Event = 0;
            int nHits = 0;

            input >> Event >> nHits;

            vector< vector< pair<int,int> > > ClusterList;
            ClusterList.clear();

            if(Event == 0 && nHits == 0){
                cout << "End of file " << fName << endl;
                break;
            } else {
                vector< pair<int,int> > Cluster;
                Cluster.clear();

                for(int n = 0; n < nHits; n++){
                    int strip = -1;
                    int time = 0;

                    input >> strip >> time;

                    if(time == 0 && strip == -1){
                        cout << "Problem with event #" << Event << " hit #" << n+1 << endl;
                        break;
                    } if(n == 0){
                        Cluster.push_back(make_pair(strip,time));
                        if(n == nHits-1)
                            AddCluster2List(Cluster,ClusterList);
                    } else {
                        if(Cluster.size() > 0){
                            if(abs(strip - Cluster.back().first) == 1 && abs(time - Cluster.back().second) < 100){
                                Cluster.push_back(make_pair(strip,time));
                                if(n == nHits-1)
                                    AddCluster2List(Cluster,ClusterList);
                            } else if(strip == Cluster.back().first && Cluster.size() > 1){
                                vector< pair<int,int> > tempCluster;
                                tempCluster.clear();
                                tempCluster.push_back(make_pair(strip,time));
                                AddCluster2List(tempCluster,ClusterList);
                                if(n == nHits-1)
                                    AddCluster2List(Cluster,ClusterList);
                            } else {
                                AddCluster2List(Cluster,ClusterList);
                                Cluster.push_back(make_pair(strip,time));
                                if(n == nHits-1)
                                    AddCluster2List(Cluster,ClusterList);
                            }
                        } else {
                            Cluster.push_back(make_pair(strip,time));
                            if(n == nHits-1)
                                AddCluster2List(Cluster,ClusterList);
                        }
                    }
                }
            }
            PrintCluster(Event,ClusterList,output);
        }
        output.close();
    }
    input.close();
}
