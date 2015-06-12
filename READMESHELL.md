When using the noise and data analying software the proces of running the program on every individual datafile can be tedious. The bash files do this task for you. Once the data has been analysed the program will also generate a plot of the data using gnuplot. Make sure gnuplot is installed!

The parameters needed are the following adjust them to fit your experiment and your pc file tree

#overal settings
Vstart=The voltage of your first run. Runnames must be e.g. 4000.dat
Vend=The voltage of the last run
Vinterval=The voltage between two runs.
Datafolder=This is the main datafolder
Location=Inside the main datafolder you might have different experiments. This is the location of the current one
Rootfolder=You need to have root installed. This is the folder where it is installed
Offlinefolder=Obviously the offlineanalysissoftware has to be installed. THis is the the folder where it is installed
width=The offlineanalysistool needs to know the width in ms of the run that it is analysing. 

#specific options for Dataanalysing
min=The offlineanalysistool also needs to know when the first event occured
max=As for the final event

#specific options for the noiseanalysing
surface=This is the active surface of your detector in square meters

Note that all folder paths should be starting in the home directory. Also not that the .sh files need to be placed in the home directory as well otherwise they will not find the correct folders.

