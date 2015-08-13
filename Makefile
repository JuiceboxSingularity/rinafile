
FLAGS=-I/include -lrina

all: rinafile

rinafile: rinafile.cc
	g++ $(FLAGS) rinafile.cc -o rinafile
