CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGET = portscanner
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

