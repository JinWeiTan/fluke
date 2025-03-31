CXX = g++

CXXFLAGS = -w -O3

EXE = bench/fluke

TARGET = $(EXE)

TARGET_DEL = $(TARGET:%=%.exe)

SRCS = src/main.cpp src/uci.cpp src/engine/attack.cpp src/engine/board.cpp \
		src/engine/engine.cpp src/engine/piece.cpp

OBJS = $(SRCS:%.cpp=bench/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

bench/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	del $(TARGET_DEL) $(OBJS)