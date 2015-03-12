#!/bin/bash

rm -f ../Gamma_off/Run210614/Strip* ../Gamma_off/Run210814/Time* ../Gamma_off/Run210814/Root* ../Gamma_off/Run210814/Results* ../Gamma_off/Run210814/Final*
mv ../Gamma_off/Run210814/Data* .


tar -jxvf Data_Run*

for dFile in RE-2-BARC-161*
do
	echo $dFile
	sed -i -e "s/XXXX/${dFile}/g" src/main.cc
	make OfflineAnalysis
	./OfflineAnalysis
	sed -i -e "s/${dFile}/XXXX/g" src/main.cc
	root -l -q 'make_histos.cc+("'$dFile'")';
done

RESULTS=$(ls Results*)
root -l -q 'make_gamma_graphs.cc+("'$RESULTS'")';

for ct in CLUSTERIZED_TIMESORTED*
do
	mv $ct TIMECLUSTERIZED${ct#CLUSTERIZED_TIMESORTED}
done

for cs in CLUSTERIZED_STRIPSORTED*
do
	mv $cs STRIPCLUSTERIZED${cs#CLUSTERIZED_STRIPSORTED}
done

for cs in TIMECLUSTERIZED_STRIPSORTED*
do
	mv $cs FINALCLUSTERIZED${cs#TIMECLUSTERIZED_STRIPSORTED}
done

rm -f RE-2-BARC-161*
tar -jcvf StripSortedData_Run210814.tar.bz2 STRIPSORTED*
tar -jcvf TimeSortedData_Run210814.tar.bz2 TIMESORTED*
tar -jcvf StripClusterizedData_Run210814.tar.bz2 STRIPCLUSTERIZED*
tar -jcvf TimeClusterizedData_Run210814.tar.bz2 TIMECLUSTERIZED*
tar -jcvf FinalClusterizedData_Run210814.tar.bz2 FINALCLUSTERIZED*
tar -jcvf RootData_Run210814.tar.bz2 ROOT*
mv *.tar.bz2 Results* ../Gamma_off/Run210814/.

rm -f FINALCLUSTERIZED* TIMECLUSTERIZED* TIMESORTED* STRIPCLUSTERIZED* STRIPSORTED* ROOT* RE-2-BARC-161*

make clean
