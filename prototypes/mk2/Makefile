SPIXELS_DIR=../../spixels
SPIXELS_LIBRARY=$(SPIXELS_DIR)/lib/libspixels.a
LDFLAGS=-L$(SPIXELS_DIR)/lib -lspixels -lncurses -lmysqlcppconn -ljsoncpp
INCLUDE_FLAGS=-I$(SPIXELS_DIR)/include
CXXFLAGS=-Wall -O3 -J3 $(INCLUDE_FLAGS)
DEPS = grid.h engine.h effects.h

thegrid : grid.c engine.c mysql.c effects.c $(SPIXELS_LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(SPIXELS_LIBRARY):
	$(MAKE)  -C ../lib

VAR = $(file < Make.txt)
$(info [${VAR}])
