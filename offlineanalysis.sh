#!/bin/bash

bin/analysis "$1"

sufix='CLUSTERIZED_SORTED_'

test=${1%%\/*}

if [ $test == ".." ]
then
	clusterdata=${1%\/*}"/"$sufix${1##*\/}
else
	clusterdata=$sufix$1
fi

root -l -q 'make_histos.cc+("'$clusterdata'",'${2-50000}','${3-35025}','${4-35375}')'

