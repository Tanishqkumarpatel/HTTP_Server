.PHONY: all clean

CXX := g++
CXXFLAGS := -std=c++17 -Wall

all: server client

server: server.cpp http.cpp router.cpp transaction.cpp
	${CXX} ${CXXFLAGS} -o server server.cpp http.cpp router.cpp transaction.cpp

client: client.cpp http.cpp
	${CXX} ${CXXFLAGS} -o client client.cpp http.cpp

clean:
	rm -f server client
