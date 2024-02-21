#! /bin/bash
# Example shell script for processing queries
# ./queryC++.sh Dog_Cat bird invertedFile
# Outputs:
# input Query: Dog_Cat bird 
# invertedFile directory: invertedFile
# The query is:  dog cat bird 
#    dog
#    cat
#    bird
echo
echo '---Querying'
echo
flex query.lex
g++ -o query invertedFile.cpp accumulator.cpp queryList.cpp lex.yy.c -lfl
./query $@
echo
echo '----Done'
echo

