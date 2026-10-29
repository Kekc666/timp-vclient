CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
LDLIBS = -lcryptopp

OBJECTS = src/user_interface.o src/data_reader.o src/authenticator.o src/communicator.o src/data_writer.o

vclient: src/main.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

src/%.o: src/%.cpp src/*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f vclient src/*.o

.PHONY: clean
