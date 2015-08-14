
FLAGS=-I/include -lrina

all: rinaserver rinaclient

rinaserver: rinaserver.cc
	g++ $(FLAGS) rinaserver.cc -o rinaserver

rinaclient: rinaclient.cc
	g++ $(FLAGS) rinaclient.cc -o rinaclient


