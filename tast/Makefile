# target: tstrings.out
TSTRINGS_CPP = tstrings.cpp
TSTRINGS_CXX = tstr.cxx tstring.cxx tstrbuf.cxx
TSTRINGS_HPP = ../telib/tstr.hpp ../telib/tstring.hpp ../telib/tstrbuf.hpp
tstrings.out : $(TSTRINGS_CPP) $(TSTRINGS_CXX) $(TSTRINGS_HPP)
	g++ -g -I .. -o tstrings.out $(TSTRINGS_CPP)
