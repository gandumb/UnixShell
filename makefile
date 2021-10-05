# makefile

all: shell

start: shell.cpp
	g++ -g -w -std=c++11 -o shell shell.cpp

clean:
	rm shell