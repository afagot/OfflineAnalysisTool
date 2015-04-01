#!/bin/bash

bin/analysis "$1"

sufix='CLUSTERIZED_SORTED_'

clusterdata=$sufix$1

root -l -q 'make_histos.cc+("'$clusterdata'",'${2-50000}','${3-35025}','${4-35375}')'

