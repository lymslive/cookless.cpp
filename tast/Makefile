CC = g++
CXXFLAG = -Wformat

# target: tscalar.out
TSCALAR_CPP = tscalar.cpp
# TSCALAR_CXX = tstr.cxx tstring.cxx tstrbuf.cxx
TSCALAR_HPP = utast.hpp ../telib/thstrbase.hpp ../telib/tscalar.hpp
TSCALAR_DEP = $(TSCALAR_CPP) $(TSCALAR_CXX) $(TSCALAR_HPP)
tscalar.out : $(TSCALAR_DEP)
	$(CC) $(CXXFLAG) -g -I .. -std=c++11 -o tscalar.out $(TSCALAR_CPP)

# target: tstrings.out
TSTRINGS_CPP = tstrings.cpp
TSTRINGS_CXX = tstr.cxx tstring.cxx tstrbuf.cxx
TSTRINGS_HPP = ../telib/tstr.hpp ../telib/tstring.hpp ../telib/tstrbuf.hpp
TSTRINGS_DEP = $(TSTRINGS_CPP) $(TSTRINGS_CXX) $(TSTRINGS_HPP)
tstrings.out : $(TSTRINGS_DEP)
	$(CC) -g -I .. -o tstrings.out $(TSTRINGS_CPP)
