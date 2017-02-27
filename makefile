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

ROOT_INC  = $(ROOTSYS)/include
ROOTLIBS     := $(shell root-config --libs)

LFLAGS     = -L$(DAQ_HOME_DIR)/lib -L/usr/lib \
             $(ROOTLIBS) 

CFLAGS     = -ggdb -fPIC -DLINUX -Wall -funsigned-char \
             -I$(DAQ_INC_DIR) -I$(ROOT_INC)

all: analysis

analysis: 	main.o Clusters.o SortDataFile.o
	g++ $(CFLAGS) $(DAQ_OBJ_DIR)/main.o \
	$(DAQ_OBJ_DIR)/Clusters.o \
	$(DAQ_OBJ_DIR)/SortDataFile.o \
        -o $(DAQ_BIN_DIR)/analysis \
        $(LFLAGS)  \
        -l CAENVME -l curses

clean:
	-rm $(DAQ_BIN_DIR)/analysis
	-rm $(DAQ_OBJ_DIR)/*.o

remove:
	-rm $(DAQ_BIN_DIR)/analysis
	-rm $(DAQ_OBJ_DIR)/*.o

main.o:
	g++ -c $(CFLAGS) $(DAQ_SRC_DIR)/main.cc -o $(DAQ_OBJ_DIR)/main.o
Clusters.o:
	g++ -c $(CFLAGS) $(DAQ_SRC_DIR)/Clusters.cc -o $(DAQ_OBJ_DIR)/Clusters.o
SortDataFile.o:
	g++ -c $(CFLAGS) $(DAQ_SRC_DIR)/SortDataFile.cc -o $(DAQ_OBJ_DIR)/SortDataFile.o
