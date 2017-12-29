.PHONY: default all clean

default: all

CXX_INCLUDE_PATHS := include
CXXFLAGS := -std=c++17 -Wno-gnu-string-literal-operator-template -O3
LDFLAGS := 

CXX := time clang++


TARGETS := math.cpp qmath.cpp
TARGETS_WITHOUT_EXTENSION := $(TARGETS:%.cpp=%)
DEPENDENCY := $(TARGETS:%.cpp=%.d)

-include $(DEPENDENCY)

all: $(TARGETS_WITHOUT_EXTENSION)

$(TARGETS_WITHOUT_EXTENSION): %: %.cpp
	$(CXX) $(LDFLAGS) -MMD $(CXXFLAGS) $(CXX_INCLUDE_PATHS:%=-I%) $< -o $@
	
clean:
	rm -f $(TARGETS_WITHOUT_EXTENSION) $(DEPENDENCY)