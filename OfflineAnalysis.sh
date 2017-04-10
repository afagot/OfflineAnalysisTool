#!/bin/bash

rm -r $1/DAT/
rm -r $1/CSV/
rm -r $1/ROOT/

cp $1/*.dat .

for dFile in *.dat
do
	echo $dFile
	bin/analysis $dFile
done

#for dFile in CLUSTERIZED*
#do
#	root -l -q 'make_histos.cc+("'$dFile'",'${2-1000}','${3-0}','${4-1000}')';
#done

mkdir -p $1/DAT/
mkdir -p $1/CSV/
mkdir -p $1/ROOT/

mv *.dat $1/DAT/.
mv RESULT*.csv $1/CSV/.
mv ROOT*.root $1/ROOT/.
