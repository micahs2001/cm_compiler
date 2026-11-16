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

.PHONY : lint
lint:
	@echo "Starting linter... Writing results to lint_report.txt"
	@rm -f lint_report.txt
	@total=$$(echo "$(SRCS)" | wc -w); \
	count=0; \
	has_warnings=0; \
	for file in $(SRCS); do \
		count=$$((count + 1)); \
		echo "[$$count/$$total] Processing $$file"; \
		output=$$(clang-tidy --quiet "$$file" -- $(CXXFLAGS) 2>&1); \
		if [ -n "$$output" ]; then \
			if [ $$has_warnings -eq 0 ]; then \
				echo "Clang-Tidy Report" > lint_report.txt; \
				echo "============================================================================" >> lint_report.txt; \
				has_warnings=1; \
			fi; \
			echo "" >> lint_report.txt; \
			echo "$$file --------------------------------" >> lint_report.txt; \
			echo "$$output" >> lint_report.txt; \
		fi; \
	done; \
	if [ $$has_warnings -eq 0 ]; then \
		echo "No warnings or errors found." > lint_report.txt; \
	fi

#############################################################

.PHONY : clean
clean :
	$(RM) $(EXEC) a.out core
	$(RM) -r $(BUILD_DIR)
	$(RM) $(TEST_DIR)/*.ast
	rm -f *.ast
	rm -f lint_report.txt

#############################################################
