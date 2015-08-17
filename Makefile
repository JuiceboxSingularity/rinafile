
FLAGS=-I/include -lrina

all: rinaserver rinaclient

rinaserver: rinaserver.cc
	g++ $(FLAGS) rinaserver.cc -o rinaserver

rinaclient: rinaclient.cc
	g++ $(FLAGS) rinaclient.cc -o rinaclient

modify: modify.cc server.o application.o
	g++ $(FLAGS) modify.cc server.o application.o -o modify

server.o: server.cc
	g++ $(FLAGS) -c server.cc -o server.o

application.o: application.cc
	g++ $(FLAGS) -c application.cc -o application.o

