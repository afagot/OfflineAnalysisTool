#!/bin/bash

cp ../GIF_DAQ/datarun/*.dat .

for dFile in *.dat
do
	echo $dFile
	bin/analysis $dFile
done

for dFile in CLUSTERIZED*
do
	root -l -q 'make_histos.cc+("'$dFile'")';
done

rm *.dat
mv *.csv results/csv/.
mv *.root results/root/.
