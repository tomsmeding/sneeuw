CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -O2
BIN = sneeuw

.PHONY: all clean remake

all: $(BIN)

clean:
	rm -f $(BIN)

remake: clean all


$(BIN): $(wildcard *.cpp)
	$(CXX) $(CXXFLAGS) -o $@ $^
