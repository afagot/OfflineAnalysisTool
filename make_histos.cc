#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "THStack.h"
#include "TH1S.h"
#include "TH1F.h"
#include "TH2S.h"
#include "TH2F.h"
#include "TH3S.h"
#include "TH3F.h"
#include "THistPainter.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TGraphPainter.h"

#include "include/RPCGeometry.h"

#define ActiveAreaB2 (3894.18/2.)

using namespace std;

void make_histos(string fName){
    if ( fName.substr(fName.find_last_of(".")) == ".txt" )
        fName = fName.erase(fName.find_last_of("."));

    string Chamber = fName.substr(0,13);
    string HV = fName.substr(17,4);
//    string Thr = fName.substr(18,3);
//    string Partition = fName.substr(32,2);
    //Part to comment if not using files in Gamma_off/Run210714
    string Partition = fName.substr(33,2);
    int offset = 0;
    if(Partition == "A1") offset = 1*16;
    if(Partition == "A2") offset = 2*16;
    if(Partition == "B1") offset = 3*16;
    if(Partition == "B2") offset = 4*16;
    if(Partition == "C1") offset = 5*16;
    if(Partition == "C2") offset = 6*16;
    //***

    string ResultName = "Results_" + Chamber + ".csv";

    ofstream CosmicFile(ResultName.c_str(),ios::app);
    CosmicFile << "Partition\t" << Partition << '\n';
    CosmicFile << "HV\t" << HV << '\n';
//    CosmicFile << "Threshold\t" << Thr << '\n';

    string fNameROOT = "ROOT_" + fName + ".root";
    TFile CosmicsROOT(fNameROOT.c_str(),"RECREATE");

    //********************************************************************

    string fNameSorted = "STRIPSORTED_" + fName + ".txt";
    ifstream SortedFile(fNameSorted.c_str(),ios::in);

    if(SortedFile){
        TH1S* StripProfile = new TH1S("StripProfile","Strip profile",16,offset-15.5,offset+.5);
        StripProfile->SetXTitle("Strip");
        StripProfile->SetYTitle("# of events");

        TH1S* TimeProfile = new TH1S("TimeProfile","Time profile",26,0,26000);
        TimeProfile->SetXTitle("Time");
        TimeProfile->SetYTitle("# of events");

        TH2S* TDCProfile = new TH2S("TDCProfile","TDC map",16,offset-15.5,offset+.5,26,0,26000);
        TDCProfile->SetXTitle("Strip");
        TDCProfile->SetYTitle("Time");

        TH1S* HitMultiplicity = new TH1S("HitMultiplicity","Multiplicity",17,-0.5,16.5);
        HitMultiplicity->SetXTitle("Multiplicity");
        HitMultiplicity->SetYTitle("# of events");

        TH1S* EffStep1 = new TH1S("EffStep1","Efficiency of level 1",2,-0.5,1.5);
        EffStep1->SetXTitle("Hit (No/Yes)");
        EffStep1->SetYTitle("# of events");

        while(SortedFile.good()){
            int Event = 0;
            int Multiplicity = -1;

            SortedFile >> Event >> Multiplicity;

            vector< pair<int,int> > HitList;
            HitList.clear();

            if(Event == 0 && Multiplicity == -1){
                cout << "End of file " << fNameSorted << endl;
                break;
            } else if(Multiplicity == 0){
                HitMultiplicity->Fill(0);
                EffStep1->Fill(0);
            } else {
                HitMultiplicity->Fill(Multiplicity);
                EffStep1->Fill(1);
                for(int h = 0; h < Multiplicity; h++){
                    int strip = -1;
                    int time = -1;

                    SortedFile >> strip >> time;

                    strip = -strip + offset;

                    StripProfile->Fill(strip);
                    TimeProfile->Fill(time);
                    TDCProfile->Fill(strip,time);
                }
            }
        }
        float e = EffStep1->GetMean();              //Efficiency after time cut
        float N = EffStep1->GetEntries();           //Number of events (should be the same as the previous one)
        float estat = sqrt(e*(1-e)/N);              //Statistical error on efficiency after time cut

        CosmicFile << "Efficiency_1\t" << e << '\t' << estat << '\n';
        CosmicFile << "Hit_Multiplicity\t" << HitMultiplicity->GetMean() << '\t' << 2*HitMultiplicity->GetRMS()/sqrt(N) << '\n';

        StripProfile->Write();
        TimeProfile->Write();
        TDCProfile->Write();
        HitMultiplicity->Write();
        EffStep1->Write();
    }
    SortedFile.close();

    //********************************************************************

    string fNameStrips = "CLUSTERIZED_STRIPSORTED_" + fName + ".txt";
    ifstream StripFile(fNameStrips.c_str(),ios::in);

    if(StripFile){
        TH1S* StripClusterMultiplicity = new TH1S("StripClusterMultiplicity","Cluster multiplicity",21,-0.5,20.5);
        StripClusterMultiplicity->SetXTitle("Cluster multiplicity");
        StripClusterMultiplicity->SetYTitle("# of events");

        TH1S* StripClusterSize = new TH1S("StripClusterSize","Cluster size",17,-0.5,16.5);
        StripClusterSize->SetXTitle("Cluster size");
        StripClusterSize->SetYTitle("# of events");

        TH1S* EffStep2A = new TH1S("EffStep2A","Efficiency of level 2A",2,-0.5,1.5);
        EffStep2A->SetXTitle("Single strip cluster (No/Yes)");
        EffStep2A->SetYTitle("# of events");

        while(StripFile.good()){
            int Event = 0;
            int Multiplicity = -1;

            StripFile >> Event >> Multiplicity;

            if(Event == 0 && Multiplicity == -1){
                cout << "End of file " << fNameStrips << endl;
                break;
            } else if(Multiplicity == 0){
                StripClusterMultiplicity->Fill(0);
                EffStep2A->Fill(0);
            } else {
                StripClusterMultiplicity->Fill(Multiplicity);
                if(Multiplicity == 1) EffStep2A->Fill(1);
                else EffStep2A->Fill(0);

                for(int c = 0; c < Multiplicity; c++){
                    int cluster = -1;
                    int size = -1;
                    StripFile >> cluster >> size;

                    StripClusterSize->Fill(size);

                    for(int h = 0; h < size; h++){
                        int strip = -1;
                        int time = -1;
                        StripFile >> strip >> time;
                    }
                }
            }
        }
        float e = EffStep2A->GetMean();
        float N = EffStep2A->GetEntries();
        float estat = sqrt(e*(1-e)/N);

        CosmicFile << "Efficiency_2A\t" << e << '\t' << estat << '\n';
        CosmicFile << "Cluster_multiplicity_2A\t"
                   << StripClusterMultiplicity->GetMean() << '\t'
                   << 2*StripClusterMultiplicity->GetRMS()/sqrt(N) << '\n';
        CosmicFile << "Cluster_size_2A\t"
                   << StripClusterSize->GetMean() << '\t'
                   << 2*StripClusterSize->GetRMS()/sqrt(N) << '\n';

        StripClusterMultiplicity->Write();
        StripClusterSize->Write();
        EffStep2A->Write();
    }
    StripFile.close();

    //********************************************************************

    string fNameTime = "CLUSTERIZED_TIMESORTED_" + fName + ".txt";
    ifstream TimeFile(fNameTime.c_str(),ios::in);

    if(TimeFile){
        TH1S* TimeClusterSize = new TH1S("TimeClusterSize","Cluster size",17,-0.5,16.5);
        TimeClusterSize->SetXTitle("Cluster size");
        TimeClusterSize->SetYTitle("# of events");

        TH1S* TimeClusterMultiplicity = new TH1S("TimeClusterMultiplicity","Cluster multiplicity",21,-0.5,20.5);
        TimeClusterMultiplicity->SetXTitle("Cluster multiplicity");
        TimeClusterMultiplicity->SetYTitle("# of events");

        TH1S* EffStep2B = new TH1S("EffStep2B","Efficiency of level 2B",2,-0.5,1.5);
        EffStep2B->SetXTitle("Single time cluster (No/Yes)");
        EffStep2B->SetYTitle("# of events");

        while(TimeFile.good()){
            int Event = 0;
            int Multiplicity = -1;

            TimeFile >> Event >> Multiplicity;

            if(Event == 0 && Multiplicity == -1){
                cout << "End of file " << fNameTime << endl;
                break;
            } else if(Multiplicity == 0){
                TimeClusterMultiplicity->Fill(0);
                EffStep2B->Fill(0);
            } else {
                TimeClusterMultiplicity->Fill(Multiplicity);
                if(Multiplicity == 1) EffStep2B->Fill(1);
                else EffStep2B->Fill(0);

                for(int c = 0; c < Multiplicity; c++){
                    int cluster = -1;
                    int size = -1;
                    TimeFile >> cluster >> size;

                    TimeClusterSize->Fill(size);

                    for(int h = 0; h < size; h++){
                        int strip = -1;
                        int time = -1;
                        TimeFile >> strip >> time;
                    }
                }
            }
        }
        float e = EffStep2B->GetMean();
        float N = EffStep2B->GetEntries();
        float estat = sqrt(e*(1-e)/N);

        CosmicFile << "Efficiency_2B\t" << e << '\t' << estat << '\n';
        CosmicFile << "Cluster_multiplicity_2B\t"
                   << TimeClusterMultiplicity->GetMean() << '\t'
                   << 2*TimeClusterMultiplicity->GetRMS()/sqrt(N) << '\n';
        CosmicFile << "Cluster_size_2B\t"
                   << TimeClusterSize->GetMean() << '\t'
                   << 2*TimeClusterSize->GetRMS()/sqrt(N) << '\n';

        TimeClusterMultiplicity->Write();
        TimeClusterSize->Write();
        EffStep2B->Write();
    }
    TimeFile.close();

    //********************************************************************

    string fNameFinal = "TIMECLUSTERIZED_STRIPSORTED_" + fName + ".txt";
    ifstream FinalFile(fNameFinal.c_str(),ios::in);

    if(FinalFile){
        TH1S* FinalClusterMultiplicity = new TH1S("FinalClusterMultiplicity","Cluster multiplicity",21,-0.5,20.5);
        FinalClusterMultiplicity->SetXTitle("Cluster multiplicity");
        FinalClusterMultiplicity->SetYTitle("# of events");

        TH1S* FinalClusterSize = new TH1S("FinalClusterSize","Cluster size",17,-0.5,16.5);
        FinalClusterSize->SetXTitle("Cluster size");
        FinalClusterSize->SetYTitle("# of events");

        TH1S* EffStep3 = new TH1S("EffStep3","Efficiency of level 3",2,-0.5,1.5);
        EffStep3->SetXTitle("Single cluster (No/Yes)");
        EffStep3->SetYTitle("# of events");

    //********************************************************************
    //a batch of new graphs

        TH1F* FinalClusterPosition = new TH1F("FinalClusterPosition","Cluster position",31,offset-15.25,offset+.25);
        TH1F* Final1ClusterPosition = new TH1F("Final1ClusterPosition","Cluster position",31,offset-15.25,offset+.25);
        TH1F* Final2ClusterPosition = new TH1F("Final2ClusterPosition","Cluster position",31,offset-15.25,offset+.25);
        TH1F* FinalXClusterPosition = new TH1F("FinalXClusterPosition","Cluster position",31,offset-15.25,offset+.25);
        TH1F* Final2ClusterDistance = new TH1F("Final2ClusterDistance","Distance between 2 clusters",63,-15.75,15.75);
        TH1F* Final2ClusterTimeDiff = new TH1F("Final2ClusterTimeDiff","Time difference between 2 clusters",10,-500,500);

        TH2F* Final2ClusterCarac = new TH2F("Final2ClusterCarac","Diff map between 2 clusters",63,-15.75,15.75,10,-500,500);

    //********************************************************************

        while(FinalFile.good()){
            int Event = 0;
            int Multiplicity = -1;

            FinalFile >> Event >> Multiplicity;

            if(Event == 0 && Multiplicity == -1){
                cout << "End of file " << fNameFinal << endl;
                break;
            } else if(Multiplicity == 0){
                FinalClusterMultiplicity->Fill(0);
                EffStep3->Fill(0);
            } else {
                FinalClusterMultiplicity->Fill(Multiplicity);
                if(Multiplicity == 1) EffStep3->Fill(1);
                else EffStep3->Fill(0);

                float Position[16] = {0};
                float Time[16] = {0};

                for(int c = 0; c < Multiplicity; c++){
                    int cluster = -1;
                    int size = -1;
                    FinalFile >> cluster >> size;

                    FinalClusterSize->Fill(size);

                    for(int h = 0; h < size; h++){
                        int strip = -1;
                        int time = -1;
                        FinalFile >> strip >> time;

                        Position[c] += offset-strip;
                        Time[c] += time/10.;
                    }
                    Position[c] /= size;
                    FinalClusterPosition->Fill(Position[c]);
                    if(Multiplicity==1) Final1ClusterPosition->Fill(Position[c]);
                    else if(Multiplicity==2) Final2ClusterPosition->Fill(Position[c]);
                    else FinalXClusterPosition->Fill(Position[c]);
                    Time[c] /= size;
                }
                if(Multiplicity == 2){
                    int a = 0;
                    int b = 0;
                    if(Time[0] > 1330 && Time[0] < 1375) b =1;
                    else a = 1;
                    Final2ClusterDistance->Fill(Position[b]-Position[a]);
                    Final2ClusterTimeDiff->Fill(Time[b]-Time[a]);
                    Final2ClusterCarac->Fill(Position[b]-Position[a],Time[b]-Time[a]);
                }
            }
        }
        float N = FinalClusterMultiplicity->GetEntries();

        float cMultiplicity = FinalClusterMultiplicity->GetMean();
        float ecMultiplicity = 2*FinalClusterMultiplicity->GetRMS()/sqrt(N);
        float cSize = FinalClusterSize->GetMean();
        float ecSize = 2*FinalClusterSize->GetRMS()/sqrt(N);

        float gammaRate = (cMultiplicity/IntTime/ActiveAreaB2)/1000;
        float egammaRate = 0.;
        if(gammaRate != 0) egammaRate = gammaRate*(ecMultiplicity/cMultiplicity+ecSize/cSize);

        CosmicFile << "Gamma_rate_(KHz)\t"
                   << gammaRate << '\t'
                   << egammaRate << '\n';

        float e = EffStep3->GetMean();
        float estat = sqrt(e*(1-e)/N);

        CosmicFile << "Efficiency_3\t" << e << '\t' << estat << '\n';
        CosmicFile << "Cluster_multiplicity_3\t"
                   << cMultiplicity << '\t'
                   << ecMultiplicity << '\n';
        CosmicFile << "Cluster_size_3\t"
                   << cSize << '\t'
                   << ecSize << '\n';

        FinalClusterMultiplicity->Write();
        FinalClusterSize->Write();
        EffStep3->Write();
        FinalClusterPosition->Write();
        Final1ClusterPosition->Write();
        Final2ClusterPosition->Write();
        FinalXClusterPosition->Write();
        Final2ClusterDistance->Write();
        Final2ClusterTimeDiff->Write();
        Final2ClusterCarac->Write();
    }
    FinalFile.close();

    CosmicFile.close();

    CosmicsROOT.Close();
    cout << "Creation of file " << fNameROOT << endl;
}
