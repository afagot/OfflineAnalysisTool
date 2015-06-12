#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <math.h>
#include <limits>
#include "./include/MsgSvc.h"
#include "./analyse_noise.h"

using namespace std;  

int main(int argc, char* argv[]){
	string fName = argv[1];
	double measurement_time = atof(argv[2]);
	double detector_surface = atof(argv[3]);
	
	count_noise(fName,measurement_time,detector_surface);
}

void count_noise(string fName,double measurement_time,double detector_surface){
	
	ifstream data_file(fName.c_str(), ios::in);

   	if(data_file){
		cout <<  "----------------------------------------------------------------------\n";
        	MSG_INFO("------------------Open the file and start analysing.------------------\n");
		cout <<  "----------------------------------------------------------------------\n";
        	if(data_file.get() == '#'){
            		data_file.ignore(numeric_limits<streamsize>::max(),'\n');
			data_file.ignore(numeric_limits<streamsize>::max(),'\n');
        	}else{
            		MSG_ERROR("Wrong type of data file!.\n");
            		exit(EXIT_FAILURE);
        	}

        	unsigned int NameInPath = fName.find_last_of("/")+1;
        	fName.insert(NameInPath,"Noise_Analysed_");
        	ofstream output_file(fName.c_str(), ios::out);
		
        	double noise = 0; //the noise count
        	double events = 0; //event count
        	if(output_file){		
            		while(true){
				double runnumber = -1;
            			double hits = -1;
				data_file >> runnumber >> hits;
				if(runnumber == -1 || hits == -1)break;	
				
                		events++;
				
                		for(double i=0; i<hits; i++){
                	    		double channel = -1;
                    			double time = -1;

                    			data_file >> channel >> time;
                    			if(channel == -1 || time == -1){ 
						cerr << "Error with hit " << i << " of event " << runnumber << endl;
                    			}else{ 
						noise++;
					}
                		}		
				
            		}
            		//now we want to write the noise/time fraction to the datafile. The error on this is a purely statistical error. More hits -->more error because this implies that hits are a frequent thing. More events --> less error because we measured a long time. We devide this value by the amount of hits to get the RF=Error/measurement
            		double relative_error_noise = 1/sqrt(events);
            		//we also need to know how long the detector has been measuring.
            		double time_measured = (events*measurement_time)/(pow(10,9));
            		double noise_per_time_surface = noise/(time_measured*detector_surface);
            		double noise_per_time_surface_error = noise_per_time_surface*relative_error_noise;
			cout << "The amount of runs is: "<<events<< " .The amount of noisehits is: "<<noise<<"\n";
			cout <<  "----------------------------------------------------------------------\n";
            		cout << "The noise per second per square meter is : "<<noise_per_time_surface<<"\n";
            		cout << "The error on this value is : "<< noise_per_time_surface_error<<"\n";
			cout <<  "----------------------------------------------------------------------\n \n";
			
		
	        	//Finaly we want to write this information away in the output file.
            		output_file << noise_per_time_surface << "    ,    " << noise_per_time_surface_error << '\n';
        	}else{
            		MSG_ERROR("Failed to open new file to store data...\n");
                	exit(EXIT_FAILURE);
        	}

    	} else {
        	MSG_ERROR("Couldn't open data file to analyse.\n");
        	exit(EXIT_FAILURE);
   	}
}
