#!/bin/bash

bin/analysis "$1"

sufix='CLUSTERIZED_SORTED_'

path=${1%\/*}
fName=${1##*\/}
clusterdata=${1%\/*}"/"$sufix${1##*\/}

echo $path
echo $fName
echo $clusterdata

#root -l -q 'make_histos.cc+("'$clusterdata'",'${2-50000}','${3-35025}','${4-35375}')'

