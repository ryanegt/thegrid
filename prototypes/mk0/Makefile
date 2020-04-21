SPIXELS_DIR=../../spixels
SPIXELS_LIBRARY=$(SPIXELS_DIR)/lib/libspixels.a
LDFLAGS=-L$(SPIXELS_DIR)/lib -lspixels
INCLUDE_FLAGS=-I$(SPIXELS_DIR)/include
CXXFLAGS=-Wall -O3 $(INCLUDE_FLAGS)
DEPS = grid.h

thegrid : grid.c funcs.c mysql.c $(SPIXELS_LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) -lmysqlcppconn

$(SPIXELS_LIBRARY):
	$(MAKE)  -C ../lib
