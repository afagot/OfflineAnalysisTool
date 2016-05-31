#!/bin/bash

bin/analysis "$1"

prefix='CLUSTERIZED_SORTED_'

path=${1%\/*}
fName=${1##*\/}
clusterdata=$path"/"$prefix$fName

root -l -q 'make_histos.cc+("'$clusterdata'",'${2-10000}','${3-5025}','${4-5375}')'

