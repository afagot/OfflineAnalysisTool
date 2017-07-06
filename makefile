#file: Makefile
#
# Offline analysis program for CMS RPC
#
# 02/03/2015  Created by A.Fagot


DAQ_HOME_DIR = .
DAQ_BIN_DIR = $(DAQ_HOME_DIR)/bin
DAQ_INC_DIR = $(DAQ_HOME_DIR)/include
DAQ_SRC_DIR = $(DAQ_HOME_DIR)/src
DAQ_OBJ_DIR = $(DAQ_HOME_DIR)/obj

ROOT_INC := $(ROOTSYS)/include
ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS := $(shell root-config --libs)

LFLAGS     = -L$(DAQ_HOME_DIR)/lib -L/usr/lib \
             $(ROOTLIBS) 

CFLAGS     = -ggdb -fPIC -DLINUX -Wall -funsigned-char \
             -I$(DAQ_INC_DIR) -I$(ROOT_INC)

CC = g++ -std=c++11

all: analysis

analysis: jsoncpp.o main.o Clusters.o SortDataFile.o utils.o
	$(CC) $(CFLAGS) $(DAQ_OBJ_DIR)/main.o \
	$(DAQ_OBJ_DIR)/jsoncpp.o \
	$(DAQ_OBJ_DIR)/Clusters.o \
	$(DAQ_OBJ_DIR)/SortDataFile.o \
	$(DAQ_OBJ_DIR)/utils.o \
        -o $(DAQ_BIN_DIR)/analysis \
        $(LFLAGS)  \
        -l curses

jsoncpp.o:
	$(CC) $(CFLAGS) -c $(DAQ_SRC_DIR)/jsoncpp.cpp -o $(DAQ_OBJ_DIR)/jsoncpp.o
main.o: 
	$(CC) $(CFLAGS) -c $(DAQ_SRC_DIR)/main.cc -o $(DAQ_OBJ_DIR)/main.o
Clusters.o:
	$(CC) $(CFLAGS) -c $(DAQ_SRC_DIR)/Clusters.cc -o $(DAQ_OBJ_DIR)/Clusters.o
SortDataFile.o:
	$(CC) $(CFLAGS) -c $(DAQ_SRC_DIR)/SortDataFile.cc -o $(DAQ_OBJ_DIR)/SortDataFile.o
utils.o:
	$(CC) $(CFLAGS) -c $(DAQ_SRC_DIR)/utils.cc -o $(DAQ_OBJ_DIR)/utils.o

$(RUN_REGISTRY):
	mkdir -p $(RUN_REGISTRY)/

$(DAQ_BIN_DIR):
	mkdir -p $(DAQ_BIN_DIR)/

$(DAQ_OBJ_DIR):
	mkdir -p $(DAQ_OBJ_DIR)/

clean:
	-rm $(DAQ_BIN_DIR)/analysis
	-rm $(DAQ_OBJ_DIR)/*.o

remove:
	-rm $(DAQ_BIN_DIR)/analysis
	-rm $(DAQ_OBJ_DIR)/*.o
