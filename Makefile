CXX = g++

CXXFLAGS = -w -O3

EXE = build/fluke

TARGET = $(EXE)

TARGET_DEBUG = $(TARGET)_debug

TARGET_DEL = $(TARGET:%=%.exe)

SRCS = src/main.cpp src/uci.cpp src/engine/attack.cpp src/engine/board.cpp \
		src/engine/engine.cpp src/engine/piece.cpp src/engine/table.cpp

OBJS = $(SRCS:%.cpp=build/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	$(TARGET)

clean:
	del $(subst /,\,$(OBJS))

debug:
	$(CXX) $(CXXFLAGS) -o $(TARGET_DEBUG) -g $(SRCS)
