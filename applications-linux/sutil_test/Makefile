# Author : Samir Menon (c) Jul 16 2011.

#This file is part of sUtil, a random collection of utilities.
#
#sUtil is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 3 of the License, or (at your option) any later version.
#
#Alternatively, you can redistribute it and/or
#modify it under the terms of the GNU General Public License as
#published by the Free Software Foundation; either version 2 of
#the License, or (at your option) any later version.
#
#sUtil is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU Lesser General Public
#License and a copy of the GNU General Public License along with
#sUtil. If not, see <http://www.gnu.org/licenses/>.


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
DBGFLAGS = -Wall -g -ggdb -pg -O0 -std=c++0x $(INCLUDEDIRS) \
           $(SU_DEFS_DBG)
RELFLAGS = -Wall -O3 -std=c++0x $(INCLUDEDIRS) \
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
