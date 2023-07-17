.PHONY:all
all: tcp_client tcp_server

tcp_client:tcpClient.cc
	g++ -o $@ $^ -std=c++11 #-lpthread
tcp_server:tcpServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f tcp_client tcp_server