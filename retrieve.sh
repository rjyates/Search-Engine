#! /bin/bash
# Example shell script for processing queries
# ./retrieve.sh Dog_Cat bird invertedFile
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
flex retrieve.lex
g++ -o retrieve lex.yy.c -lfl
chmod 755 retrieve
./retrieve $@
echo
echo '----Done'
echo
