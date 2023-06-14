all: UDPEchoClientLoop UDPEchoServer-Fork UDPObserver
UDPEchoClientLoop: UDPEchoClientLoop.c DieWithError.c
	gcc UDPEchoClientLoop.c DieWithError.c -o UDPEchoClientLoop
UDPEchoServer-Fork: UDPEchoServer.h UDPEchoServer-Fork.c DieWithError.c
	gcc UDPEchoServer-Fork.c DieWithError.c \
	CreateUDPServerSocket.c\
	 -o UDPEchoServer-Fork
UDPObserver: UDPObserver.c DieWithError.c
	gcc UDPObserver.c DieWithError.c\
	 -o UDPObserver