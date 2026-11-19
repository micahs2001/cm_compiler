Team Members:  Micah Schafer

> make 
     Builds program

> make clean 
     Removes generated files

> make test TEST_DIR=<path-to-test-dir>
     Runs program on all files in directory <path-to-test-dir>

> make lint 
     


###########################################################################
# Non-lab-related changes
###########################################################################


###########################################################################
# Implementation Issues and Solutions
###########################################################################

- Stockpiling errors in semantic analysis to emit
     For the semantic analysis visitor, I decided to give the class a 
     vector to keep track of the errors. I wanted as much information
     as I could be passed along, so I created the exceptions when found
     and placed them in the array as exception pointers.

     The driver will check if the vector is empty, if not, it will emit
     each exception to cerr and exit(1)

###########################################################################
# Linting Report
###########################################################################


###########################################################################
