#! /bin/bash

echo '----Regan Yates'
echo '----starter code from Dr. Gauch'
echo '----Working Individually'
echo '----Memory Based Algorithm'
flex multiple.lex
g++ -o multi lex.yy.c List.cpp documentHt.cpp globalHt.cpp invertedFile.cpp -lfl
time ./multi $1 $2
echo '----Finished'
