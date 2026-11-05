########################################################
# Makefile for CSCI 435 
# Author: Micah Schafer, Dr. Gary M. Zoppetti
########################################################

########################################################
# Variable definitions
########################################################
CXX := g++

# C++ compiler flags
CXXFLAGS := -Werror -Wall -std=c++26 -Isrc

# Linker. For C++ should be $(CXX).
LINK := $(CXX)

# Executable name. 
EXEC := CMinus

# Source directory name
SRC_DIR := src
# All .cc files recursively under src/
SRCS := $(shell find $(SRC_DIR) -name '*.cc')

BUILD_DIR := build

OBJS := $(patsubst %.cc, $(BUILD_DIR)/%.o, $(SRCS))

# Name of test directory in program root
TEST_DIR := tests
# All .cm files in the test directory
TEST_FILES := $(wildcard $(TEST_DIR)/*.cm)


#############################################################
# Rules
#   Rules have the form
#   target : prerequisites
#   	  recipe
#############################################################

.PHONY : all
all : $(EXEC)


$(EXEC) : $(OBJS)
	$(LINK) $^ -o $@


$(BUILD_DIR)/%.o: %.cc
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@


%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

#############################################################

.PHONY : test
test: $(EXEC)
	@for file in $(TEST_FILES); do \
		echo "Processing: $$file"; \
		./$(EXEC) "$$file"; \
		echo "-----------------------------"; \
	done

#############################################################

.PHONY : clean
clean :
	$(RM) $(EXEC) a.out core
	$(RM) -r $(BUILD_DIR)
	$(RM) $(TEST_DIR)/*.ast
	rm -f *.ast

#############################################################
