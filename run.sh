#!/bin/sh

rm -f *.o *.exe

g++ -g -c Relay.cpp -DWINDOWS
g++ -g -c main.cpp
g++ -g main.o Relay.o -o main.exe

./main.exe COM8





