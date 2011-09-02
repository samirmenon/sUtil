# Author : Samir Menon
# (c) Jul 16 2011, All rights reserved.

############################# COMPILER OPTIONS ############################
CXX = g++

############################# INCLUDE OPTIONS ############################
SU_INC_DIR = ../../src/
SU_DEFS_DBG = -DASSERT=assert -DDEBUG=1 -DSU_TESTING=1
SU_DEFS_REL = -DRELEASE=1

############################# INCLUDE AND LINK OPTIONS ############################
#Collate all the include directiories
INCLUDEDIRS = -I$(SU_INC_DIR)

############################# Compile options ############################
DBGFLAGS = -Wall -g -ggdb -pg -O0 $(INCLUDEDIRS) \
           $(SU_DEFS_DBG)
RELFLAGS = -Wall -O3 $(INCLUDEDIRS) \
           $(SU_DEFS_REL) 

############################ SOURCE FILES ############################
#The application source(s)
SOURCES = test_main.cpp

############################ BUILD SPECIFICATIONS ############################
TARGET = sutil_test

.PHONY: all

#Build all simulations
all: 
	$(CXX) $(DBGFLAGS) -o $(TARGET) $(SOURCES)

release:
	$(CXX) $(RELFLAGS) -o $(TARGET) $(SOURCES)

debug:
	$(CXX) $(DBGFLAGS) -o $(TARGET) $(SOURCES)

#Clean up options
.PHONY: clean
clean: 
	@rm -f *.o $(TARGET) gmon.out
