CXX = g++

CXXFLAGS = -w -O3

EXE = fluke

TARGET = $(EXE:%=bench/%)

TARGET_DEL = $(TARGET:%=%.exe)

SRCS = src/main.cpp src/uci.cpp src/engine/attack.cpp src/engine/board.cpp \
		src/engine/engine.cpp src/engine/piece.cpp

OBJS = $(SRCS:%.cpp=bench/%.o)

all: createfile $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

createfile:
	if not exist bench mkdir bench
	if not exist bench\src mkdir bench\src
	if not exist bench\src\engine mkdir bench\src\engine

bench/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	del $(TARGET_DEL) $(OBJS)