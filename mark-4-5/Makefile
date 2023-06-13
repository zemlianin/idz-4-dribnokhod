all: UDPEchoClientLoop UDPEchoServer-Fork
TCPEchoClientLoop: UDPEchoClientLoop.c DieWithError.c
	gcc UDPEchoClientLoop.c DieWithError.c -o UDPEchoClientLoop
TCPEchoServer-Fork: UDPEchoServer.h UDPEchoServer-Fork.c DieWithError.c
	gcc UDPEchoServer-Fork.c DieWithError.c \
	CreateUDPServerSocket.c\
	-o UDPEchoServer-Fork
