#!/bin/bash

##################################
#Fit these for your experiment. Note I expect the offlineanalysismap to be in your home directory. In your homedirectory I also expect a map Data in wich you stored your different runs. Each run has a location in this map wich you can adjust. The program will take the raw data in the given map and run the offlineanalysis tool on it. After that It will gather the results and plot them using gnuplot
##################################
#starting voltage
Vstart=4000
Vend=10000
Vinterval=250
Location=BeforeCleanSignals
Rootfolder=root
Offlinefolder=OfflineAnalysisTool
Datafolder=Data
width=1000
min=413
max=454

#this tells the script where to find root
. ./$Rootfolder/bin/thisroot.sh

#here we change to the offlinefolder becouse the software there only runs when called from that folder
cd ./$Offlinefolder

#loops over all the voltages in the run
for (( Voltage=$Vstart; Voltage<=$Vend; Voltage=Voltage+Vinterval ))
do
#the next line does all the work. Adjust it so that it does what you want it to do. First argument = location of the data 2nd widht of signal 3th first signal 4th last signal
	echo "Analysing Data of the"$Voltage"Volts Run"
 	./offlineanalysis.sh ~/$Datafolder/$Location/$Voltage.dat $width $min $max
	echo ""
	echo ""
done

#now a bunch of CSV file have been created and we want to get the information out of them and put them in a file that we can plot with gnuplot

#save old IFS
OLDIFS=$IFS
IFS=","

#we go back to the home directory
cd --

#create the file where we will write the efficenty to
touch ./$Datafolder/$Location/EffeciencyOverVoltage.txt
echo "Creation of File EffeciencyOverVoltage.txt"

for (( Voltage=$Vstart; Voltage<=$Vend; Voltage=Voltage+Vinterval ))
do
	while read Efficiency EfficiencyError ClusterSize ClusterSizeError
	do
		echo "$Voltage		$Efficiency		$EfficiencyError" >> ./$Datafolder/$Location/EffeciencyOverVoltage.txt
	done < ./$Datafolder/$Location/RESULT_$Voltage.csv
done

#now we reset the IFS
IFS=$OLDIFS

#now we want to change to the directory in wich we made the new files. This is easier to work from with gnuplot
cd ./$Datafolder/$Location
echo "plotting EffeciencyOverVOltage.txt"

read -r -d '' GNUPLOT_SCRIPT <<EOF
set terminal pdf;
set output 'EffeciencyOverVoltage.pdf';
set xlabel 'Voltage(V)';
set ylabel 'Effeciency';
unset key;
set title 'Effeciency plot';
plot 'EffeciencyOverVoltage.txt' with yerrorbars
EOF

echo "$database" | gnuplot -e "$GNUPLOT_SCRIPT"







